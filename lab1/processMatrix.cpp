#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm> // For std::swap
#include <iomanip>  // For std::fixed and std::setprecision

void fillMatrix(std::vector<std::vector<int>>& matrix, int matrixSize) {

	matrix.resize(matrixSize, std::vector<int>(matrixSize));

	for (int i = 0; i < matrixSize; i++) {
		for (int j = 0; j < matrixSize; j++) {
			matrix[i][j] = rand() % 100;
		}
	}
}

void printMatrix(std::vector<std::vector<int>>& matrix, int matrixSize) {

	std::cout << "\n\n";
	for (int i = 0; i < matrixSize; i++) {
		for (int j = 0; j < matrixSize; j++) {
			std::cout << matrix[i][j] << "\t";
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

void processMatrixLinear(std::vector<std::vector<int>> matrix, int matrixSize) {

	for (int i = 0; i < matrixSize; i++) {
		processRow(matrix[i], i, matrixSize);
	}
}

void processMatrixParallel(std::vector<std::vector<int>> matrix, int matrixSize, int threadsNumber) {
	int rowsNumberPerThread = matrixSize / threadsNumber;
	std::vector<std::thread> threads;

	if (rowsNumberPerThread == 0) {
		std::cout << "Matrix is too small for parallel processing with this number of threads, using " << matrixSize << " threads.\n";
		rowsNumberPerThread = 1;
		threadsNumber = matrixSize;
	}

	for (int i = 0; i < threadsNumber; i++) {
		int startRow = i * rowsNumberPerThread;
		int endRow = (i == threadsNumber - 1) ? matrixSize : (startRow + rowsNumberPerThread);

		threads.push_back(std::thread([startRow, endRow, &matrix, matrixSize]() {
			for (int i = startRow; i < endRow; i++) {
				processRow(matrix[i], i, matrixSize);
			}
			}));
	}

	for (auto& thread : threads) {
		thread.join();
	}
}

int main()

{
	unsigned int n = std::thread::hardware_concurrency();
	std::cout << n << " concurrent threads are supported.\n";

	std::vector<int> matrixSizeCounts = { 1000, 10000, 30000 };
	for (int i = 0; i < matrixSizeCounts.size(); i++) {

		auto matrix_creation_begin = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<int>> matrix;
		int matrixSize = matrixSizeCounts[i];
		fillMatrix(matrix, matrixSize);
		auto matrix_creation_end = std::chrono::high_resolution_clock::now();
		auto matrix_creation_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(matrix_creation_end - matrix_creation_begin);

		std::cout << std::endl << std::endl << "Matrix " << matrixSize << "x" << matrixSize << " Creation Time: " << std::fixed << std::setprecision(9) << matrix_creation_duration.count() * 1e-9 << " seconds." << std::endl;

		int logicalCores = std::thread::hardware_concurrency();
		int physicalCores = logicalCores / 2;
		std::vector<int> threadCounts = {
			physicalCores / 2,
			physicalCores,
			logicalCores,
			logicalCores * 2,
			logicalCores * 4,
			logicalCores * 8,
			logicalCores * 16,
			10000,
		};

		std::cout << "Liniar decision" << std::endl;

		auto linear_begin = std::chrono::high_resolution_clock::now();
		processMatrixLinear(matrix, matrixSize);
		auto linear_end = std::chrono::high_resolution_clock::now();
		auto linear_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(linear_end - linear_begin);

		std::cout << "Processing Time: " << std::fixed << std::setprecision(9) << linear_duration.count() * 1e-9 << " seconds." << std::endl;


		std::cout << "Parallel decisions" << std::endl;
		for (int i = 0; i < threadCounts.size(); i++) {

			auto parllel_begin = std::chrono::high_resolution_clock::now();
			processMatrixParallel(matrix, matrixSize, threadCounts[i]);
			auto parallel_end = std::chrono::high_resolution_clock::now();
			auto parallel_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(parallel_end - parllel_begin);

			std::cout << "Threads Number: " << threadCounts[i] << " Processing Time: " << std::fixed << std::setprecision(9) << parallel_duration.count() * 1e-9 << " seconds." << std::endl;

		}
	}

	system("pause");
	return 0;
}