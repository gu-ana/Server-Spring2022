// Source: https://github.com/jpbarrette/curlpp/tree/master/examples
#include "palette_api.h"
#include "logger.h"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#define HEX_START 22
#define HEX_LENGTH 7

using namespace curlpp::options;

// TODO: currently only returns one color
std::vector<std::string> PaletteAPI::get_colors(std::string word)
{
    std::vector<std::string> colors;
    try
    {
        curlpp::Cleanup myCleanup;

        // Handle multiple words
        std::replace(word.begin(), word.end(), ' ', '+');

        std::ostringstream url;
        url << "https://alexbeals.com/projects/colorize/search.php?q=" << word;

        std::ostringstream os;
        os << Url(url.str());

        // Extract hex value from web page
        std::size_t found = os.str().find("\"selectText('.hex');\">");
        std::string color = os.str().substr(found + HEX_START, HEX_LENGTH);

        colors.push_back(color);
    }
    catch (curlpp::RuntimeError &e)
    {
        LOG(error) << "While getting color of " << word << " : " << e.what() << "\n";
    }
    catch (curlpp::LogicError &e)
    {
        LOG(error) << "While getting color of " << word << " : " << e.what() << "\n";
    }

    return colors;
}

// TODO: dummy implementation for now
std::vector<std::string> PaletteAPI::get_names(std::vector<std::string> hexColors)
{
    return hexColors;
}