// сначала парсит файл, затем преобразует в векторы, переводит векторы в пнг и на выходе создает файл .png
#include "fb_parser.hpp"
#include "svg_renderer.hpp"
#include "png_renderer.hpp"
#include <iostream>

int main(int argc, char **argv) {
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

      
    fbsvg::FBModel model = fbsvg::FBParser::parse_from_file(inputFile);

    std::string svg = fbsvg::SVGRenderer::render(model);

    fbsvg::PNGRenderer::render_from_svg(svg, outputFile);

    std::cout << "Файл формата .png сгенерирован: " << outputFile << "\n";



    return 0;
}
