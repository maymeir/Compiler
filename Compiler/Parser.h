#include "Token.h"
#include "Semantic.h"


void Parser();
int match(eTOKENS t);
void error_handler(eTOKENS* follow_tokens_array, int size);
void Parse_PROG();
void Parse_GLOBAL_VARS();
void Parse_GLOBAL_VARS_TAG();
void Parse_VAR_DEC(int isParam);
void Parse_VAR_DEC_TAG(elementType icurr_type, int isUsed);
elementType Parse_TYPE();
void Parse_DIM_SIZES();
void Parse_DIM_SIZES_TAG();
elementType Parse_FUNC_PROTOTYPE(int isProto);
void Parse_FUNC_FULL_DEFS();
void Parse_FUNC_FULL_DEFS_TAG();
void Parse_FUNC_WITH_BODY();
elementType Parse_RETURNED_TYPE();
int Parse_PARAMS();
int Parse_PARAM_LIST(int isParam);
int Parse_PARAM_LIST_TAG(int isParam);
int Parse_PARAM(int isParam);
int Parse_PARAM_TAG();
void Parse_COMP_STMT(elementType curr_type);
void Parse_VAR_DEC_LIST(int isParam);
void Parse_STMT_LIST(elementType curr_type);
void Parse_STMT_LIST_TAG(elementType curr_type);
void Parse_STMT(elementType currt_type);
void Parse_STMT_TAG();
void Parse_IF_STMT(elementType curr_type);
void Parse_CALL();
int Parse_ARGS();
int Parse_ARG_LIST(elementType curr_type, int isParam, int isUsed);
int Parse_ARG_LIST_TAG(elementType curr_type, int isParam, int isUsed);
elementType Parse_RETURN_STMT(elementType curr_type);
elementType Parse_RETURN_STMT_TAG();
void Parse_VAR();
void Parse_VAR_TAG();
void Parse_EXPR_LIST();
void Parse_EXPR_LIST_TAG();
void Parse_CONDITION();
elementType Parse_EXPR();
elementType Parse_EXPR_TAG();
elementType Parse_TERM();
elementType Parse_TERM_TAG();
elementType Parse_FACTOR();
elementType Parse_FACTOR_TAG();

void printEntries();
const char* typeToStringConverter(type num);

void checkIfAllVarsUsed(tablePtr curr_scope);
void checkIfReturnExistInBlock(tablePtr curr_scope);
void checkIfAllProtoUsed(tablePtr curr_scope);

void addParamToFunc(tablePtr curr_scope);
void addPrevFuncEntriesToTable(tablePtr curr_scope);
void addElement(int numToAdd);
tableEntry findFuncEntry(tablePtr curr_scope);
