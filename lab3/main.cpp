#include "Task.h"
#include "ThreadPool.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <iostream>


int main() {
    srand(time(0));
    ThreadPool pool(4);

    auto startTime = std::chrono::steady_clock::now();
    int taskId = 0;

    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(15)) {
        Task task(taskId++, 5 + rand() % (12 - 5 + 1), [] { std::cout << "Task executed!" << std::endl;});
        pool.enqueue(task);
        if (std::chrono::steady_clock::now() - startTime > std::chrono::seconds(5)) {
            std::cout<<"task stopped"<<std::endl;
            pool.stopThreadPool();
        }
        if (std::chrono::steady_clock::now() - startTime > std::chrono::seconds(10)) {
            //std::cout<<"task stopped"<<std::endl;
            pool.startThreadPool();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));
    }

    pool.endThreadPool();
    return 0;
}
