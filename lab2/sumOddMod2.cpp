#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <cstdlib>

void fillArray(std::vector<int>& array, int arraySize) {
    array.resize(arraySize);
    for (int i = 0; i < arraySize; i++) {
            array[i] = rand() % 100;
    }
}

void processArrayLinear(const std::vector<int> &data, long long &sum, int &modTwo) {

    for (int i = 0; i < data.size(); i++) {
        if (data[i] % 2 != 0) {
            sum += data[i];
        }
    }
    modTwo = sum % 2;
}


void processChunkWithMutex(const std::vector<int>& data, long long& sum, int& currentIndex,  std::mutex& mutex, const int chunk_size) {

    int startIndex;
    int endIndex;
    {
        std::lock_guard<std::mutex> lock(mutex);
        startIndex = currentIndex;
        endIndex = std::min((currentIndex + chunk_size), static_cast<int>(data.size()));
        currentIndex = endIndex;
    }

    long long localSum = 0;
    for (int i = startIndex; i < endIndex; ++i) {
        if (data[i] % 2 != 0) {
            localSum += data[i];
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        sum += localSum;
    }

}

void processArrayWithMutex(const std::vector<int>& data, long long& sum, int& modTwo, int& maxThreads) {
    int chunk_size = data.size() / maxThreads;
    int currentIndex = 0;
    int currentThreadsNumber = 0;
    std::vector<std::thread> threads;
    std::mutex mutex;

    while (currentThreadsNumber <= maxThreads) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (currentIndex >= data.size()) break;
        }

        threads.emplace_back(processChunkWithMutex, std::ref(data), std::ref(sum), std::ref(currentIndex), std::ref(mutex), chunk_size);
        currentThreadsNumber++;
    }

    for (auto& thread : threads) {
        thread.join();
    }

    modTwo = sum % 2;
}

void processChunkWithCAS(const std::vector<int>& data, std::atomic<long long> &sum , std::atomic<int> &currentIndex, const int chunk_size ) {

    int startIndex = currentIndex.fetch_add(chunk_size);
    int endIndex = std::min((startIndex + chunk_size), static_cast<int>(data.size()));

    long long localSum = 0;
    for (int i = startIndex; i < endIndex; ++i) {
        if (data[i] % 2 != 0) {
            localSum += data[i];
        }
    }

    sum.fetch_add(localSum);
}

void processArrayWithCAS(const std::vector<int> &data, std::atomic<long long>&sum, int& modTwo, int maxThreads) {
    int chunk_size = data.size() / maxThreads;
    std::atomic<int> currentIndex(0);
    int currentThreadsNumber = 0;
    std::vector<std::thread> threads;

    while (currentThreadsNumber <= maxThreads) {
        if (currentIndex.load() >= data.size()) break;
        threads.emplace_back(processChunkWithCAS, std::ref(data), std::ref(sum), std::ref(currentIndex), chunk_size);
        currentThreadsNumber++;
    }

    for (auto& thread : threads) {
        thread.join();
    }

    modTwo = sum.load() % 2;

}

int main() {

    std::vector<int> arraySizeCounts = { 10000, 1000000, 100000000, 1000000000};
    std::vector<int> threadCounts = {2, 4, 8, 16, 32, 64, 128};

    for (int i = 0; i < arraySizeCounts.size(); i++) {

        auto array_creation_begin = std::chrono::high_resolution_clock::now();
        std::vector<int> array;
        fillArray(array, arraySizeCounts[i]);
        auto array_creation_end = std::chrono::high_resolution_clock::now();
        auto array_creation_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(array_creation_end - array_creation_begin);

        std::cout << std::endl << std::endl << "Array " << "with " << arraySizeCounts[i] << " size" <<" Creation Time: " << std::fixed << std::setprecision(9) << array_creation_duration.count() * 1e-9 << " seconds." << std::endl;


        std::cout << "Liniar decision" << std::endl;

        long long sum = 0;
        int modTwo = 0;

        auto linear_begin = std::chrono::high_resolution_clock::now();
        processArrayLinear(array, sum, modTwo);
        auto linear_end = std::chrono::high_resolution_clock::now();
        auto linear_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(linear_end - linear_begin);

        std::cout << "Processing Time: " << std::fixed << std::setprecision(14) << linear_duration.count() * 1e-9 << " seconds." << " Sum: " << sum << " Mod2: "<< modTwo << std::endl;


        std::cout << "Parallel decisions with Mutex" << std::endl;

        for (int i = 0; i < threadCounts.size(); i++) {

            long long sum = 0;
            int modTwo = 0;

            auto parallel_mutex_begin = std::chrono::high_resolution_clock::now();
            processArrayWithMutex(array, sum, modTwo, threadCounts[i]);
            auto parallel_mutex_end = std::chrono::high_resolution_clock::now();
            auto parallel_mutex_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(parallel_mutex_end - parallel_mutex_begin);

            std::cout << "Threads Number: " << threadCounts[i] << " Processing Time: " << std::fixed << std::setprecision(9) << parallel_mutex_duration.count() * 1e-9 << " seconds." << " Sum: " << sum << " Mod2: "<< modTwo << std::endl;

        }

        std::cout << "Parallel decisions with CAS" << std::endl;
        for (int i = 0; i < threadCounts.size(); i++) {

            std::atomic<long long> sum(0);
            int modTwo = 0;

            auto parallel_CAS_begin = std::chrono::high_resolution_clock::now();
            processArrayWithCAS(array, sum, modTwo, threadCounts[i]);
            auto parallel_CAS_end = std::chrono::high_resolution_clock::now();
            auto parallel_CAS_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(parallel_CAS_end - parallel_CAS_begin);

            std::cout << "Threads Number: " << threadCounts[i] << " Processing Time: " << std::fixed << std::setprecision(9) << parallel_CAS_duration.count() * 1e-9 << " seconds." << " Sum: " << sum.load() << " Mod2: "<< modTwo << std::endl;

        }
    }

}











