#pragma once

#include <string>
#include <vector>

namespace zparsernmsp {

    using namespace std;

    enum BUILTIN_TYPE {
        VOID,
        INT8,
        UINT8,
        INT32,
        UINT32,
        DOUBLE,
        FLOAT,
        STRUCT,
        CLASS
    };

    class Type {
    public:
        Type(const string &name = "", enum BUILTIN_TYPE type = VOID)
                : mName(name), mType(type) {}

        string mName;
        enum BUILTIN_TYPE mType;
        vector<Type> mFields; // for STRUCT only.
    };

}
