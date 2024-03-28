#include "dir.h"
#include "display.h"
#include "file.h"
#include "text_data.h"
#include <gba_console.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_types.h>
#include <gba_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 16
#define MAX_CHARS 599

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

void setup() {
	// Enable VBlank interrupt for VBlankIntrWait() to work
	irqInit();
	irqEnable(IRQ_VBLANK);

	// Initialize console
	consoleDemoInit();

	// Set background color to dark grey
	REG_DISPCNT = MODE_0 | BG0_ON;
	BG_PALETTE[0] = RGB5(4, 4, 5);
}

// Function to print all subdirectories and files and allow selection
Directory *printCursor(Directory *directory, int cursor_position, int *selectedIsFile) {
	if (!directory) {
		// printf("Invalid directory\n");
		return NULL;
	}

	printf("Contents of '%s':\n", directory->name);

	// Calculate total number of items
	// int totalItems = directory->subdirectory_count + directory->file_count;

	// Print subdirectories
	printf("Subdirectories:\n");
	for (int i = 0; i < directory->subdirectory_count; ++i) {
		if (i == cursor_position) {
			printf("-> ");
			*selectedIsFile = 0; // Selected entry is a directory
		} else {
			printf("   ");
		}
		printf("%s\n", directory->subdirectories[i]->name);
	}

	// Print files
	printf("Files:\n");
	for (int i = 0; i < directory->file_count; ++i) {
		if ((directory->subdirectory_count + i) == cursor_position) {
			printf("-> ");
			*selectedIsFile = 1; // Selected entry is a file
		} else {
			printf("   ");
		}
		printf("%s\n", directory->files[i]->name);
	}

	return directory;
}

void importFiles(Directory *rootDir) {
	if (text_data[0] != '\0') {
		if (strlen(text_data) > MAX_CHARS) {
			int numFiles = strlen(text_data) / MAX_CHARS;
			int remainingChars = strlen(text_data) % MAX_CHARS;
			for (int i = 0; i < numFiles; i++) {
				if (rootDir->file_count + rootDir->subdirectory_count >= MAX_ITEMS) {
					break;
				}
				char *fileData = (char *)malloc(MAX_CHARS + 1);
				strlcpy(fileData, text_data + i * MAX_CHARS, MAX_CHARS + 1);
				File *file = createFile("user.txt", rootDir);
				writeFile(file, fileData);
			}
			if (remainingChars > 0) {
				char *fileData = (char *)malloc(remainingChars + 1);
				strlcpy(fileData, text_data + numFiles * MAX_CHARS, remainingChars + 1);
				File *file = createFile("user.txt", rootDir);
				writeFile(file, fileData);
			}
		} else {
			File *file = createFile("user.txt", rootDir);
			writeFile(file, text_data);
		}
	}
}

void editFile(File *file) {
	clearScr();

	char validLetters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;':,.<>?/`";
	int selectedLetter = 0;

	int cursor_position = file->cursor_position;

	while (1) {
		clearScr();
		if (file->data != NULL) {
			printf("%.*s", cursor_position, file->data); // Print characters up to cursor position
		}

		printf("%c", validLetters[selectedLetter]); // Print the selected letter

		if (file->data != NULL) {
			printf("%s", file->data + cursor_position); // Print remaining part of the file
		}

		scanKeys();

		int keys_pressed = keysDown();
		int keys_held = keysHeld();

		if (keys_pressed & KEY_R) {
			if (keys_held & KEY_SELECT) {
				selectedLetter = (selectedLetter + 26) % strlen(validLetters);
			} else {
				selectedLetter = (selectedLetter + 1) % strlen(validLetters);
			}
		} else if (keys_pressed & KEY_L) {
			if (keys_held & KEY_SELECT) {
				selectedLetter = (selectedLetter - 26) % strlen(validLetters);
			} else {
				selectedLetter = (selectedLetter - 1 + strlen(validLetters)) % strlen(validLetters);
			}
		} else if (keys_pressed & KEY_A && file->size < MAX_CHARS) {
			char letter[2] = {validLetters[selectedLetter], '\0'};
			insertAtPosition(file, letter, cursor_position);
			cursor_position++;
		} else if (keys_pressed & KEY_B && file->size < MAX_CHARS) {
			insertAtPosition(file, " ", cursor_position);
			cursor_position++;
		} else if (keys_pressed & KEY_START && !(keys_held & KEY_SELECT)) { // Delete character
			if (cursor_position > 0) {
				cursor_position--;
				file->data[cursor_position] = ' ';
			}
		} else if (keys_pressed & KEY_UP) {
			cursor_position -= 30; // Move cursor up a line
			if (cursor_position < 0) {
				cursor_position = 0; // Prevent moving above the first line
			}
		} else if (keys_pressed & KEY_DOWN) {
			for (int i = 0; i < 30; i++) {
				if (cursor_position >= MAX_CHARS) {
					cursor_position = MAX_CHARS; // Prevent moving below the last line
				}
				if (cursor_position < file->size) {
					cursor_position++; // Move cursor right by one character
				} else {
					if (file->size < MAX_CHARS) {
						insertAtPosition(file, " ", cursor_position);
					}
					cursor_position++;
				}
			}
		} else if (keys_pressed & KEY_LEFT && file->data != NULL) {
			if (cursor_position > 0) {
				cursor_position--; // Move cursor left by one character
			}
		} else if (keys_pressed & KEY_RIGHT) {
			if (cursor_position < file->size) {
				cursor_position++; // Move cursor right by one character
			} else {
				insertAtPosition(file, " ", cursor_position);
				cursor_position++;
			}
		} else if (keys_held & KEY_START && keys_held & KEY_SELECT) // Save and exit
		{
			file->cursor_position = cursor_position;
			break;
		}
		VBlankIntrWait();
	}
}

int main(void) {
	setup();

	Directory *rootDir;
	char *address_check = (char *)0x0E000000;
	if (address_check[0] == 'D') {
		rootDir = loadDirectory();
	} else {
		rootDir = createDirectory("root", NULL);
	}

	// importFiles(rootDir);

	int cursor_position = 0;
	Directory *currentDirectory = rootDir;

	while (1) {
		// Calculate totalEntries before printing the cursor
		int totalEntries = currentDirectory->subdirectory_count + currentDirectory->file_count;

		clearScr();

		// Flag to determine if the selected item is a file (1) or directory (0)
		int selectedIsFile = 0;

		currentDirectory = printCursor(currentDirectory, cursor_position, &selectedIsFile);

		scanKeys();
		int keys_pressed = keysDown();

		// Handle directional input
		if (keys_pressed & KEY_UP) {
			cursor_position = (cursor_position - 1 + totalEntries) % totalEntries;
		} else if (keys_pressed & KEY_DOWN) {
			cursor_position = (cursor_position + 1) % totalEntries;
		} else if (keys_pressed & KEY_LEFT) {
			if (currentDirectory->parent_directory != NULL) {
				currentDirectory = currentDirectory->parent_directory;
				cursor_position = 0;
			}
		} else if (keys_pressed & KEY_RIGHT) {
			if (selectedIsFile) {
				editFile(currentDirectory->files[cursor_position - currentDirectory->subdirectory_count]);
			} else if (cursor_position >= 0 && cursor_position < currentDirectory->subdirectory_count) {
				currentDirectory = currentDirectory->subdirectories[cursor_position];
				cursor_position = 0;
			}
		}

		// Create new directory or file
		else if (keys_pressed & KEY_B) {
			if (totalEntries < MAX_ITEMS) {
				createDirectory("new_dir", currentDirectory);
			}
		} else if (keys_pressed & KEY_A) {
			if (totalEntries < MAX_ITEMS) {
				createFile("new_file.txt", currentDirectory);
			}
		} else if (keys_pressed & KEY_SELECT) {
			// Check if a subdirectory or file is highlighted
			if (cursor_position >= 0 && cursor_position < currentDirectory->subdirectory_count) {
				// Set the current directory to its parent directory
				Directory *parentDirectory = currentDirectory->parent_directory;
				if (parentDirectory != NULL) {
					currentDirectory = parentDirectory;
				}

				// Delete the highlighted subdirectory
				Directory *dirToDelete = currentDirectory->subdirectories[cursor_position];
				deleteDirectory(dirToDelete);

				if (cursor_position != 0) {
					cursor_position--;
				}
			}
		} else if (keys_pressed & KEY_START) {
			saveDirectory(rootDir);
		}

		VBlankIntrWait();
	}
};
