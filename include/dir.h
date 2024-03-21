#ifndef DIR_H
#define DIR_H

#define MAX_SUBDIRECTORIES 128
#define MAX_FILES 128

#include "file.h"

typedef struct Directory
{
    const char *name;
    struct Directory **subdirectories;
    int num_subdirectories;
    File **files;
    int num_files;
    struct Directory *parentDirectory; // Reference to the parent directory
} Directory;

Directory *createDirectory(const char *name, Directory *parentDirectory);
void deleteDirectory(Directory *directory);
void renameDirectory(Directory *directory, const char *newName);
File *getFileFromDirectory(Directory *directory, const char *filename);
File **getAllFilesFromDirectory(Directory *directory);
Directory **getAllSubdirectoriesFromDirectory(Directory *directory);
void moveDirectory(Directory *directory, Directory *newParentDirectory);

#endif /* DIR_H */
