#include "dir.h"
#include "file.h"
#include "display.h"
#include <gba.h>
#include <stdbool.h>
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

void delay(uint32_t milliseconds) {
	// Calculate the number of CPU cycles required for the given delay
	volatile uint32_t cycles = milliseconds * 350; // 1ms delay is approximately 350 CPU cycles

	// Run a busy loop for the calculated number of cycles
	for (volatile uint32_t i = 0; i < cycles; ++i) {
		// This loop consumes CPU cycles, introducing a delay
	}
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

// Function to serialize the directory structure
void serializeHelper(Directory *directory, char **address) {
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
		serializeHelper(directory->subdirectories[i], address);
	}

	strcpy(*address, "}");
	*address += 1;
}

void serialize(Directory *directory) {
	if (directory == NULL)
		return;

	char *address = (char *)0x0E000000; // Start address
	serializeHelper(directory, &address);
}

Directory *deserialize(const char *serialized_str) {
	Directory *root_dir = NULL;
	Directory *current_dir = NULL;
	Directory *parent_dir = NULL;
	File *current_file = NULL;
	char newDirName[MAX_SIZE];
	char newFileName[MAX_SIZE];
	char newFileContents[MAX_SIZE];
	int newFileContentsIndex = 0;
	bool isInFile = false;

	printf("Deserializing...\n");

	for (int i = 0; serialized_str[i] != '\0'; i++) {
		printf("loop char is: %c\n", serialized_str[i]);
		delay(3000);
		if (serialized_str[i] == 'D' && !isInFile) {
			// Skip one more character to ignore the space after 'D'
			i = i + 2;

			// Read directory name
			int j = 0;
			while (serialized_str[i] != ' ' && serialized_str[i] != '{') {
				printf("char for dir is: %c\n", serialized_str[i]);
				delay(500);
				newDirName[j++] = serialized_str[i++];
			}
			newDirName[j] = '\0';



			// Create directory
			if (root_dir == NULL) {
							printf("Creating directory: %s\n", newDirName);
			// Add delay for debugging
			delay(2000);
				root_dir = createDirectory(newDirName, NULL);
				current_dir = root_dir;
			} else {
				printf("Creating subdirectory: %s with parent %s\n", newDirName, current_dir->name);
				current_dir = createDirectory(newDirName, current_dir);
			}
			parent_dir = current_dir->parent_directory;
			while (serialized_str[i] != '}' && serialized_str[i] != '\0' && !isInFile) {
				i++;
				if (serialized_str[i-1] == 'F' && !isInFile) {
					// Skip one more character to ignore the space after 'F'
					i = i + 1;

					// Read file name
					int k = 0;
					while (serialized_str[i] != ' ' && serialized_str[i] != '{') {
						newFileName[k++] = serialized_str[i++];
					}
					newFileName[k] = '\0';
					printf("Creating file: %s\n", newFileName);
					delay(2000);

					// Skip one more character to ignore the space after '{'
					i = i + 2;

					// Read file contents
					int l = 0;
					while (serialized_str[i] != '}') {
						newFileContents[l++] = serialized_str[i++];
					}
					i++;

					newFileContents[l] = '\0';

					printf("Writing to file: %s\n", newFileContents);
					// Add delay for debugging
					delay(2000);

					// Create file
					current_file = createFile(newFileName, current_dir);
					writeFile(current_file, newFileContents);

					printf("serialized string at %c\n", serialized_str[i]);
					delay(2000);
				}
			}
			// i--;
			printf("Exiting while with char %c\n", serialized_str[i]);
			delay(1000);
		}
	}
	printf("Deserialization complete\n");
	// Add delay for debugging
	delay(2000);

	printBreak();
	printAllFiles(root_dir);
	delay(2000);
	printAllSubdirectories(root_dir);
	delay(2000);
	return root_dir;
}

// Function to save the serialized directory structure at address 0x0E000000
void saveDirectory(Directory *directory) {
	if (directory == NULL)
		return;

	char *address = (char *)0x0E000000;
	_SC_changeMode(SC_MODE_RAM);
	memset((void *)address, 0xFFFF, 0x0E00FFFF - 0x0E000000 + 1);
	address += 0x0E00FFFF - 0x0E000000 + 1;
	serializeHelper(directory, &address);
	_SC_changeMode(SC_MODE_MEDIA);
}

// char* printString(const char *str) {
// 	int i = 0;
// 	int length = 0;
// 	while (str[i] != '\0') {
// 		putchar(str[i]);
// 		delay(1000);
// 		i++;
// 		length++;
// 	}
	
// 	char* buffer = malloc(length + 1);
// 	strncpy(buffer, str, length);
// 	buffer[length] = '\0';
	
// 	return buffer;
// }

// Function to load the serialized directory structure from address 0x0E000000
Directory *loadDirectory() {
	printf("Loading directory from SRAM...\n");
	delay(2000);
	char *serialized_data = (char *)0x0E000000; // Start address

	// Create a temporary buffer to hold the serialized data
	// char buffer[MAX_SIZE];
	// strcpy(buffer, serialized_data);

	// Deserialize the data from the buffer
	Directory *deserialized_dir = deserialize(serialized_data);
	// printString(serialized_data);
	delay(20000);

	return deserialized_dir;
}
