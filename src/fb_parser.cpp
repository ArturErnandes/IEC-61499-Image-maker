// парсит xml, возвращает  читаемую информацию с xml 
#include "fb_parser.hpp"
#include "pugixml.hpp"
#include <stdexcept>

using namespace fbsvg;

FBModel FBParser::parse_from_file(const std::string &filename) {
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(filename.c_str());
    if (!res) throw std::runtime_error(std::string("XML parse error: ") + res.description());

    pugi::xml_node fbType = doc.child("FBType");
    if (!fbType) throw std::runtime_error("Missing FBType root node");

    FBModel m;
    m.name = fbType.attribute("Name").as_string();
    pugi::xml_node versionInfo = fbType.child("VersionInfo");
    if (versionInfo) {
        m.version = versionInfo.attribute("Version").as_string();
        m.author = versionInfo.attribute("Author").as_string();
    }

    pugi::xml_node interfaceList = fbType.child("InterfaceList");
    if (!interfaceList) return m; 

    // EventInputs
    pugi::xml_node eventInputs = interfaceList.child("EventInputs");
    if (eventInputs) {
        for (pugi::xml_node ev : eventInputs.children("Event")) {
            EventPort ep;
            ep.name = ev.attribute("Name").as_string();
            ep.type = ev.attribute("Type").as_string();         
            for (pugi::xml_node withNode : ev.children("With")) {
                WithRef w; w.var = withNode.attribute("Var").as_string();
                ep.withs.push_back(w);
            }
            m.eventInputs.push_back(ep);
        }
    }

    // EventOutputs
    pugi::xml_node eventOutputs = interfaceList.child("EventOutputs");
    if (eventOutputs) {
        for (pugi::xml_node ev : eventOutputs.children("Event")) {
            EventPort ep;
            ep.name = ev.attribute("Name").as_string();
            ep.type = ev.attribute("Type").as_string();     
            for (pugi::xml_node withNode : ev.children("With")) {
                WithRef w; w.var = withNode.attribute("Var").as_string();
                ep.withs.push_back(w);
            }
            m.eventOutputs.push_back(ep);
        }
    }

    // InputVars
    pugi::xml_node inputVars = interfaceList.child("InputVars");
    if (inputVars) {
        for (pugi::xml_node v : inputVars.children("VarDeclaration")) {
            VarDecl vd;
            vd.name = v.attribute("Name").as_string();
            vd.type = v.attribute("Type").as_string();

            m.inputVars.push_back(vd);
        }
    }

    // OutputVars
    pugi::xml_node outputVars = interfaceList.child("OutputVars");
    if (outputVars) {
        for (pugi::xml_node v : outputVars.children("VarDeclaration")) {
            VarDecl vd;
            vd.name = v.attribute("Name").as_string();
            vd.type = v.attribute("Type").as_string();
            m.outputVars.push_back(vd);
        }
    }


    return m;
}
