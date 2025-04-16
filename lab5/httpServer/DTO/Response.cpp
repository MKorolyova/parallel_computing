#include "iostream"
#include "Response.h"

    void Response::setContentType(std::string contentType){
      this->contentType = "Content-Type: " + contentType + "\r\n\r\n";
    }

    void Response::setStatusOk(){
      this->status = "200 OK\r\n";
    }

    void Response::setStatusNotFound(){
      this->status = "404 Not Found\r\n";
      this->contentType = "Content-Type: text/html\r\n\r\n";
      this->contentLength = "Content-Length: 13\r\n";
      this->content = "404 Not Found";
    }

    void Response::setContent(std::string content){
      this->contentLength = "Content-Length: " + std::to_string(content.size()) + "\r\n";
      this->content = content;
    }

    std::string Response::encode(){
      return version + status + contentLength + contentType + content;
    }
