#ifndef __CONFIG_H__
#define __CONFIG_H__

// hardwired into BIOS
// number of entries in drivemap.txt
#define DRIVES		4

#if defined(USE_SD)
#define PROGRAMS	"/cpm80/"
#else
#define PROGRAMS	"/"
#endif

// boot RAM
// we _must_ have memory above $BRAM_BASE
#define BRAM_BASE	0xe400u

#if (SPIRAM_SIZE + RAM_SIZE >= 0x10000u)

// we can fill the entire 64kB address space
#define RAM_BASE	0x0000u
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)
#define SPIRAM_BASE	RAM_SIZE
#define SPIRAM_EXTENT	(0x10000u - RAM_SIZE) / Memory::page_size
#else

// we need to leave a gap below $BRAM_BASE
#define BRAM_PAGES	(0x10000 - BRAM_BASE) / ram<>::page_size
#define RAM_BASE	0x0000u
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size - BRAM_PAGES)
#define SPIRAM_BASE	RAM_PAGES * ram<>::page_size
#define SPIRAM_EXTENT	min(SPIRAM_SIZE, BRAM_BASE - SPIRAM_BASE) / Memory::page_size
#endif

#define INSTRUCTIONS	1000

#define BG_COLOUR	BLACK
#define FG_COLOUR	WHITE

#if defined(USE_UTFT)
#define ORIENT	landscape
#elif defined(USE_ESPI)
#define ORIENT	reverse_landscape
#elif defined(USE_VGA)
#define ORIENT	landscape
#endif

#if !defined(FONT)
#define FONT	plain_font
#endif

#if FONT == plain_font
#define ROWS    	30
#define COLS    	64
#define FONT_W		5
#define FONT_H		8
#define FONT_OFF	0x20

#elif FONT == tama_font
#define ROWS		30
#define COLS		80
#define FONT_W		4
#define FONT_H		8
#define FONT_OFF	0x20
#endif

#if defined(USE_OWN_KBD)
//#define HW_SERIAL_KBD
#define PS2_SERIAL_KBD
#define PS2_SERIAL_KBD_LAYOUT	"UK"
#endif

#endif
