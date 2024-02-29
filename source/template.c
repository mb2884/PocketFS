
#include <gba_types.h>
#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

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

  // Set the font color to white
  BG_PALETTE[1] = RGB5(0, 0, 31); // Setting the font color to bright blue (RGB(0, 0, 255))
  
  // ansi escape sequence to clear screen and home cursor
	// /x1b[line;columnH
	iprintf("\x1b[2J");
  
  printf("Hello, world!\n");
  do {

		int keys_pressed, keys_released;
		// Vsync
		VBlankIntrWait();
	  // Get current values for keys
		scanKeys();
    // Get keys when both pressed and held
		keys_pressed = keysDown();
		keys_released = keysUp();

		// If key B is pressed, print out a msg
		if ( keys_pressed & KEY_B ) {
		  printf("You pressed a key!\n");
		}

	}
  while( 1 );
}

