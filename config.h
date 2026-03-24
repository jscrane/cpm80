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

#define RAM_BASE	0x0000u
#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)
#define SPIRAM_BASE	RAM_SIZE
#define SPIRAM_EXTENT	(0x10000u - RAM_SIZE)

#define BG_COLOUR	BLACK
#define FG_COLOUR	WHITE

#if defined(USE_ESPI)
#define ORIENT	reverse_landscape
#else
#define ORIENT	portrait
#endif
