#include <iostream>
#include <map>
#include <functional>
#include <memory>
#include "StaticController.h"
#include "BasicController.h"
#include "iostream"
#include "../DTO/Response.h"
#include "../DTO/Request.h"
#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {

    public:
        std::map<std::string, BasicController*> controllerRegistry;
        Controller();
        void dispatch(Request& request, Response& response);
};

#endif //CONTROLLER_H
