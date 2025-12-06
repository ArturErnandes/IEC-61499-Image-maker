#pragma once
#include "fb_model.hpp"
#include <string>

namespace pugi { class xml_document; } 

namespace fbsvg {

class FBParser {
public:
    static FBModel parse_from_file(const std::string &filename);
};

} // namespace fbsvg
