// генерирует svg файл, рисует контур блока, входы, выходы, надписи и остальное оформление
#include <fbsvg/svg_renderer.hpp>
#include <sstream>
#include <algorithm>


static std::string xml_escape(const std::string &s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default: out += c;
        }
    }
    return out;
}

namespace fbsvg {

std::string SVGRenderer::render(const FBModel &m, const Layout &L) {
    int evInCount = (int)m.eventInputs.size();
    int evOutCount = (int)m.eventOutputs.size();
    int dataInCount = (int)m.inputVars.size();
    int dataOutCount = (int)m.outputVars.size();

    int rowsTop = std::max(evInCount, evOutCount);
    int rowsBottom = std::max(dataInCount, dataOutCount);
    int bodyRows = std::max(rowsTop, rowsBottom);

    int minBodyWidth = 2 * (L.leftMargin + 80) + L.columnGap;
    int width = std::max(L.minWidth, minBodyWidth);
    int height = std::max(L.minHeight, L.headerHeight + std::max(1, bodyRows) * L.portRowHeight + L.leftMargin);

    std::ostringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
       << "width=\"" << width << "\" height=\"" << height << "\">\n";

    // Стили
    ss << "<defs>\n";
    ss << "<style type=\"text/css\"><![CDATA[\n";
    ss << " .fb-title { font-family: Arial, Helvetica, sans-serif; font-weight: bold; font-size: " << L.nameFontSize << "px; }\n";
    ss << " .fb-port { font-family: Arial, Helvetica, sans-serif; font-size: " << L.portFontSize << "px; }\n";
    ss << " .fb-type { font-style: italic; font-family: Arial, Helvetica, sans-serif; font-size: " << (L.portFontSize - 1) << "px; }\n";
    ss << " .fb-line { stroke:#000; stroke-width:1; }\n";
    ss << " .fb-node { stroke:#000; fill:none; stroke-width:1; }\n";
    ss << "]]></style>\n";
    ss << "</defs>\n";

    // Масштабирование
    const double originalWidth = 60.0;
    const double originalHeight = 80.0;
    double scaleX = (double)width / originalWidth;
    double scaleY = (double)height / originalHeight;
    double scale = std::min(scaleX, scaleY);
    double offsetX = (width - originalWidth * scale) / 2.0;
    double offsetY = (height - originalHeight * scale) / 2.0;

    // Контур блока по стандарту
    ss << "<g transform=\"translate(" << offsetX << "," << offsetY << ") scale(" << scale << ")\">\n";
    ss << "  <path d=\"M1,3.13v10.49c0,1.17.95,2.13,2.13,2.13h4.82v21.69H3.13c-1.17,0-2.13.95-2.13,2.13v36.85c0,1.17.95,2.13,2.13,2.13h55.13c1.17,0,2.13-.95,2.13-2.13v-36.28c0-1.17-.95-2.13-2.13-2.13h-4.82V15.31h4.82c1.17,0,2.13-.95,2.13-2.13V3.13c0-1.17-.95-2.13-2.13-2.13H3.13c-1.17,0-2.13.95-2.13,2.13Z\" fill=\"#ffffff\" stroke=\"#222\" stroke-width=\"1.0\"/>\n";
    ss << "</g>\n";

    // REQ/CNF
    double topY = offsetY + 10 * scale;
    ss << "<text x=\"" << (offsetX + 3.0 * scale) << "\" y=\"" << topY << "\" class=\"fb-port\" fill=\"#000\">REQ</text>\n";
    ss << "<text x=\"" << (offsetX + (originalWidth - 13.0) * scale) << "\" y=\"" << topY << "\" class=\"fb-port\" fill=\"#000\">CNF</text>\n";

    // Типы событий на вход + Оформление 
    double leftTopSquareCX = 0, leftTopSquareBottomY = 0;
    if (!m.eventInputs.empty()) {
        double tx = offsetX - 10.0 * scale;

        ss << "<text x=\"" << tx << "\" y=\"" << topY
           << "\" class=\"fb-type\" text-anchor=\"end\" fill=\"#000\">"
           << xml_escape(m.eventInputs[0].type) << "</text>\n";

        double y = topY - 3;
        double xBlock = offsetX + 2;


        ss << "<line class=\"fb-line\" x1=\"" << tx << "\" y1=\"" << y
           << "\" x2=\"" << xBlock << "\" y2=\"" << y << "\" />\n";


        double sqX = xBlock - 18;
        double sqY = y - 7;
        ss << "<rect class=\"fb-node\" x=\"" << sqX
           << "\" y=\"" << sqY
           << "\" width=\"14\" height=\"14\" />\n";

        leftTopSquareCX = sqX + 7;       
        leftTopSquareBottomY = sqY + 14; 
    }

    // Типы событий на выход + Оформление
    double rightTopSquareCX = 0, rightTopSquareBottomY = 0;
    if (!m.eventOutputs.empty()) {
        double tx = offsetX + originalWidth * scale + 10.0 * scale;

        ss << "<text x=\"" << tx << "\" y=\"" << topY
           << "\" class=\"fb-type\" fill=\"#000\">"
           << xml_escape(m.eventOutputs[0].type) << "</text>\n";

        double y = topY - 4;
        double xBlock = offsetX + originalWidth * scale;

        ss << "<line class=\"fb-line\" x1=\"" << xBlock
           << "\" y1=\"" << y
           << "\" x2=\"" << tx << "\" y2=\"" << y << "\" />\n";

        double sqX = xBlock + 8;
        double sqY = y - 7;

        ss << "<rect class=\"fb-node\" x=\"" << sqX
           << "\" y=\"" << sqY
           << "\" width=\"14\" height=\"14\" />\n";
   
        rightTopSquareCX = sqX + 7;
        rightTopSquareBottomY = sqY + 14;
    }

    // Название блока
    double titleX = offsetX + originalWidth / 2.0 * scale;
    double titleY = offsetY + 25.0 * scale;
    ss << "<text x=\"" << titleX << "\" y=\"" << titleY << "\" text-anchor=\"middle\" class=\"fb-title\" fill=\"#000\">"
       << xml_escape(m.name.empty() ? "FB" : m.name) << "</text>\n";

    // Нижние порты
    double dataStartY = offsetY + 50.0 * scale;
    double dataGap = 12.0 * scale;
    double inputX = offsetX - 4 * scale;
    double outputX = offsetX + originalWidth * scale;

    double leftBottomSquareTopY = 0;
    double rightBottomSquareTopY = 0;

    // Входы
    for (size_t i = 0; i < m.inputVars.size(); ++i) {
        double y = dataStartY + i * dataGap;

        double nameX = inputX + 8.0 * scale;
        double typeX = inputX - 10.0 * scale;

        ss << "<text x=\"" << nameX << "\" y=\"" << (y + 5.0 * scale)
           << "\" class=\"fb-port\" fill=\"#000\">"
           << xml_escape(m.inputVars[i].name) << "</text>\n";

        ss << "<text x=\"" << typeX << "\" y=\"" << (y + 5.0 * scale)
           << "\" class=\"fb-type\" text-anchor=\"end\" fill=\"#000\">"
           << xml_escape(m.inputVars[i].type) << "</text>\n";

        double yLine = y + 4.0 * scale;
        double xBlock = offsetX + 4;

        ss << "<line class=\"fb-line\" x1=\"" << typeX << "\" y1=\"" << yLine
           << "\" x2=\"" << xBlock << "\" y2=\"" << yLine << "\" />\n";

        double sqX = xBlock - 20;
        double sqY = yLine - 7;

        ss << "<rect class=\"fb-node\" x=\"" << sqX
           << "\" y=\"" << sqY
           << "\" width=\"14\" height=\"14\" />\n";

        if (i == m.inputVars.size() - 1)
            leftBottomSquareTopY = sqY;
    }

    // Выходы
    double outY = dataStartY;
    double outNameX = outputX - 13.0 * scale;

    ss << "<text x=\"" << outNameX << "\" y=\"" << (outY + 5.0 * scale)
       << "\" class=\"fb-port\" fill=\"#000\">OUT</text>\n";

    if (!m.outputVars.empty()) {
        double typeX = outputX + 20.0 * scale;

        ss << "<text x=\"" << typeX << "\" y=\"" << (outY + 5.0 * scale)
           << "\" class=\"fb-type\" fill=\"#000\">"
           << xml_escape(m.outputVars[0].type) << "</text>\n";

        double yLine = outY + 4.0 * scale;
        double xBlock = offsetX + originalWidth * scale;

        ss << "<line class=\"fb-line\" x1=\"" << xBlock
           << "\" y1=\"" << yLine
           << "\" x2=\"" << typeX << "\" y2=\"" << yLine << "\" />\n";

        double sqX = xBlock + 8;
        double sqY = yLine - 7;

        ss << "<rect class=\"fb-node\" x=\"" << sqX
           << "\" y=\"" << sqY
           << "\" width=\"14\" height=\"14\" />\n";

        rightBottomSquareTopY = sqY;
    }

    // Левые линии
    if (leftTopSquareCX != 0 && leftBottomSquareTopY != 0) {
        ss << "<line class=\"fb-line\" x1=\"" << leftTopSquareCX
           << "\" y1=\"" << leftTopSquareBottomY
           << "\" x2=\"" << leftTopSquareCX
           << "\" y2=\"" << leftBottomSquareTopY
           << "\" />\n";
    }

    // Правые линии
    if (rightTopSquareCX != 0 && rightBottomSquareTopY != 0) {
        ss << "<line class=\"fb-line\" x1=\"" << rightTopSquareCX
           << "\" y1=\"" << rightTopSquareBottomY
           << "\" x2=\"" << rightTopSquareCX
           << "\" y2=\"" << rightBottomSquareTopY
           << "\" />\n";
    }

    ss << "</svg>\n";
    return ss.str();
}

} // namespace fbsvg
