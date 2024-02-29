
#include <gba_types.h>
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

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
	BG_PALETTE[0] = RGB5(4, 4, 5); // grey background color using RGB5 macro

  // ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");

  File* myFile = createFile("example.txt");
  
  writeFile(myFile, "Hello, ");
  writeFile(myFile, "world!");

  printf("File content: %s\n", myFile->data);

  closeFile(myFile);

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
