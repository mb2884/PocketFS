#ifndef FILE_H
#define FILE_H
#define MAX_SIZE 1000

typedef struct Directory Directory;

typedef struct {
	char name[MAX_SIZE];
	char data[MAX_SIZE];
	int size;
	int capacity;
	int cursor_position;
	Directory *parent_directory;
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
