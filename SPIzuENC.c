/*
 * ISPzuENC.c
 *
 *      Autor: Ludwig Kürzinger
 *		SS 2011, WS 2011/12, 2014
 *
 *      Im Rahmen einer Studienarbeit
 *		Implementierung eines sicheren Bootloaders mit SecTFTP
 *		auf dem Atxmega32A4
 *
 *
 *		Am
 *		Lehrstuhl für Sicherheit in der Informationstechnik
 *		Technische Universität München
 */


// I constructed some functions here from the document AVR1309: Using the XMEGA SPI

// Microcontroller-specific commands for the ports, registers can be found in the
// AVR XMEGA A Manual Preliminary and in the included header file iox32a4.h

// The routines for sending and receiving are tailored for the needs of the ENC28J60

#include "SPIzuENC.h"


// initialize pins and interrupts.
void InitialisiereSPI()
{
	// enable ports
	SPIzuENC_PORT.DIRSET = SPIzuENC_SS | SPIzuENC_MOSI | SPIzuENC_SCK;
	SPIzuENC_PORT.DIRCLR = SPIzuENC_MISO;   // MISO input

	// Polling Mode
	SPIzuENC_SPIMODUL.INTCTRL = SPI_INTLVL_OFF_gc;

	// set SPI behaviour
	SPIzuENC_SPIMODUL.CTRL =  (SPI_MODE_0_gc | SPI_ENABLE_bm | SPI_MASTER_bm );

	// we don't have to send something now
	SPIzuENC_AUS;

	// you can check whether initialization has been successfull with:
	//if (SPIzuENC_SPIMODUL.CTRL & (SPI_ENABLE_bm | SPI_MASTER_bm)) success();
}

// send a single byte and wait until it is flushed out of the buffer
void SPI_SendeByte(char paket)
{
	SPIzuENC_SPIMODUL.DATA = paket;
	while (!(SPIzuENC_SPIMODUL.STATUS & SPI_IF_bm)) {}
}

// sends a zero byte and returns eventually received data.
// nothing received? return value 0x00
uint8_t SPI_EmpfangeByte()
{
	SPIzuENC_SPIMODUL.DATA = 0x00;
	while (!(SPIzuENC_SPIMODUL.STATUS & SPI_IF_bm)) {}
	return SPIzuENC_SPIMODUL.DATA;
}

// Control the ENC28J60
// this function can read and write registers
uint8_t SPI_SendeCMD(uint8_t Byte1, uint8_t Byte2)
{
	// enable transmission
	SPIzuENC_AN;

	// send the commands
	SPI_SendeByte(Byte1);
	SPI_SendeByte(Byte2);

	// A different handling for MAC or a MII register:
	// (for further information: ENC28J60 datasheet)
	// reading these registers one should dismiss the first sent byte
	// and evalute the second sent byte.
	// (in most cases the value of the first byte is correct, not as the datasheet says)

	// currently, the program does not use any of the affected registers.

	// nevertheless, the following lines will assure compability:
	if ((Byte1 & 0xE0) == ENC_READCTRL_bm) {
		SPIzuENC_SPIMODUL.DATA = 0x00;
		while (!(SPIzuENC_SPIMODUL.STATUS & SPI_IF_bm)) {}
	}

	// stop transmission
	SPIzuENC_AUS;
	return SPIzuENC_SPIMODUL.DATA;
}
