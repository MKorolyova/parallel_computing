#include <fstream>
#include <sstream>
#include <filesystem>
#ifndef STATICMODEL_H
#define STATICMODEL_H

class StaticModel {
    public:
    std::string findFile(std::string path) {

        std::ifstream file("../static/" + path, std::ios::binary);

        if (!file.is_open()) {
            return "";
        }

        std::ostringstream contents;
        contents << file.rdbuf();
        file.close();
        return contents.str();
    }

    std::string getContentType(const std::string& path) {
        if (path.ends_with(".html") || path.ends_with(".htm")) return "text/html";
        if (path.ends_with(".css")) return "text/css";
        if (path.ends_with(".js")) return "application/javascript";
        if (path.ends_with(".json")) return "application/json";
        if (path.ends_with(".png")) return "image/png";
        if (path.ends_with(".jpg") || path.ends_with(".jpeg")) return "image/jpeg";
        if (path.ends_with(".gif")) return "image/gif";
        if (path.ends_with(".svg")) return "image/svg+xml";
        if (path.ends_with(".ico")) return "image/x-icon";
        return "application/octet-stream";
    }
};


#endif //STATICMODEL_H
