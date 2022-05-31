#ifndef PALETTEAPI_H
#define PALETTEAPI_H

#include <string>
#include <vector>

namespace PaletteAPI {
    std::vector<std::string> get_colors(std::string word);
    std::vector<std::string> get_names(std::vector<std::string> hexColors);
};

#endif