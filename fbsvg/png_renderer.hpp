#pragma once
#include <string>

namespace fbsvg {

class PNGRenderer {
public:
    // Принимает строку SVG и путь для сохранения PNG
    static void render_from_svg(const std::string &svgContent, const std::string &filename);
};

}
