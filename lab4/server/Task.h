#include <functional>
#include <winsock2.h>
#ifndef TASK_H
#define TASK_H


class Task {
public:

    int id{};
    std::function<void()> task;

    Task();
    Task(const int id, const SOCKET clientSocket, const std::function<void()>& task);
    bool isValid() const;

};

#endif //TASK_H
