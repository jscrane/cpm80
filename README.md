CP/M-80
=======

CP/M 8080 hardware emulator on Stellarpad, ESP8266 and ESP32, using [r65emu](jscrane/r65emu).

- Digital Research manual for [CP/M 2.2](http://www.gaby.de/cpm/manuals/archive/cpm22htm/)
- useful info [here](http://cpuville.com/Code/CPM-on-a-new-computer.html)
- [cpmtools](https://github.com/lipro-cpm4l/cpmtools) for linux
- software: [z80.eu](http://www.z80.eu/cpmsoft.html), [cpmarchives](http://cpmarchives.classiccmp.org/), [cpm.z80.de](http://www.cpm.z80.de/)
- fonts are from [here](http://jared.geek.nz/2014/jan/custom-fonts-for-microcontrollers)

Creating disks
--------------
Install ```cpmtools```:

```
$ sudo apt install cpmtools
```

Create a disk image:

```
$ mkfs.cpm foo
```

Copy files into it:

```
$ cpmcp foo bar baz 0:
```

Put it into ```disks/```:

```
$ mv foo disks
```

Edit ```disks/drivemap.txt``` to assign the new disk to a drive.

Use of SD card storage is recommended where possible.

Building CP/M
-------------
The current system is a "CP/M in ROM".

Two files are involved (see the directory [images](images)):
- ```CBIOS.ASM``` the "skeletal CBIOS" from Appendix B of the
  [CP/M 2.2 manual](http://www.gaby.de/cpm/manuals/archive/cpm22htm/)
- ```CPM22.ASM``` CP/M 2.2 itself, from [here](http://cpuville.com/Code/cpm22_asm.txt)

Each of these was assembled to corresponding ```.bin``` files by [asm8080](https://sourceforge.net/p/asm8080/)
and from there to ```.h``` files by [makerom](https://github.com/jscrane/emul8/tree/master/util):

```
$ asm8080 images/CPM22.ASM
$ makerom -bp images/CPM22.h cpm22 > roms/cpm22.h
```

The ROMs are loaded at addresses ```0xe400``` (cpm22) and ```0xfa00``` (cbios), leaving 57kB free.
