#ifndef DIR_H
#define DIR_H

#define MAX_SUBDIRECTORIES 128
#define MAX_FILES 128
#define MAX_SIZE 1000

#include "file.h"

struct Directory {
	char name[MAX_SIZE];
	Directory *subdirectories[MAX_SIZE];
	File *files[MAX_SIZE];
	int subdirectory_count;
	int file_count;
	Directory *parent_directory;
};

Directory *createDirectory(const char *name, Directory *parentDirectory);
void deleteDirectory(Directory *directory);
void renameDirectory(Directory *directory, const char *newName);
File *getFileFromDirectory(Directory *directory, const char *filename);
File **getAllFilesFromDirectory(Directory *directory);
Directory **getAllSubdirectoriesFromDirectory(Directory *directory);
void moveDirectory(Directory *directory, Directory *newParentDirectory);
void saveDirectory(Directory *directory);
Directory *loadDirectory();
void serialize(Directory *directory, char **address);
Directory *deserialize(const char *serialized_str);

#endif
