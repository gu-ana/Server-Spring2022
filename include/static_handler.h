#ifndef STATICHANDLER_H 
#define STATICHANDLER_H 

#include "request_handler.h"

class StaticHandler: public RequestHandler {
    public:
        int handle_request(http::response<http::string_body>& httpResponse);
        std::string longest_prefix_match(std::map<std::string, std::string> map, std::string target);
        void set_map(std::map<std::string, std::string> map);
    private:
        std::map<std::string, std::string> map_;
};

#endif
