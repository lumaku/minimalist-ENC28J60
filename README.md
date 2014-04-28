minimalist-ENC28J60
===================

Features:

- a truly minimalist initialisation routine for the ENC28J60 ethernet controller
- a small IP implementation
- a small SPI driver talking to the ENC28J60
- programmed for the Atmel Atxmega32A4


Further information
-------------------

These files were part of a bigger project for a bootloader for a µC of the Atmel Xmega processor family.

I put a lot of work into getting the smallest program possible out of it. The project never finished, as the effort was too big to put the complete bootloader into 4 kilobytes of flash, which is the size of the .boot section of the µC.

I'd estimate the NVM size of the compiled code (with compiler size optimization, of course) to about 300 bytes for the ENC28J60 driver and to about 600 bytes for the included files.
