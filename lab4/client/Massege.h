#ifndef MESSAGE_H
#define MESSAGE_H
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <vector>
#include <thread>


class Message {
private:
    char command='o';
    int threadNumber = 0;
    int matrixSize = 0;
    std::vector<std::vector<int>> matrix;
    double processTime = 0;
    size_t totalSize = 0;

public:

    // setters
    void setCommand(const char command) {
        this->command = command;
    }
    void setThreadNumber(const int threadNumber) {
        this->threadNumber = threadNumber;
        this->setTotalSize();
    }
    void setProcessTime(const double processTime) {
        this->processTime = processTime;
        this->setTotalSize();
    }
    void setMatrix(const std::vector<std::vector<int>>& data){
        this->matrix = data;
        this->matrixSize = data.size();
        this->setTotalSize();
    }
    void setTotalSize() {
        this->totalSize = 1 + 4 + // command + total message size
            4 + std::to_string(this->threadNumber).size() +
            4 + std::to_string(this->matrixSize).size() +
            4 + std::to_string(this->processTime).size() +
            4 + (this->matrixSize * this->matrixSize * sizeof(uint8_t));
    }

    //getters
    int getThreadNumber() {
        return this->threadNumber;
    }
    int getTotalSize() {
        return this->totalSize;
    }
    std::vector<std::vector<int>> getMatrix(){
        return this->matrix;
    }
    int getMatrixSize() {
        return this->matrixSize;
    }

    void print() const {
        std::cout << "Command: " << command << std::endl;
        std::cout << "Thread Number: " << threadNumber << std::endl;
        std::cout << "Matrix Size: " << matrixSize << std::endl;
        std::cout << "Process Time: " << processTime << " ms" << std::endl;
        std::cout << "Total Size: " << totalSize << std::endl;
    }

    char* encode() {
        std::string threadsStr = std::to_string(this->threadNumber);
        std::string matrixSizeStr = std::to_string(this->matrixSize);
        std::string processTimeStr = std::to_string(this->processTime);

        char* buffer = new char[this->totalSize];
        char* ptr = buffer;

        // Command
        *ptr = this->command;
        ptr += 1;

        // Total message size
        uint32_t networkTotalSize = htonl(this->totalSize - 5 );
        std::memcpy(ptr, &networkTotalSize, 4);
        ptr += 4;

        // Threads number
        uint32_t len_threads = htonl(threadsStr.size());
        std::memcpy(ptr, &len_threads, 4); ptr += 4;
        std::memcpy(ptr, threadsStr.c_str(), threadsStr.size()); ptr += threadsStr.size();

        // Matrix size
        uint32_t len_matrixSize = htonl(matrixSizeStr.size());
        std::memcpy(ptr, &len_matrixSize, 4); ptr += 4;
        std::memcpy(ptr, matrixSizeStr.c_str(), matrixSizeStr.size()); ptr += matrixSizeStr.size();

        // Process time
        uint32_t len_processTime = htonl(processTimeStr.size());
        std::memcpy(ptr, &len_processTime, 4); ptr += 4;
        std::memcpy(ptr, processTimeStr.c_str(), processTimeStr.size()); ptr += processTimeStr.size();

        // Matrix bytes size
        int matrix_bytes_len = this->matrixSize * this->matrixSize * sizeof(uint8_t);
        uint32_t len_matrix = htonl(matrix_bytes_len);
        std::memcpy(ptr, &len_matrix, 4); ptr += 4;

        // Matrix
        for (const std::vector<int>& row : matrix) {
            for (int number : row) {
                uint8_t number_net = static_cast<uint8_t>(number);
                std::memcpy(ptr, &number_net, sizeof(uint8_t));
                ptr += sizeof(uint8_t);
            }
        }

        return buffer;
    }


    void decode(const char* buffer) {

        const char* ptr = buffer;

        // Threads number
        uint32_t len_threads;
        std::memcpy(&len_threads, ptr, 4);
        len_threads = ntohl(len_threads);
        ptr += 4;

        std::string threadsStr(ptr, len_threads);
        this->threadNumber = std::stoi(threadsStr);
        ptr += len_threads;

        // Matrix size
        uint32_t len_matrixSize;
        std::memcpy(&len_matrixSize, ptr, 4);
        len_matrixSize = ntohl(len_matrixSize);
        ptr += 4;

        std::string matrixSizeStr(ptr, len_matrixSize);
        this->matrixSize = std::stoi(matrixSizeStr);
        ptr += len_matrixSize;

        // Process time
        uint32_t len_processTime;
        std::memcpy(&len_processTime, ptr, 4);
        len_processTime = ntohl(len_processTime);
        ptr += 4;

        std::string processTimeStr(ptr, len_processTime);
        this->processTime = std::stod(processTimeStr);
        ptr += len_processTime;

        // Matrix bytes size skipped
        uint32_t len_matrix;
        std::memcpy(&len_matrix, ptr, 4);
        len_matrix = ntohl(len_matrix);
        ptr += 4;

        // Matrix
        matrix.resize(this->matrixSize, std::vector<int>(this->matrixSize));
        for (int i = 0; i < this->matrixSize; ++i) {
            for (int j = 0; j < this->matrixSize; ++j) {
                uint8_t value;
                std::memcpy(&value, ptr, sizeof(uint8_t));
                matrix[i][j] = static_cast<int>(value);
                ptr += sizeof(uint8_t);
            }
        }
        this->setTotalSize();
    }
};

#endif //MESSAGE_H
