CP/M-80
=======

CP/M 8080 hardware emulator on Stellarpad, ESP8266 and ESP32, using [r65emu](jscrane/r65emu).

- Digital Research manual for [CP/M 2.2](http://www.gaby.de/cpm/manuals/archive/cpm22htm/)
- useful info [here](http://cpuville.com/Code/CPM-on-a-new-computer.html)
- [cpmtools](https://github.com/lipro-cpm4l/cpmtools) for linux
- software: [z80.eu](http://www.z80.eu/cpmsoft.html), [cpmarchives](http://cpmarchives.classiccmp.org/), [cpm.z80.de](http://www.cpm.z80.de/)
- fonts are from [here](https://jared.geek.nz/2014/01/custom-fonts-for-microcontrollers)

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

CP/M in ROM
-----------
A previous system was a "CP/M in ROM".

This consisted of two files (see the directory [system](system)):
- ```CBIOS.ASM``` the "skeletal CBIOS" from Appendix B of the
  [CP/M 2.2 manual](http://www.gaby.de/cpm/manuals/archive/cpm22htm/)
- ```CPM22.ASM``` CP/M 2.2 itself, from [here](http://cpuville.com/Code/cpm22_asm.txt)

Each of these was assembled to its corresponding ```.bin``` file by [asm8080](https://sourceforge.net/p/asm8080/)
and from there to ```.h``` by [makerom](https://github.com/jscrane/emul8/tree/master/util):

```
$ asm8080 CPM22.ASM
$ makerom -bp CPM22.h cpm22 > cpm22.h
```

The ROMs were loaded at addresses ```0xe400``` (cpm22) and ```0xfa00``` (cbios), leaving 57kB free.

Booting CP/M from disk
----------------------

The current system loads CP/M from disk, as nature intended. The boot disk ```cpma.cpm``` comes
from the excellent [emu8080](https://st.sdf-eu.org/i8080/index.html).

A modified 8080 port-mapping supports the BIOS found on this disk (see ```io.cpp```). The file
```system/bios8080.asm``` (from [z80pack](https://github.com/udo-munk/z80pack/tree/master/cpmsim/srccpm2)) 
is compatible with this port-mapping scheme --- it's a near ancestor of the BIOS in _emu8080_.

The system tracks for this are broken out into ```system/system.bin```. This can be used to create
bootable disk-images as follows:

```
$ mkfs.cpm -b system.bin foo.cpm
```

