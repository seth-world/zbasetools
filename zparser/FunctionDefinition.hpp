#pragma once

#include "type.h"
#include "Statement.hpp"
#include <string>
#include <vector>

namespace zparsernmsp {

    using namespace std;

    class ParameterDefinition {
    public:
        string mName; // Empty string means no name given.
        Type mType;

        void debugPrint(size_t indent) const;
    };

    class FunctionDefinition {
    public:
        string mName;
        vector<ParameterDefinition> mParameters;
        vector<Statement> mStatements;

        void debugPrint() const;
    };

}
