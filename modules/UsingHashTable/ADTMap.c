/////////////////////////////////////////////////////////////////////////////
//
// Implementation of ADT Map with Hash Table using buckets
//
/////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>

#include "ADTMap.h"

// Struct of every node that a hash table has
struct map_node{
	Pointer key;
	Pointer value;
};

// Struct of Map (it has all the information that a hash table needs)
struct map {
	List* array;				// The array of lists that I will use for map
	int capacity;				// The capacity of the hash table
	int size;					// How many elemetns I have inserted
	CompareFunc compare;		
	HashFunc hash_function;		
	DestroyFunc destroy_key;	
	DestroyFunc destroy_value;
};


Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value,int capacity) {
	// Allocating the space I need for the hash table
	Map map = malloc(sizeof(*map));
	// The capacity is given as an variable
	map->capacity = capacity;
	map->array = malloc(map->capacity * sizeof(map->array));

	// Initialiazation of the nodes
	for (int i = 0; i < map->capacity; i++)
		map->array[i] = list_create(NULL);

	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

// Returns the number of entries of the map at a time
int map_size(Map map) {
	return map->size;
}


// Insertion of the tuple (key, item). If the key exists then skip the insertion,
// because I dont want duplicates to exist so it returns false and its mngstd's
// responsibility to handle them, on the other way the function returns true

bool map_insert(Map map, Pointer key, Pointer value) {
	// Scanning the list so that I can see if a node with the same key exists
	int pos = map->hash_function(key) % map->capacity;		 // In pos position the key is hashing

	bool already_in_map = false;

	ListNode node;
	for(node = list_first(map->array[pos]);
	node != LIST_EOF;
	node = list_next(map->array[pos],node)){
		MapNode m_node = (MapNode)list_node_value(map->array[pos],node);
		if(map->compare(key, m_node->key) == 0){
			already_in_map = true;
			return false;
		}
	}

	if (!already_in_map) {
		// New element, increasing the els of the map
		map->size++;

		MapNode node = malloc(sizeof(*node));
		node->key = key;
		node->value = value;

		list_insert(map->array[pos], LIST_BOF, node);
	}

	return true;
}

// This insert function is used to implement func get_top_N_postals. The difference
// with the one before is that this one once it detects a duplicate it replacing the
// key with the new one and it increases the old value by one. Doing this allow me to 
// detect how many zip codes there are

void map_insert_post(Map map, Pointer key, Pointer value) {
	// Scanning the list so that I can see if a node with the same key exists
	int pos = map->hash_function(key) % map->capacity;		 // In pos position the key is hashing

	bool already_in_map = false;

	ListNode node;
	for(node = list_first(map->array[pos]);
	node != LIST_EOF;
	node = list_next(map->array[pos],node)){
		MapNode m_node = (MapNode)list_node_value(map->array[pos],node);
		if(map->compare(key, m_node->key) == 0){
			already_in_map = true;
			// Replacing old key and destroying it
			if(map->destroy_key != NULL){
				map->destroy_key(m_node->key);
			}
			m_node->key = key;
			free(value);
			// Increases the value by 1
			(*(int*)m_node->value)++;
			return;
		}
	}

	if (!already_in_map) {
		// New element, increasing the elements of the map
		map->size++;

		MapNode node = malloc(sizeof(*node));
		node->key = key;
		node->value = value;

		list_insert(map->array[pos], LIST_BOF, node);
	}
}


// Delete the key from the hash table
bool map_remove(Map map, Pointer key) {
	int pos = map->hash_function(key) % map->capacity;
	bool found = false;

	MapNode mnode;
	ListNode lnode;
	for(lnode = list_first(map->array[pos]);
	lnode != LIST_EOF;
	lnode = list_next(map->array[pos],lnode)){
		
		mnode = (MapNode)list_node_value(map->array[pos], lnode);
		if(map->compare(mnode->key, key) == 0){
			found = true;
			break;
		}
	}

	if(found){
		if (map->destroy_key != NULL)
			map->destroy_key(mnode->key);
		if (map->destroy_value != NULL)
			map->destroy_value(mnode->value);

		free(mnode);
		list_remove(map->array[pos], lnode);
	}
	else
		return false;
	

	map->size--;
	return true;
}

// Finds and returns the value of the key, if there is not such key in the hash table it returns NULL
Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}

// Free the memory allocated by the map
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if (list_size(map->array[i]) != 0) {
			for(ListNode node = list_first(map->array[i]);
			node != LIST_EOF;
			node = list_next(map->array[i],node)){
				MapNode m_node = (MapNode)list_node_value(map->array[i], node);
				if (map->destroy_key != NULL)
					map->destroy_key(m_node->key);
				if (map->destroy_value != NULL)
					map->destroy_value(m_node->value);
				
				free(m_node);
			}
		}
		list_destroy(map->array[i]);
	}

	free(map->array);
	free(map);
}

// Map traversal ////

MapNode map_first(Map map) {
	// Go through the list array and find the first non empty bucket, then return the first mapnode of the list
	for(int i = 0 ; i<map->capacity ; i++){
		if(list_size(map->array[i]) != 0)
			return list_node_value(map->array[i], list_first(map->array[i]));
	}

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	int pos = map->hash_function(node->key) % map->capacity;	// In pos position the key is hashing

	// First find the position of the node it the list it hashed
	ListNode lnode;
	MapNode m_node;
	for(lnode = list_first(map->array[pos]);
	lnode != LIST_EOF;
	lnode = list_next(map->array[pos], lnode)){
		m_node = (MapNode)list_node_value(map->array[pos], lnode);
		if(map->compare(node->key, m_node->key) == 0){
			break;
		}
	}
	ListNode list_node = list_next(map->array[pos], lnode);
	// If node is not the last list node of the current list return the next list node
	if(list_node != LIST_EOF) {
		return list_node_value(map->array[pos], list_node);
	}
	else {											// If it is the last find the next list and return its first list node
		for(int i = pos+1 ; i<map->capacity ; i++)
			if(list_size(map->array[i]) != 0)
				return (MapNode)list_node_value(map->array[i], list_first(map->array[i]));
	}
	
	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	
	int pos = map->hash_function(key) % map->capacity;		// In pos position the key is hashing
	
	for(ListNode node = list_first(map->array[pos]);
	node != LIST_EOF;
	node = list_next(map->array[pos], node)){
		MapNode m_node = (MapNode)list_node_value(map->array[pos],node);
		if(map->compare(key, m_node->key) == 0)
			return m_node;
	}
	return MAP_EOF;
}

// Initialization of map's hash function
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

int hash_int(Pointer value) {
	return *(int*)value;
}

void get_top_N_postals(Map map, int rank, int hash_size) {
	// If the map size is 0 then there aren't any students in the struct
	if(!map_size(map)){
		printf("No students are enrolled");
		return;
	}

	// Create another map so that I can copy all the values from the main map with
	// the key being the zip code and the value a integer number which counts zip codes
	Map tmap = map_create(compare_ints, free, free, hash_size);
	map_set_hash_function(tmap, hash_int);

	for(MapNode mnode = map_first(map);
	mnode != MAP_EOF;
	mnode = map_next(map, mnode)) {
		Record record = map_node_value(map, mnode);
		map_insert_post(tmap, create_int(record->zip), create_int(1));
	}

	// If there are not that many zip codes as the input prompt the user for a smaller rank
	if(map_size(tmap) < rank){
		printf("Please give a smaller rank (should be <= %d)\n", map_size(tmap));
		map_destroy(tmap);
		return;
	}
	// Find the 'rank' top postal code
	find_N_postal(tmap, rank);
	map_destroy(tmap);
}

void find_N_postal(Map map, int rank) {
	// Every time I find a top zip code that is not rank'th I save it in the list and keep searching
	List temp_list = list_create(free);
	int temp_rank = rank;

	int max_zip_oc;
	int max_zip;
	// Outer loop is to find the rank'th zip code
	while(temp_rank--) {
		int *occurences;
		int *zip;
		
		max_zip_oc = 0;
		// Finds the value of the top rank'th zip code
		MapNode mnode = map_first(map);
		while(mnode != MAP_EOF) {
			occurences = map_node_value(map, mnode);
			zip = map_node_key(map, mnode);
			if(*occurences >= max_zip_oc) {
				max_zip_oc = *occurences;
				max_zip = *zip;
			}
			mnode = map_next(map, mnode);
		}
		// All below is to remove the zip code if it is not the wanted one
		int *p_zip;
		p_zip = &max_zip;
		MapNode temp_node = malloc(sizeof(*temp_node));
		temp_node->key = create_int(max_zip);
		temp_node->value = create_int(max_zip_oc);
		list_insert(temp_list, LIST_BOF, temp_node);
		map_remove(map, p_zip);
	}
	// Restores the saved zip codes in map
	for(ListNode lnode = list_first(temp_list);
	lnode != LIST_EOF; 
	lnode = list_next(temp_list, lnode)) {
	 	MapNode mnode = list_node_value(temp_list, lnode);
		map_insert_post(map, mnode->key, mnode->value);
	}

	list_destroy(temp_list);

	// Looping again through the map so that I can find all the zip codes that they occured rank times
	int students;
	for(MapNode mnode = map_first(map);
	mnode != MAP_EOF;
	mnode = map_next(map, mnode)) {
		int* key = map_node_key(map, mnode);
		int *value = map_node_value(map, mnode);
		if(*value == max_zip_oc){
			printf("[%d], ",*key);
			students = *value;
		}
	}

	printf("is/are %d most popular with %d student(s)\n", rank, students);
}