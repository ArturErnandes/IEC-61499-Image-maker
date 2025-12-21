// сначала парсит файл на входе, затем преобразует его в векторы и на выходе создает файл .svg
#include <iostream>
#include <fstream>
#include <string>

#include "fbsvg/fb_parser.hpp"
#include "fbsvg/svg_renderer.hpp"

int main(int argc, char** argv){

    const std::string input_path  = argv[1];
    const std::string output_path = argv[2];


    fbsvg::FBModel model = fbsvg::FBParser::parse_from_file(input_path);

    std::string svg = fbsvg::SVGRenderer::render(model);

    std::ofstream out(output_path);

    out << svg;
    out.close();

    std::cout << "Файл формата .svg сгенерирован: " << output_path << "\n";



    return 0;
}
