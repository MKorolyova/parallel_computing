#include <iostream>
#include <cstring>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ThreadPool.h"
#include "Task.h"
#include "DTO/Response.h"
#include "DTO/Request.h"
#include "Controllers/Controller.h"

SOCKET  setUpSocket( int port, std::string ip ) {
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
    service.sin_addr.s_addr = inet_addr(ip.c_str());  // Replace with your desired IP address
    service.sin_port = htons(port);  // Choose a port number

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

    ThreadPool pool;
    int taskId = 0;
    SOCKET serverSocket = setUpSocket(8888, "127.0.0.1");
    std::cout << "Server listening on port " << 8888 << "...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "accept failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            continue;
        }

        Task task(taskId++, clientSocket, [clientSocket] {

            while (true) {
                std::string requestData;
                char buffer[1];
                int bytesReceived;
                while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
                    requestData.append(buffer, bytesReceived);
                    // End of the first line - "\r\n" End of the request - "\r\n\r\n"
                    if (requestData.find("\r\n\r\n") != std::string::npos) {
                        break;
                    }
                }

                requestData = requestData.substr(0, requestData.find("\r\n"));


                if (bytesReceived <= 0) {
                    break;
                }

                std::cout << "Received command: " << requestData<< std::endl;
                Request request = Request(requestData);
                Response response;
                Controller controller;
                controller.dispatch(request, response);

                std::string dataToSend = response.encode();
                send(clientSocket, dataToSend.c_str(), dataToSend.size(), 0);



            }
        });
        pool.enqueue(task);
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}