#include "iostream"
#include <map>
#ifndef REQUEST_H
#define REQUEST_H

class Request {

  private:

    std::string method;
    std::string url;
    std::string requestType;
    std::string controllerName;
    std::string actionName;
    std::map<std::string, std::string> parameters;
    std::string version;

  public:

    Request( std::string &headerFirstLine);
    std::string getControllerName();
    std::string getActionName();
    std::map<std::string, std::string> getParameters();
    void print() const;

};


#endif //REQUEST_H
