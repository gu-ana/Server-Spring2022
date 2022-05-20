#include "sleep_handler.h"
#include "logger.h"

#define SLEEP_TIME 500

bool SleepHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::string target = std::string(httpRequest.target()).c_str();
    LOG(info) << "Client requested " << target << '\n';
    
    // blocks execution of this thread for SLEEP_TIME seconds
    std::chrono::milliseconds sleep_duration(SLEEP_TIME);
    std::this_thread::sleep_for(sleep_duration);
    set_response(http::status::ok, "text/plain", "Thread slept for " + std::to_string(SLEEP_TIME) + " seconds.\n", httpResponse);

    return true;
}
