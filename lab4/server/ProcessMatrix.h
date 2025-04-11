#include <iostream>
#include <thread>
#include "ThreadPool.h"
#include <chrono>
#include <vector>
#include <algorithm>

#ifndef PROCESSMATRIX_H
#define PROCESSMATRIX_H

class ProcessMatrix{
private:
	std::vector<std::vector<int>>matrix;
	int matrixSize;
	int threadsNumber;
	double processTime;
	std::atomic<bool> finished = false;
	std::atomic<bool> inProgress = false;
	std::atomic<bool> waiting = true;

public:

	double getProcessTime() {
		if (finished) {
			return processTime;
		}
		return 0;
	}

	std::vector<std::vector<int>> getProcessedMatrix() {
		if (finished) {
			return matrix;
		}
		std::vector<std::vector<int>> emptyMatrix;
		return emptyMatrix;
	}

	int getThreadsNumber() {
		if (finished) {
			return threadsNumber;
		}
		return 0;
	}

	void setMatrix(const std::vector<std::vector<int>>& matrix) {
		if (waiting) {
			this->matrix = matrix;

		}
	}

	void setMatrixSize(const int matrixSize) {
		if (waiting) {
			this->matrixSize = matrixSize;
		}
	}

	void setThreadsNumber(const int threadsNumber) {
		if (waiting) {
			this->threadsNumber = threadsNumber;
		}
	}

	char processStatus() {
		if (waiting) {
			return 'W';
		}
		if (inProgress) {
			return 'P';
		}
		return 'E';
	}

	void printMatrix() {
		std::cout << "\n\n";
		for (int i = 0; i < this->matrixSize; i++) {
			for (int j = 0; j < this->matrixSize; j++) {
				std::cout << this->matrix[i][j] << "\t";
			}
			std::cout << "\n";
		}
		std::cout << "\n\n";
	}

	void processRow(std::vector<int>& row, int currentRow, int matrixSize) {
		for (int j = 0; j < matrixSize; j++) {
			if (row[currentRow] < row[j]) {
				std::swap(row[currentRow], row[j]);
			}
		}
	}

	void processMatrixParallel() {
		if (this->matrix.empty()) {

			this->processTime = 0;
			this->inProgress = false;
			this->finished = true;
			std::cout << "\n\nError: Matrix is empty!\n";
			return;
		}
		auto parllel_begin = std::chrono::high_resolution_clock::now();
		this->waiting = false;
		this->inProgress = true;

		int rowsNumberPerThread = this->matrixSize / this->threadsNumber;
		std::vector<std::thread> threads;

		if (rowsNumberPerThread == 0) {
			rowsNumberPerThread = 1;
			this->threadsNumber = this->matrixSize;
		}

		for (int i = 0; i < this->threadsNumber; i++) {
			int startRow = i * rowsNumberPerThread;
			int endRow = (i == this->threadsNumber - 1) ? this->matrixSize : (startRow + rowsNumberPerThread);

			threads.push_back(std::thread([this, startRow, endRow]() {
				for (int i = startRow; i < endRow; i++) {
					this->processRow(this->matrix[i], i, this->matrixSize);
				}
				}));
		}

		for (auto& thread : threads) {
			thread.join();
		}

		auto parallel_end = std::chrono::high_resolution_clock::now();
		auto parallel_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(parallel_end - parllel_begin);
		this->processTime = parallel_duration.count()* 1e-9;
		this->inProgress = false;
		this->finished = true;

	}
};

#endif //PROCESSMATRIX_H
