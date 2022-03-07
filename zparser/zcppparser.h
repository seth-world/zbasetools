#pragma once

#include "zcpptokenizer.h"
#include "type.h"
#include "FunctionDefinition.hpp"
#include "Statement.hpp"
#include <optional>
#include <string>
#include <map>


namespace zparsernmsp {

using namespace std;
using namespace zbs;

class ZCppField  {
  ZTokentype_type Type;
  ZCppToken*      Descriptor=nullptr;
  ZCppToken*      Initializator=nullptr;
  ZCppToken*      Whole=nullptr; /* array of all tokens related to field */
};

class ZCppArgument  {
  ZTokentype_type Type;
  ZCppToken*      Descriptor=nullptr;
  ZCppToken*      DefaultValue=nullptr;
};

class ZCppMethod : ZArray<ZCppArgument> {
  ZTokentype_type ReturnType;
  ZCppToken*      Descriptor=nullptr;
};


class ZCppObject : public ZArray<ZCppField> {
  ZTokentype_type     Type;
  ZArray<ZCppMethod>  Methods;
};



class ZCppParser {
public:
        ZCppParser();

        enum ZCppPaserOptions : uint32_t {
          ZCPO_Nothing  =   0,
          ZCPO_Include  = 0x01,  /* search for include files */
          ZCPO_Objects  = 0x02, /* parse objects : structs and classes */
          ZCPO_Methods  = 0x04  /* parse class and struct methods including contructors and destructors */
        };

        void parse(ZCppTokenizer &tokens);

        void debugPrint() const;

 //       map<string, FunctionDefinition> GetFunctions() const { return mFunctions; }

        void setIncludePath(const utf8VaryingString& pIncludePath);

private:


  void loadIncludeFiles(ZCppTokenizer &tokens);
  void parseTokens(ZCppTokenizer &tokens);

  optional<ZCppObject> getObject();

  optional<ZCppField> getField();

  optional<ZCppMethod> getMethod();

  optional<ZCppArgument> getMethodArg();

#ifdef __COMMENT__

        //! Empty string means match any identifier.
        optional<Token> expectIdentifier(const string& name = string());

        //! Empty string means match any operator.
        optional<Token> expectOperator(const string& name = string());

        bool expectFunctionDefinition();

        vector<Token>::iterator mCurrentToken;
        vector<Token>::iterator mEndToken;
        map<string, Type> mTypes;
        map<string, FunctionDefinition> mFunctions;

        optional<vector<Statement>> parseFunctionBody();

        optional<Statement> expectOneValue();

        optional<Statement> expectWhileLoop();

        optional<Statement> expectStatement();

        optional<Statement> expectVariableDeclaration();

        optional<Statement> expectFunctionCall();

        optional <Statement> expectExpression();

        size_t operatorPrecedence(const string &operatorName);

        Statement *findRightmostStatement(Statement *lhs, size_t rhsPrecedence);
#endif        __COMMENT__
};

}
