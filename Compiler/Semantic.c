#define _CRT_SECURE_NO_WARNINGS
#include "semantic.h"
#include "Parser.h"
#include "Token.h"


// to do: change errors

tablePtr makeTable(tablePtr curr_table)
{
	
	tablePtr node;

	// If current table is empty (meaning, we don't have any symbol table at all) --> create new table and return it as the current table pointer
	if (!curr_table) {
		curr_table = (tableNode*)malloc(sizeof(tableNode));
		curr_table->next_table = NULL;
		node = curr_table;
	}
	else { //If the current table is not empty, add new table to the stack and point the next table pointer to point to the "father" symbol table
		node = (tableNode*)malloc(sizeof(tableNode));
		node->next_table = curr_table;
	}

	node->data = NULL; //No data in the table yet, so it remains empty

	return node; //Return pointer of the newly created table. 
}

tablePtr popTable(tablePtr curr_table)
{
	entry* entry_to_free, * temp_entry;
	tablePtr father = curr_table->next_table;

	/*
	HASH_ITER - API function for iterating over a hash table.
	Gets:
	hh - hash table handler - described in semantic_analyzer.h
	curr_table->data - this is a pointer to the table to iterate
	node_to_free - pointer that will be used to iterate over the table (iterates over entries in the table).
	temp_node - temp pointer for saftey reasons - in order not to delete NULL fields (part of the API requriments)
	*/
	HASH_ITER(hh, curr_table->data, entry_to_free, temp_entry)
	{
		/*
		HASH_DEL - API function for deleting a specific entry from a hash table.
		Gets:
		curr_table->data - this is a pointer to the table to delete
		node_to_free - pointer that will be used to iterate over the table (iterates over entries in the table).
		*/
		HASH_DEL(curr_table->data, entry_to_free);
		//free(entry_to_free->name);

		free(entry_to_free);
	}
	free(curr_table->data);
	free(curr_table);

	return father; //Returns pointer to the "father" symbol table.
}

tableEntry find(tablePtr curr_table, char* id_name)
{
	tableEntry entry = NULL;

	while (curr_table != NULL)
	{
		entry = lookup(curr_table, id_name);
		if (entry != NULL) { //If we found the id in the symbol table (using the lookup function)
			return entry;
		}
		curr_table = curr_table->next_table; //If we didn'd find the id in the symbol table, go to the father symbol table and start searching again.
	}
	return NULL;
}


tableEntry insert(tablePtr curr_table, char* id_name) 
{

	tableEntry node = NULL;

	node = lookup(curr_table, id_name);
	if (node != NULL) //If this entry already exists in the symbol table
	{
		return NULL; //don't add it to the table
	}
	else
	{
		node = (entry*)malloc(sizeof(entry));
		node->name = (char*)malloc(sizeof(char) * strlen(id_name) + 1);
		strcpy(node->name, id_name);
		node->size = -1;
		/*
		HASH_ADD_STR - API function for adding element to a hash table.
		Gets:
		curr_table->data - this is a pointer to the table which we will add the element to .
		name - the field of the structure that will be used as the key that we want to add to the table.
		node - pointer to the new element we entered to the table
		*/
		HASH_ADD_STR(curr_table->data, name, node); //best
		return node;
	}
}

/*
Searches for a variable name in symbol table of the current scope (only);
returns pointer to the found entry, or NULL if the name is not found in that table
*/
tableEntry lookup(tablePtr curr_table, char* id_name) 
{
	tableEntry entry = NULL;

	/*
	HASH_FIND_STR - API function for finding an element from hash table by using key of type string.
	Gets:
	curr_table->data - this is a pointer to the hash table.
	entry - pointer of the entry we looked for in the table
	*/

	HASH_FIND_STR(curr_table->data, id_name, entry); //best
														//HASH_FIND_PTR(curr_table->data,id_name,entry); works
														//if(entry)
														//	fprintf(semantic_output,"id: %s found in the hash table.\n ",id_name);
	return entry;
}

/*
Performs hierarchical search of the variable name, starting from the symbol table of the current scope.
We use the lookup() function each time and check for the relevant entry.
In case we didn't find the relevant entry in the current table, we move to the "father" and continue checking.
Returns the found entry pointer, or NULL (if the variable is undeclared).
*/

elementType get_id_type(tableEntry id_entry)
{
	return id_entry->varType;
}


void set_id_type(tableEntry id_entry, elementType id_type)
{
	id_entry->varType = id_type;
}


void set_size(tableEntry id_entry, int size)
{
	id_entry->size = size;
}