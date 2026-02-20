#pragma once

// max number of entries in drivemap.txt
// hardwired into BIOS:
// ;       fixed data tables for four-drive standard
// ;       ibm-compatible 8" disks
#define FD_DRIVES	4
#define HD_DRIVES	2
#define DRIVES (FD_DRIVES + HD_DRIVES)

#if !defined(PROGRAMS)
#if defined(USE_SD)
#define PROGRAMS	"/cpm80/"
#else
#define PROGRAMS	"/"
#endif
#endif

#if defined(USE_Z80)
#define PROCESSOR_H	"z80.h"
typedef class z80 processor_t;
#elif defined(USE_UZ80)
#define PROCESSOR_H	"uz80.h"
typedef class uz80 processor_t;
#else
#define PROCESSOR_H	"i8080.h"
typedef class i8080 processor_t;
#endif

// boot RAM (cpm and cbios)
// we _must_ have memory above $BRAM_BASE
#define BRAM_BASE	0xe400u
#define CPM_BASE	BRAM_BASE
#define CBIOS_BASE	0xfa00u

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

#define BG_COLOUR	BLACK
#define FG_COLOUR	WHITE

#if defined(USE_ESPI)
#define ORIENT	reverse_landscape
#else
#define ORIENT	portrait
#endif
