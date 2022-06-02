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

// Call API to get palette given a color
std::vector<std::string> get_palette(std::string color)
{
    std::vector<std::string> palette;

    try
    {
        curlpp::Cleanup myCleanup;

        std::ostringstream url;
        url << "https://mycolor.space/?hex=%23" << color << "&sub=1";

        std::ostringstream os;
        os << Url(url.str());

        palette = parse_html(os.str());
    }
    catch (curlpp::RuntimeError &e)
    {
        LOG(error) << "While getting palette from " << color << ": " << e.what() << "\n";
    }
    catch (curlpp::LogicError &e)
    {
        LOG(error) << "While getting palette from " << color << ": " << e.what() << "\n";
    }

    return palette;
}

// Call API to get color given a word
std::string get_color(std::string word)
{
    std::string color;

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
            color = os.str().substr(found + hex_div.size() + 1, HEX_LENGTH - 1);
        }
        else
        {
            LOG(error) << "No colors found for " << word << "\n";
        }
    }
    catch (curlpp::RuntimeError &e)
    {
        LOG(error) << "While getting color of " << word << ": " << e.what() << "\n";
    }
    catch (curlpp::LogicError &e)
    {
        LOG(error) << "While getting color of " << word << ": " << e.what() << "\n";
    }

    return color;
}

std::vector<std::string> PaletteAPI::get_colors(std::string word)
{
    // Get color from word to color API
    std::string color = get_color(word);

    // Get palette from color, if it existed
    if (color == "")
    {
        return {};
    }
    else
    {
        return get_palette(color);
    }
}

// Call API to get names of colors
std::vector<std::string> PaletteAPI::get_names(std::vector<std::string> hex_colors)
{
    std::vector<std::string> colorNames;
    try
    {
        curlpp::Cleanup myCleanup;

        for (int i = 0; i < hex_colors.size(); i++)
        {
            std::ostringstream url;
            url << "https://www.thecolorapi.com/id?hex=" << hex_colors[i].substr(1);

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