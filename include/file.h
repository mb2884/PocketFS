#ifndef FILE_H
#define FILE_H

typedef struct {
    const char *name;
    char *data;
    int size;
    int capacity;
} File;

File* createFile(const char* name);
void writeFile(File* file, const char* data);
void closeFile(File* file);

#endif /* FILE_H */
