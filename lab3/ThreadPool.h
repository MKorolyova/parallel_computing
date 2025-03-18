#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "Task.h"
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <iomanip>


class ThreadPool {

public:

    ThreadPool(const int threadsNumber = std::thread::hardware_concurrency());
    ~ThreadPool();

    void startThreadPool(const bool f);
    void stopThreadPool(const bool t);
    void endThreadPool();


    void enqueue(Task task);
    void printStatistics();

private:
    std::vector<std::thread> threads;
    std::queue<Task> tasks;
    std::mutex queueMutex;
    std::mutex coutMutex;
    std::condition_variable cv;
    bool stop = false;
    bool pause = false;
    int processTime = 0;

    std::atomic<int> droppedTasks{0};
    std::atomic<double> totalWaitTime{0.0};
    std::atomic<int> totalWaitSamples{0};
    double maxQueueFullTime = 0.0;
    double minQueueFullTime = std::numeric_limits<double>::max();
    bool queueWasFull = false;
    std::chrono::time_point<std::chrono::high_resolution_clock>  queueFullStart;

    void workerThread();
    void executeTask(const Task task);
    Task getTask();
};
#endif //THREADPOOL_H
