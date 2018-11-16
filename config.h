#ifndef __CONFIG_H__
#define __CONFIG_H__

// hardwired into BIOS
// number of entries in drivemap.txt
#define DRIVES		2

#if defined(USE_SD)
#define PROGRAMS	"/cpm80/"
#else
#define PROGRAMS	"/"
#endif

#define BRAM_BASE	0xe400
#define BRAM_PAGES	7

#if defined(USE_SPIRAM)
#define SPIRAM_BASE	0x0000
#define SPIRAM_EXTENT	BRAM_BASE / 0x100
#else
#define RAM_BASE	0x0000
#define RAM_PAGES	32
#endif

#define INSTRUCTIONS	1000

#define TFT_BG	BLACK
#define TFT_FG	YELLOW

#define FONT	plain_font
#define ROWS    30
#define COLS    64
#define FONT_W		5
#define FONT_H		8
#define FONT_OFF	0x20

//#define FONT	tama_font
//#define ROWS	30
//#define COLS	80
//#define FONT_W	4
//#define FONT_H	8
//#define FONT_OFF	0x20

#endif
