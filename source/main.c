#include <gba_types.h>
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "dir.h"
#include "display.h"

#define MAX_ITEMS 16

void setup();
Directory *printCursor(Directory *directory, int cursorPosition, int *selectedIsFile);

void setup()
{
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
Directory *printCursor(Directory *directory, int cursorPosition, int *selectedIsFile)
{
  if (!directory)
  {
    printf("Invalid directory\n");
    return NULL;
  }

  printf("Contents of '%s':\n", directory->name);

  // Calculate total number of items
  // int totalItems = directory->num_subdirectories + directory->num_files;

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

void editFile(File *file)
{
  clearScr();

  char validLetters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;':,.<>?/`";
  int selectedLetter = 0;

  int cursorPosition = file->cursorPosition;

  while (1)
  {
    clearScr();
    if (file->data != NULL)
    {
      printf("%s", file->data);
    }

    printf("%c", validLetters[selectedLetter]);

    scanKeys();

    int keys_pressed = keysDown();
    int keys_held = keysHeld();

    if (keys_pressed & KEY_R)
    {
      if (keys_held & KEY_SELECT)
      {
        selectedLetter = (selectedLetter + 26) % strlen(validLetters);
      }
      else
      {
        selectedLetter = (selectedLetter + 1) % strlen(validLetters);
      }
    }
    else if (keys_pressed & KEY_L)
    {
      if (keys_held & KEY_SELECT)
      {
        selectedLetter = (selectedLetter - 26) % strlen(validLetters);
      }
      else
      {
        selectedLetter = (selectedLetter - 1 + strlen(validLetters)) % strlen(validLetters);
      }
    }
    else if (keys_pressed & KEY_A)
    {
      char letter[2] = {validLetters[selectedLetter], '\0'};
      insertAtPosition(file, letter, cursorPosition);
      cursorPosition++;
    }
    else if (keys_pressed & KEY_B)
    {
      insertAtPosition(file, " ", cursorPosition);
      cursorPosition++;
    }
    else if (keys_pressed & KEY_START)
    {
      file->cursorPosition = cursorPosition;
      break;
    }
    VBlankIntrWait();
  }
}

int main(void)
{
  setup();

  // Create root directory
  Directory *rootDir = createDirectory("root", NULL);

  // Create subdirectories and files manually
  createDirectory("dir1", rootDir);
  createDirectory("dir2", rootDir);
  createDirectory("dir3", rootDir);
  createDirectory("dir4", rootDir);
  createDirectory("dir5", rootDir);

  createFile("file1.txt", rootDir);
  createFile("file2.txt", rootDir);
  createFile("file3.txt", rootDir);

  int cursorPosition = 0;
  Directory *currentDirectory = rootDir;

  while (1)
  {
    // Calculate totalEntries before printing the cursor
    int totalEntries = currentDirectory->num_subdirectories + currentDirectory->num_files;

    clearScr();

    // Flag to determine if the selected item is a file (1) or directory (0)
    int selectedIsFile = 0;

    currentDirectory = printCursor(currentDirectory, cursorPosition, &selectedIsFile);

    scanKeys();
    int keys_pressed = keysDown();

    // Handle directional input
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
      if (currentDirectory->parentDirectory != NULL)
      {
        currentDirectory = currentDirectory->parentDirectory;
        cursorPosition = 0;
      }
    }
    else if (keys_pressed & KEY_RIGHT)
    {
      if (selectedIsFile)
      {
        editFile(currentDirectory->files[cursorPosition - currentDirectory->num_subdirectories]);
      }
      else if (cursorPosition >= 0 && cursorPosition < currentDirectory->num_subdirectories)
      {
        currentDirectory = currentDirectory->subdirectories[cursorPosition];
        cursorPosition = 0;
      }
    }

    // Create new directory or file
    else if (keys_pressed & KEY_B)
    {
      if (totalEntries < MAX_ITEMS)
      {
        createDirectory("new_dir", currentDirectory);
      }
    }
    else if (keys_pressed & KEY_A)
    {
      if (totalEntries < MAX_ITEMS)
      {
        createFile("new_file.txt", currentDirectory);
      }
    }
    // Delete directory or file
    else if (keys_pressed & KEY_SELECT)
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
        if (cursorPosition != 0)
        {
          cursorPosition--;
        }
      }
    }
    else if (keys_pressed & KEY_START)
    {
    }

    VBlankIntrWait();
  };
}
