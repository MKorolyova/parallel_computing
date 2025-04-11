#include <iostream>
#include <winsock2.h>
#include <thread>
#include "Message.h"
#include "ProcessMatrix.h"
#include "ThreadPool.h"
#include "Task.h"
#include <chrono>

SOCKET  setUpSocket() {
	WSADATA wsaData;
	int wsInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsInit != 0) {
		std::cerr << "WSAStartup failed with error: " << wsInit << std::endl;
		return 0;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with your desired IP address
	service.sin_port = htons(55555);  // Choose a port number

	if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
		std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}

	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
	}

	return serverSocket;
}

int main() {

    ThreadPool pool(4);
    int taskId = 0;
    SOCKET serverSocket = setUpSocket();

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
        }else {
            Task task(taskId++, clientSocket, [clientSocket] {
            	ProcessMatrix processMatrix = ProcessMatrix();
                while (true) {
	                char receiveBuffer[1];
					int rbyteCount = recv(clientSocket, receiveBuffer, 1, 0);
					if (rbyteCount < 0) {
						std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
						break;
					}
					char command = receiveBuffer[0];
					std::cout << "Received command: " << command<< std::endl;

					switch (command) {
						case 'R': {
							char requestBuffer[1] = {'W'};
							int sbyteCount = send(clientSocket, requestBuffer, 1, 0);
							if (sbyteCount == SOCKET_ERROR) {
								std::cout << "Server send error: " << WSAGetLastError() << std::endl;
								goto end_loop;
							}
							break;
						}
						case 'D': {
							Message msg;

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
								std::cout<<"totalReceived length: "<<totalReceived<<std::endl;
								int rbyteCount = recv(clientSocket, receiveBufferMessage + totalReceived, length - totalReceived, 0);
								if (rbyteCount <= 0) {
									std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
									delete[] receiveBufferMessage;
									goto end_loop;
								}
								totalReceived += rbyteCount;
							}

							msg.setCommand(command);
							msg.decode(receiveBufferMessage);
							msg.print();
							processMatrix.setMatrixSize(msg.getMatrixSize());
							processMatrix.setMatrix(msg.getMatrix());
							processMatrix.setThreadsNumber(msg.getThreadNumber());

							char requestBuffer[1] = {'L'};
							int sbyteCount = send(clientSocket, requestBuffer , 1, 0);
							if (sbyteCount == SOCKET_ERROR) {
								std::cout << "Server send error: " << WSAGetLastError() << std::endl;
								goto end_loop;
							}
						break;
						}
						case 'B': {
							std::thread processingThread(&ProcessMatrix::processMatrixParallel, &processMatrix);
							processingThread.detach();

							 char requestBuffer[1] = {'K'};
							 int sbyteCount = send(clientSocket, requestBuffer , 1, 0);
							 if (sbyteCount == SOCKET_ERROR) {
								 std::cout << "Server send error: " << WSAGetLastError() << std::endl;
							 	 goto end_loop;
							 }
						break;
						}
						case 'S': {
							char status = processMatrix.processStatus();

							char requestBuffer[1] = {status};
							int sbyteCount = send(clientSocket, requestBuffer , 1, 0);
							if (sbyteCount == SOCKET_ERROR) {
								std::cout << "Server send error: " << WSAGetLastError() << std::endl;
								goto end_loop;
							}

						break;
						}
						case 'G': {
							Message msg;
							msg.setCommand('A');
							msg.setThreadNumber(processMatrix.getThreadsNumber());
							msg.setMatrix(processMatrix.getProcessedMatrix());
							msg.setProcessTime(processMatrix.getProcessTime());
							msg.print();

						   int sbyteCount = send(clientSocket, msg.encode() , msg.getTotalSize(), 0);
						   if (sbyteCount == SOCKET_ERROR) {
							   std::cout << "Server send error: " << WSAGetLastError() << std::endl;
						   	goto end_loop;
						   }
						break;
						}
						case 'Q': {
							closesocket(clientSocket);
							std::cout << "Connection closed with client." << std::endl;
							goto end_loop;
						}
						default: {
							std::cout << "Unknown command." << std::endl;

							char requestBuffer[1] = {'U'};
							int sbyteCount = send(clientSocket, requestBuffer , 1, 0);
							if (sbyteCount == SOCKET_ERROR) {
								std::cout << "Server send error: " << WSAGetLastError() << std::endl;
								goto end_loop;
							}
						}
					}
                } end_loop:;
                std::cout << "Task executed!" << std::endl;
            });
            pool.enqueue(task);
        }
    }
}



