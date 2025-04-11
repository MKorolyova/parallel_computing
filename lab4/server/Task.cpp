#include "Task.h"

Task::Task() {
    this->task = nullptr;
}

Task::Task(const int id, const SOCKET clientSocket, const std::function<void()>& task) {
    this->id = id;
    this->task = task;
}

bool Task::isValid() const {
    return task != nullptr;
}