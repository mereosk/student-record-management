/////////////////////////////////////////////////////////////////////////////
//
// Implementation of Inverted Index
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "records.h"
#include "InvertedIndex.h"


struct inverted_index {
    List main_list;
    int num_of_students;
    DestroyFunc destroy_func;
    CompareFunc compare_years;
};

struct inverted_index_list {
    List secondary_list;
    Pointer year;
};

InvertedIndex inverted_index_create(DestroyFunc destroy_func, CompareFunc compare_year) {
    InvertedIndex inv_index = malloc(sizeof(*inv_index));
    if(!inv_index){
        printf("Error allocating memory!");
        exit(0);
    }

    inv_index->num_of_students = 0;
    inv_index->destroy_func = destroy_func;
    inv_index->main_list = list_create(inv_index->destroy_func);
    inv_index->compare_years = compare_year;

    return inv_index;
}


void inverted_index_insert(InvertedIndex inve_index, Pointer value) {

    bool check_if_exist = false;

    /* Check if year is already at the list */
    for(ListNode lnode = list_first(inve_index->main_list);
    lnode != LIST_EOF;
    lnode = list_next(inve_index->main_list, lnode)){
        InvertedIndexList inv_index_list = list_node_value(inve_index->main_list, lnode);
        int *year;
        year = &(((Record)value)->year);
        // If the below comparison is true then the list already
        // so just insert it in the secondary list
        if(inve_index->compare_years(year, inv_index_list->year) == 0){
            check_if_exist = true;
            list_insert(inv_index_list->secondary_list, LIST_BOF, value);
        }
    }
    
    /* If the year doesn't exist create a InvertedIndexList and the secondary list
       that the above struct has */
    if(!check_if_exist) {
        InvertedIndexList inv_index_list = malloc(sizeof(*inv_index_list));
        int *year = malloc(sizeof(int));
        *year = ((Record)value)->year;
        inv_index_list->year = year;

        inv_index_list->secondary_list = list_create(inve_index->destroy_func);
        list_insert(inv_index_list->secondary_list, LIST_BOF, value);

        list_insert(inve_index->main_list, LIST_BOF, inv_index_list);   // Insert the new struct in the main list
    }

    inve_index->num_of_students++;
}


int inverted_index_get_students(InvertedIndex inv_index) {
    return inv_index->num_of_students;
}


int inverted_index_get_students_year(InvertedIndex inverted_index, int year) {
    // If there are no students in the struct then just return 0, on the other hand return their number
    if(find_inverted_index_list(inverted_index, year) == NULL)
        return 0;
    else 
        return list_size(find_inverted_index_list(inverted_index, year)->secondary_list);
}


InvertedIndexList find_inverted_index_list(InvertedIndex inverted_index, int year) {

    // Find the InvertedIndexList and return it, if it doesn't exist then return NULL
    for(ListNode lnode = list_first(inverted_index->main_list);
    lnode != LIST_EOF;
    lnode = list_next(inverted_index->main_list, lnode)){

        InvertedIndexList inverted_index_list = list_node_value(inverted_index->main_list, lnode);
        int *p_year;
        p_year = &year;
        if(inverted_index->compare_years(p_year, inverted_index_list->year) == 0)
            return inverted_index_list;
    }

    return NULL;
}


bool inverted_index_remove(InvertedIndex inv_index, Pointer value) {
    // First find the InvertedIndexList
    for(ListNode lnode = list_first(inv_index->main_list);
    lnode != LIST_EOF;
    lnode = list_next(inv_index->main_list, lnode)){
        InvertedIndexList inverted_index_list = list_node_value(inv_index->main_list, lnode);
        // Find the value in the secondary list
        for(ListNode node = list_first(inverted_index_list->secondary_list);
        node != LIST_EOF;
        node = list_next(inverted_index_list->secondary_list, node)){
            Record record = list_node_value(inverted_index_list->secondary_list, node);

            if(*(int*)value == record->studentId) {
                // If the value has a destroy_func then use it to destroy it correctly
                if(inv_index->destroy_func != NULL) 
                    inv_index->destroy_func(record);

                // Remove it from the secondary list
                list_remove(inverted_index_list->secondary_list, node);
                inv_index->num_of_students--;

                // If there are no students in the secondary list destroy the whole InvertedIndexList
                // from the main list, because there is no reason to keep an empty secondary list
                if(!list_size(inverted_index_list->secondary_list)) {
                    list_destroy(inverted_index_list->secondary_list);
                    free(inverted_index_list->year);
                    free(inverted_index_list);
                    list_remove(inv_index->main_list, lnode);
                }
                return true;
            }
        }
    }
    // Value doesn't exist
    return false;
}


void inverted_index_destroy(InvertedIndex inv_index) {
    // First destroy all the InvertedIndexLists from the main list
    for(ListNode lnode = list_first(inv_index->main_list);
    lnode != LIST_EOF;
    lnode = list_next(inv_index->main_list, lnode)){
        InvertedIndexList inv_index_list = list_node_value(inv_index->main_list, lnode);
        list_destroy(inv_index_list->secondary_list);
        free(inv_index_list->year);
        free(inv_index_list);
    }

    //Then destroy the main list and the whole struct
    list_destroy(inv_index->main_list);
    free(inv_index);
}

float average_year(InvertedIndex inverted_index, int year) {
    float average;
    float sum = 0.0;

    // Find the InvertedIndexList and return its average. If it doesn't exist return -1.0
    InvertedIndexList inv_list = find_inverted_index_list(inverted_index, year);
    if(inv_list == NULL)
        return -1.0;
    else {
        for(ListNode lnode = list_first(inv_list->secondary_list);
        lnode != LIST_EOF;
        lnode = list_next(inv_list->secondary_list, lnode)){
            float gpa = ((Record)list_node_value(inv_list->secondary_list, lnode))->gpa;
            sum += gpa;
        }
    }

    average = sum/list_size(inv_list->secondary_list);
    return average;
}


void minimum(InvertedIndex inv_index, int year) {
    InvertedIndexList inv_list = find_inverted_index_list(inv_index, year);

    // Temp list helps me removing and inserted the records i want
    List temp_list = list_create(NULL);

    // If InvertedIndexList doesn't exist print an error message, if it does print
    // all the students that have the same minimum gpa
    if(!inv_list)
        printf("No students enrolled in %d\n",year);
    else {
        Record rec;
        // Find the minimum gpa
        float min = find_minimum_gpa(inv_list->secondary_list, &rec);
        printf("[%d]\n", rec->studentId);
        // Inserts the record in the temp list and removes it from the secondary list
        // so that I can keep searching for another student that has the same gpa
        list_insert(temp_list, LIST_BOF, rec);
        delete_list_node(inv_list->secondary_list, rec);
        
        // The below while does the same as before till there aren't other students 
        // with the same minimum gpa
        float eq_min;
        while(1) {
            Record rec;
            eq_min = find_minimum_gpa(inv_list->secondary_list, &rec);
            
            if(eq_min == min) {
                printf("[%d],\n", rec->studentId);
                list_insert(temp_list, LIST_BOF, rec);
                delete_list_node(inv_list->secondary_list, rec);
            }
            else            // There are no multiple minimums
                break;
        }

        // This function restores all the deleted values from the secondary list
        list_restoration(inv_list->secondary_list, temp_list);
        list_destroy(temp_list);
    }
}

float find_minimum_gpa(List list, Record *record) {
    // Finds the minimum gpa from a list of Records and returns it with the record that has it(with pointer)
    float min = 10.01;
    Record temp_rec;

    for(ListNode lnode = list_first(list);
    lnode != LIST_EOF;
    lnode = list_next(list, lnode)){
        Record record = (Record)list_node_value(list, lnode);
        if(record->gpa < min){
            min = record->gpa;
            temp_rec = record;
        }
    }

    *record = temp_rec;
    return min;
}


void delete_list_node(List list, Pointer value) {
    for(ListNode lnode = list_first(list);
    lnode != LIST_EOF;
    lnode = list_next(list, lnode)){
        int id = ((Record)list_node_value(list, lnode))->studentId;

        if(id == ((Record)value)->studentId){
            list_remove(list, lnode);
            return;
        }
    }
}

void list_restoration(List main_list, List temp_list) {
    // This function copies all the values of temp_list in the main_list
    for(ListNode lnode = list_first(temp_list);
    lnode != LIST_EOF;
    lnode = list_next(temp_list, lnode)){
        list_insert(main_list, LIST_BOF, list_node_value(temp_list, lnode));
    }
}


void top_students(InvertedIndex inv_index, int num, int year) {
    InvertedIndexList inv_list = find_inverted_index_list(inv_index, year);

    // Temp list helps me removing and inserted the records i want
    List temp_list = list_create(NULL);

    // If InvertedIndexList doesn't exist print an error message, if it does print
    // all the top students
    if(!inv_list)
        printf("No students enrolled in %d\n", year);
    else {
        int counter = 1;
        // This outer while is looping num times to take the num top students
        while(num--) {
            Record rec;
            if(!list_size(inv_list->secondary_list)) {     //If num is larger then the loop stops there
                list_restoration(inv_list->secondary_list, temp_list);
                list_destroy(temp_list);
                return;
            }
            // This function finds the student with the maximum gpa and prints it
            rec = find_maximum_gpa(inv_list->secondary_list);
            printf("%d) %d with %.2f\n",counter, rec->studentId, rec->gpa);
            // Inserts the record in the temp list and removes it from the secondary list
            // so that I can keep searching for studets with num'th maximum gpa
            list_insert(temp_list, LIST_BOF, rec);
            delete_list_node(inv_list->secondary_list, rec);
            counter++;
        }
        // This function restores all the deleted values from the secondary list
        list_restoration(inv_list->secondary_list, temp_list);
        list_destroy(temp_list);
    }
}


Record find_maximum_gpa(List list) {
    // Finds the maximum gpa from a list of Records and returns the record
    float max = 0.0;
    Record temp_rec;

    for(ListNode lnode = list_first(list);
    lnode != LIST_EOF;
    lnode = list_next(list, lnode)){
        Record record = (Record)list_node_value(list, lnode);
        if(record->gpa > max){
            max = record->gpa;
            temp_rec = record;
        }
    }

    return temp_rec;
}


void count(InvertedIndex inv_index) {
    if(!inv_index->num_of_students)
        printf("No students are enrolled\n");
    else {
        /* Print every student of the structure */
        for(ListNode lnode = list_first(inv_index->main_list);
        lnode != LIST_EOF;
        lnode = list_next(inv_index->main_list, lnode)){
            
            InvertedIndexList inv_list = list_node_value(inv_index->main_list, lnode);
            printf("[%d, %d]\n", *(int*)inv_list->year, list_size(inv_list->secondary_list));
        }
    }
}

