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

#define MAX_DIRECTORIES 16

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

// Function to print all subdirectories and allow selection
Directory *printCursor(Directory *directory, int cursorPosition)
{
  if (!directory)
  {
    printf("Invalid directory\n");
    return NULL;
  }

  printf("Subdirectories: \n");
  for (int i = 0; i < directory->num_subdirectories; ++i)
  {
    if (i == cursorPosition)
    {
      printf("-> ");
    }
    else
    {
      printf("   ");
    }
    printf("%s\n", directory->subdirectories[i]->name);
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

  int cursorPosition = 0;
  Directory *currentDirectory = rootDir;

  while (1)
  {
    clearScr();

    currentDirectory = printCursor(currentDirectory, cursorPosition);

    // Get current values for keys
    scanKeys();

    // Get keys when both pressed and held
    int keys_pressed = keysDown();

    if (keys_pressed & KEY_DOWN)
    {
      cursorPosition = (cursorPosition + 1) % currentDirectory->num_subdirectories;
    }
    else if (keys_pressed & KEY_UP)
    {
      cursorPosition = (cursorPosition - 1 + currentDirectory->num_subdirectories) % currentDirectory->num_subdirectories;
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
      if (cursorPosition >= 0 && cursorPosition < currentDirectory->num_subdirectories)
      {
        // Set the highlighted subdirectory as the new root directory
        currentDirectory = currentDirectory->subdirectories[cursorPosition];
        cursorPosition = 0; // Reset cursor position to the first subdirectory
      }
    }
    else if (keys_pressed & KEY_B)
    {
      if (currentDirectory->num_subdirectories < MAX_DIRECTORIES)
      {
        Directory *subDir = createDirectory("dir", currentDirectory);
      }
    }
    else if (keys_pressed & KEY_A)
    {
      // Check if a subdirectory is highlighted
      if (cursorPosition >= 0 && cursorPosition < currentDirectory->num_subdirectories)
      {
        // Delete the highlighted subdirectory
        Directory *dirToDelete = currentDirectory->subdirectories[cursorPosition];
        Directory *parentDirectory = dirToDelete->parentDirectory;
        deleteDirectory(dirToDelete);

        // Set the current directory to the parent directory of the deleted one
        if (parentDirectory != NULL)
        {
          currentDirectory = parentDirectory;
          cursorPosition = 0; // Reset cursor position to the first subdirectory
        }
      }
    }

    VBlankIntrWait();
  };
}
