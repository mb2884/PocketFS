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
    Directory *subDir6 = createDirectory("dir6", rootDir);
    Directory *subDir7 = createDirectory("dir7", rootDir);
    Directory *subDir8 = createDirectory("dir8", rootDir);
    Directory *subDir9 = createDirectory("dir9", rootDir);
    Directory *subDir10 = createDirectory("dir10", rootDir);
    Directory *subDir11 = createDirectory("dir11", rootDir);
    Directory *subDir12 = createDirectory("dir12", rootDir);
    Directory *subDir13 = createDirectory("dir13", rootDir);
    Directory *subDir14 = createDirectory("dir14", rootDir);
    Directory *subDir15 = createDirectory("dir15", rootDir);
    Directory *subDir16 = createDirectory("dir16", rootDir);
    Directory *subDir17 = createDirectory("dir17", rootDir);
    Directory *subDir18 = createDirectory("dir18", rootDir);
    Directory *subDir19 = createDirectory("dir19", rootDir);
    Directory *subDir20 = createDirectory("dir20", rootDir);


  // clearScr();

  // for (int i = 0; i < rootDir->num_subdirectories; ++i)
  // {
  //   printf("- %s\n", rootDir->subdirectories[i]->name);
  // }

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

    if (keys_pressed & KEY_A)
    {
      clearScr();
      printf("Commands:\n");
      printf("mkdir .... A\n");
      printf("mkfile .... B\n");
    }
    VBlankIntrWait();
  };
}
