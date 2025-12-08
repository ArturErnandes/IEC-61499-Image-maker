// сначала парсит файл, затем преобразует в векторы, переводит векторы в пнг и на выходе создает файл .png
#include <fbsvg/fb_parser.hpp>
#include <fbsvg/svg_renderer.hpp>
#include <fbsvg/png_renderer.hpp>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Пример ввода: " << argv[0] << " input.fbt output.png\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    try {
        fbsvg::FBModel model = fbsvg::FBParser::parse_from_file(inputFile);

        std::string svg = fbsvg::SVGRenderer::render(model);

        fbsvg::PNGRenderer::render_from_svg(svg, outputFile);

        std::cout << "Файл формата .png сгенерирован: " << outputFile << "\n";
    } catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
