#include "utils.h"
#include "dir.h"
#include "file.h"
#include <stdio.h>

void printAllFiles(Directory *directory) {
	if (!directory) {
		// printf("Invalid directory\n");
		return;
	}

	printf("Files in directory '%s':\n", directory->name);
	for (int i = 0; i < directory->file_count; ++i) {
		printf("- %s\n", directory->files[i]->name);
	}
}

void printAllSubdirectories(Directory *directory) {
	if (!directory) {
		// printf("Invalid directory\n");
		return;
	}

	printf("Subdirectories in directory '%s':\n", directory->name);
	for (int i = 0; i < directory->subdirectory_count; ++i) {
		printf("- %s\n", directory->subdirectories[i]->name);
	}
}

void printBreak() {
	for (int i = 0; i < 30; i++) {
		printf("-");
	}
}

void clearScr() {
	// ansi escape sequence to clear screen and home cursor
	iprintf("\x1b[2J");
}

// Busy loop
void delay(uint32_t milliseconds) {
	volatile uint32_t cycles = milliseconds * 350; // 1ms delay is approximately 350 CPU cycles

	for (volatile uint32_t i = 0; i < cycles; ++i) {
	}
}