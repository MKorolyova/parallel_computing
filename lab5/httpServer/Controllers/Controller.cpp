#include "Controller.h"

    Controller::Controller() {
        controllerRegistry["StaticController"] = new StaticController();
        // other controllers
    }

    void  Controller::dispatch(Request& request, Response& response) {

        if (controllerRegistry.count(request.getControllerName())) {
            controllerRegistry[request.getControllerName()]->callMethod(request, response);
        } else {
            response.setStatusNotFound();
        }
    }
