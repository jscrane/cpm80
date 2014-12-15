;	skeletal cbios for first level of CP/M 2.0 alteration
;
msize	equ	64		;cp/m version memory size in kilobytes
;
;	"bias" is address offset from 3400h for memory systems
;	than 16k (referred to as"b" throughout the text)
;
bias	equ	(msize-20)*1024
ccp	equ	3400h+bias	;base of ccp
bdos	equ	ccp+806h	;base of bdos
bios	equ	ccp+1600h	;base of bios
cdisk	equ	0004h		;current disk number 0=a,... l5=p
iobyte	equ	0003h		;intel i/o byte
;
	org	bios		;origin of this program
nsects	equ	($-ccp)/128	;warm start sector count
;
;	jump vector for individual subroutines
;
	jmp	boot	;cold start
wboote:	jmp	wboot	;warm start
	jmp	const	;console status
	jmp	conin	;console character in
	jmp	conout	;console character out
	jmp	list	;list character out
	jmp	punch	;punch character out
	jmp	reader	;reader character out
	jmp	home	;move head to home position
	jmp	seldsk	;select disk
	jmp	settrk	;set track number
	jmp	setsec	;set sector number
	jmp	setdma	;set dma address
	jmp	read	;read disk
	jmp	write	;write disk
	jmp	listst	;return list status
	jmp	sectran	;sector translate
;
;	fixed data tables for four-drive standard
;	ibm-compatible 8" disks
;
;	disk Parameter header for disk 00
dpbase:	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk00, all00
;	disk parameter header for disk 01
	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk01, all01
;	disk parameter header for disk 02
	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk02, all02
;	disk parameter header for disk 03
	dw	trans, 0000h
	dw	0000h, 0000h
	dw	dirbf, dpblk
	dw	chk03, all03
;	SECTOR TRANSLATE VECTOR
trans:	DB	 1,  7, 13, 19	;SECTORS  1,  2,  3,  4
 	DB	25,  5, 11, 17	;SECTORS  5,  6,  7,  6
 	DB	23,  3,  9, 15	;SECTORS  9, 10, 11, 12
 	DB	21,  2,  8, 14	;SECTORS 13, 14, 15, 16
 	DB	20, 26,  6, 12	;SECTORS 17, 18, 19, 20
 	DB	18, 24,  4, 10	;SECTORS 21, 22, 23, 24
     	DB	16, 22		;SECTORS 25, 26
;
dpblk:	;disk parameter block, common to all disks
	dw	26		;sectors per track
	db	3		;block shift factor
	db	7		;block mask
	db	0		;null mask
	dw	242		;disk size-1
	dw	63		;directory max
	db	192		;alloc 0
	db	0		;alloc 1
	dw	16		;check size
	dw	2		;track offset
;
;	end of fixed tables
;
;	individual subroutines to perform each function
boot:	;simplest case is to just perform parameter initialization
	xra	a		;zero in the accum
	sta	iobyte		;clear the iobyte
	sta	cdisk		;select disk zero
	jmp	gocpm		;initialize and go to cp/m
;
wboot:
gocpm:
	mvi	a, 0c3h		;c3 is a jmp instruction
	sta	0		;for jmp to wboot
	lxi	h, wboote	;wboot entry point
	shld	1		;set address field for jmp at 0
;
	sta	5		;for jmp to bdos
	lxi	h, bdos		;bdos entry point
	shld	6		;address field of Jump at 5 to bdos
;
	lxi	b, 80h		;default dma address is 80h
	call	setdma
;
	ei			;enable the interrupt system
	lda	cdisk		;get current disk number
	mov	c, a		;send to the ccp
	jmp	ccp		;go to cp/m for further processing
;
;
;	simple i/o handlers (must be filled in by user)
;	in each case, the entry point is provided, with space reserved
;	to insert your own code
;
const:	;console status, return 0ffh if character ready, 00h if not
	in	(2), A
	ret
;
conin:	;console character into register a
	in	(4), A
	ani	7fh		;strip parity bit
	ret
;
conout:	;console character output from register c
	mov	a, c		;get to accumulator
	out	(4), A
	ret
;
list:	;list character from register c
	mov	a, c	  	;character to register a
	ret		  	;null subroutine
;
listst:	;return list status (0 if not ready, 1 if ready)
	xra	a	 	;0 is always ok to return
	ret
;
punch:	;punch	character from	register C
	mov	a, c		;character to register a
	ret			;null subroutine
;
;
reader:	;reader character into register a from reader device
	mvi    a, 1ah		;enter end of file for now (replace later)
	ani    7fh		;remember to strip parity bit
	ret
;
;
;	i/o drivers for the disk follow
;	for now, we will simply store the parameters away for use
;	in the read and write	subroutines
;
home:	;move to the track 00	position of current drive
;	translate this call into a settrk call with Parameter 00
	mvi    c, 0		;select track 0
	call   settrk
	ret			;we will move to 00 on first read/write
;
seldsk:	;select disk given by register c
	lxi	h, 0000h	;error return code
	mov	a, c
	sta	diskno
	cpi	4		;must be between 0 and 3
	rnc			;no carry if 4, 5,...
;	disk number is in the proper range
;	compute proper disk Parameter header address
	lda	diskno
	out	(20), a
	mov	l, a		;l=disk number 0, 1, 2, 3
	mvi	h, 0		;high order zero
	dad	h		;*2
	dad	h		;*4
	dad	h		;*8
	dad	h		;*16 (size of each header)
	lxi	d, dpbase
	dad	d		;hl=,dpbase (diskno*16)
	ret
;
settrk:	;set track given by register c
	mov	a, c
	sta	track
	out	(21), a
	ret
;
setsec:	;set sector given by register c
	mov	a, c
	sta	sector
	out	(22), a
	ret
;
;
sectran:
	;translate the sector given by bc using the
	;translate table given by de
       	XCHG			;HL=.TRANS
       	DAD	B		;HL=.TRANS (SECTOR)
       	MOV	L, M		;L=TRANS (SECTOR)
     	MVI	H, 0		;HL=TRANS (SECTOR)
	ret			;with value in hl
;
setdma:	;set	dma address given by registers b and c
	mov	l, c		;low order address
	mov	h, b		;high order address
	shld	dmaad		;save the address
	out	(23), a
	ret
;
read:	;perform read operation
	in	(14), A
	ret
;
write:	;perform a write operation
	in	(15), A
	ret
;
;	the remainder of the cbios is reserved uninitialized
;	data area, and does not need to be a Part of the
;	system	memory image (the space must be available,
;	however, between"begdat" and"enddat").
;
track:	ds	2		;two bytes for expansion
sector:	ds	2		;two bytes for expansion
dmaad:	ds	2		;direct memory address
diskno:	ds	1		;disk number 0-15
;
;	scratch ram area for bdos use
begdat	equ	$	 	;beginning of data area
dirbf:	ds	128	 	;scratch directory area
all00:	ds	31	 	;allocation vector 0
all01:	ds	31	 	;allocation vector 1
all02:	ds	31	 	;allocation vector 2
all03:	ds	31	 	;allocation vector 3
chk00:	ds	16		;check vector 0
chk01:	ds	16		;check vector 1
chk02:	ds	16	 	;check vector 2
chk03:	ds	16	 	;check vector 3
;
enddat	equ	$	 	;end of data area
datsiz	equ	$-begdat;	;size of data area
	end
