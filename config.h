#ifndef __CONFIG_H__
#define __CONFIG_H__

// hardwired into BIOS
#define DRIVES		4
#if defined(USE_SD)
#define PROGRAMS	"/cpm80/"
#else
#define PROGRAMS	"/"
#endif

#define BRAM_BASE	0xe400
#define BRAM_PAGES	7

#if defined(SPIRAM_CS)
#define SPIRAM_BASE	0x0000
#define SPIRAM_EXTENT	BRAM_BASE / 0x100

#else
#define RAM_BASE	0x0000
#define RAM_PAGES	32
#endif

#define INSTRUCTIONS	1000

#define TFT_BG	BLACK
#define TFT_FG	YELLOW

#endif
