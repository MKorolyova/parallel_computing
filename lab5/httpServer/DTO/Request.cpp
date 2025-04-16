#include "Request.h"
#include <sstream>

// http://data/ControllerName/ActionName/someDataOne/one/someDataTwo/two
// http://some/static/file.html

    Request::Request( std::string &headerFirstLine){

        std::istringstream headerFirstLineStream(headerFirstLine);
        headerFirstLineStream >> this->method >> this->url >> this->version;

        std::istringstream urlStream(url);
        std::string segment;

        std::getline(urlStream, segment, '/');
        std::getline(urlStream, segment, '/');

        if (segment == "data") {
            this->requestType = segment;

            if (std::getline(urlStream, segment, '/'))
                this->controllerName = segment;

            if (std::getline(urlStream, segment, '/'))
                this->actionName = segment;

            std::string paramName, paramValue;
            while (std::getline(urlStream, paramName, '/') && std::getline(urlStream, paramValue, '/')) {
                this->parameters[paramName] = paramValue;
            }

        } else {
            requestType = "Static";
            controllerName = "StaticController";
            actionName = "returnStatic";
            if(url == "/"){
                parameters["path"] = "index.html";
            }else{
                parameters["path"] = url;
            }
        }
    }

    std::string Request::getControllerName(){
        return controllerName;
    }

    std::string Request::getActionName(){
      return actionName;
    }

    std::map<std::string, std::string> Request::getParameters(){
      return parameters;
    }

void Request::print() const {
        std::cout << "=== Request Info ===" << std::endl;
        std::cout << "Method: " << method << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Version: " << version << std::endl;
        std::cout << "Request Type: " << requestType << std::endl;
        std::cout << "Controller: " << controllerName << std::endl;
        std::cout << "Action: " << actionName << std::endl;

        std::cout << "Parameters:" << std::endl;
        for (const auto& [key, value] : parameters) {
            std::cout << "  " << key << ": " << value << std::endl;
        }

        std::cout << "=====================" << std::endl;
    }


