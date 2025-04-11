#include <iostream>
#include <winsock2.h>
#include <vector>
#include <string>
#include <random>
#include <cstring>
#include <cstdlib>
#include "Massege.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 55555

SOCKET  setUpSocket() {
	WSADATA wsaData;
	SOCKET clientSocket;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "Winsock initialization failed." << std::endl;
		return 1;
	}

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		std::cout << "Socket creation failed." << std::endl;
		WSACleanup();
		return 1;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		std::cout << "Connection failed." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	return clientSocket;
}

void generateMatrix( std::vector<std::vector<int>>*matrix, int matrixSize) {
	for (int i = 0; i < matrixSize; ++i) {
		std::vector<int> row;
		for (int j = 0; j < matrixSize; ++j) {
			row.push_back(rand() % 101);
		}
		matrix->push_back(row);
	}
}

int main() {

    SOCKET clientSocket = setUpSocket();
    std::cout << "Connected to the server!" << std::endl;

    while (true) {
        char command;
        std::cout << "Enter command: R to start, D to send data, B to start processing, S to check status, G to get answer, Q to quit:";
    	std::cin >> command;

		switch (command) {

			case 'G': {
				Message msg;

				char requestBuffer[1] = {command};
				int sbyteCount = send(clientSocket, requestBuffer, 1, 0);
				if (sbyteCount == SOCKET_ERROR) {
					std::cout << "Server send error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}

				char receiveBufferCommand[1];
				int rbyteCountCommand = recv(clientSocket, receiveBufferCommand, 1, 0);
				if (rbyteCountCommand < 0) {
					std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}
				msg.setCommand(receiveBufferCommand[0]);

				char receiveBufferMessageSize[4];
				int rbyteCount = recv(clientSocket, receiveBufferMessageSize, 4, 0);
				if (rbyteCount < 0) {
					std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}

				uint32_t massegeLength;
				std::memcpy(&massegeLength, receiveBufferMessageSize, 4);
				massegeLength = ntohl(massegeLength);
				int length = int(massegeLength);

				char* receiveBufferMessage = new char[length];
				int totalReceived = 0;

				while (totalReceived < length) {
					int rbyteCount = recv(clientSocket, receiveBufferMessage + totalReceived, length - totalReceived, 0);
					if (rbyteCount <= 0) {
						std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
						delete[] receiveBufferMessage;
						goto end_loop;
					}
					totalReceived += rbyteCount;
				}

				msg.decode(receiveBufferMessage);
				msg.print();

			break;
			}
			case 'D': {
				Message msg;
				int threadsNumber;
				int matrixSize;

				std::cout << "Enter threads number: ";
				std::cin >> threadsNumber;
				std::cout << "Enter matrix size: ";
				std::cin >> matrixSize;

				msg.setCommand('D');
				msg.setThreadNumber(threadsNumber);
				std::vector<std::vector<int>> matrix;
				generateMatrix(&matrix, matrixSize);
				msg.setMatrix(matrix);
				msg.print();

				int sbyteCount = send(clientSocket, msg.encode(), msg.getTotalSize(), 0);
				if (sbyteCount == SOCKET_ERROR) {
					std::cout << "Server send error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}

				char receiveBuffer[1];
				int rbyteCount = recv(clientSocket, receiveBuffer, 1, 0);
				if (rbyteCount < 0) {
					std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}
				char commandA = receiveBuffer[0];
				std::cout << "Received command: " << commandA<< std::endl;
			break;
			}
			case 'Q': {
				std::cout << "exiting....." << std::endl;
				goto end_loop;
			}
			default: {
				char requestBuffer[1] = {command};
				int sbyteCount = send(clientSocket, requestBuffer, 1, 0);
				if (sbyteCount == SOCKET_ERROR) {
					std::cout << "Server send error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}

				char receiveBuffer[1];
				int rbyteCount = recv(clientSocket, receiveBuffer, 1, 0);
				if (rbyteCount < 0) {
					std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
					goto end_loop;
				}
				std::cout << "Received command: " << receiveBuffer[0]<< std::endl;
			}
		}
    } end_loop:;

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}