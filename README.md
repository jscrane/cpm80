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
