// описывает структуру, которая будет использоваться в последующей отрисовке векторов 
#pragma once
#include <string>
#include <vector>

namespace fbsvg {

struct WithRef {
    std::string var; 
};

struct EventPort {
    std::string name;
    std::string type;
    std::vector<WithRef> withs;
};


struct VarDecl {
    std::string name;
    std::string type;
};


struct FBModel {
    std::string name;
    std::string version;
    std::string author;

    std::vector<EventPort> eventInputs;
    std::vector<EventPort> eventOutputs;
    std::vector<VarDecl> inputVars;
    std::vector<VarDecl> outputVars;
};

} // namespace fbsvg
