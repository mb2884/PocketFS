#ifndef FILE_H
#define FILE_H

typedef struct Directory Directory;

typedef struct
{
    char *name;
    char *data;
    int size;
    int capacity;
    int cursorPosition;
    Directory *parentDirectory;
} File;

File *createFile(char *name, Directory *parentDirectory);
void writeFile(File *file, const char *data);
void appendFile(File *file, const char *data);
void insertAtPosition(File *file, const char *data, int position);
void deleteFile(File *file);
void renameFile(File *file, char *newName);
void copyFile(File *sourceFile, Directory *destinationDirectory);
void moveFile(File *file, Directory *newParentDirectory);

#endif /* FILE_H */
