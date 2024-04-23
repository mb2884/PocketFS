#include "dir.h"
#include "file.h"
#include "utils.h"
#include <gba.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values for changing mode
// #define SC_MODE_RAM 0x5
// #define SC_MODE_MEDIA 0x3
// #define SC_MODE_RAM_RO 0x1

// Function to change memory mode
// inline void __attribute__((optimize("O0"))) _SC_changeMode(u16 mode) {
// 	vu16 *unlockAddress = (vu16 *)0x09FFFFFE;
// 	*unlockAddress = 0xA55A;
// 	*unlockAddress = 0xA55A;
// 	*unlockAddress = mode;
// 	*unlockAddress = mode;
// }

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

// Function to delete a directory
void deleteDirectory(Directory *directory) {
	if (directory == NULL)
		return;

	// Remove directory from its parent's subdirectories array
	Directory *parent = directory->parent_directory;
	if (parent != NULL) {
		for (int i = 0; i < parent->subdirectory_count; i++) {
			if (parent->subdirectories[i] == directory) {
				// Shift remaining elements in the array to fill the gap
				for (int j = i; j < parent->subdirectory_count - 1; j++) {
					parent->subdirectories[j] = parent->subdirectories[j + 1];
				}
				parent->subdirectories[parent->subdirectory_count - 1] = NULL;
				parent->subdirectory_count--;
				break;
			}
		}
	}

	// Recursively delete subdirectories and files
	for (int i = 0; i < directory->subdirectory_count; i++) {
		deleteDirectory(directory->subdirectories[i]);
	}
	for (int i = 0; i < directory->file_count; i++) {
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

void serialize(Directory *directory, char **address) {
	if (directory == NULL)
		return;

	char buffer[MAX_SIZE];

	sprintf(buffer, "D %s {", directory->name);
	strcpy(*address, buffer);
	*address += strlen(buffer);

	for (int i = 0; i < directory->file_count; i++) {
		File *file = directory->files[i];
		sprintf(buffer, "F %s {%s}", file->name, file->data);
		strcpy(*address, buffer);
		*address += strlen(buffer);
	}

	for (int i = 0; i < directory->subdirectory_count; i++) {
		serialize(directory->subdirectories[i], address);
	}

	strcpy(*address, "}");
	*address += 1;
}

Directory *deserialize(const char *serialized_str) {
	Directory *root_dir = NULL;
	Directory *parent_dir = NULL;
	File *current_file = NULL;
	char newDirName[MAX_NAME_SIZE];
	char newFileName[MAX_NAME_SIZE];
	char newFileContents[MAX_SIZE];
	int newFileContentsIndex = 0;

	// Scanning
	for (int i = 0; serialized_str[i] != '\0'; i++) {
		// Hit directory
		if (serialized_str[i] == 'D') {
			i++;
			// Found directory name
			if (serialized_str[i] == ' ') {
				i++;
			}

			// Scanned in directory name
			int j = 0;
			while (serialized_str[i] != '{') {
				if (serialized_str[i] != ' ') {
					newDirName[j] = serialized_str[i];
					i++;
					j++;
				} else {
					i++;
				}

				// i++;
				// j++;
			}
			newDirName[j] = '\0';

			// Save directory
			if (root_dir == NULL) {
				root_dir = createDirectory(newDirName, NULL);
				parent_dir = root_dir;
			} else {
				parent_dir = createDirectory(newDirName, parent_dir);
			}

		} else if (serialized_str[i] == 'F') {
			i++;

			// Found file name
			if (serialized_str[i] == ' ') {
				i++;
			}

			// Scanned in file name
			int j = 0;
			while (serialized_str[i] != '{') {
				newFileName[j] = serialized_str[i];
				i++;
				j++;
			}
			newFileName[j] = '\0';
			// Save file
			current_file = createFile(newFileName, parent_dir);

			// Scanning in file contents
			i++;
			while (serialized_str[i] != '}') {
				newFileContents[newFileContentsIndex] = serialized_str[i];
				newFileContentsIndex++;
				i++;
			}
			newFileContents[newFileContentsIndex] = '\0';

			// Save file contents
			writeFile(current_file, newFileContents);
			newFileContentsIndex = 0;

		} else if (serialized_str[i] == '}') {
			parent_dir = parent_dir->parent_directory;
		}
	}
	return root_dir;
}

// Function to save the serialized directory structure at address 0x0E000000
void saveDirectory(Directory *directory) {
	if (directory == NULL)
		return;

	char *address = (char *)0x0E000000;
	// Wipe RAM before writing
	// _SC_changeMode(SC_MODE_RAM);
	memset((void *)address, 0xFFFF, 0x0E00FFFF - 0x0E000000 + 1);
	address += 0x0E00FFFF - 0x0E000000 + 1;
	serialize(directory, &address);
	// _SC_changeMode(SC_MODE_MEDIA);
}

// Function to load the serialized directory structure from address 0x0E000000
Directory *loadDirectory() {
	printf("Loading directory from SRAM...\n");
	delay(2000);
	char *serialized_data = (char *)0x0E000000; // Start address

	// Deserialize the data from the buffer
	Directory *deserialized_dir = deserialize(serialized_data);

	return deserialized_dir;
}
