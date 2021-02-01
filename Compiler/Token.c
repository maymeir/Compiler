#include "Token.h"
#define strdup _strdup
/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0, backTokenCnt = 0;
Node* currentNode = NULL;

#define TOKEN_ARRAY_SIZE 1000

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{ 
	int length = strlen(lexeme)+1;
	
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if(currentNode == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->tokensArray = 
			(Token*) calloc(sizeof(Token),TOKEN_ARRAY_SIZE);
		if(currentNode->tokensArray == NULL)
		{
			fprintf(yyout,"\nUnable to allocate memory! \n"); 
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if(currentNode == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = 
			 (Token*)calloc(sizeof(Token),TOKEN_ARRAY_SIZE);

			if(currentNode->tokensArray == NULL)
			{
				fprintf(yyout,"\nUnable to allocate memory! \n"); 
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;	
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char)*length);
	#ifdef _WIN32
		strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
	#else
		strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	#endif	

}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token* back_token()
{
	backTokenCnt++;
	//current token is the first token of the current node's array
	if (currentIndex == 0)
	{
		//case: previous node exists 
		if (currentNode->prev != NULL)
		{
			currentNode = currentNode->prev;
			currentIndex = TOKEN_ARRAY_SIZE - 1;
			return &(currentNode->tokensArray[currentIndex]);
		}
		//case: previous node not exists - therfore current token is first token
		return NULL;
	}

	//case: current token is not the first token of the array 
	currentIndex--;
	return &(currentNode->tokensArray[currentIndex]);
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/

Token* next_token()
{
	//case: create and store a new token 
	if (backTokenCnt == 0)
		yylex();
	//case: token is already stored
	else {
		backTokenCnt--;
		if (currentIndex == TOKEN_ARRAY_SIZE - 1) {
			currentNode = currentNode->next;
			currentIndex = 0;
		}
		else currentIndex++;
	}
	return &(currentNode->tokensArray[currentIndex]);
}


// to do 

void clearIndex() {
	currentIndex = 0;
}

const char* token_kind_to_string(eTOKENS index) {
	char* valid_tokens[] = { 
	"TOK_FLOAT_NUM",
	"TOK_INT_NUM",
	"TOK_ID",
	"TOK_KW_IF",
	"TOK_KW_FLOAT",
	"TOK_KW_INT",
	"TOK_KW_VOID",
	"TOK_KW_RETURN",
	"TOK_OP_EQUAL",
	"TOK_OP_ADD",
	"TOK_OP_MUL",
	"TOK_REAL_OP_NE",
	"TOK_REAL_OP_AR",
	"TOK_REAL_OP_SE",
	"TOK_REAL_OP_AL",
	"TOK_REAL_OP_IE",
	"TOK_REAL_OP_BE",
	"TOK_SEP_CURLY_BRACES_L",
	"TOK_SEP_CURLY_BRACES_R",
	"TOK_SEP_BRACKETS_L",
	"TOK_SEP_BRACKETS_R",
	"TOK_SEP_PARENTHESES_L",
	"TOK_SEP_PARENTHESES_R",
	"TOK_SEP_COMMA",
	"TOK_SEP_SEMICOLON",
	"TOK_SEP_COLON",
	"TOK_COMMENT",
	"TOK_EOF" };

	return valid_tokens[index];
}
