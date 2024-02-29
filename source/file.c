#include "file.h"

// Create a file object
File* createFile(const char* name, int size) {
    File* file = (File*)malloc(sizeof(File));
    if (file) {
        file->name = name;
        file->size = size;
        file->position = 0;
    }
    return file;
}

// Open a file for writing
File* openFile(const char* name) {
    // For simplicity, we'll assume that the file exists on the FAT filesystem
    // and we'll just create a File structure to represent it
    int size = 0; // Dummy size for now
    return createFile(name, size);
}

// Write to a file
void writeFile(File* file, const char* data) {
    // For simplicity, we'll just print the data to stdout
    printf("%s\n", data);
}

// Close a file
void closeFile(File* file) {
    free(file);
}
