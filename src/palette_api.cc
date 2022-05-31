// Source: https://github.com/jpbarrette/curlpp/tree/master/examples
#include "palette_api.h"
#include "logger.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <boost/property_tree/json_parser.hpp>

#define HEX_START 22
#define HEX_LENGTH 7

using namespace curlpp::options;
namespace pt = boost::property_tree;

// Source: https://stackoverflow.com/questions/43725302/reading-json-with-boost-property-tree
std::string parse_json(std::string json)
{
    std::stringstream ss;
    ss << json;

    pt::ptree json_tree;
    pt::read_json(ss, json_tree);
    const pt::ptree& name = json_tree.get_child("name");
    return name.get<std::string>("value");
}

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

// TODO: currently outputs json, will need to be parsed
std::vector<std::string> PaletteAPI::get_names(std::vector<std::string> hexColors)
{
    std::vector<std::string> colorNames;
    try
    {
        curlpp::Cleanup myCleanup;

        for (int i = 0; i < hexColors.size(); i++)
        {
            std::ostringstream url;
            url << "https://www.thecolorapi.com/id?hex=" << hexColors[i].substr(1);

            std::ostringstream os;
            os << Url(url.str());

            // Extract name from web page
            std::string name = parse_json(os.str());
            colorNames.push_back(name);
        }
    }
    catch (curlpp::RuntimeError &e)
    {
        LOG(error) << "While getting name of colors: " << e.what() << "\n";
    }
    catch (curlpp::LogicError &e)
    {
        LOG(error) << "While getting name of colors: " << e.what() << "\n";
    }

    return colorNames;
}