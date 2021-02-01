#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout, *syntactic_output, *semantic_output;

typedef enum eTOKENS
{
	TOK_FLOAT_NUM,
	TOK_INT_NUM,
	TOK_ID,
	TOK_KW_IF,
	TOK_KW_FLOAT,
	TOK_KW_INT,
	TOK_KW_VOID,
	TOK_KW_RETURN,
	TOK_OP_EQUAL,
	TOK_OP_ADD,
	TOK_OP_MUL,
	TOK_REAL_OP_NE,
	TOK_REAL_OP_AR,
	TOK_REAL_OP_SE,
	TOK_REAL_OP_AL,
	TOK_REAL_OP_IE,
	TOK_REAL_OP_BE,
	TOK_SEP_CURLY_BRACES_L,
	TOK_SEP_CURLY_BRACES_R,
	TOK_SEP_BRACKETS_L,
	TOK_SEP_BRACKETS_R,
	TOK_SEP_PARENTHESES_L,
	TOK_SEP_PARENTHESES_R,
	TOK_SEP_COMMA,
	TOK_SEP_SEMICOLON,
	TOK_SEP_COLON,
	TOK_COMMENT,
	TOK_EOF
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token *next_token();
Token *back_token();
const char* token_kind_to_string(eTOKENS index);
#endif