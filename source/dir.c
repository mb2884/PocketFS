#include "dir.h"
#include "file.h"
#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values for changing mode
#define SC_MODE_RAM 0x5
#define SC_MODE_MEDIA 0x3
#define SC_MODE_RAM_RO 0x1

#define MAX_SIZE 1000

// Function to change memory mode
inline void __attribute__((optimize("O0"))) _SC_changeMode(u16 mode) {
	vu16 *unlockAddress = (vu16 *)0x09FFFFFE;
	*unlockAddress = 0xA55A;
	*unlockAddress = 0xA55A;
	*unlockAddress = mode;
	*unlockAddress = mode;
}

// Function to create a directory
Directory *createDirectory(const char *name, Directory *parentDirectory) {
	Directory *directory = (Directory *)malloc(sizeof(Directory));
	if (directory == NULL)
		return NULL;
	strcpy(directory->name, name);
	directory->subdirectory_count = 0;
	directory->file_count = 0;
	directory->parent_directory = parentDirectory;
	if (parentDirectory != NULL)
		parentDirectory->subdirectories[parentDirectory->subdirectory_count++] = directory;
	return directory;
}

void deleteDirectory(Directory *directory) {
	if (directory == NULL)
		return;
	for (int i = 0; i < directory->subdirectory_count; i++) {
		deleteDirectory(directory->subdirectories[i]);
	}
	for (int i = 0; i < directory->file_count; i++) {
		// Free the memory allocated for file data
		free(directory->files[i]->data);
		// Free the file itself
		free(directory->files[i]);
	}
	free(directory);
}

// Function to rename a directory
void renameDirectory(Directory *directory, const char *newName) {
	if (directory != NULL && newName != NULL)
		strcpy(directory->name, newName);
}

// Function to get a file from a directory
File *getFileFromDirectory(Directory *directory, const char *filename) {
	if (directory == NULL || filename == NULL)
		return NULL;
	for (int i = 0; i < directory->file_count; i++) {
		if (strcmp(directory->files[i]->name, filename) == 0)
			return directory->files[i];
	}
	return NULL;
}

// Function to get all files from a directory
File **getAllFilesFromDirectory(Directory *directory) {
	if (directory == NULL)
		return NULL;
	return directory->files;
}

// Function to get all subdirectories from a directory
Directory **getAllSubdirectoriesFromDirectory(Directory *directory) {
	if (directory == NULL)
		return NULL;
	return directory->subdirectories;
}

// Function to move a directory to a new parent directory
void moveDirectory(Directory *directory, Directory *newParentDirectory) {
	if (directory == NULL || newParentDirectory == NULL)
		return;
	Directory *oldParent = directory->parent_directory;
	if (oldParent != NULL) {
		for (int i = 0; i < oldParent->subdirectory_count; i++) {
			if (oldParent->subdirectories[i] == directory) {
				for (int j = i; j < oldParent->subdirectory_count - 1; j++) {
					oldParent->subdirectories[j] = oldParent->subdirectories[j + 1];
				}
				oldParent->subdirectories[oldParent->subdirectory_count - 1] = NULL;
				oldParent->subdirectory_count--;
				break;
			}
		}
	}
	directory->parent_directory = newParentDirectory;
	newParentDirectory->subdirectories[newParentDirectory->subdirectory_count++] = directory;
}

// Function to serialize the directory structure
void serializeHelper(Directory* directory, int indent, char** address) {
    if (directory == NULL)
        return;

    char buffer[MAX_SIZE];

    sprintf(buffer, "%*sD %s\n", indent, "", directory->name);
    strcpy(*address, buffer);
    *address += strlen(buffer);

    for (int i = 0; i < directory->file_count; i++) {
        File* file = directory->files[i];
        sprintf(buffer, "%*sF %s\n", indent + 4, "", file->name);
        strcpy(*address, buffer);
        *address += strlen(buffer);

        sprintf(buffer, "%*s%d\n", indent + 8, "", file->size);
        strcpy(*address, buffer);
        *address += strlen(buffer);

        sprintf(buffer, "%*s%d\n", indent + 8, "", file->capacity);
        strcpy(*address, buffer);
        *address += strlen(buffer);

        sprintf(buffer, "%*s%d\n", indent + 8, "", file->cursor_position);
        strcpy(*address, buffer);
        *address += strlen(buffer);

        sprintf(buffer, "%*s%s\n", indent + 8, "", file->data);
        strcpy(*address, buffer);
        *address += strlen(buffer);
    }

    for (int i = 0; i < directory->subdirectory_count; i++) {
        serializeHelper(directory->subdirectories[i], indent + 4, address);
    }
}

void serialize(Directory* directory) {
    if (directory == NULL)
        return;

    char* address = (char*)0x0E000000; // Start address
    serializeHelper(directory, 0, &address);
}

// Function to deserialize the serialized string into directory structure
Directory* deserialize(const char* serialized_str) {
    char buffer[MAX_SIZE];
    char name[MAX_SIZE];
    char type[MAX_SIZE];
    int size, capacity, cursor_position;
    char data[MAX_SIZE];

    printf("Deserializing...\n");

    FILE* stream = fmemopen((void*)serialized_str, strlen(serialized_str), "r");
    Directory* root_dir = NULL;
    Directory* current_dir = NULL;

    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        sscanf(buffer, "%s %s", type, name);

        if (strcmp(type, "D") == 0) {
            Directory* new_dir = createDirectory(name, current_dir);
            if (root_dir == NULL)
                root_dir = new_dir;
            current_dir = new_dir;
        } else if (strcmp(type, "F") == 0) {
            File* new_file = createFile(name, current_dir);
            if (fgets(buffer, sizeof(buffer), stream) == NULL || sscanf(buffer, "%d", &size) != 1) {
                printf("Error parsing file size.\n");
                continue;
            }
            if (fgets(buffer, sizeof(buffer), stream) == NULL || sscanf(buffer, "%d", &capacity) != 1) {
                printf("Error parsing file capacity.\n");
                continue;
            }
            if (fgets(buffer, sizeof(buffer), stream) == NULL || sscanf(buffer, "%d", &cursor_position) != 1) {
                printf("Error parsing file cursor position.\n");
                continue;
            }
            if (fgets(data, sizeof(data), stream) == NULL) {
                printf("Error reading file data.\n");
                continue;
            }
            data[strlen(data) - 1] = '\0'; // Removing newline
            strcpy(new_file->data, data);
            new_file->size = size;
            new_file->capacity = capacity;
            new_file->cursor_position = cursor_position;
        }
    }

    fclose(stream);
    return root_dir;
}


// Function to save the serialized directory structure at address 0x0E000000
void saveDirectory(Directory* directory) {
    if (directory == NULL)
        return;

    char* address = (char*)0x0E000000; // Start address
	_SC_changeMode(SC_MODE_RAM);
    serializeHelper(directory, 0, &address);
	_SC_changeMode(SC_MODE_MEDIA);
}

// Function to load the serialized directory structure from address 0x0E000000
Directory* loadDirectory() {
    char* serialized_data = (char*)0x0E000000; // Start address

    // Create a temporary buffer to hold the serialized data
    char buffer[MAX_SIZE];
    strcpy(buffer, serialized_data);

    // Deserialize the data from the buffer
    Directory* deserialized_dir = deserialize(buffer);

    return deserialized_dir;
}

