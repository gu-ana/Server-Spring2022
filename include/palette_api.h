#ifndef PALETTEAPI_H
#define PALETTEAPI_H

#include <string>
#include <vector>

// Helper functions
std::string parse_json(std::string json);
std::vector<std::string> parse_html(std::string html);
std::vector<std::string> get_palette(std::string color);
std::string get_color(std::string word);

class PaletteAPI 
{
	public:
    	static std::vector<std::string> get_colors(std::string word);
    	static std::vector<std::string> get_names(std::vector<std::string> hex_colors);
};

#endif