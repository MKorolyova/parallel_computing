#include "StaticController.h"
#include <map>

    StaticController::StaticController() {
        methodMap["returnStatic"] = [this](Request& request, Response& response) { this->returnStatic(request, response);};
    }

    void StaticController::returnStatic(Request& request, Response& response) {
        std::map<std::string, std::string> parameters = request.getParameters();
        if(parameters.count("path")) {
            std::string content = staticModel.findFile(parameters["path"]);
            std::string contentType = staticModel.getContentType(parameters["path"]);
            if(content != ""){
                response.setStatusOk();
                response.setContentType(contentType);
                response.setContent(content);
                return;
            }
        }
        response.setStatusNotFound();

    }

    void StaticController::callMethod(Request& request, Response& response) {
        auto it = methodMap.find(request.getActionName());
        if (it != methodMap.end()) {
            it->second(request, response);
        } else {
            response.setStatusNotFound();
        }
    }

