#include "dir.h"
#include "utils.h"
#include "file.h"
#include "imported_data.h"
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
#define MAX_NAME_LENGTH 27
#define MAX_CHARS 599

#define USE_SAVE 0
#define USE_IMPORT 1

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

// Perform necessary setup
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
void printCursor(Directory *directory, int cursor_position, int *selectedIsFile) {
	if (!directory) {
		printf("Invalid directory\n");
		return;
	}

	clearScr();

	printf("Contents of '%s':\n", directory->name);

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
}

void editFile(File *file) {
	clearScr();

	char validLetters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;':,.<>?/`";
	int selectedLetter = 0;

	int cursor_position = file->cursor_position;

	while (1) {
		clearScr();
		if (file != NULL) {
			printf("%.*s", cursor_position, file->data); // Print characters up to cursor position
		}

		printf("%c", validLetters[selectedLetter]); // Print the selected letter

		if (file != NULL) {
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
		} else if (keys_pressed & KEY_LEFT) {
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

void renameEntity(void *entity, int isFile) {
	char *entityName = isFile ? ((File *)entity)->name : ((Directory *)entity)->name;
	
	char validLetters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;':,.<>?/`";
	int selectedLetter = 0;

	char new_name[MAX_NAME_LENGTH];
	int cursor_position = 0;

	while (1) {
		clearScr();
		printf("Renaming \"%s\":\n", entityName);
		printf("\x1b[2;0H");
		printf("%.*s", cursor_position, new_name); // Print characters up to cursor position
		printf("\x1b[2;%dH", cursor_position);
		printf("%c", validLetters[selectedLetter]); // Print the selected letter

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
		} else if (keys_pressed & KEY_A && cursor_position < MAX_NAME_LENGTH - 1) {
			new_name[cursor_position] = validLetters[selectedLetter];
			cursor_position++;
			new_name[cursor_position] = '\0'; // Null-terminate the string
		} else if (keys_pressed & KEY_B && cursor_position > 0) {
			cursor_position--;
			new_name[cursor_position] = '\0'; // Null-terminate the string
		} else if (keys_pressed & KEY_START) { // Save and exit
			if (isFile) {
				File *file = (File *)entity;
				strncpy(file->name, new_name, MAX_NAME_LENGTH);
				file->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
			} else {
				Directory *dir = (Directory *)entity;
				strncpy(dir->name, new_name, MAX_NAME_LENGTH);
				dir->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
			}
			break;
		}

		VBlankIntrWait();
	}
}

int main(void) {
	setup();
	// Load previous SAV if it exists
	Directory *rootDir;
	char *address_check = (char *)0x0E000000;
	if (address_check[0] == 'D' && USE_SAVE) {
		rootDir = loadDirectory();
	} else if (USE_IMPORT) {
		rootDir = deserialize(imported_text_data);
	} else {
		rootDir = createDirectory("root", NULL);
	}

	int cursor_position = 0;
	Directory *currentDirectory = rootDir;

	while (1) {
		int totalEntries = currentDirectory->subdirectory_count + currentDirectory->file_count;

		int selectedIsFile = 0;
		printCursor(currentDirectory, cursor_position, &selectedIsFile);

		scanKeys();
		int keys_pressed = keysDown();

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
		} else if (keys_pressed & KEY_B) {
			if (totalEntries < MAX_ITEMS) {
				createDirectory("new_dir", currentDirectory);
			}
		} else if (keys_pressed & KEY_A) {
			if (totalEntries < MAX_ITEMS) {
				createFile("new_file.txt", currentDirectory);
			}
		} else if (keys_pressed & KEY_SELECT) {
			if (cursor_position >= 0 && cursor_position < totalEntries) {
				if (selectedIsFile) {
					// Handle file deletion
					deleteFile(currentDirectory->files[cursor_position - currentDirectory->subdirectory_count]);
				} else {
					// Handle directory deletion
					deleteDirectory(currentDirectory->subdirectories[cursor_position]);
				}
				if (cursor_position != 0) {
					cursor_position--;
				}
			}
		} else if (keys_pressed & KEY_START) {
			printf("Saving directory to SRAM...\n");
			delay(1000);
			saveDirectory(rootDir);

		} else if (keys_pressed & KEY_L && totalEntries > 0) {
			if (selectedIsFile) {
				renameEntity(currentDirectory->files[cursor_position - currentDirectory->subdirectory_count], 1);
			} else {
				renameEntity(currentDirectory->subdirectories[cursor_position], 0);
			}
		}

		VBlankIntrWait();
	}
}