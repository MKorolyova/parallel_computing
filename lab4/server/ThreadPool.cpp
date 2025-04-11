#include "ThreadPool.h"
#include <iostream>
//public

ThreadPool::ThreadPool(const int threadsNumber){ //Number of concurrent threads supported
    for (int i = 0; i < threadsNumber; i++) {
        threads.emplace_back( &ThreadPool::workerThread, this);
    }
}

void ThreadPool::startThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        pause = false;
    }
    cv.notify_all();
}

void ThreadPool::stopThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        pause = true;
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
}


void ThreadPool::enqueue(Task task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::move(task));
    }
    cv.notify_one();
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
        std::cout << "Task: " << task.id <<  ", Thead: "<< std::this_thread::get_id() <<"start" << std::endl;
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    task.task();
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = end_time - start_time;
    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Task: " << task.id << ", Thead: "<< std::this_thread::get_id() << "end,  process time: " << duration.count() << " seconds." << std::endl;
    }
}

Task ThreadPool::getTask() {
    std::unique_lock<std::mutex> lock(queueMutex);
    cv.wait(lock, [this] { return (!tasks.empty() && !pause) || stop; });
    if (stop && tasks.empty()) return Task();

    Task task = std::move(tasks.front());
    tasks.pop();

    return task;
}