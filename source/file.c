#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

File* createFile(const char* name) {
    File* file = (File*)malloc(sizeof(File));
    if (file) {
        file->name = name;
        file->data = NULL;
        file->size = 0;
        file->capacity = 0;
    }
    return file;
}

void writeFile(File* file, const char* data) {
    int dataLength = strlen(data);
    if (file->size + dataLength >= file->capacity) {
        int newCapacity = file->capacity + dataLength + 1;
        char* newData = (char*)realloc(file->data, newCapacity);
        if (!newData) {
            fprintf(stderr, "Failed to allocate memory for file data\n");
            return;
        }
        file->data = newData;
        file->capacity = newCapacity;
    }
    strcpy(file->data + file->size, data);
    file->size += dataLength;
}

void closeFile(File* file) {
    free(file->data);
    free(file);
}
