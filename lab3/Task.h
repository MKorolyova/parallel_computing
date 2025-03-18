#include <functional>
#ifndef TASK_H
#define TASK_H


class Task {
public:

    int id{};
    int processTime{};
    std::function<void()> task;

    Task();
    Task(const int id, const int processTime, const std::function<void()>& task);
    bool isValid() const;

};

#endif //TASK_H
