#ifndef APIHANDLER_H 
#define APIHANDLER_H 

#include "request_handler/request_handler.h"
#include "file_system.h"

class ApiHandler: public RequestHandler 
{
    public:
        ApiHandler(std::string location, std::string data_path, std::shared_ptr<FileSystem> fs);
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
    private:
        std::string data_path_;
        std::string location_;
        std::shared_ptr<FileSystem> fs_;
        bool handlePost(std::vector<std::string> uri, http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
        bool handleGet(std::vector<std::string> uri, http::response<http::string_body>& httpResponse); 
        bool handleDelete(std::vector<std::string> uri, http::response<http::string_body>& httpResponse); 
        bool handlePut(std::vector<std::string> uri, http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
};

#endif
