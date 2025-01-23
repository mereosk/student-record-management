#include <stdlib.h>
#include "func.h"

int *create_int(int i){
    int* pointer = malloc(sizeof(int));
    *pointer = i;
    return pointer;
}


int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}