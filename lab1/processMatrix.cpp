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
		for (int j = 0; j <matrixSize; j++) {
			std::cout << matrix[i][j] << "\t";
		}
		std::cout << "\n";
	}
	std::cout << "\n\n";
}


void processMatrixLinear(std::vector<std::vector<int>> matrix, int matrixSize) {

	for (int i = 0; i < matrixSize; i++) {
		for (int j = 0; j < matrixSize; j++) {
			if (matrix[i][i] < matrix[i][j]) {
				std::swap(matrix[i][i], matrix[i][j]);
			}

		}
	}
}

void processRow(std::vector<std::vector<int>> matrix, int matrixSize, int startRow, int endRow) {

	for (int i = startRow; i < endRow; i++) {
		for (int j = 0; j < matrixSize; j++) {
			if (matrix[i][i] < matrix[i][j]) {
				std::swap(matrix[i][i], matrix[i][j]);
			}

		}
	}
}

void processMatrixParallel(std::vector<std::vector<int>> matrix, int matrixSize, std::vector<std::thread>& threads, int threadsNumber) {

	int rowsNumberForthread = matrixSize / threadsNumber;

	if (rowsNumberForthread == 0) {
		std::cout << "Matrix is too small for parallel processing with this number of threads, so we will use " << matrixSize << " threads." << std::endl;
		rowsNumberForthread = 1;
	}

	for (int i = 0; i < threadsNumber; i++) {
		int startRow = i * rowsNumberForthread;
		int endRow = (i == threadsNumber - 1) ? (matrixSize - 1) : (startRow + rowsNumberForthread - 1);

		threads.push_back(std::thread(processRow, matrix, matrixSize, startRow, endRow)); //emplace_back

		//std::cout << "Thread " << i + 1 << ": processing rows " << startRow << " to " << endRow << std::endl;

	}

}


int main()

{

	auto matrix_creation_begin = std::chrono::high_resolution_clock::now();
	std::vector<std::vector<int>> matrix;
	int matrixSize = 20000;
	fillMatrix(matrix, matrixSize);
	auto matrix_creation_end = std::chrono::high_resolution_clock::now();
	auto matrix_creation_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(matrix_creation_end - matrix_creation_begin);

	std::cout << "Matrix Creation Time: " << std::fixed << std::setprecision(9) << matrix_creation_duration.count() * 1e-9 << " seconds." << std::endl;

	std::vector<std::thread> threads;
	int logicalCores = std::thread::hardware_concurrency();
	int physicalCores = logicalCores / 2;
	std::vector<int> threadCounts = {
		physicalCores / 2,
		3,
		physicalCores,
		logicalCores,
		logicalCores * 2,
		logicalCores * 4,
		logicalCores * 8,
		logicalCores * 16
	};


	std::cout << "Liniar decision" << std::endl;

	auto linear_begin = std::chrono::high_resolution_clock::now();
	processMatrixLinear(matrix, matrixSize);
	auto linear_end = std::chrono::high_resolution_clock::now();
	auto linear_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(linear_end - linear_begin);

	std::cout << "Processing Time: " << std::fixed << std::setprecision(9) << linear_duration.count() * 1e-9 << " seconds." << std::endl;



	std::cout << "Parallel decisions" << std::endl;


	for(int i = 0; i < threadCounts.size(); i++){


		auto parllel_bigin = std::chrono::high_resolution_clock::now();
		processMatrixParallel(matrix, matrixSize, threads, threadCounts[i]);
		auto parallel_end = std::chrono::high_resolution_clock::now();
		auto parallel_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(parallel_end - parllel_bigin);

		std::cout << "Threads Number: " << threadCounts[i] << " Processing Time: " << std::fixed << std::setprecision(9) << parallel_duration.count() * 1e-9 << " seconds." << std::endl;

		for (auto& thread : threads) {
			thread.join();
		}
		threads.clear();

	}
	return 0;
}
