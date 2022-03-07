#pragma once

#include <ztoolset/zarray.h>
#include <ztoolset/utfvaryingstring.h>
#include <ztoolset/zaierrors.h>
//#include <vector>
//#include <string>

namespace zparsernmsp {

    using namespace std;
    using namespace zbs;
/*
    enum TokenType {
        WHITESPACE, // No token ever has this type.
        IDENTIFIER,
        INTEGER_LITERAL,
        DOUBLE_LITERAL,
        FLOAT_LITERAL,
        LONGDOUBLE_LITERAL,
        STRING_LITERAL,
        OPERATOR,
        OPERATOR_OPENBRACKET,
        OPERATOR_CLOSEBRACKET,
        OPERATOR_CLOSEINSTRUCTION,
        OPERATOR_OPENPARENTHESIS,
        OPERATOR_CLOSEPARENTHESIS,
        STRING_ESCAPE_SEQUENCE,
        MAYBE_DOUBLE,
        MAYBE_COMMENT,
        MAYBE_ENDCOMMENT,
        COMMENT_DOUBLESLASH,
        COMMENT_SLASHSTAR,
        MAYBE_CLASS,
        CLASS_TOKEN,
        MAYBE_STRUCT,
        STRUCT_TOKEN,
        OTHER_TOKEN
    };
*/

    extern int Line,Column;
    typedef unsigned int ZTokentype_type;
    enum ZTokentype : ZTokentype_type {
      ZTTP_SPACE                  =        1,
      ZTTP_MAYBE_IDENTIFIER       =        2,
      ZTTP_IDENTIFIER             =        4,

      ZTTP_NUMERIC_LITERAL        =   0x1000,
      ZTTP_INTEGER_LITERAL        =   0x1100,
      ZTTP_MAYBE_DOUBLE_LITERAL   =   0x1201,
      ZTTP_DOUBLE_LITERAL         =   0x1200,
      ZTTP_FLOAT_LITERAL          =   0x1400,
      ZTTP_LONGDOUBLE_LITERAL     =   0x1800,

      ZTTP_STRING_LITERAL         =   0x2000,
      ZTTP_STRING_ESCAPE_SEQUENCE =   0x2001,

      ZTTP_MAYBE_COMMENT          =   0x2100,
      ZTTP_MAYBE_ENDCOMMENT       =   0x2101,
      ZTTP_COMMENT_DOUBLESLASH    =   0x2300,
      ZTTP_COMMENT_SLASHSTAR      =   0x2500,
      ZTTP_COMMENT_ENDSLASHSTAR   =   0x2900,

      ZTTP_OPERATOR               =   0x4000,

      ZTTP_OPERATOR_ARITHMETIC    =   0x4001,
      ZTTP_OPERATOR_LOGICAL       =   0x4002,
      ZTTP_OPERATOR_BITWISE       =   0x4003,  /* & and | */

      ZTTP_OPERATOR_OTHER         =   0x4004,

      ZTTP_OPERATOR_OPENBRACKET   =   0x4100,
      ZTTP_OPERATOR_CLOSEBRACKET  =   0x4200,
      ZTTP_OPERATOR_OPENPARENTHESIS = 0x4500,
      ZTTP_OPERATOR_CLOSEPARENTHESIS= 0x4600,
      ZTTP_OPERATOR_SEMICOLON     =   0x4800,
      ZTTP_OPERATOR_COLON         =   0x4900,
      ZTTP_OPERATOR_DOUBLECOLON   =   0x4901,

      ZTTP_SIMPLE_TOKEN           = 0x010000,
      ZTTP_MAYBE_CLASS            = 0x010100,
      ZTTP_CLASS_TOKEN            = 0x010500,
      ZTTP_CLASS_IDENTIFIER       = 0x010600,
      ZTTP_MAYBE_STRUCT           = 0x010200,
      ZTTP_STRUCT_IDENTIFIER      = 0x010300,
      ZTTP_STRUCT_TOKEN           = 0x010A00,

      ZTTP_DATA_TYPE              = 0x011000,
      ZTTP_MAYBE_DATA_TYPE        = 0x011001,

      ZTTP_CONST_TOKEN            = 0x100000,
      ZTTP_UNSIGNED_TOKEN         = 0x200000,
      ZTTP_REFERENCE_TOKEN        = 0x400000,
      ZTTP_POINTER_TOKEN          = 0x800000,

      ZTTP_DATA_TYPE_CONST        = ZTTP_DATA_TYPE|ZTTP_CONST_TOKEN,

/* atomic data types */

      ZTTP_CHAR                   = 0x011001,
      ZTTP_INTEGER                = ZTTP_CHAR + 1,
      ZTTP_LONG                   = ZTTP_INTEGER + 1,
      ZTTP_LONG_LONG              = ZTTP_LONG + 1,
      ZTTP_FLOAT                  = ZTTP_LONG_LONG + 1,
      ZTTP_DOUBLE                 = ZTTP_FLOAT + 1,
      ZTTP_LONG_DOUBLE            = ZTTP_DOUBLE + 1,

      ZTTP_INT8                   = 0x011101,
      ZTTP_INT16                   = ZTTP_INT8+1,
      ZTTP_INT32                   = ZTTP_INT16+1,
      ZTTP_INT64                   = ZTTP_INT32+1,

/* methods and functions */

      ZTTP_MAYBE_METHOD           = 0x012000,
      ZTTP_METHOD                 = 0x012001,
      ZTTP_METHOD_ARGUMENT        = 0x012010,

/* preprocessor clauses */

      ZTTP_MAYBE_PREPROC          = 0x020000,
      ZTTP_INCLUDE                = 0x020001,
      ZTTP_PRAGMA                 = 0x020002,
      ZTTP_IFDEF                  = 0x020004,
      ZTTP_IFNDEF                 = 0x020014,
      ZTTP_DEFINE                 = 0x020008,
      ZTTP_ENDIF                  = 0x020020,
      ZTTP_MAYBE_INCLUDE_FILE     = 0x020010,
      ZTTP_INCLUDE_FILE           = 0x020011
    };


    enum DebugType {
      NODEBUG,
      FULLDEBUG,
      SHORTDEBUG
    };

class ZCppToken {
    public:
        ZTokentype_type Type{ZTTP_SPACE};
        utf8VaryingString Text;


        void reset() {Type=ZTTP_SPACE; Text.clear(); TokenLine=Line; TokenColumn=Column;} /* NB: Keep line number and columns number */

        void newLine() {Column=0; Line++;}

        void setCoords(int pLine,int pColumn) {TokenColumn=pColumn; TokenLine=pLine;}

        int TokenColumn;
        int TokenLine;

        void display() const;
    };

    class ZCppTokenizer : protected ZArray<ZCppToken> {
    public:
      typedef ZArray<ZCppToken> _Base;
      ZCppTokenizer(DebugType pDebug) {Debug=pDebug;}

      void parse(const utf8VaryingString &pIn);

      ZArray<ZCppToken> getTokenList() {return (*this);}

      void report() {
          for(long wi=0; wi < count(); wi++) {
            Tab[wi].display();
          }
      }

      ZaiErrors ErrorLog;
    private:
      DebugType Debug=NODEBUG;
      void endToken(ZCppToken &pToken);
      void getMethodArguments(long &pidx, const utf8VaryingString &pIn);
    };

    utf8String
    decode_TokenType(ZTokentype_type pType);

}//zparsernmsp
