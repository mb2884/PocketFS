#ifndef FILE_H
#define FILE_H

// Include necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure to represent a file
typedef struct {
    const char *name;
    int size;
    int position;
} File;

// Function prototypes
File* createFile(const char* name, int size);
File* openFile(const char* name);
void writeFile(File* file, const char* data);
void closeFile(File* file);

#endif /* FILE_H */
