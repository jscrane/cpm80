Missing Port I/O
----------------

Input polling:
==============

These are ```cons1_in```, ```cons2_in``` and ```nets1_in``` on z80pack.

```
IO: unhandled IN(40)
IO: unhandled IN(42)
IO: unhandled IN(50)
```

They are called from ```tty_pollirq()``` in ```Kernel/dev/z80pack/devtty.c```.

Defined in ```devtty.c```:
```
__sfr __at 0 tty1stat;
__sfr __at 1 tty1data;
__sfr __at 40 tty2stat;
__sfr __at 41 tty2data;
__sfr __at 42 tty3stat;
__sfr __at 43 tty3data;
__sfr __at 50 tty4stat;
__sfr __at 51 tty4data;
```

A received character is dispatched by ```tty_inproc()``` in ```Kernel/tty.c```.

These are ignored at the moment. (All I/O is to the Console.)

Clock:
======

This is ```clkc_out``` on z80pack:
```
IO: unhandled OUT(25, 255)
IO: unhandled OUT(25, 0)
```

These arise from ```zrtc_init()``` in ```Kernel/dev/z80pack/devrtc.c```.

This is ```clkd_in``` on z80pack:
```
IO: unhandled IN(26)
```

See ```z80pack/iodevices/rtc80.c```. What is returned from this depends on the last
command sent with ```clkc_out```.
