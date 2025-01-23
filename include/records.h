#pragma once // #include one time at most

struct record {
    int studentId;
	char *fName;
	char *lName;
    int zip;
    int year;
    float gpa;
};

typedef struct record* Record;