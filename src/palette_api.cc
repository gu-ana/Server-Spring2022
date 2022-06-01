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

// Parse string containing html for a specified palette
std::vector<std::string> parse_html(std::string html)
{
    std::string palette = "Classy Palette";
    std::string color_div = "<div class='color' style='background:";
    std::size_t find_start_palette = html.find("Classy Palette");
    std::size_t find_end_palette = html.find("<div class='color-palette js-sr-palette'>", find_start_palette);

    std::vector<std::string> colors;

    // Search through html for all colors associated with palette
    if (find_start_palette != std::string::npos && find_end_palette != std::string::npos)
    {
        std::size_t find_color = find_start_palette;

        // Stop finding colors when done with current palette
        while ((find_color = html.find(color_div, find_color)) != std::string::npos && find_color < find_end_palette)
        {
            colors.push_back(html.substr(find_color + color_div.size(), HEX_LENGTH));
            find_color += color_div.size();
        }
    }

    return colors;
}

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
        std::string hex_div = "\"selectText('.hex');\">";
        std::size_t found = os.str().find(hex_div);

        if (found != std::string::npos)
        {
            std::string color = os.str().substr(found + hex_div.size(), HEX_LENGTH);

            // Extract palette given one color
            std::ostringstream url2;
            url2 << "https://mycolor.space/?hex=%23" << color.substr(1) << "&sub=1";

            std::ostringstream os2;
            os2 << Url(url2.str());

            colors = parse_html(os2.str());
        }
        else
        {
            LOG(error) << "No colors found for " << word << "\n";
            return colors;
        }
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