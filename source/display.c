
#include <gba_types.h>
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "dir.h"

void printAllFiles(Directory* directory) {
    if (!directory) {
        printf("Invalid directory\n");
        return;
    }

    printf("Files in directory '%s':\n", directory->name);
    for (int i = 0; i < directory->num_files; ++i) {
        printf("- %s\n", directory->files[i]->name);
    }
}

void printAllSubdirectories(Directory* directory) {
    if (!directory) {
        printf("Invalid directory\n");
        return;
    }

    printf("Subdirectories in directory '%s':\n", directory->name);
    for (int i = 0; i < directory->num_subdirectories; ++i) {
        printf("- %s\n", directory->subdirectories[i]->name);
    }
}



void testFileOperations() {
    printf("Testing File Operations:\n");

    // Create a directory
    Directory* rootDirectory = createDirectory("Root", NULL);

    // Create a file
    File* file1 = createFile("file1.txt", rootDirectory);
    writeFile(file1, "This is file1.txt content.");
    
    // Edit file
    editFile(file1, "This is updated content for file1.txt.");

    // Rename file
    renameFile(file1, "file1_updated.txt");

    // Create another file
    File* file2 = createFile("file2.txt", rootDirectory);
    writeFile(file2, "This is file2.txt content.");

    // Copy file
    copyFile(file2, rootDirectory);
    
    // Print all files
    printAllFiles(rootDirectory);

    // Move file
    Directory* subdir = createDirectory("Subdirectory", rootDirectory);
    moveFile(file2, subdir);

    // Delete file
    deleteFile(file1);
    deleteFile(file2);

    // Delete root directory
    deleteDirectory(rootDirectory);

    printf("File operations testing complete.\n\n");
}

void testDirectoryOperations() {
    printf("Testing Directory Operations:\n");

    // Create a directory
    Directory* rootDirectory = createDirectory("Root", NULL);

    // Create subdirectories
    Directory* subDir1 = createDirectory("Subdirectory1", rootDirectory);
    Directory* subDir2 = createDirectory("Subdirectory2", rootDirectory);

    // Rename directory
    renameDirectory(subDir2, "UpdatedSubdirectory2");

    // Create files in directories
    File* file1 = createFile("file1.txt", rootDirectory);
    File* file2 = createFile("file2.txt", subDir1);

    // Get files from directory
    File* fileFromRoot = getFileFromDirectory(rootDirectory, "file1.txt");
    File* fileFromSubdir = getFileFromDirectory(subDir1, "file2.txt");

    if (fileFromRoot && fileFromSubdir) {
        printf("Found files:\n");
        printf("File from root directory: %s\n", fileFromRoot->name);
        printf("File from subdirectory: %s\n", fileFromSubdir->name);
    } else {
        printf("Failed to find files.\n");
    }

    // Move directory
    moveDirectory(subDir1, subDir2);

    // Get all files from directory
    printAllFiles(rootDirectory);

    // Get all subdirectories from directory
    printAllSubdirectories(rootDirectory);

    // Delete files and directories
    deleteFile(file1);
    deleteFile(file2);
    deleteDirectory(rootDirectory);

    printf("Directory operations testing complete.\n\n");
}


//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();

  // Set background color to dark grey 
  REG_DISPCNT = MODE_0 | BG0_ON;
	BG_PALETTE[0] = RGB5(4, 4, 5); 

  // ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

  testDirectoryOperations();
  testFileOperations();

  do {
		int keys_pressed;
		
    // Vsync
		VBlankIntrWait();
	  
    // Get current values for keys
		scanKeys();
    
    // Get keys when both pressed and held
		keys_pressed = keysDown();

		// If key B is pressed, print out a msg
		if ( keys_pressed & KEY_B ) {
		  printf("You pressed a key!\n");
		}

	}
  while( 1 );
}

