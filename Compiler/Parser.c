#include "Parser.h"

tablePtr currTable = NULL;
int* size = NULL;
int arrSize = 0;

void Parser()
{
	currTable = makeTable(currTable);
	Parse_PROG();
	match(TOK_EOF);
	checkIfAllVarsUsed(currTable);
	checkIfAllProtoUsed(currTable);
	currTable = popTable(currTable);
}

int match(eTOKENS t)
{
	Token *current_token;
	current_token = next_token();
	if (current_token->kind != t)
	{
		fprintf(syntactic_output, "Expected token of type: '%s' at line %d,\nActual token: '%s, lexeme: '%s'.\n", token_kind_to_string(t), current_token->lineNumber, token_kind_to_string(current_token->kind), current_token->lexeme);
		return 0;
	}

	return 1;
}

void error_handler(eTOKENS* follow_tokens_array, int size)
{
	Token *current_token = next_token();
	int i;
	current_token = back_token(); 
	if (current_token->kind == TOK_EOF)
	{
		exit(1);
	}
	while (current_token->kind != TOK_EOF)
	{ 
		for (i = 0; i < size; i++)
		{
			if (current_token->kind == follow_tokens_array[i] || current_token->kind == TOK_SEP_SEMICOLON)
			{
				current_token = back_token();
				return;
			}
		}
		current_token = next_token();
	}
}

void Parse_PROG()
{
	Token* current_token;
	Token* temp_token;
	tableEntry tempEntry = NULL;
	elementType funcType;
	int isProto = 1;
	fprintf(syntactic_output, "Rule (PROG -> GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
	Parse_GLOBAL_VARS();
	current_token = next_token();
	eTOKENS lookaheadTok;

	do
	{
		lookaheadTok = checkIfProtoOrFunc();
		if (lookaheadTok == TOK_EOF)
		{
			fprintf(syntactic_output, "Unexpected EOF token at line: %d \n", current_token->lineNumber);
			exit(1);
		}
		else if (lookaheadTok == TOK_SEP_SEMICOLON)
		{
			fprintf(syntactic_output, "Rule (FUNC_PREDEFS -> FUNC_PROTOTYPE; FUNC_PREDEFS_TAG)\n");
			fprintf(syntactic_output, "Rule (FUNC_PREDEFS_TAG -> FUNC_PROTOTYPE; FUNC_PREDEFS_TAG)\n");
			funcType = Parse_FUNC_PROTOTYPE(isProto);
			current_token = next_token();
			current_token = next_token();
		}
		else if (lookaheadTok == TOK_SEP_CURLY_BRACES_L)
		{
			fprintf(syntactic_output, "Rule (FUNC_PREDEFS_TAG -> ϵ)\n");
			break;
		}

	} while (lookaheadTok != TOK_EOF || current_token->kind != TOK_SEP_SEMICOLON);

	Parse_FUNC_FULL_DEFS();


}

eTOKENS checkIfProtoOrFunc()
{
	int lookAheadCount = 0;
	Token* current_token;
	do
	{
		current_token = next_token();
		lookAheadCount++;

		if (current_token->kind == TOK_SEP_SEMICOLON)
		{
			for (int i = 0; i < lookAheadCount; i++)
			{
				current_token = back_token();
			}

			current_token = back_token();
			return TOK_SEP_SEMICOLON;
		}

		if (current_token->kind == TOK_SEP_CURLY_BRACES_L)
		{
			for (int i = 0; i < lookAheadCount; i++)
			{
				current_token = back_token();
			}

			current_token = back_token();
			return TOK_SEP_CURLY_BRACES_L;
		}

		if (current_token->kind == TOK_EOF)
		{
			for (int i = 0; i < lookAheadCount; i++)
			{
				current_token = back_token();
			}

			return TOK_EOF;
		}

	} while ((current_token->kind != TOK_SEP_SEMICOLON) || (current_token->kind != TOK_SEP_CURLY_BRACES_L) || (current_token->kind != TOK_EOF ));
}


void Parse_GLOBAL_VARS()
{
	int isParam = 0;
	fprintf(syntactic_output, "Rule (GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_TAG )\n");
	Parse_VAR_DEC(isParam);
	Parse_GLOBAL_VARS_TAG();
}

void Parse_GLOBAL_VARS_TAG()
{
	Token *current_token;
	current_token = next_token();
	eTOKENS follow_array[3] = { TOK_KW_INT, TOK_KW_FLOAT, TOK_KW_VOID };
	int isParam = 0;
	current_token = next_token();
	current_token = next_token();

	if (current_token->kind == TOK_SEP_PARENTHESES_L) 
	{
		fprintf(syntactic_output, "Rule (GLOBAL_VARS_TAG -> ϵ)\n");
		current_token = back_token();
		current_token = back_token();
		current_token = back_token();
		return;
	}
	else
	{
		current_token = back_token();
		current_token = back_token();
		switch (current_token->kind)
		{
			case (TOK_KW_FLOAT):
			case (TOK_KW_INT):
				fprintf(syntactic_output, "Rule (GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_TAG)\n");
				current_token = back_token();
				Parse_VAR_DEC(isParam);
				Parse_GLOBAL_VARS_TAG();
				break;
			case(TOK_KW_VOID):
				fprintf(syntactic_output, "Rule (GLOBAL_VARS -> VAR_DEC GLOBAL_VARS_TAG)\n");
				current_token = back_token();
				break;
			default:
				fprintf(syntactic_output, "Expected token of type: 'TOK_KW_FLOAT' or 'TOK_KW_INT' at line: %d \n", current_token->lineNumber);
				fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
				error_handler(follow_array, 3);
				break;
		}
	}
}

void Parse_VAR_DEC(int isParam)
{
	Token* current_token;
	int isUsed = 1;
	eTOKENS follow_array[7] = { TOK_KW_FLOAT, TOK_KW_INT, TOK_KW_VOID, TOK_ID, TOK_SEP_CURLY_BRACES_L, TOK_KW_IF, TOK_KW_RETURN };
	arrSize = 0;
	size = NULL;
	char* VarId;
	fprintf(syntactic_output, "Rule (VAR_DEC -> TYPE id VAR_DEC_TAG )\n");
	elementType curr_type = Parse_TYPE();
	current_token = next_token();
	VarId = current_token->lexeme;
	current_token = back_token();
	match(TOK_ID);
	Parse_VAR_DEC_TAG(curr_type, isUsed);
	Token* tempToken = NULL;
	current_token = next_token();
	current_token = back_token();
	tableEntry tempEntry = NULL;
	tempToken = back_token();
	tempEntry = insert(currTable, VarId);


	if (arrSize == 0)
	{
		addElement(0);
	}

	if (tempEntry != NULL)
	{
		tempEntry->lineNum = tempToken->lineNumber;
		tempEntry->isUsed = 0;
		tempEntry->name = VarId;
		tempEntry->size = size;
		tempEntry->arrSize = arrSize;
		tempEntry->varType = curr_type;
		tempEntry->isFunc = 0;
		tempEntry->isProto = 0;
		tempEntry->isParam = isParam;
	}
	else
	{
		if (isUsed == 0)
		{
			fprintf(semantic_output, "ERROR!! this variable has Duplicate definition '%s' in line number %d.\n", current_token->lexeme, current_token->lineNumber);
			error_handler(follow_array, 7);
		}
		else
		{
			tempEntry = find(currTable, tempToken->lexeme);
			if (tempEntry)
			{
				tempEntry->isUsed = isUsed;
			}
		}
	}
	
	tempToken = next_token();
}

void Parse_VAR_DEC_TAG(elementType icurr_type, int isUsed)
{
	Token* temp_token = NULL;
	tableEntry tempEntry = NULL;
	Token* current_token = next_token();
	eTOKENS follow_array[7] = { TOK_KW_FLOAT, TOK_KW_INT, TOK_KW_VOID, TOK_ID, TOK_SEP_CURLY_BRACES_L, TOK_KW_IF, TOK_KW_RETURN };

	switch (current_token->kind)
	{
		case(TOK_SEP_BRACKETS_L):
			fprintf(syntactic_output, "Rule (VAR_DEC_TAG ->  [DIM_SIZES] ;)\n");
			Parse_DIM_SIZES();
			match(TOK_SEP_BRACKETS_R);
			match(TOK_SEP_SEMICOLON);
			return;
		case(TOK_SEP_SEMICOLON):
			fprintf(syntactic_output, "Rule (VAR_DEC_TAG  -> ;)\n");
			return NULL;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_BRACKETS_L' or 'TOK_SEP_SEMICOLON' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 7);
			return NULL;
			break;
	}

	return NULL;
}

elementType Parse_TYPE()
{
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_ID };

	switch (current_token->kind)
	{
		case (TOK_KW_FLOAT):
			fprintf(syntactic_output, "Rule (TYPE -> float)\n");
			return FLOAT;
		case (TOK_KW_INT):
			fprintf(syntactic_output, "Rule (TYPE -> int)\n");
			return INT;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_KW_FLOAT' or 'TOK_KW_INT' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			fprintf(semantic_output, "Expected 'FLOAT' or 'INT' at line: %d.\n", current_token->lineNumber);
			error_handler(follow_array, 1);
			return ERROR_TYPE;
	}
}

void Parse_DIM_SIZES()
{
	Token* temp_token = next_token();
	int count = 0;
	eTOKENS follow_array[1] = { TOK_SEP_BRACKETS_R };
	fprintf(syntactic_output, "Rule (DIM_SIZES -> int_num DIM_SIZES_TAG)\n");
	if (temp_token->kind == TOK_INT_NUM)
	{
		temp_token = back_token();
		temp_token = next_token();
		addElement(atoi(temp_token->lexeme));
	}
	else
	{
		temp_token = back_token();
		temp_token = next_token();
		fprintf(semantic_output, "Error!! the array index must be int. Line number: %d \n", temp_token->lineNumber);
		addElement(0);
		error_handler(follow_array, 1);
	}

	Parse_DIM_SIZES_TAG();
}

void Parse_DIM_SIZES_TAG()
{
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_SEP_BRACKETS_R };

		switch (current_token->kind)
		{
		case (TOK_SEP_COMMA):
			fprintf(syntactic_output, "Rule (DIM_SIZES_TAG -> , DIM_SIZES)\n");
			return Parse_DIM_SIZES();
		case(TOK_SEP_BRACKETS_R):
			fprintf(syntactic_output, "Rule (DIM_SIZES_TAG -> ϵ)\n");
			current_token = back_token();
			return;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_COMMA' or 'TOK_SEP_BRACKETS_R'  line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s',lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
}

elementType Parse_FUNC_PROTOTYPE(int isProto)
{
	tableEntry tempEntry = NULL, tempEntryCopy = NULL;
	Token* temp_token = NULL;
	eTOKENS follow_array[2] = { TOK_SEP_SEMICOLON, TOK_SEP_CURLY_BRACES_L };
	char* varId;
	elementType funcType;
	int* size = NULL;
	int paramsNum = 0;
	fprintf(syntactic_output, "Rule (FUNC_PROTOTYPE -> RETURNED_TYPE id (PARAMS))\n");
	funcType = Parse_RETURNED_TYPE();
	match(TOK_ID);
	temp_token = back_token();
	temp_token = next_token();
	varId = temp_token->lexeme;

	if (isProto == 1) // we parsing prototype
	{
		tempEntry = find(currTable, varId);
		if (tempEntry != NULL)
		{
			fprintf(semantic_output, "ERROR!! prototype already have a definition '%s'  line number: %d.\n", temp_token->lexeme, temp_token->lineNumber);
			error_handler(follow_array, 2);
		}
		else
		{
			tempEntry = insert(currTable, temp_token->lexeme);
			if (tempEntry)
			{
				tempEntry->isFunc = 0;
				tempEntry->isProto = 1;
				tempEntry->isUsed = 0;
				tempEntry->name = temp_token->lexeme;
				tempEntry->lineNum = temp_token->lineNumber;
				tempEntry->funcType = funcType;
				match(TOK_SEP_PARENTHESES_L);
				currTable = makeTable(currTable);
				addPrevFuncEntriesToTable(currTable);
				addParamToFunc(currTable);
				tempEntry->paramsNum = Parse_PARAMS();
				match(TOK_SEP_PARENTHESES_R);
				currTable = popTable(currTable);
			}
		}
	}
	else // we parsing full function
	{
		tempEntry = find(currTable, varId);
		if (tempEntry != NULL) // there is id in the table
		{
			if (tempEntry->isFunc == 0) // enter full func here 
			{
				tempEntry->isFunc = 1;
				tempEntry->isUsed = 0;
				if (tempEntry->funcType != funcType)
				{
					tempEntry->funcType = funcType;
					fprintf(semantic_output, "ERROR!! Prototype and full definition '%s' dont return the same type.  line number: %d.\n", tempEntry->name, temp_token->lineNumber);
				}

				match(TOK_SEP_PARENTHESES_L);
				currTable = makeTable(currTable);
				addPrevFuncEntriesToTable(currTable);
				addParamToFunc(currTable);
				paramsNum = Parse_PARAMS();
				if (paramsNum != tempEntry->paramsNum)
				{
					fprintf(semantic_output, "ERROR!! Prototype and full definition '%s' have different number of parameters.  line number: %d.\n", tempEntry->name, temp_token->lineNumber);
					tempEntry->paramsNum = paramsNum;
				}

				match(TOK_SEP_PARENTHESES_R);
			}
			else
			{
				fprintf(semantic_output, "ERROR!! full definition already have a definition '%s' line number: %d.\n", tempEntry->name, temp_token->lineNumber);
				tempEntry->isFunc = 1;
				tempEntry->isUsed = 0;
				if (tempEntry->funcType != funcType)
				{
					tempEntry->funcType = funcType;
					fprintf(semantic_output, "ERROR!! Prototype and full definition '%s' dont return the same type. in line number: %d.\n", tempEntry->name, temp_token->lineNumber);
				}

				match(TOK_SEP_PARENTHESES_L);
				currTable = makeTable(currTable);
				addPrevFuncEntriesToTable(currTable);
				addParamToFunc(currTable);
				paramsNum = Parse_PARAMS();
				if (paramsNum != tempEntry->paramsNum)
				{
					fprintf(semantic_output, "ERROR!! Prototype and full definition '%s' have different number of parameters. line number: %d.\n", tempEntry->name, temp_token->lineNumber);
					tempEntry->paramsNum = paramsNum;
				}

				match(TOK_SEP_PARENTHESES_R);
			}
		}
		else
		{
			fprintf(semantic_output, "ERROR!! function '%s' declared with no protoype. line number: %d.\n", temp_token->lexeme, temp_token->lineNumber);
			tempEntry = insert(currTable, temp_token->lexeme);
			if (tempEntry)
			{
				tempEntry->isFunc = 0;
				tempEntry->isProto = 1;
				tempEntry->isUsed = 0;
				tempEntry->name = temp_token->lexeme;
				tempEntry->lineNum = temp_token->lineNumber;
				tempEntry->funcType = funcType;
				match(TOK_SEP_PARENTHESES_L);
				currTable = makeTable(currTable);
				addPrevFuncEntriesToTable(currTable);
				addParamToFunc(currTable);
				tempEntry->paramsNum = Parse_PARAMS();
				match(TOK_SEP_PARENTHESES_R);
			}
		}


	}

	return funcType;
}

void Parse_FUNC_FULL_DEFS()
{
	fprintf(syntactic_output, "Rule (FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS_TAG)\n");
	Parse_FUNC_WITH_BODY();
	Parse_FUNC_FULL_DEFS_TAG();
}

void Parse_FUNC_FULL_DEFS_TAG()
{
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_EOF };
	switch (current_token->kind)
	{
		case (TOK_KW_FLOAT):
		case (TOK_KW_INT):
		case (TOK_KW_VOID):
			fprintf(syntactic_output, "Rule (FUNC_FULL_DEFS_TAG ->  FUNC_FULL_DEFS)\n");
			current_token = back_token();
			Parse_FUNC_FULL_DEFS();
			break;
		case(TOK_EOF):
			fprintf(syntactic_output, "Rule (FUNC_FULL_DEFS_TAG -> ϵ)\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_KW_FLOAT' or 'TOK_KW_INT' or 'TOK_KW_VOID' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			Parse_FUNC_FULL_DEFS();
			break;
	}
}

void Parse_FUNC_WITH_BODY()	
{
	elementType curr_type;
	int isProto = 0;
	fprintf(syntactic_output, "Rule (FUNC_WITH_BODY  -> FUNC_PROTOTYPE COMP_STMT)\n");
	curr_type = Parse_FUNC_PROTOTYPE(isProto);
	Parse_COMP_STMT(curr_type);
}

elementType Parse_RETURNED_TYPE()
{
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_ID };
	switch (current_token->kind)
	{
		case (TOK_KW_FLOAT):
		case (TOK_KW_INT):
			fprintf(syntactic_output, "Rule (RETURNED_TYPE -> TYPE)\n");
			current_token = back_token();
			return Parse_TYPE();
		case (TOK_KW_VOID):
			fprintf(syntactic_output, "Rule (RETURNED_TYPE ->  void)\n");
			return VOID; 
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_KW_FLOAT' or 'TOK_KW_INT' or 'TOK_KW_VOID' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			return ERROR_TYPE;
	}
}

int Parse_PARAMS()
{
	int count = 0;
	int isParam = 1;
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_SEP_PARENTHESES_R };

	switch (current_token->kind)
	{
		case (TOK_KW_FLOAT):
		case (TOK_KW_INT):
			fprintf(syntactic_output, "Rule (PARAMS -> PARAM_LIST )\n");
			current_token = back_token();
			count = Parse_PARAM_LIST(isParam);
			break;
		case(TOK_SEP_PARENTHESES_R):
			fprintf(syntactic_output, "Rule (PARAMS -> ϵ)\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_KW_FLOAT' or 'TOK_KW_INT' or 'TOK_SEP_PARENTHESES_R' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
	return count;
}

int Parse_PARAM_LIST(int isParam)
{
	int count = 0;
	fprintf(syntactic_output, "Rule (PARAM_LIST -> PARAM PARAM_LIST_TAG )\n");
	count = (Parse_PARAM(isParam) + Parse_PARAM_LIST_TAG(isParam));
	return count;
}

int Parse_PARAM_LIST_TAG(int isParam)
{
	int count = 0;
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_SEP_PARENTHESES_R };
	switch (current_token->kind)
	{
		case (TOK_SEP_COMMA):
			fprintf(syntactic_output, "Rule (PARAM_LIST_TAG -> , PARAM PARAM_LIST_TAG  )\n");
			count = (Parse_PARAM(isParam) + Parse_PARAM_LIST_TAG(isParam));
			break;
		case(TOK_SEP_PARENTHESES_R):
			fprintf(syntactic_output, "Rule (PARAM_LIST_TAG -> ϵ)\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_COMMA'  at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
	return count;
}

int Parse_PARAM(int isParam)
{
	size = NULL;
	arrSize = 0;
	tableEntry tempEntry = NULL;
	eTOKENS follow_array[2] = { TOK_SEP_PARENTHESES_L , TOK_SEP_COMMA };
	int count = 0, isUsed = 0;
	elementType current_type;
	fprintf(syntactic_output, "Rule (PARAM -> TYPE id PARAM_TAG)\n");
	current_type = Parse_TYPE();
	match(TOK_ID);
	Token* current_token, * temp_token;
	current_token = next_token();
	current_token = back_token();
	temp_token = back_token();
	temp_token = next_token();
	tempEntry = insert(currTable, temp_token->lexeme);
	count = (Parse_PARAM_TAG() + 1);
	if (arrSize == 0)
	{
		addElement(0);
	}

	if (tempEntry != NULL)
	{
		tempEntry->name = current_token->lexeme;
		tempEntry->size = size;
		tempEntry->arrSize = arrSize;
		tempEntry->varType = current_type;
		tempEntry->isUsed = 0;
		tempEntry->isFunc = 0;
		tempEntry->lineNum = current_token->lineNumber;
		tempEntry->isParam = isParam;
	}
	else // tempEntry = NULL means that we alredy using this id
	{
		fprintf(semantic_output, "ERROR!! variable already has a declaration '%s'. line number %d.\n", current_token->lexeme, current_token->lineNumber);
	}

	return count;
}

int Parse_PARAM_TAG()
{
	int count = 0;
	Token *current_token = next_token();
	eTOKENS follow_array[2] = { TOK_SEP_PARENTHESES_R , TOK_SEP_COMMA };

	switch (current_token->kind)
	{
		case(TOK_SEP_BRACKETS_L):
			fprintf(syntactic_output, "Rule (PARAM_TAG ->  [DIM_SIZES])\n");
			Parse_DIM_SIZES();
			match(TOK_SEP_BRACKETS_R);
			break;
		case(TOK_SEP_PARENTHESES_R):
		case (TOK_SEP_COMMA):
			fprintf(syntactic_output, "Rule (PARAM_TAG -> ϵ)\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_BRACKETS_L' or 'TOK_SEP_PARENTHESES_R' or 'TOK_SEP_COMMA' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 2);
			break;
	}

	return count;
}

void Parse_COMP_STMT(elementType curr_type)
{
	Token *current_token = next_token();
	eTOKENS follow_array[5] = { TOK_SEP_SEMICOLON , TOK_KW_INT, TOK_KW_FLOAT, TOK_KW_VOID ,TOK_EOF };
	int isFunc = 0, isParam = 0;

	fprintf(syntactic_output, "Rule (COMP_STMT -> {VAR_DEC_LIST STMT_LIST})\n");
	if (current_token->kind == TOK_SEP_CURLY_BRACES_L)
	{
		Parse_VAR_DEC_LIST(isParam);
		Parse_STMT_LIST(curr_type);
		match(TOK_SEP_CURLY_BRACES_R);
		checkIfAllVarsUsed(currTable);
		checkIfReturnExistInBlock(currTable);
		currTable = popTable(currTable);
	}
	else 
	{
		
		fprintf(syntactic_output, "Expected token of type : 'TOK_SEP_CURLY_BRACES_L' at line: %d \n", current_token->lineNumber);
		fprintf(syntactic_output, "Actual token: '%s',lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
		error_handler(follow_array, 5);
	}
}

void Parse_VAR_DEC_LIST(int isParam)
{
	int count = 0;
	Token *current_token = next_token();
	eTOKENS follow_array[4] = { TOK_ID , TOK_KW_IF ,TOK_KW_RETURN ,TOK_SEP_CURLY_BRACES_L };
	switch (current_token->kind)
	{
		case (TOK_KW_FLOAT):
		case (TOK_KW_INT):
			fprintf(syntactic_output, "Rule (VAR_DEC_LIST_TAG -> VAR_DEC VAR_DEC_LIST)\n");
			current_token = back_token();
			Parse_VAR_DEC(isParam);
			Parse_VAR_DEC_LIST(isParam);
			break;
		case (TOK_ID):
		case (TOK_KW_IF):
		case (TOK_KW_RETURN):
		case (TOK_SEP_CURLY_BRACES_L):
			fprintf(syntactic_output, "Rule (VAR_DEC_LIST_TAG ->  ϵ)\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_KW_FLOAT' or 'TOK_KW_INT' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 4);
			break;
	}
}

void Parse_STMT_LIST(elementType curr_type)
{
	fprintf(syntactic_output, "Rule (STMT_LIST -> STMT ; STMT_LIST_TAG)\n");
	Parse_STMT(curr_type);
	Parse_STMT_LIST_TAG(curr_type);
}

void Parse_STMT_LIST_TAG(elementType curr_type)
{
	eTOKENS follow_array[1] = { TOK_SEP_CURLY_BRACES_R };
	Token *current_token = next_token();

	if (current_token->kind == TOK_SEP_SEMICOLON)
	{
		fprintf(syntactic_output, "Rule (STMT_LIST' -> ; STMT STMT_LIST_TAG )\n");
		Parse_STMT(curr_type);
		Parse_STMT_LIST_TAG(curr_type);
	}
	else if (current_token->kind == TOK_EOF)
	{
		current_token = back_token();
		if (current_token->kind == TOK_SEP_CURLY_BRACES_R)
		{
			fprintf(syntactic_output, "Rule (STMT_LIST' ->  epsilon)\n");
			current_token = back_token();
			return;
		}
	}
	else if (current_token->kind == TOK_SEP_CURLY_BRACES_R)
	{
		fprintf(syntactic_output, "Rule (STMT_LIST' ->  epsilon)\n");
		current_token = back_token();
		return;
	}
	else if (current_token->kind == TOK_KW_RETURN)
	{
		current_token = back_token();
		Parse_STMT(curr_type);
	}
	else
	{
		fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_SEMICOLON' or 'TOK_SEP_CURLY_BRACES_R' at line: %d \n", current_token->lineNumber);
		fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
		error_handler(follow_array, 1);
	}
}

void Parse_STMT(elementType curr_type)
{
	elementType funcType;
	tableEntry tempEntry = NULL, originalEntry = NULL;
	int isFunc = 0;
	Token *current_token = next_token();
	eTOKENS follow_array[2] = { TOK_SEP_SEMICOLON , TOK_SEP_CURLY_BRACES_R };
	fprintf(syntactic_output, "Rule (STMT -> VAR = EXPR | COMP_STMT | IF_STMT | CALL | RETURN_STMT)\n");

	switch (current_token->kind)
	{
		case (TOK_ID):
			fprintf(syntactic_output, "Rule (STMT -> id STMT_TAG)\n");
			Parse_STMT_TAG();
			break;
		case (TOK_KW_IF):
			fprintf(syntactic_output, "Rule (STMT -> IF_STMT)\n");
			current_token = back_token();
			Parse_IF_STMT(curr_type);
			break;
		case (TOK_KW_RETURN):
			fprintf(syntactic_output, "Rule (STMT ->  RETURN_STMT)\n");
			current_token = back_token();
			funcType = Parse_RETURN_STMT(curr_type);
			tempEntry = findFuncEntry(currTable);
			if (tempEntry) // not getting in this if statment
			{
				tempEntry->returnExist = 1;
				originalEntry = find(currTable->next_table, tempEntry->name);
				if (originalEntry)
				{
					originalEntry->returnExist = 1;
				}
			}

			if (funcType != curr_type)
			{
				fprintf(semantic_output, "ERROR!! Returned type: %s different from function type: %s. Line number: %d \n", typeToStringConverter(funcType), typeToStringConverter(curr_type), current_token->lineNumber);
			}
			break;
		case (TOK_SEP_CURLY_BRACES_L):
			fprintf(syntactic_output, "Rule (STMT -> COMP_STMT )\n");
			current_token = back_token();
			currTable = makeTable(currTable);
			addPrevFuncEntriesToTable(currTable);
			addParamToFunc(currTable);
			Parse_COMP_STMT(curr_type);
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_ID' or 'TOK_KW_IF' or 'TOK_KW_RETURN'  or 'TOK_SEP_CURLY_BRACES_L'at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
}

void Parse_STMT_TAG()
{
	int isParam = 0, currentArrayIndex;
	int isUsed = 0; 
	int paramsNum = 0;
	char* varId;
	tableEntry current_entry = NULL;
	Token *current_token = next_token();
	Token* temp_token = NULL;
	eTOKENS follow_array[2] = { TOK_SEP_SEMICOLON , TOK_SEP_CURLY_BRACES_R };
	elementType rightSideType;
	elementType  leftSideType;
	switch (current_token->kind)
	{
		case (TOK_OP_EQUAL):
		case (TOK_SEP_BRACKETS_L):
			temp_token = back_token(); 
			varId = temp_token->lexeme;
			fprintf(syntactic_output, "Rule (STMT_TAG -> VAR = EXPR)\n");
			Parse_VAR();
			current_entry = find(currTable, varId); // find id in table 
			match(TOK_OP_EQUAL);
			rightSideType = Parse_EXPR();
			if (current_entry) {
				leftSideType = current_entry->varType;
				if (leftSideType == ERROR_TYPE) {
					fprintf(semantic_output, "Error!! types dont match. Line number: %d \n", current_token->lineNumber);
				}
				if (rightSideType == ERROR_TYPE) {
					fprintf(semantic_output, "Error!! types dont match. Line number: %d \n", current_token->lineNumber);
				}
				else if (rightSideType == FLOAT && leftSideType == INT) {
					fprintf(semantic_output, "Error!! types dont match. Line number: %d \n", current_token->lineNumber);
				}
			}

			current_token = back_token();
			break;
		case (TOK_SEP_PARENTHESES_L):
			fprintf(syntactic_output, "Rule (STMT_TAG ->  (ARGS))\n");
			paramsNum = Parse_ARGS();
			current_token = back_token(); // go back to id
			current_token = back_token();
			current_entry = find(currTable, current_token->lexeme);
			if (!current_entry)
			{
				fprintf(semantic_output, "ERROR!! function '%s' used, but not declared. Line number: %d \n", current_token->lexeme, current_token->lineNumber);
			}
			if (current_entry)
			{
				if (current_entry->paramsNum != paramsNum)
				{
					fprintf(semantic_output, "ERROR!! function-call and decleration of function '%s' contain different number of parametersn. Line number: %d \n", current_token->lexeme, current_token->lineNumber);
				}
			}

			current_token = next_token();

			current_token = next_token();
			match(TOK_SEP_PARENTHESES_R);
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_BRACKETS_L' or 'TOK_SEP_PARENTHESES_L'  or 'TOK_OP_EQUAL' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 2);
			break;
	}
}

void Parse_IF_STMT(elementType curr_type)
{
	Token* curreny_token;
	fprintf(syntactic_output, "Rule (IF_STMT -> if ( CONDITION ) STMT)\n");
	match(TOK_KW_IF);
	match(TOK_SEP_PARENTHESES_L);
	Parse_CONDITION();
	curreny_token = back_token();
	match(TOK_SEP_PARENTHESES_R);
	Parse_STMT(curr_type);
}

void Parse_CALL()
{
	Token* current_token = next_token();
	current_token = back_token();

	tableEntry current_entry = NULL;
	int assignedParam;
	Token* temp_token = NULL;
	fprintf(syntactic_output, "Rule CALL -> id ( ARGS ))\n");
	match(TOK_ID);
	current_entry = find(currTable, current_token->lexeme);
	if (!current_entry)
	{
		fprintf(semantic_output, "ERROR!! function '%s' was used but not declared. Line number: %d \n", current_token->lexeme, current_token->lineNumber);
	}

	match(TOK_SEP_PARENTHESES_L);
	assignedParam = Parse_ARGS();
	if (current_entry)
	{
		if (current_entry->paramsNum != assignedParam)
		{
			fprintf(semantic_output, "ERROR!! function-call and definition of function '%s' contain different number of parameters. Line number: %d \n", current_token->lexeme, current_token->lineNumber);
		}
	}

	match(TOK_SEP_PARENTHESES_R);
}

int Parse_ARGS()
{
	int paramsNum, isParam = 1, isUsed = 1;
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_SEP_PARENTHESES_R };
	switch (current_token->kind)
	{
		case (TOK_INT_NUM):
		case (TOK_SEP_PARENTHESES_L):
		case (TOK_ID):
		case (TOK_FLOAT_NUM):
			fprintf(syntactic_output, "Rule (ARGS -> ARG_LIST)\n");
			current_token = back_token();
			paramsNum = Parse_ARG_LIST(INT, isParam, isUsed);// check params numbr here
			break;
		case(TOK_SEP_PARENTHESES_R):
			fprintf(syntactic_output, "Rule (ARGS ->  ϵ)\n");
			current_token = back_token();
			paramsNum = 0;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_ID' or 'TOK_FLOAT_NUM' or 'TOK_INT_NUM'  or 'TOK_SEP_PARENTHESES_L'at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			paramsNum = 0;
			break;
	}
	return paramsNum;
}

int Parse_ARG_LIST(elementType curr_type, int isParam, int isUsed)
{
	int count = 0;
	fprintf(syntactic_output, "Rule (ARG_LIST -> EXPR ARG_LIST_TAG)\n");
	Parse_EXPR(curr_type, isParam, isUsed);
	count = (Parse_ARG_LIST_TAG(curr_type, isParam, isUsed) + 1);
	return count;
}

int Parse_ARG_LIST_TAG(elementType curr_type, int isParam, int isUsed)
{
	int count = 0;
	Token *current_token = next_token();
	current_token = back_token();
	eTOKENS follow_array[1] = { TOK_SEP_PARENTHESES_R };
	switch (current_token->kind)
	{
		case (TOK_SEP_COMMA):
			fprintf(syntactic_output, "Rule (ARG_LIST_TAG -> , EXPR ARG_LIST_TAG) \n");
			Parse_EXPR(curr_type, isParam, isUsed);
			count = (Parse_ARG_LIST_TAG(curr_type, isParam, isUsed) + 1);
			break;
		case(TOK_SEP_PARENTHESES_R):
			current_token = back_token();
			fprintf(syntactic_output, "Rule (ARG_LIST_TAG ->  ϵ)\n");
			count = 0;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_COMMA' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
	return count;
}

elementType Parse_RETURN_STMT(elementType curr_type)
{
	elementType returned_type;
	fprintf(syntactic_output, "Rule (RETURN_STMT -> return RETURN_STMT_TAG )\n");
	match(TOK_KW_RETURN);
	returned_type = Parse_RETURN_STMT_TAG();
	if (returned_type != curr_type)
	{
		return ERROR_TYPE;
	}
	else
	{
		return returned_type;
	}
}

elementType Parse_RETURN_STMT_TAG()
{
	elementType returned_type;
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_SEP_SEMICOLON };
	switch (current_token->kind)
	{
		case (TOK_ID):
		case (TOK_FLOAT_NUM):
		case (TOK_INT_NUM):
		case (TOK_SEP_PARENTHESES_L):
			fprintf(syntactic_output, "Rule (RETURN_STMT_TAG -> EXPR \n");
			current_token = back_token();
			returned_type = Parse_EXPR();
			current_token = back_token();
			break;
		case(TOK_SEP_SEMICOLON):
		case (TOK_SEP_CURLY_BRACES_R):
			current_token = back_token();
			fprintf(syntactic_output, "Rule (RETURN_STMT_TAG -> ϵ) \n");
			returned_type = VOID;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_ID' or 'TOK_FLOAT_NUM' or 'TOK_INT_NUM'  or 'TOK_SEP_PARENTHESES_L' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			returned_type = VOID;
			break;
	}
	return returned_type;

}

void Parse_VAR()
{
	Token* temp_token = NULL;
	tableEntry tempEntry = NULL;
	arrSize = 0;
	size = NULL;
	char* varName;
	Token *current_token = next_token();
	eTOKENS follow_array[13] = { TOK_SEP_PARENTHESES_R , TOK_SEP_SEMICOLON  , TOK_SEP_COMMA  ,TOK_SEP_BRACKETS_R  ,TOK_OP_ADD ,TOK_OP_MUL,TOK_REAL_OP_SE ,TOK_REAL_OP_BE ,TOK_REAL_OP_AR , TOK_REAL_OP_NE ,TOK_REAL_OP_AL  , TOK_OP_EQUAL };
	fprintf(syntactic_output, "Rule( VAR -> id VAR_TAG)\n");
	current_token = back_token(); // go back to id
	varName = current_token->lexeme;
	current_token = next_token();
	if (current_token->kind == TOK_SEP_BRACKETS_L)
	{
		current_token = back_token();
		Parse_VAR_TAG();
	}
	else
	{
		addElement(0); // add back_token after that ? 
		current_token = back_token();
		Parse_VAR_TAG();


	}

	tempEntry = find(currTable, varName);
	if (tempEntry != NULL)
	{
		tempEntry->isUsed = 1;
		if (arrSize != tempEntry->arrSize)
		{
			fprintf(semantic_output, "ERROR!! Wrong access. variable '%s' as array. Line number %d.\n", current_token->lexeme, current_token->lineNumber);
		}
		else
		{

			for (int i = 0; i < tempEntry->arrSize; i++)
			{
				if (tempEntry->size[i] < size[i])
				{
					fprintf(semantic_output, "ERROR!! Array '%s' index out of range. Line number %d.\n", current_token->lexeme, current_token->lineNumber);
				}
			}
		}

	}
	else
	{
		fprintf(semantic_output, "ERROR!! Using undefined variable '%s'. Line number %d.\n", current_token->lexeme, current_token->lineNumber);
	}
}

void Parse_VAR_TAG()
{
	Token *current_token = next_token();
	eTOKENS follow_array[13] = { TOK_SEP_PARENTHESES_R , TOK_SEP_SEMICOLON  , TOK_SEP_COMMA  ,TOK_SEP_BRACKETS_R  ,TOK_OP_ADD ,TOK_OP_MUL,TOK_REAL_OP_SE ,TOK_REAL_OP_BE ,TOK_REAL_OP_AR , TOK_REAL_OP_NE ,TOK_REAL_OP_AL, TOK_REAL_OP_IE  , TOK_OP_EQUAL };
	switch (current_token->kind)
	{
		case(TOK_SEP_BRACKETS_L):
			fprintf(syntactic_output, "Rule( VAR_TAG -> [EXPR_LIST])\n");
			Parse_EXPR_LIST();
			match(TOK_SEP_BRACKETS_R);
			break;
		case (TOK_REAL_OP_SE):
		case (TOK_REAL_OP_IE):
		case (TOK_REAL_OP_BE):
		case (TOK_REAL_OP_AR):
		case (TOK_REAL_OP_NE):
		case (TOK_REAL_OP_AL):
		case (TOK_OP_EQUAL):
		case (TOK_SEP_PARENTHESES_R):
		case (TOK_SEP_SEMICOLON):
		case (TOK_SEP_COMMA):
		case (TOK_OP_ADD):
		case (TOK_SEP_BRACKETS_R):
		case (TOK_OP_MUL):
		case (TOK_SEP_CURLY_BRACES_R):
			fprintf(syntactic_output, "Rule (VAR_TAG -> ϵ)\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_SEP_BRACKETS_L' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 13);
			break;
	}
}

void Parse_EXPR_LIST()
{

	Token* current_token = next_token();
	elementType parsedType;
	eTOKENS follow_array[1] = { TOK_SEP_BRACKETS_R };
	fprintf(syntactic_output, "Rule (EXPR_LIST -> EXPR EXPR_LIST_TAG)\n");
	current_token = back_token();
	parsedType = Parse_EXPR();
	if (parsedType == INT)
	{
		current_token = back_token(); // go back to the number
		addElement(atoi(current_token->lexeme));
		Parse_EXPR_LIST_TAG();

	}
	else
	{
		fprintf(semantic_output, "Error!! Array index must be of type INT. Line number: %d \n", current_token->lineNumber);
		addElement(0);
		error_handler(follow_array, 1);
	}
}

void Parse_EXPR_LIST_TAG()
{
	elementType parsedType;
	Token *current_token = next_token();
	eTOKENS follow_array[1] = { TOK_SEP_BRACKETS_R };
	switch (current_token->kind)
	{
		case (TOK_SEP_COMMA):
			fprintf(syntactic_output, "Rule (EXPR_LIST_TAG -> , EXPR EXPR_LIST_TAG \n");
			parsedType = Parse_EXPR();
			if (parsedType == INT)
			{
				current_token = back_token(); // go back to the number
				addElement(atoi(current_token->lexeme));
				//current_token = next_token();
				Parse_EXPR_LIST_TAG();

			}
			else
			{
				fprintf(semantic_output, "Error!! Array index must be of type INT. Line number: %d \n", current_token->lineNumber);
				addElement(0);
				error_handler(follow_array, 1);
			}
			break;
		case(TOK_SEP_BRACKETS_R):
			fprintf(syntactic_output, "Rule (EXPR_LIST_TAG ->  ϵ )\n");
			current_token = back_token();
			break;
		default:
			fprintf(syntactic_output, "Expected token of type:  'TOK_SEP_COMMA' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
}

void Parse_CONDITION()
{
	fprintf(syntactic_output, "Rule (CONDITION -> EXPR rel_op EXPR)\n");
	elementType leftType, rightType;
	leftType = Parse_EXPR();
	Token *current_token = next_token();
	current_token = back_token();
	if (leftType == ERROR_TYPE) {
		fprintf(semantic_output, "Error!! Types dont match. Line number: %d \n", current_token->lineNumber);
	}
	eTOKENS follow_array[1] = { TOK_SEP_PARENTHESES_R };
	switch (current_token->kind)
	{
		case (TOK_SEP_BRACKETS_R):
		case (TOK_REAL_OP_SE):
		case (TOK_REAL_OP_IE):
		case (TOK_REAL_OP_BE):
		case (TOK_REAL_OP_AR):
		case (TOK_REAL_OP_NE):
		case (TOK_REAL_OP_AL):
			rightType = Parse_EXPR();
			if (rightType == ERROR_TYPE) {
				fprintf(semantic_output, "Error!! Types dont match. Line number: %d \n", current_token->lineNumber);
			}
			else if (rightType == FLOAT && leftType == INT) {
				fprintf(semantic_output, "Error!! Types dont match. Line number: %d \n", current_token->lineNumber);
			}
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_REAL_OP_SE' or 'TOK_REAL_OP_IE' or 'TOK_REAL_OP_BE'  or 'TOK_REAL_OP_AR'  or 'TOK_REAL_OP_NE'   or 'TOK_REAL_OP_AL' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 1);
			break;
	}
}

elementType Parse_EXPR()
{
	elementType leftType, rightType, returned_type = ERROR_TYPE;
	fprintf(syntactic_output, "Rule (EXPR -> TERM EXPR_TAG)\n");
	leftType = Parse_TERM();
	rightType = Parse_EXPR_TAG();
	if (rightType == ERROR_TYPE || leftType == ERROR_TYPE)
	{
		returned_type = ERROR_TYPE;
	}
	else if (leftType == FLOAT)
	{
		returned_type = FLOAT;
	}
	else if (rightType == FLOAT && leftType == INT)
	{
		returned_type = ERROR_TYPE;
	}
	else
	{
		returned_type = INT;
	}

	return returned_type;
}

elementType Parse_EXPR_TAG()
{
	elementType leftType, rightType, returned_type = ERROR_TYPE;
	Token *current_token = next_token();
	eTOKENS follow_array[10] = { TOK_SEP_PARENTHESES_R , TOK_SEP_SEMICOLON  , TOK_SEP_COMMA  ,TOK_SEP_BRACKETS_R ,TOK_REAL_OP_SE ,TOK_REAL_OP_BE ,TOK_REAL_OP_AR , TOK_REAL_OP_NE ,TOK_REAL_OP_AL, TOK_REAL_OP_IE };
	switch (current_token->kind)
	{
		case (TOK_OP_ADD):
			fprintf(syntactic_output, "Rule (EXPR_TAG -> + TERM EXPR_TAG)\n");
			leftType = Parse_TERM();
			rightType = Parse_EXPR_TAG();
			if (rightType == ERROR_TYPE || leftType == ERROR_TYPE)
			{
				returned_type = ERROR_TYPE;
			}
			else if (leftType == FLOAT)
			{
				returned_type = FLOAT;
			}
			else if (rightType == FLOAT && leftType == INT)
			{
				returned_type = ERROR_TYPE;
			}
			else
			{
				returned_type = INT;
			}
			break;
		case (TOK_REAL_OP_IE):
		case(TOK_SEP_CURLY_BRACES_R):
		case (TOK_REAL_OP_BE):
		case (TOK_REAL_OP_AR):
		case (TOK_REAL_OP_NE):
		case (TOK_REAL_OP_AL):
		case (TOK_SEP_PARENTHESES_R):
		case (TOK_SEP_SEMICOLON):
		case (TOK_SEP_COMMA):
		case (TOK_SEP_BRACKETS_R):
		case (TOK_REAL_OP_SE):
			fprintf(syntactic_output, "Rule (EXPR_TAG -> ϵ) \n");
		//	current_token = back_token();
			returned_type = VOID;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_OP_ADD' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 10);
			returned_type = VOID;
			break;
	}
	return returned_type;
}

elementType Parse_TERM()
{
	elementType leftType, rightType, returned_type = ERROR_TYPE;
	fprintf(syntactic_output, "Rule (TERM -> FACTOR TERM_TAG)\n");
	leftType = Parse_FACTOR();
	rightType = Parse_TERM_TAG();
	if (rightType == ERROR_TYPE || leftType == ERROR_TYPE)
	{
		returned_type = ERROR_TYPE;
	}
	else if (leftType == FLOAT)
	{
		returned_type = FLOAT;
	}
	else if (rightType == FLOAT && leftType == INT)
	{
		returned_type = ERROR_TYPE;
	}
	else
	{
		returned_type = INT;
	}

	return returned_type;
}

elementType Parse_TERM_TAG()
{
	elementType leftType, rightType, returned_type = ERROR_TYPE;
	Token *current_token = next_token();
	eTOKENS follow_array[12] = { TOK_SEP_PARENTHESES_R , TOK_SEP_SEMICOLON  , TOK_SEP_COMMA  ,TOK_SEP_BRACKETS_R ,TOK_OP_ADD ,TOK_REAL_OP_SE ,TOK_REAL_OP_BE ,TOK_REAL_OP_AR , TOK_REAL_OP_NE ,TOK_REAL_OP_AL, TOK_REAL_OP_IE, TOK_SEP_CURLY_BRACES_R };
	switch (current_token->kind)
	{
		case (TOK_OP_MUL):
			fprintf(syntactic_output, "Rule (TERM_TAG -> * FACTOR TERM_TAG)\n");
			leftType = Parse_FACTOR();
			rightType = Parse_TERM_TAG();
			if (rightType == ERROR_TYPE || leftType == ERROR_TYPE)
			{
				returned_type = ERROR_TYPE;
			}
			else if (leftType == FLOAT)
			{
				returned_type = FLOAT;
			}
			else if (rightType == FLOAT && leftType == INT) {

				returned_type = ERROR_TYPE;
			}
			else
			{
				returned_type = INT;
			}
			break;
		case (TOK_OP_ADD):
		case (TOK_REAL_OP_SE):
		case (TOK_REAL_OP_IE):
		case (TOK_REAL_OP_BE):
		case (TOK_REAL_OP_AR):
		case (TOK_REAL_OP_NE):
		case (TOK_REAL_OP_AL):
		case (TOK_SEP_PARENTHESES_R):
		case (TOK_SEP_SEMICOLON):
		case (TOK_SEP_COMMA):
		case (TOK_SEP_BRACKETS_R):
		case (TOK_SEP_CURLY_BRACES_R):
			fprintf(syntactic_output, "Rule ((TERM_TAG ->  ϵ) \n");
			current_token = back_token();
			returned_type = VOID;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type: 'TOK_OP_MUL' or 'TOK_SEP_PARENTHESES_R' or 'TOK_SEP_SEMICOLON' or 'TOK_SEP_COMMA' or 'TOK_SEP_BRACKETS_R' or 'TOK_OP_ADD' or 'TOK_REAL_OP_SE' or 'TOK_REAL_OP_IE' or 'TOK_REAL_OP_BE' or 'TOK_REAL_OP_AR' or 'TOK_REAL_OP_NE' or 'TOK_REAL_OP_AL' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 12);
			returned_type = VOID;
			break;
	}
	return returned_type;
}

elementType Parse_FACTOR()
{
	elementType returned_type = ERROR_TYPE, rightSideType = ERROR_TYPE, leftSideType = ERROR_TYPE;
	Token *current_token = next_token();
	tableEntry current_entry = NULL;
	eTOKENS follow_array[13] = { TOK_SEP_PARENTHESES_R , TOK_SEP_SEMICOLON  , TOK_SEP_COMMA  ,TOK_SEP_BRACKETS_R ,TOK_OP_ADD , TOK_OP_MUL ,TOK_REAL_OP_SE ,TOK_REAL_OP_BE ,TOK_REAL_OP_AR , TOK_REAL_OP_NE ,TOK_REAL_OP_AL, TOK_REAL_OP_IE };
	switch (current_token->kind)
	{
		case (TOK_SEP_PARENTHESES_L):
			fprintf(syntactic_output, "Rule (FACTOR ->  (EXPR))\n");
			returned_type = Parse_EXPR();
			match(TOK_SEP_PARENTHESES_R);
			break;
		case (TOK_ID):
			fprintf(syntactic_output, "Rule (FACTOR ->  id FACROT_TAG)\n");
			current_entry = find(currTable, current_token->lexeme);
			if (!current_entry)
			{
				fprintf(semantic_output, "ERROR!! Using undeclared variable '%s'. Line number: %d\n", current_token->lexeme, current_token->lineNumber);
			}
			else
			{
				current_entry->isUsed = 1;
				leftSideType = current_entry->varType;
			}
			rightSideType = Parse_FACTOR_TAG();
			if (rightSideType == ERROR_TYPE || leftSideType == ERROR_TYPE)
			{
				returned_type = ERROR_TYPE;
			}
			else if (leftSideType == FLOAT)
			{
				returned_type = FLOAT;
			}
			else if (rightSideType == FLOAT && leftSideType == INT) {
				returned_type = ERROR_TYPE;
			}
			else {
				returned_type = INT;
			}
			break;
		case (TOK_FLOAT_NUM):
			fprintf(syntactic_output, "Rule (FACTOR ->  float_num)\n");
			returned_type = FLOAT;
			break;
		case (TOK_INT_NUM):
			fprintf(syntactic_output, "Rule (FACTOR ->  int_num)\n");
			returned_type = INT;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type:  'TOK_ID' or 'TOK_FLOAT_NUM' or 'TOK_INT_NUM'  or 'TOK_SEP_PARENTHESES_L' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 13);
			break;
	}
	return returned_type;

}

elementType Parse_FACTOR_TAG() {
	elementType typeToReturn = ERROR_TYPE;
	tableEntry current_entry = NULL;
	int assignedParams, isParam = 0,isUsed = 1;
	Token *current_token = next_token();
	eTOKENS follow_array[13] = { TOK_SEP_PARENTHESES_R , TOK_SEP_SEMICOLON  , TOK_SEP_COMMA  ,TOK_SEP_BRACKETS_R ,TOK_OP_ADD , TOK_OP_MUL ,TOK_REAL_OP_SE ,TOK_REAL_OP_BE ,TOK_REAL_OP_AR , TOK_REAL_OP_NE ,TOK_REAL_OP_AL, TOK_REAL_OP_IE , TOK_SEP_CURLY_BRACES_R};
	switch (current_token->kind)
	{
		case (TOK_SEP_PARENTHESES_L):
			fprintf(syntactic_output, "Rule (FACTOR_TAG ->  ( ARGS ) )\n");
			current_token = back_token(); // go back to id 
			current_entry = find(currTable, current_token->lexeme);
			if (current_entry == NULL)
			{
				fprintf(semantic_output, "ERROR!! The function '%s' is used but never declared. Line number: %d \n", current_token->lexeme, current_token->lineNumber);
			}
			else
			{
				typeToReturn = current_entry->funcType;
				current_entry->isUsed = 1;
			}

			current_token = next_token();
			assignedParams = Parse_ARGS();
			if (current_entry != NULL)
			{
				if (current_entry->paramsNum != assignedParams)
				{
					fprintf(semantic_output, "ERROR!! function-call and definition of function '%s' contain different number of parameters. Line number: %d \n", current_entry->name, current_token->lineNumber);
				}
			}

			match(TOK_SEP_PARENTHESES_R);
			break;
		case (TOK_SEP_BRACKETS_L):
			fprintf(syntactic_output, "Rule (FACTOR_TAG ->  VAR)\n");
			current_token = back_token();
			current_entry = find(currTable, current_token->lexeme);
			Parse_VAR(VOID, isParam, isUsed); // go bak to id 
			if (current_entry != NULL)
			{
				typeToReturn = current_entry->varType;
				current_entry->isUsed = 1;
			}
			else
			{
				fprintf(semantic_output, "ERROR!! The variable '%s' is used but never declared. Line number: %d \n", current_token->lexeme, current_token->lineNumber);
			}
			break;
		case (TOK_SEP_BRACKETS_R):
		case (TOK_OP_EQUAL):
		case (TOK_OP_MUL):
		case (TOK_SEP_PARENTHESES_R):
		case (TOK_SEP_SEMICOLON):
		case (TOK_SEP_COMMA):
		case (TOK_OP_ADD):
		case (TOK_REAL_OP_SE):
		case (TOK_REAL_OP_IE):
		case (TOK_REAL_OP_BE):
		case (TOK_REAL_OP_AR):
		case (TOK_REAL_OP_NE):
		case (TOK_REAL_OP_AL):
		case (TOK_SEP_CURLY_BRACES_R):
			fprintf(syntactic_output, "Rule (FACTOR_TAG ->  ϵ) \n");
			current_token = back_token();
			typeToReturn = VOID;
			break;
		default:
			fprintf(syntactic_output, "Expected token of type:  'TOK_ID' or 'TOK_FLOAT_NUM' or 'TOK_INT_NUM'  or 'TOK_SEP_PARENTHESES_L' at line: %d \n", current_token->lineNumber);
			fprintf(syntactic_output, "Actual token: '%s', lexeme:'%s'.\n", token_kind_to_string(current_token->kind), current_token->lexeme);
			error_handler(follow_array, 13);
			typeToReturn = VOID;	
			break;
	}
	return typeToReturn;

}



void printEntries() 
{
	entry* entry1, * temp;

	HASH_ITER(hh, currTable->data, entry1, temp)
	{
		printf("entry name %s\n", entry1->name);
		/* ... it is safe to delete and free s here */
	}
}

const char* typeToStringConverter(type num)
{
	char* validTok[] = { "INT","FLOAT","ERROR_TYPE","VOID" , "NUM_INT", "NUM_FLOAT" };
	return validTok[num];
}



void checkIfAllVarsUsed(tablePtr curr_scope)
{
	tableEntry iterator, temp, original;


	HASH_ITER(hh, currTable->data, iterator, temp)
	{
		if (iterator->isUsed == 0)
		{
			if (iterator->isFunc == 0 && iterator->isProto == 0)
			{
				fprintf(semantic_output, "Warning!! The variable '%s' is declared but never used. Line number: %d \n", iterator->name, iterator->lineNum);
				original = find(currTable->next_table, iterator->name);
				if (original)
				{
					original->isUsed = 1;
				}
			}
		}
	}
}



void checkIfReturnExistInBlock(tablePtr curr_scope)
{
		tableEntry iterator, temp, originalEntry;

		HASH_ITER(hh, curr_scope->data, iterator, temp)
		{
			if ((iterator->returnExist == 0) && (iterator->varType != VOID) && (iterator->isFunc == 1) )
			{
				fprintf(semantic_output, "ERROR!! Return statement not exist in function: '%s', the return statment is different from VOID. Function declaration in line number: %d \n", iterator->name, iterator->lineNum);
			}
			iterator->returnExist = 1;
			originalEntry = find(curr_scope->next_table, iterator->name);
			if (originalEntry)
			{
				originalEntry->returnExist = 1;
			}
		}
}

void checkIfAllProtoUsed(tablePtr curr_scope)
{
	tableEntry iterator, temp, original;

	HASH_ITER(hh, currTable->data, iterator, temp)
	{
		if (iterator->isUsed == 0)
		{
			if (iterator->isFunc == 0 && iterator->isProto == 1)
			{
				fprintf(semantic_output, "Warning!! The prototype '%s' is declared but his full declaration not found . Line number: %d \n", iterator->name, iterator->lineNum);
				original = find(currTable->next_table, iterator->name);
				if (original)
				{
					original->isUsed = 1;
					original->isFunc = 1;
				}
			}
		}
	}
}


void addParamToFunc(tablePtr curr_scope)
{
	tablePtr father = curr_scope->next_table, son = curr_scope;
	tableEntry fatherData = father->data, sonData = son->data, iterator, temp, copyEntry;

	HASH_ITER(hh, fatherData, iterator, temp)
	{
		if (iterator->isParam == 1)
		{
			copyEntry = insert(son, iterator->name);
			if (copyEntry)
			{
				copyEntry->isUsed = iterator->isUsed;
				copyEntry->lineNum = iterator->lineNum;
				copyEntry->isFunc = iterator->isFunc;
				copyEntry->isParam = iterator->isParam;
				copyEntry->name = iterator->name;
				copyEntry->varType = iterator->varType;
			}
		}
	}
}

void addPrevFuncEntriesToTable(tablePtr curr_scope)
{
	tablePtr father = curr_scope->next_table;
	tableEntry funcEntry, tempEntryCopy;
	tableEntry iterator, temp;

	if (father && father->data)
	{
		HASH_ITER(hh, father->data, iterator, temp)
		{
			if (iterator->isFunc == 1)
			{
				tempEntryCopy = insert(curr_scope, iterator->name);
				if (tempEntryCopy)
				{
					tempEntryCopy->isUsed = iterator->isUsed;
					tempEntryCopy->lineNum = iterator->lineNum;
					tempEntryCopy->isFunc = iterator->isFunc;
					tempEntryCopy->name = iterator->name;
					tempEntryCopy->varType = iterator->varType;
					tempEntryCopy->paramsNum = iterator->paramsNum;
					tempEntryCopy->returnExist = iterator->returnExist;
				}
			}
		}
	}
}



void addElement(int numToAdd)
{
	int* tempArr;

	if (arrSize == 0) // no array defined
	{
		arrSize = arrSize + 1;
		tempArr = (int)calloc(size, sizeof(int));
		tempArr[arrSize - 1] = numToAdd;
	}
	else
	{
		arrSize = arrSize + 1;
		tempArr = (int)realloc(size, arrSize * sizeof(int));
		tempArr[arrSize - 1] = numToAdd;
	}

	size = tempArr;
}


tableEntry findFuncEntry(tablePtr curr_scope)
{
	tableEntry iterator, temp, result = NULL;

	HASH_ITER(hh, curr_scope->data, iterator, temp) {
		if (iterator->isFunc == 1)
		{
			result = iterator;
			break;
		}
	}

	if (result)
		return result;
	else
		return NULL;
}
