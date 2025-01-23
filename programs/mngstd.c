#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ADTMap.h"
#include "records.h"
#include "func.h"
#include "InvertedIndex.h"


int count_for_dup = 0;

// Below function passes as a destroy_function to Map so that a node will be deleted correctly

void freeRecord(Pointer r) {
    free(((Record)r)->fName);
    free(((Record)r)->lName);
    free((Record)r);
}

// For debugging purposes
void printRecords(Record r) {
    printf("%d %s %s %d %d %f\n"
    ,r->studentId, r->fName, r->lName,r->zip, r->year, r->gpa);
}

// Checks if all the elements of a list is num. The delimeter is space -> " "

bool checkString(char *str, int num) {
    int counter = 0;
    char *token;
    char s[2] = " ";
    char temp[100];

    strcpy(temp, str);

    token = strtok(temp, s);

    /* walk through other tokens */
    while( token != NULL ) {
            
        token = strtok(NULL, s);
        counter+=1;
    }

    if(counter != num)
        return false;
    return true;
}

// Inserts all the records from an inputfile in the structures

void insert_file_data(FILE *fp, Map map, InvertedIndex inv_index) {
    char* str = malloc(201*sizeof(char));
    if(!str) {
        printf("Error allocating memory!");
        exit(0);
    }

    while(fgets(str, 201, fp) != NULL){
        size_t ln = strlen(str)-1;       //Do not include \n
        if (str[ln] == '\n')
            str[ln] = '\0';
        // printf("%s\n",str);

        Record record = malloc(sizeof(*record));
        if(!record) {
            printf("Error allocating memory!");
            exit(0);
        }
        /* walk through other tokens */
        int number;
        float fnumber;
        int countForType = 0;
        char *token;
        char *s = " ";

        /* Get the first token */
        token = strtok(str, s);
        do {
            
            if(countForType == 0) {       //It is integer
                number = atoi(token);
                record->studentId = number;
            }
             else if(countForType == 1) {       //It is a stirng

                record->fName = strdup(token);
            }
            else if(countForType == 2) {       //It is a stirng
                record->lName = strdup(token);
            }
            else if(countForType == 3) {       //It is integer
                number = atoi(token);
                record->zip = number;
            }
            else if(countForType == 4) {       //It is integer
                number = atoi(token);
                record->year = number;
            }
            else if(countForType == 5) {       //It is a float
                fnumber = atof(token);
                record->gpa = fnumber;
            }
            countForType+=1;
            token = strtok(NULL, s);
        }while( token != NULL );
        int *temp = create_int(record->studentId);
        bool check_if_duplicate = map_insert(map, temp, record);

        // If the input has a duplicate free all the memory that the record has malloced and continue
        if(!check_if_duplicate){
            freeRecord(record);
            free(temp);
            count_for_dup++;
            continue;
        }
        
        inverted_index_insert(inv_index, record);
    }
    free(str);
}

// Returns the number(by pointer) that is in the config file. This number is the size of the hash table

void take_config_data(FILE *fp,const char *delim, int *size) {
    char *token;
    char* buff = malloc(202*sizeof(char));
    if(!buff) {
        printf("Error allocating memory!");
        exit(0);
    }

    fgets(buff, 201, fp);
    size_t ln = strlen(buff)-1;       //Do not include \n
    if (buff[ln] == '\n')
        buff[ln] = '\0';
    token = strtok(buff, delim);  // This is the "size"
    token = strtok(NULL, delim);
    *size = atoi(token);
    free(buff);
}



int main(int argc, char const *argv[])
{
    FILE *input_file;
    FILE *config_file;
    const char s[2] = " ";
    const char input_var[3] = "-i";
    const char conf_var[3] = "-c";
    char *token;
    char* firstChar;
    int hash_table_size = 1000;

    Map hash;

    /* Initialization of inverted index */
    InvertedIndex inv_index = inverted_index_create(NULL, compare_ints);


    if(argc == 1){                              // This means that there are no input and config files
        hash = map_create(compare_ints, free, freeRecord, hash_table_size);
        map_set_hash_function(hash, hash_int);
    }
    else if(argc == 3) {
        if(strcmp(argv[1], input_var) == 0) {       // It is "./exec -i input.txt" form
            /* Initialization of hash table */
            hash = map_create(compare_ints, free, freeRecord, hash_table_size);
            map_set_hash_function(hash, hash_int);

            input_file = fopen(argv[2], "r");
            insert_file_data(input_file, hash, inv_index);
            fclose(input_file);
        }
        else if(strcmp(argv[1], conf_var) == 0) {   // It is "./exec -c config.txt" form
            config_file = fopen(argv[2], "r");
            take_config_data(config_file, s, &hash_table_size);
            /* Initialization of hash table */
            hash = map_create(compare_ints, free, freeRecord, hash_table_size);
            map_set_hash_function(hash, hash_int);
            fclose(config_file);
        }
    }
    else if(argc == 5){
        if(strcmp(argv[1], input_var) == 0) {        // It is "./exec -i input.txt -c config.txt" form
            config_file = fopen(argv[4], "r");
            take_config_data(config_file, s, &hash_table_size);
            hash = map_create(compare_ints, free, freeRecord, hash_table_size);
            map_set_hash_function(hash, hash_int);
            fclose(config_file);

            input_file = fopen(argv[2], "r");
            insert_file_data(input_file, hash, inv_index);
            fclose(input_file);
        }
        else if(strcmp(argv[1], conf_var) == 0) {        // It is "./exec -c config.txt -i input.txt" form
            config_file = fopen(argv[2], "r");
            take_config_data(config_file, s, &hash_table_size);
            hash = map_create(compare_ints, free, freeRecord, hash_table_size);
            map_set_hash_function(hash, hash_int);
            fclose(config_file);

            input_file = fopen(argv[4], "r");
            insert_file_data(input_file, hash, inv_index);
            fclose(input_file);
        }
    }
    else{
        printf("Wrong parameters\n");
        exit(0);
    }

    char* choice = malloc(201*sizeof(char));
    if(!choice) {
        printf("Error allocating memory!");
        exit(0);
    }

    /* The start of the interface is here */
    while(1) {

        printf("Duplicates are %d\n", count_for_dup);

        /* Main Interface */
        printf("+----------------------------------------------------------------------------------------------+\n");
        printf("|                                                                                              |\n");
        printf("|                                          INTERFACE                                           |\n");
        printf("|                                                                                              |\n");
        printf("|    Εxplanation                                       Command Line                            |\n");
        printf("|                                                                                              |\n");
        printf("|    i. Insert a student                               (i studentid lname fname zip year gpa)  |\n");
        printf("|    l. Look up a student                              (l studentid)                           |\n");
        printf("|    d. Delete a student                               (d studentid)                           |\n");
        printf("|    n. Find how many students are still active        (n year)                                |\n");
        printf("|    t. Find the students with the best results        (t num year)                            |\n");
        printf("|    a. Find the average score of students at a year   (a year)                                |\n");
        printf("|    m. Find the student with the lowest average score (m year)                                |\n");
        printf("|    c. How many students overall                      (c)                                     |\n");
        printf("|    p. Find the x most popular city                   (p rank)                                |\n");
        printf("|    e. Exit the program                               (exit)                                  |\n");
        printf("|                                                                                              |\n");
        printf("+----------------------------------------------------------------------------------------------+\n");
        printf("\nGive a choice: ");
        fgets(choice, 100, stdin);
        size_t ln = strlen(choice)-1;       //Do not include \n
        if (choice[ln] == '\n')
            choice[ln] = '\0';

        char tempStr[100];     
        strcpy(tempStr, choice);
        // Take the first character of the string
        firstChar = strtok(tempStr, s);

        if(strcmp(firstChar, "i") == 0) {
            bool check = checkString(choice, 7);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }
            
            Record record = malloc(sizeof(*record));
            if(!record) {
                printf("Error allocating memory!");
                exit(0);
            }
            /* walk through other tokens */
            int number;
            float fnumber;
            int countForType = 0;

            /* get the first token */
            token = strtok(choice, s);
            while( token != NULL ) {
                // printf( " %s\n", token );
            
                token = strtok(NULL, s);
                if(countForType == 0) {       //It is integer
                    number = atoi(token);
                    record->studentId = number;
                }
                else if(countForType == 1) {       //It is a stirng

                    record->fName = strdup(token);
                }
                else if(countForType == 2) {       //It is a stirng
                    record->lName = strdup(token);
                }
                else if(countForType == 3) {       //It is integer
                    number = atoi(token);
                    record->zip = number;
                }
                else if(countForType == 4) {       //It is integer
                    number = atoi(token);
                    record->year = number;
                }
                else if(countForType == 5) {       //It is a float
                    fnumber = atof(token);
                    record->gpa = fnumber;
                }
                countForType+=1;
            }
            
            int *temp = create_int(record->studentId);
            bool check_if_duplicate = map_insert(hash, temp, record);
            // If the input has a duplicate free all the memory that the record has malloced and continue
            if(!check_if_duplicate){
                printf("Student %d exists\n\n", record->studentId);
                freeRecord(record);
                free(temp);
                count_for_dup++;
                continue;
            }
            inverted_index_insert(inv_index, record);
            printf("Student %d inserted\n", record->studentId);
        }
        else if(strcmp(firstChar, "l") == 0) {

            bool check = checkString(choice, 2);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }
            Record rec;

            /* get the first token */
            token = strtok(choice, s);  // This is the 'l'
            token = strtok(NULL, s);    // This is the studentid
            int *number;
            int num = atoi(token);
            number = &num;
            rec = (Record)map_find(hash, number);
            if(rec == NULL)
                printf("Student %s does not exist\n", token);
            else
                printRecords(rec);
        }
        else if(strcmp(firstChar, "d") == 0) {
            bool check = checkString(choice, 2);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            token = strtok(choice, s);  // This is the 'd'
            token = strtok(NULL, s);    // This is the studentid
            int *number;
            int num = atoi(token);
            number = &num;

            bool inv_record_exists = inverted_index_remove(inv_index, number);
            bool hash_record_exists = map_remove(hash, number);
            
            if(hash_record_exists && inv_record_exists)    // If is was succesfully removed
                printf("Record %s deleted", token);
            else                //It does not exist
                printf("Studen %s does not exist", token);
        }
        else if(strcmp(firstChar, "n") == 0) {
            bool check = checkString(choice, 2);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            token = strtok(choice, s);  // This is the 'n'
            token = strtok(NULL, s);    // This is the year
            int year = atoi(token);

            int students = inverted_index_get_students_year(inv_index, year);

            if(!students)
                printf("No students enrolled in %d", year);
            else
                printf("%d student(s) in %d\n", students, year);
        }
        else if(strcmp(firstChar, "t") == 0) {
            bool check = checkString(choice, 3);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            token = strtok(choice, s);  // This is the 't'
            token = strtok(NULL, s);    // This is the num
            int num = atoi(token);

            token = strtok(NULL, s);    // This is the year
            int year = atoi(token);

            top_students(inv_index, num, year);
        }
        else if(strcmp(firstChar, "a") == 0) {
            bool check = checkString(choice, 2);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            token = strtok(choice, s);  // This is the 'a'
            token = strtok(NULL, s);    // This is the year
            int year = atoi(token);

            float avg = average_year(inv_index, year);

            if(avg == -1.0)
                printf("No students enrolled in %d\n", year);
            else
                printf("%.2f\n",avg);
        }
        else if(strcmp(firstChar, "m") == 0) {
            bool check = checkString(choice, 2);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            token = strtok(choice, s);  // This is the 'm'
            token = strtok(NULL, s);    // This is the year
            int year = atoi(token);
            minimum(inv_index, year);
        }
        else if(strcmp(firstChar, "c") == 0) {
            bool check = checkString(choice, 1);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            count(inv_index);
        }
        else if(strcmp(firstChar, "p") == 0) {
            bool check = checkString(choice, 2);
            if(!check){
                printf("Wrong input continue..\n");
                continue;
            }

            token = strtok(choice, s);  // This is the 'p'
            token = strtok(NULL, s);    // This is the num
            int rank = atoi(token);

            get_top_N_postals(hash, rank, hash_table_size);
        }
        else if(strcmp(firstChar, "exit") == 0) {
            map_destroy(hash);
            inverted_index_destroy(inv_index);
            free(choice);
            printf("Exiting the program!\n~~~~~~~~~~~~~~~~~~~~\n");
            break;
        }
        else {             // Wrong input
            printf("Wrong input. . .\n");
            printf("\n\n");
            continue;
        }
        printf("\n\n");
    }

    return 0;
}
