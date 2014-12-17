- handling function keys
- disk cache
  - 128kB (23LC1024): 64kB disk cache + 64kB memory
  - 512 x 128-byte sectors for 8008 disk sectors (4 x 2002 x 128)
  - map sector index (0..8007) to cache index (0..511) by remainder on dividing by 512 (2^9)
  - need 4-bits/sector for occupancy id (sector index divided by 512): 256 bytes
  - need 1-bit/sector for presence/absence id: 64 bytes
- wordstar using instructions here:
  http://jefftranter.blogspot.ie/2014/03/wordstar-30-on-briel-altair-8800.html
