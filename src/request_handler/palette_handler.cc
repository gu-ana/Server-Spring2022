#include "request_handler/palette_handler.h"
#include "logger.h"
#include <algorithm>

const std::string WHITESPACE = " \n";
 
std::string trim_whitespace(const std::string &s) 
{
    std::string s_trimmed;

    // remove whitespaces at start of string
    size_t start = s.find_first_not_of(WHITESPACE);
    s_trimmed = (start == std::string::npos) ? "" : s.substr(start);
    
    // remove whitespace at end of string
    size_t end = s.find_last_not_of(WHITESPACE);
    s_trimmed = (end == std::string::npos) ? "" : s.substr(0, end + 1);
    
    return s_trimmed;
}

bool valid_input(const std::string &input)
{
    // if string is empty, it is considered invalid
    if (input.empty())
    {
        return false;
    }
    
    // check that at all characters are alphanumeric or spaces
    auto it = std::find_if_not(input.begin(), input.end(), [](char const &c) 
    {
        return isalnum(c) || isspace(c);
    });
 
    // will return true if no non-alphanumeric or non-space characters were found
    return it == input.end();
}

// this can take inputs in the future once the api calling has been implemented
std::string generate_json_response(std::vector<std::string> colors, std::vector<std::string> colorNames)
{
    std::stringstream jsonResponse;

    jsonResponse << "{\n";
    
    // Palette
    jsonResponse << "\"Palette\": {\n";

    for (int i = 0; i < colors.size(); i++) 
    {
        jsonResponse << "\"Color" << i << "\":\n";
        jsonResponse << "{\n";
        jsonResponse << "\"hex\":" << "\"" << colors[i] << "\", ";
        jsonResponse << "\"name\":" << "\"" << colorNames[i] << "\",\n";
        jsonResponse << "},\n";
    }
    jsonResponse << "},\n";

    jsonResponse << "}\n";

    return jsonResponse.str();
}

bool PaletteHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::string target = std::string(httpRequest.target()).c_str();
    std::string method = httpRequest.method_string().to_string();
    
    LOG(info) << "Client requested " << method << " " << target << '\n';

    // check for POST method
    if (method != "POST") 
    {
        LOG(error) << "Invalid HTTP method, target only excepts POST\n";
        set_response(http::status::bad_request, "text/plain", "Invalid HTTP method for target\n", httpResponse);
        return false;
    }
    
    // extract and sanitize request body
    std::string body = std::string(httpRequest.body());
    body = trim_whitespace(body);
    if (!valid_input(body))
    {
        LOG(error) << "Invalid request body\n";
        set_response(http::status::bad_request, "text/html", "Request body can only contain letters, numbers, and spaces\n", httpResponse);
        return false;
    }

    std::vector<std::string> colors = PaletteAPI::get_colors(body);
    std::vector<std::string> colorNames = PaletteAPI::get_names(colors);
    std::string jsonBody = generate_json_response(colors, colorNames);

    // send response
    set_response(http::status::ok, "application/json", jsonBody, httpResponse);
    
    return true;
}

std::string PaletteHandler::getName()
{
    return std::string("PaletteHandler");
}