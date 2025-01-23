///////////////////////////////////////////////////////////
//
// ADT Map
//
///////////////////////////////////////////////////////////

#pragma once

#include "common_types.h"
#include "ADTBList.h"
#include "func.h"
#include "records.h"

typedef struct map* Map;

// Creates and returns a map in which the elements are comparing with the compare func.
// If destory_key and/or destory_value != NULL, then it calls destroy_key(key) or/and
// destroy_value(value) every time an element is removed

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value, int capacity);

// Returns the number of elements that the map has.

int map_size(Map map);

// Inserts the key with 'value' as value. If the key exists then the it returns false or else true.

bool map_insert(Map map, Pointer key, Pointer value);

void map_insert_post(Map map, Pointer key, Pointer value);

// Removes the key if it exists and it returns true, or else it returns false.

bool map_remove(Map map, Pointer key);

// Returns the value that the key has or NULL if the key doesn't exist in the map.

Pointer map_find(Map map, Pointer key);

// Changes the function that is being called after removal/replacement of key/value.
// Returns the previous value of the function.

DestroyFunc map_set_destroy_key  (Map map, DestroyFunc destroy_key  );
DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value);

// Frees all the memory that the map has malloced

void map_destroy(Map map);



// Traversal of map /////

#define MAP_EOF (MapNode)0

typedef struct map_node* MapNode;

// Returns the first node of the map or MAP_EOF if the map is empty.

MapNode map_first(Map map);

// Returns the next node of 'node' or MAP_EOF if node doesn't has next.

MapNode map_next(Map map, MapNode node);

// Returns the key of node.

Pointer map_node_key(Map map, MapNode node);

// Returns the value of node.

Pointer map_node_value(Map map, MapNode node);

// Finds and return the node that has 'key' or MAP_EOF if the key doesn't exist.

MapNode map_find_node(Map map, Pointer key);


//Hashing Function ////

typedef int (*HashFunc)(Pointer);

int hash_int(Pointer value);			// Used when key is int*

// The map's hash function. It must be called right after map_create.

void map_set_hash_function(Map map, HashFunc hash_func);

// Get the rank most popular towns in the map.

void get_top_N_postals(Map map, int rank, int hash_size);

// Helper fucntion to above one

void find_N_postal(Map map, int rank);

