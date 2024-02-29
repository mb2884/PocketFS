#ifndef FILE_H
#define FILE_H

typedef struct Directory Directory;

typedef struct {
    const char *name;
    const char *data;
    int size;
    int capacity;
    Directory *parentDirectory;
} File;

File* createFile(const char* name, Directory *parentDirectory);
void writeFile(File* file, const char* data);
void editFile(File* file, const char* newData);
void deleteFile(File* file);
void renameFile(File* file, const char* newName);
void copyFile(File* sourceFile, Directory* destinationDirectory);
void moveFile(File* file, Directory* newParentDirectory);

#endif /* FILE_H */
