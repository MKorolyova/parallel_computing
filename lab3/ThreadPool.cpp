#include "ThreadPool.h"

//public

ThreadPool::ThreadPool(const int threadsNumber){ //Number of concurrent threads supported
    for (int i = 0; i < threadsNumber; i++) {
        threads.emplace_back( &ThreadPool::workerThread, this);
    }
}

void ThreadPool::startThreadPool(const bool f) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        pause = f;
    }
    cv.notify_all();
}

void ThreadPool::stopThreadPool(const bool t) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        pause = t;
    }
}

void ThreadPool::endThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
    threads.clear();
}

ThreadPool::~ThreadPool()
{
    endThreadPool();
    printStatistics();
}


void ThreadPool::enqueue(Task task)
{
    bool taskAdded = false;

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (processTime + task.processTime <= 60) {
            tasks.emplace(std::move(task));
            processTime += task.processTime;
            taskAdded = true;
            if (queueWasFull) {
                auto queueFullEnd = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> queueFullTime =  std::chrono::duration<double>(queueFullEnd - queueFullStart);
                maxQueueFullTime=std::max(maxQueueFullTime, queueFullTime.count());
                minQueueFullTime=std::min(minQueueFullTime, queueFullTime.count());
                queueWasFull = false;
            }
        }else {
            if (!queueWasFull) {
                queueFullStart = std::chrono::high_resolution_clock::now();
                queueWasFull = true;
            };
            droppedTasks++;
        }
    }
    if (taskAdded) {
        cv.notify_one();
    }
}

//private

void ThreadPool::workerThread(){
    while (true) {

        Task task = this->getTask();
        if (!task.isValid()) {
            return;
        }

        executeTask(task);

    }
}


void ThreadPool::executeTask(const Task task){
    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Task: " << task.id <<  ", Thead: "<< std::this_thread::get_id() <<", expected process time: " << task.processTime << " seconds" << std::endl;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(task.processTime)); //task.task();
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end_time - start_time;
    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Task: " << task.id << ", Thead: "<< std::this_thread::get_id() << ", real process time: " << duration.count() << " seconds." << std::endl;
    }
}


Task ThreadPool::getTask() {
    std::unique_lock<std::mutex> lock(queueMutex);

    auto startWait = std::chrono::high_resolution_clock::now();
    cv.wait(lock, [this] { return (!tasks.empty() && !pause) || stop; });
    auto endWait = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> waitTime = endWait - startWait;
    totalWaitTime.fetch_add(waitTime.count());
    totalWaitSamples.fetch_add(1);

    if (stop && tasks.empty()) return Task();

    Task task = std::move(tasks.front());
    tasks.pop();
    processTime -= task.processTime;

    return task;
}




void ThreadPool::printStatistics() {
    std::cout << "Created threads: " << 4 << std::endl;
    std::cout << "Average wait time: " << std::fixed << std::setprecision(9)<< totalWaitTime/totalWaitSamples << " seconds" << std::endl;/// totalWaitSamples) << " ms" << std::endl;
    std::cout << "Dropped tasks: " << droppedTasks << std::endl;
    std::cout << "Max queue full time: " << std::fixed << std::setprecision(9)<< maxQueueFullTime << " ms" << std::endl;
    std::cout << "Min queue full time: " << std::fixed << std::setprecision(9)<< minQueueFullTime << " ms" << std::endl;

}



