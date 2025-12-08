#pragma once
#include <fbsvg/fb_model.hpp>
#include <fbsvg/layout.hpp>
#include <string>

namespace fbsvg {

class SVGRenderer {
public:
    static std::string render(const FBModel &m, const Layout &layout = Layout());
};



} // namespace fbsvg
