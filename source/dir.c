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

// Function to change memory mode
inline void __attribute__((optimize("O0"))) _SC_changeMode(u16 mode) {
	vu16 *unlockAddress = (vu16 *)0x09FFFFFE;
	*unlockAddress = 0xA55A;
	*unlockAddress = 0xA55A;
	*unlockAddress = mode;
	*unlockAddress = mode;
}

Directory *createDirectory(const char *name, Directory *parentDirectory) {
	Directory *directory = (Directory *)malloc(sizeof(Directory));
	if (directory) {
		directory->name = name;
		directory->num_subdirectories = 0;
		directory->subdirectories = NULL;
		directory->num_files = 0;
		directory->files = NULL;
		directory->parentDirectory = parentDirectory;

		// Update the parent directory's list of subdirectories
		if (parentDirectory) {
			parentDirectory->subdirectories = (Directory **)realloc(parentDirectory->subdirectories, (parentDirectory->num_subdirectories + 1) * sizeof(Directory *));
			if (!parentDirectory->subdirectories) {
				fprintf(stderr, "Failed to allocate memory for subdirectories\n");
				free(directory); // Free allocated memory for directory
				return NULL;
			}
			parentDirectory->subdirectories[parentDirectory->num_subdirectories] = directory;
			parentDirectory->num_subdirectories++;
		}
	}
	return directory;
}

void deleteDirectory(Directory *directory) {
	if (!directory)
		return;

	// Remove the directory from its parent's list of subdirectories
	if (directory->parentDirectory) {
		for (int i = 0; i < directory->parentDirectory->num_subdirectories; ++i) {
			if (directory->parentDirectory->subdirectories[i] == directory) {
				// Shift the remaining subdirectories to fill the gap
				for (int j = i; j < directory->parentDirectory->num_subdirectories - 1; ++j) {
					directory->parentDirectory->subdirectories[j] = directory->parentDirectory->subdirectories[j + 1];
				}
				directory->parentDirectory->num_subdirectories--;
				break;
			}
		}
	}

	// Free memory allocated for subdirectories and files
	for (int i = 0; i < directory->num_subdirectories; ++i) {
		deleteDirectory(directory->subdirectories[i]);
	}
	free(directory->subdirectories);

	for (int i = 0; i < directory->num_files; ++i) {
		deleteFile(directory->files[i]);
	}
	free(directory->files);

	free(directory);
}

void renameDirectory(Directory *directory, const char *newName) {
	directory->name = newName;
}

File *getFileFromDirectory(Directory *directory, const char *filename) {
	for (int i = 0; i < directory->num_files; ++i) {
		if (strcmp(directory->files[i]->name, filename) == 0) {
			return directory->files[i];
		}
	}
	return NULL;
}

File **getAllFilesFromDirectory(Directory *directory) {
	return directory->files;
}

Directory **getAllSubdirectoriesFromDirectory(Directory *directory) {
	return directory->subdirectories;
}

void moveDirectory(Directory *directory, Directory *newParentDirectory) {
	// Remove the directory from its current parent's subdirectories
	if (directory->parentDirectory) {
		for (int i = 0; i < directory->parentDirectory->num_subdirectories; ++i) {
			if (directory->parentDirectory->subdirectories[i] == directory) {
				// Shift the remaining subdirectories to fill the gap
				for (int j = i; j < directory->parentDirectory->num_subdirectories - 1; ++j) {
					directory->parentDirectory->subdirectories[j] = directory->parentDirectory->subdirectories[j + 1];
				}
				directory->parentDirectory->num_subdirectories--;
				break;
			}
		}
	}

	// Update the directory's parent to the new parent
	directory->parentDirectory = newParentDirectory;

	// Add the directory to the new parent's subdirectories
	if (newParentDirectory) {
		newParentDirectory->subdirectories = (Directory **)realloc(newParentDirectory->subdirectories, (newParentDirectory->num_subdirectories + 1) * sizeof(Directory *));
		if (!newParentDirectory->subdirectories) {
			fprintf(stderr, "Failed to allocate memory for subdirectories\n");
			return;
		}
		newParentDirectory->subdirectories[newParentDirectory->num_subdirectories] = directory;
		newParentDirectory->num_subdirectories++;
	}

	// Update the number of subdirectories in the old parent directory
	if (directory->parentDirectory) {
		directory->parentDirectory->num_subdirectories--;
	}
}

size_t calculateDirectorySize(Directory *directory) {
	size_t size = sizeof(Directory); // Size of the Directory structure itself

	// Add the size of the name string if it's dynamically allocated
	if (directory->name != NULL) {
		size += strlen(directory->name) + 1; // Include the null terminator
	}

	// Calculate the size of subdirectories recursively
	for (int i = 0; i < directory->num_subdirectories; ++i) {
		size += calculateDirectorySize(directory->subdirectories[i]);
	}

	// Calculate the size of files and their data
	for (int i = 0; i < directory->num_files; ++i) {
		// Add the size of the File structure
		size += sizeof(File);

		// Add the size of the name string if it's dynamically allocated
		if (directory->files[i]->name != NULL) {
			size += strlen(directory->files[i]->name) + 1; // Include the null terminator
		}

		// Add the size of file data
		if (directory->files[i]->data != NULL) {
			size += strlen(directory->files[i]->data) + 1; // Include the null terminator
		}
	}

	return size;
}

// Serialize directory to a string
char *serializeDirectory(Directory *directory) {
	if (!directory)
		return NULL;

	char *result = malloc(1); // Allocate memory for the result
	result[0] = '\0';		  // Null-terminate the string

	// Serialize directory name
	strcat(result, "D ");
	strcat(result, directory->name);
	strcat(result, "\n");

	// Serialize subdirectories
	for (int i = 0; i < directory->num_subdirectories; i++) {
		char *subdirectoryString = serializeDirectory(directory->subdirectories[i]);
		strcat(result, subdirectoryString);
		free(subdirectoryString);
	}

	// Serialize files
	for (int i = 0; i < directory->num_files; i++) {
		strcat(result, "F ");
		strcat(result, directory->files[i]->name);
		strcat(result, "\n");
		strcat(result, directory->files[i]->data);
		strcat(result, "\n");
	}

	// Mark end of directory
	strcat(result, "E\n");

	return result;
}

Directory *deserializeDirectory(const char *data, Directory *parentDirectory) {
    if (!data)
        return NULL;

    Directory *directory = (Directory *)malloc(sizeof(Directory));
    directory->parentDirectory = parentDirectory;
    directory->num_subdirectories = 0;
    directory->num_files = 0;

    const char *currentChar = data;
	printf("a Current char: %c\n", *currentChar);
    while (*currentChar != '\0') {
		printf("d Current char: %c\n", *currentChar);
        if (*currentChar == 'D') {
            // Deserialize directory name
            currentChar += 2; // Skip 'D ' prefix
			printf("b Current char: %c\n", *currentChar);
            const char *endOfName = strchr(currentChar, '\n');
            int nameLength = endOfName - currentChar;
            char *name = malloc(nameLength + 1);
            strncpy(name, currentChar, nameLength);
            name[nameLength] = '\0';
			printf("name: %s\n", name);
            // Create subdirectory and add to directory
            Directory *subdirectory = createDirectory(name, directory);
			printf("endOfName: %s\n", endOfName);
            currentChar = endOfName + 1; // Move to next line
			printf("c Current char: %c\n", *currentChar);
            // Recursively deserialize subdirectory
            deserializeDirectory(currentChar, subdirectory);
            // Skip subdirectory content
            while (*currentChar != '\n') {
				printf("e Current char: %c\n", *currentChar);
                if (*currentChar == '\0')
                    break;
                currentChar++;
            }
            if (*currentChar == '\n')
                currentChar++; // Move to next line
        } else if (*currentChar == 'F') {
            // Deserialize file name
            currentChar += 2; // Skip 'F ' prefix
            const char *endOfName = strchr(currentChar, '\n');
            int nameLength = endOfName - currentChar;
            char *name = malloc(nameLength + 1);
            strncpy(name, currentChar, nameLength);
            name[nameLength] = '\0';
            // Create file and add to directory
            File *file = createFile(name, directory);
            // Read file data and set it
            // Assuming the next line contains file data
            const char *endOfFileData = strchr(endOfName + 1, '\n');
            int fileDataLength = endOfFileData - (endOfName + 1);
            char *fileData = malloc(fileDataLength + 1);
            strncpy(fileData, endOfName + 1, fileDataLength);
            fileData[fileDataLength] = '\0';
            writeFile(file, fileData);
            currentChar = endOfFileData + 1; // Move to next line
        } else if (*currentChar == 'E') {
            // End of directory
			printf("End of directory\n");
            break;
        } else {
            // Unexpected character, move to the next one
            currentChar++;
        }
    }

    return directory;
}

	

// Function to save directory data to memory
void saveDirectory(Directory *directory) {
    printf("Serializing directory...\n");
    char *serializedDirectory = serializeDirectory(directory);
    if (!serializedDirectory) {
        fprintf(stderr, "Failed to serialize directory\n");
        return;
    }

    printf("Copying serialized directory data to RAM...\n");
    // Change memory mode to RAM for writing
    _SC_changeMode(SC_MODE_RAM);

    // Write serialized directory data to RAM
    size_t serializedDirectorySize = strlen(serializedDirectory);
    memcpy((void *)0x0E000000, serializedDirectory, serializedDirectorySize);

    // Change memory mode back to media
    _SC_changeMode(SC_MODE_MEDIA);

    printf("Serialized directory data copied successfully!\n");

    // Free serialized directory string
    free(serializedDirectory);
}

// Function to load directory data from memory
Directory *loadDirectory() {
    printf("Getting serialized directory data from RAM...\n");

    // Change memory mode to RAM for reading
    _SC_changeMode(SC_MODE_RAM_RO);

    // Read serialized directory data from RAM
    char *serializedDirectory = (char *)0x0E000000;

    // Deserialize directory from serialized data
    printf("Deserializing directory...\n");
    Directory *directory = deserializeDirectory(serializedDirectory, NULL);
	printf("Out of the call");
    if (!directory) {
        fprintf(stderr, "Failed to deserialize directory\n");
        return NULL;
    }

    printf("Directory data loaded successfully!\n");

    // Change memory mode back to media
    _SC_changeMode(SC_MODE_MEDIA);

    return directory;
}


