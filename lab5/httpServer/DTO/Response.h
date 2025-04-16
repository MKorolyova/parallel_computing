#ifndef RESPONSE_H
#define RESPONSE_H

class Response {
    private:
        std::string version = "HTTP/1.1";
        std::string contentLength;
        std::string contentType;
        std::string status;
        std::string content;
    public:
        void setContentType(std::string contentType);
        void setStatusOk();
        void setStatusNotFound();
        void setContent(std::string content);

        std::string encode();
};

#endif //RESPONSE_H
