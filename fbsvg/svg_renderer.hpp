#pragma once
#include "fb_model.hpp"
#include "layout.hpp"
#include <string>

namespace fbsvg {

class SVGRenderer {
public:
    static std::string render(const FBModel &m, const Layout &layout = Layout());
};



} // namespace fbsvg
