//
// Created by mkoro on 15.04.2025.
//
#include <iostream>
#include <map>
#include <functional>
#include "BasicController.h"
#include "../DTO/Response.h"
#include "../DTO/Request.h"
#include "../Models/StaticModel.h"

#ifndef STATICCONTROLLER_H
#define STATICCONTROLLER_H

class StaticController: public BasicController {
private:
    StaticModel staticModel;
    std::map<std::string, std::function<void(Request&, Response&)>> methodMap;

public:
    StaticController();
    void returnStatic(Request& request, Response& response);
    void callMethod(Request& request, Response& response) override;
};



#endif //STATICCONTROLLER_H
