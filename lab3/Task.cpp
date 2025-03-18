#include "Task.h"


Task::Task() {
    this->task = nullptr;
}

Task::Task(const int id, const int processTime, const std::function<void()>& task) {
    this->id = id;
    this->processTime = processTime;
    this->task = task;
}

bool Task::isValid() const {
    return task != nullptr;
}

