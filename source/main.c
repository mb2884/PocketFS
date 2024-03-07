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
#include "test.h"
#include "display.h"

#define MAX_ITEMS 16

void setup()
{
  // the vblank interrupt must be enabled for VBlankIntrWait() to work
  // since the default dispatcher handles the bios flags no vblank handler
  // is required

  irqInit();
  irqEnable(IRQ_VBLANK);

  consoleDemoInit();

  // Set background color to dark grey
  REG_DISPCNT = MODE_0 | BG0_ON;
  BG_PALETTE[0] = RGB5(4, 4, 5);
}

// Function to print all subdirectories and files and allow selection
Directory *printCursor(Directory *directory, int cursorPosition, int *selectedIsFile)
{
  if (!directory)
  {
    printf("Invalid directory\n");
    return NULL;
  }

  printf("Contents of '%s':\n", directory->name);

  // Calculate total number of items
  int totalItems = directory->num_subdirectories + directory->num_files;

  // Print subdirectories
  printf("Subdirectories:\n");
  for (int i = 0; i < directory->num_subdirectories; ++i)
  {
    if (i == cursorPosition)
    {
      printf("-> ");
      *selectedIsFile = 0; // Selected entry is a directory
    }
    else
    {
      printf("   ");
    }
    printf("%s\n", directory->subdirectories[i]->name);
  }

  // Print files
  printf("Files:\n");
  for (int i = 0; i < directory->num_files; ++i)
  {
    if ((directory->num_subdirectories + i) == cursorPosition)
    {
      printf("-> ");
      *selectedIsFile = 1; // Selected entry is a file
    }
    else
    {
      printf("   ");
    }
    printf("%s\n", directory->files[i]->name);
  }

  return directory;
}

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void)
{
  setup();
  // testDirectoryOperations();
  // testFileOperations();

  // Create root directory
  Directory *rootDir = createDirectory("root", NULL);

  // Create subdirectories manually
  Directory *subDir1 = createDirectory("dir1", rootDir);
  Directory *subDir2 = createDirectory("dir2", rootDir);
  Directory *subDir3 = createDirectory("dir3", rootDir);
  Directory *subDir4 = createDirectory("dir4", rootDir);
  Directory *subDir5 = createDirectory("dir5", rootDir);

  File *file1 = createFile("file1.txt", rootDir);
  File *file2 = createFile("file2.txt", rootDir);
  File *file3 = createFile("file3.txt", rootDir);

  int cursorPosition = 0;
  Directory *currentDirectory = rootDir;

  while (1)
  {
    // Calculate totalEntries before printing the cursor
    int totalEntries = currentDirectory->num_subdirectories + currentDirectory->num_files;

    clearScr();

    // Flag to determine if the selected item is a file (1) or directory (0)
    int selectedIsFile = 0;

    // Print directory contents and allow selection
    currentDirectory = printCursor(currentDirectory, cursorPosition, &selectedIsFile);

    // Get current values for keys
    scanKeys();

    // Get keys when both pressed and held
    int keys_pressed = keysDown();

    if (keys_pressed & KEY_DOWN)
    {
      cursorPosition = (cursorPosition + 1) % (currentDirectory->num_subdirectories + currentDirectory->num_files);
    }
    else if (keys_pressed & KEY_UP)
    {
      cursorPosition = (cursorPosition - 1 + (currentDirectory->num_subdirectories + currentDirectory->num_files)) % (currentDirectory->num_subdirectories + currentDirectory->num_files);
    }
    else if (keys_pressed & KEY_LEFT)
    {
      // Navigate back to the parent directory if not already in the root directory
      if (currentDirectory->parentDirectory != NULL)
      {
        currentDirectory = currentDirectory->parentDirectory;
        cursorPosition = 0; // Reset cursor position to the first subdirectory
      }
    }
    else if (keys_pressed & KEY_RIGHT)
    {
      // Check if a subdirectory is highlighted
      if (selectedIsFile)
      {
        // Handle file selection (navigate to edit file screen)
        printf("Selected file: %s\n", currentDirectory->files[cursorPosition - currentDirectory->num_subdirectories]->name);
      }
      else if (cursorPosition >= 0 && cursorPosition < currentDirectory->num_subdirectories)
      {
        // Set the highlighted subdirectory as the new root directory
        currentDirectory = currentDirectory->subdirectories[cursorPosition];
        cursorPosition = 0; // Reset cursor position to the first subdirectory
      }
    }
    else if (keys_pressed & KEY_B)
    {
      // Create a new directory within the current directory
      if (totalEntries < MAX_ITEMS)
      {
        Directory *newDir = createDirectory("new_dir", currentDirectory);
      }
    }
    else if (keys_pressed & KEY_SELECT)
    {
      // Create a new file within the current directory
      if (totalEntries < MAX_ITEMS)
      {
        File *newFile = createFile("new_file.txt", currentDirectory);
      }
    }
    else if (keys_pressed & KEY_A)
    {
      // Check if a subdirectory or file is highlighted
      if (cursorPosition >= 0 && cursorPosition < currentDirectory->num_subdirectories)
      {
        // Delete the highlighted subdirectory
        Directory *dirToDelete = currentDirectory->subdirectories[cursorPosition];
        deleteDirectory(dirToDelete);

        // Set the current directory to the parent directory of the deleted one
        currentDirectory = dirToDelete->parentDirectory;
        if (cursorPosition != 0)
        {
          cursorPosition--;
        }
      }
      else if (cursorPosition >= currentDirectory->num_subdirectories && cursorPosition < totalEntries)
      {
        // Delete the highlighted file
        File *fileToDelete = currentDirectory->files[cursorPosition - currentDirectory->num_subdirectories];
        deleteFile(fileToDelete);

        if (cursorPosition != currentDirectory->num_subdirectories)
        {
          cursorPosition--;
        }
      }
    }

    VBlankIntrWait();
  };
}
