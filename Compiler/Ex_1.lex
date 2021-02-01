%option noyywrap

%{	
#include "Token.h"
#include "Parser.h"
#include "Semantic.h"
int lineNum=1;
FILE *syntactic_output = NULL, *semantic_output = NULL;
%}

LETTER    [A-Za-z]
DIGIT       [0-9]
INT 	[1-9][0-9]*|0
FLOAT	(0|([1-9][0-9]*))"."[0-9]+['E'|'e'][+-]?(0|([1-9][0-9]*))
ID 	[a-z][['_']*[A-Za-z]*[0-9]*]*[[A-Za-z][0-9]]+

add "+"  
asterisk "*"
arrowLeft "<" 
arrowRight ">" 
smallEqual "<="
bigEqual ">=" 
isEqual "=="
notEqual "!="
equal "="
comma ","
colon ":"
semicolon ";"
parenthesesLeft "("
parenthesesRight ")" 
bracketsLeft "["
bracketsRight "]"
curlyBracesLeft "{" 
curlyBracesRight "}" 



%x comment



%%



"int"	{fprintf(yyout, "Token of type '{KEYWORD_INT}', lexeme: '{%s}', found in line: {%d}  \n", yytext, lineNum); create_and_store_token(TOK_KW_INT,yytext,lineNum); return 1;}

"float"	{fprintf(yyout, "Token of type '{KEYWORD_FLOAT}', lexeme: '{%s}', found in line: {%d}  \n", yytext, lineNum); create_and_store_token(TOK_KW_FLOAT,yytext,lineNum); return 1;}

"if"	{fprintf(yyout, "Token of type '{KEYWORD_IF}', lexeme: '{%s}', found in line: {%d}  \n", yytext, lineNum); create_and_store_token(TOK_KW_IF,yytext,lineNum); return 1;}

"return" {fprintf (yyout, "Token of type '{RETURN}', lexeme: '{%s}', found in line: {%d}  \n", yytext, lineNum); create_and_store_token(TOK_KW_RETURN,yytext,lineNum); return 1;}

"void"	{fprintf(yyout, "Token of type '{KEYWORD_VOID}', lexeme: '{%s}', found in line: {%d}  \n", yytext, lineNum); create_and_store_token(TOK_KW_VOID,yytext,lineNum); return 1;}

{INT} {fprintf(yyout, "Token of type '{INT_NUMBER}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_INT_NUM,yytext,lineNum); return 1;}

{FLOAT} {fprintf(yyout, "Token of type '{FLOAT_NUMBER}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_FLOAT_NUM,yytext,lineNum); return 1;}

([a-z]({LETTER}|{DIGIT})*)|{LETTER}+({DIGIT}|{LETTER})*("_"({DIGIT}|{LETTER})+)* {fprintf(yyout, "Token of type '{ID}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_ID,yytext,lineNum); return 1;}

{add} {fprintf(yyout, "Token of type '{OPERATOR_ADD}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_OP_ADD,yytext,lineNum); return 1;}

{asterisk} {fprintf(yyout, "Token of type '{OPERATOR_MUL}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_OP_MUL,yytext,lineNum); return 1;}

{arrowLeft}  {fprintf(yyout, "Token of type '{ARROW_LEFT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_REAL_OP_AL,yytext,lineNum); return 1;}

{arrowRight}  {fprintf(yyout, "Token of type '{ARROW_RIGHT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_REAL_OP_AR,yytext,lineNum); return 1;}

{smallEqual} {fprintf(yyout, "Token of type '{SMALL_EQUAL}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_REAL_OP_SE,yytext,lineNum); return 1;}

{bigEqual} {fprintf(yyout, "Token of type '{BIG_EQUAL}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_REAL_OP_BE,yytext,lineNum); return 1;}

{notEqual} {fprintf(yyout, "Token of type '{NOT_EQUAL}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_REAL_OP_NE,yytext,lineNum); return 1;}

{isEqual}	{fprintf(yyout, "Token of type '{IS_EQUAL}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_REAL_OP_IE,yytext,lineNum); return 1;}

{equal}  {fprintf(yyout, "Token of type '{EQUAL}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_OP_EQUAL,yytext,lineNum); return 1;}

{comma}	{fprintf(yyout, "Token of type '{SEPARATION_COMMA}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum);create_and_store_token(TOK_SEP_COMMA,yytext,lineNum); return 1;}

{semicolon}  {fprintf(yyout, "Token of type '{SEPARATION_SEMICOLON}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_SEMICOLON,yytext,lineNum); return 1;}

{colon} {fprintf(yyout, "Token of type '{SEPARATION_COLON}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_COLON,yytext,lineNum); return 1;}
						
{parenthesesLeft}	{fprintf(yyout, "Token of type '{SEPARATION_PARENTHESES_LEFT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_PARENTHESES_L,yytext,lineNum); return 1;}

{parenthesesRight}	{fprintf(yyout, "Token of type '{SEPARATION_PARENTHESES_RIGHT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_PARENTHESES_R,yytext,lineNum); return 1;}

{bracketsRight}	{fprintf(yyout, "Token of type '{SEPARATION_BRACKETS_RIGHT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_BRACKETS_R,yytext,lineNum); return 1;}

{bracketsLeft}	{fprintf(yyout, "Token of type '{SEPARATION_BRACKETS_LEFT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_BRACKETS_L,yytext,lineNum); return 1;}

{curlyBracesLeft}	{fprintf(yyout, "Token of type '{SEPARATION_CURLY_BRACES_LEFT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_CURLY_BRACES_L,yytext,lineNum); return 1;}

{curlyBracesRight}	{fprintf(yyout, "Token of type '{SEPARATION_CURLY_BRACES_RIGHT}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_SEP_CURLY_BRACES_R,yytext,lineNum); return 1;}



"/*"         BEGIN(comment);
<comment>[^*\n]*        
<comment>"*"+[^*/\n]*   
<comment>\n             ++lineNum;
<comment>"*"+"/"        BEGIN(0); 

<<EOF>>  {fprintf(yyout, "Token of type '{END_OF_FILE}', lexeme: '{%s}', found in line: {%d} \n", yytext, lineNum); create_and_store_token(TOK_EOF,yytext,lineNum); return 0;}
									

[ ]
[\t]
[\n] 												 lineNum++;
.       											 fprintf(yyout, "Character {%s} in line: {%d} does not begin any legal token in the language.  \n", yytext, lineNum);



%%

int main(int argc, char **argv ){


yyin = fopen("c:\\temp\\test1.txt", "r");
yyout = fopen("c:\\temp\\test1_204189047_313162877_lex.txt", "w");
syntactic_output = fopen("c:\\temp\\test1_204189047_313162877_syntactic.txt", "w");
semantic_output = fopen("C:\\temp\\test1_204189047_313162877_semantic.txt", "w");
Parser();
fclose(yyin);
fclose(yyout);
fclose(syntactic_output);
fclose(semantic_output);
lineNum=1;


yyin = fopen("c:\\temp\\test2.txt", "r");
yyrestart(yyin);
yyout = fopen("c:\\temp\\test2_204189047_313162877_lex.txt", "w");
syntactic_output = fopen("c:\\temp\\test2_204189047_313162877_syntactic.txt", "w");
semantic_output = fopen("C:\\temp\\test2_204189047_313162877_semantic.txt", "w");
Parser();
fclose(yyin);
fclose(yyout);
fclose(syntactic_output);
fclose(semantic_output);
lineNum=1;

}


