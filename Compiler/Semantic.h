#ifndef	SEMANTIC
#define SEMANTIC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h"

// to do:
// bilbuley sehel



typedef enum type {
	INT,
	FLOAT,
	ERROR_TYPE,
	VOID,
	NUM_INT,
	NUM_FLOAT
}type;

typedef enum role {
	var,
	preDef,
	fullDef
}role;


typedef struct parameter {
	char* lexeme;
	type var_type;
} parameter;



// to do- change errors

typedef struct tableSymbol {
	/*
	The UT_hash_handle field must be present in our structure. It is used for the internal bookkeeping that makes the hash work.
	It does not require initialization. This allows us to use the easier "convenience" macros to add, find and delete items.
	*/
	UT_hash_handle hh;
	int isUsed; // flag to check if used
	int lineNum;
	int isFunc; // flag to check if variable or function
	int isProto; // flag to check if variable or function prototype
	//type returned_value;

	//relevant for variables
	char* name;
	type varType;
	int isParam;
	int* size; // if size[0]  = 0 regular var else this is in array array dimensions is: [int_num, int_num, ...... ] 
	int arrSize;

	//relevant for functions prototype
	int paramsNum;
	char* paramsList;
	type funcType;

	//relevant for full budy functions
	int hasProto;
	int returnExist;

} entry;

typedef struct tableNode {
	struct tableNode* next_table;
	entry* data;
} tableNode;

typedef tableNode* tablePtr; //pointer to table
typedef entry* tableEntry; //pointer to entry in the table
typedef type elementType; //pointer to type

tablePtr makeTable(tablePtr curr_table);
tablePtr popTable(tablePtr curr_table);
tableEntry find(tablePtr curr_table, char* id_name);
tableEntry insert(tablePtr curr_table, char* id_name);
tableEntry lookup(tablePtr curr_table, char* id_name);
elementType get_id_type(tableEntry id_entry);
void set_id_type(tableEntry id_entry, elementType id_type);
void set_size(tableEntry id_entry, int size);




#endif

