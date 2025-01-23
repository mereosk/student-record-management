///////////////////////////////////////////////////////////////////
//
// Inverted Index
//
///////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include "common_types.h"
#include "ADTBList.h"

typedef struct inverted_index* InvertedIndex;
typedef struct inverted_index_list* InvertedIndexList;

// Creates and returns a new InvertedIndex
// If destroy_func != NULL the it calls destroy_func(value) every time an element is removed.

InvertedIndex inverted_index_create(DestroyFunc destroy_func, CompareFunc compare_year);

// Inserts a value in an InvertedIndex. InvertedIndex has a list that has InvertedIndexList's.
// They have lists with 'value'. They also have 'year' which is the value that desides in which
// InvertedIndexList the value will be inserted.

void inverted_index_insert(InvertedIndex inve_index, Pointer value);

// Returns the size of all the values of the InvertedIndex (in this Records).

int inverted_index_get_students(InvertedIndex inv_index);

// Returns the size of all the values in an inner InvertedIndexList (in this Records).

int inverted_index_get_students_year(InvertedIndex inverted_index, int year);

// Returns the InvertedIndexList that has year = 'year' o NULL if it doesn't find it.

InvertedIndexList find_inverted_index_list(InvertedIndex inverted_index, int year);

// Removes value from an InvertedIndex. It return true or false. If after the deletion
// the inner list of InvertedIndexList becomes empty then it destroys it.

bool inverted_index_remove(InvertedIndex inv_index, Pointer value);

// Destroys an InvertedIndex, but in this spesific excercise it doesn't delete
// the value (in this case the Record) because it is the map's job to do it.

void inverted_index_destroy(InvertedIndex inv_index);

// Returns the average gpa of a specific year in an InvertedIndexList.

float average_year(InvertedIndex inverted_index, int year);

// It finds the minimum value of a specific year in an InvertedIndexList.
// Then it prints all the students that has this gpa or an error message 
// if in this year there aren't students enrolled.

void minimum(InvertedIndex inv_index, int year);

// Fints the minimum gpa and returns it. Also return the id of the student
// that has it.

float find_minimum_gpa(List list, Record *student);

// It deletes a list node.

void delete_list_node(List list, Pointer value);

// Copies all the values of temp_list in list.

void list_restoration(List list, List temp_list);

// Finds the top num students in a specific year and prints them
// or prints an error message if there are not students enrolled.

void top_students(InvertedIndex inv_index, int num, int year);

// Finds the maximum gpa of a student in an InvertedIndexList.

Record find_maximum_gpa(List list);

// It prints how many students are in each year or an error message
// if there are not students enrolled.

void count(InvertedIndex inv_index);