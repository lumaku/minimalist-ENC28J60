/*
 * ENC_Register.h
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

#ifndef ENC_REGISTER_H_
#define ENC_REGISTER_H_


//////////////////////////////////////////////////////
// SPI INSTRUCTION SET FOR THE ENC28J60
// When the microcontroller communicates with the ENC:Bei der Aufnahme der Kommunikation mit dem ENC besteht das erste Byte aus Opcode und Argument.
//   The first byte contains either an OP-code and an argument or a one-byte-long command.
//   Here, these OP-codes are used as bitmask. 
//
// Command bytes
#define ENC_READBUF  			0b00111010 		// Read Buffer Memory
#define ENC_WRITEBUF			0b01111010 		// Write Buffer Memory
#define ENC_RESET			0b11111111		// System Reset Command
// Bitmasks of opcodes 0bxxx0 0000
#define ENC_READCTRL_bm 		0b00000000		// Read Control register
#define ENC_WRITECTRL_bm 		0b01000000		// Write Control Register
#define ENC_BFS_bm			0b10000000		// Bit Field Set
#define ENC_BFC_bm			0b10100000		// Bit Field Clear
// addresses of important control registers
#define ENC_EIE					0x1B
#define ENC_EIR					0x1C
#define ENC_ESTAT				0x1D
#define ENC_ECON2				0x1E
#define ENC_ECON1				0x1F

// ECON1
#define ENC_ECON1_BANK0_bm		0x00
#define ENC_ECON1_BANK1_bm		0x01
#define ENC_ECON1_BANK2_bm		0x02
#define ENC_ECON1_BANK3_bm		0x03
#define ENC_ECON1_BNKSEL_bm 	0x03
#define ENC_ECON1_RXEN_bm 		0x04
#define ENC_ECON1_TXRTS_bm		0x08
#define ENC_ECON1_RXRST_bm 		0x40
#define ENC_ECON1_TXRST_bm		0x80

#define ENC_ECON2_PKTDEC_bm		0x40

// common registers for error detection
#define ENC_EIR_RXERIF			0x01
#define ENC_EIR_TXERIF			0x02
#define ENC_EIR_TXIF			0x08
#define ENC_EIR_PKTIF			0x40

// registers in bank 0
#define ENC_ERDPTL				0x00
#define ENC_ERDPTH				0x01
#define ENC_EWRPTL				0x02
#define ENC_EWRPTH				0x03
#define ENC_ETXSTL				0x04
#define ENC_ETXSTH				0x05
#define ENC_ETXNDL				0x06
#define ENC_ETXNDH				0x07
#define ENC_ERXSTL				0x08
#define ENC_ERXSTH				0x09
#define ENC_ERXNDL				0x0A
#define ENC_ERXNDH				0x0B
#define ENC_ERXRDPTL			0x0C
#define ENC_ERXRDPTH			0x0D
#define ENC_ERXWRPTL			0x0E
#define ENC_ERXWRPTH			0x0F

// registers in bank 1
#define ENC_ERXFCON				0x18
#define ENC_EPKTCNT				0x19

// registers in bank 2
#define ENC_MACON1				0x00
#define ENC_MACON3				0x02
#define ENC_MACON4				0x03
#define ENC_MABBIG				0x04
#define ENC_MAIPGL				0x06
#define ENC_MAIPGH				0x07
#define ENC_MACLCON1				0x08
#define ENC_MACLCON2				0x09
#define ENC_MAMXFLL				0x0A
#define ENC_MAMXFLH				0x0B
#define ENC_MICMD				0x12
#define ENC_MIREGADR				0x14
#define ENC_MIWRL				0x16
#define ENC_MIWRH				0x17
#define ENC_MIRDL				0x18
#define ENC_MIRDH				0x19

// registers in bank 3
#define ENC_MAADR5				0x00
#define ENC_MAADR6				0x01
#define ENC_MAADR3				0x02
#define ENC_MAADR4				0x03
#define ENC_MAADR1				0x04
#define ENC_MAADR2				0x05
#define ENC_MISTAT				0x0A
#define ENC_EREVID				0x12
#define ENC_ECOCON				0x15
#define ENC_EFLOCON 			0x17
#define ENC_EPAUSL	 			0x18
#define ENC_EPAUSH	 			0x19

// PHY-Register
#define ENC_PHCON1	 			0x00
#define ENC_PHSTAT1	 			0x01
#define ENC_PHID1	 			0x02
#define ENC_PHID2	 			0x03
#define ENC_PHCON2	 			0x10
#define ENC_PHSTAT2	 			0x11
#define ENC_PHIE	 			0x12
#define ENC_PHIR	 			0x13
#define ENC_PHLCON	 			0x14


// ----------------------------------
// register values for initialization

// Der TX buffer should be allocated automatically (really?)
// Errata Sheet Rev.7:
//		RX Puffer:		0000h to n
//						0x0000 bis 0x0FFF
//		TX Puffer:		((n + 1) to 8191)
//						0x1000 bis 0x1FFF
#define ENC_ERXST_INIT			0x0000
#define ENC_ERXSTL_INIT			0x00
#define ENC_ERXSTH_INIT			0x00
#define ENC_ERXND_INIT			0x0FFF
#define ENC_ERXNDL_INIT			0xFF
#define ENC_ERXNDH_INIT			0x0F
// TX Puffer
#define ENC_ETXSTL_INIT			0x00
#define ENC_ETXSTH_INIT			0x10
#define ENC_ETXNDL_INIT			0xFF
#define ENC_ETXNDH_INIT			0x1F


// Media Access Control
#define ENC_MACON1_INIT			0x01
#define ENC_MACON3_INIT			0xF0// | 0x02 for frame length check
#define ENC_MACON4_INIT			0x40
#define ENC_MABBIG_INIT			0x12
#define ENC_MAIPGL_INIT			0x12
#define ENC_MAIPGH_INIT			0x0C
//#define ENC_MAMXFLL_INIT		0x00
//#define ENC_MAMXFLH_INIT		0x06
#define ENC_MAMXFLL_INIT		0x80
#define ENC_MAMXFLH_INIT		0x00
#define ENC_MAMXFL_INIT			0x0600

// further settings
#define ENC_MACON1_PASSALL_bm 			0x02
#define ENC_MICMD_MIIRD				0x01


// filter settings
//#define ENC_ERXFCON_INIT		0xA1 // preset
#define ENC_ERXFCON_UCEN_bm		0x80
#define ENC_ERXFCON_ANDOR_bm	0x40
#define ENC_ERXFCON_CRCEN_bm	0x20
#define ENC_ERXFCON_PMEN_bm		0x10
#define ENC_ERXFCON_MPEN_bm		0x08
#define ENC_ERXFCON_HTEN_bm		0x04
#define ENC_ERXFCON_MCEN_bm		0x02
#define ENC_ERXFCON_BCEN_bm		0x01
#define ENC_ERXFCON_INIT		(ENC_ERXFCON_UCEN_bm | ENC_ERXFCON_CRCEN_bm)

// PHY - Register
// LED Control Register, default value: 0x3422
#define ENC_PHLCONH_INIT		0x31 // Led A blinks at TX
#define ENC_PHLCONL_INIT		0x22 // Led B blinks at RX
// PHCON1
#define ENC_PHCON1H_INIT		0x00
#define ENC_PHCON1L_INIT		0x00
// PHCON2
#define ENC_PHCON2H_INIT		0x01
#define ENC_PHCON2L_INIT		0x00


//////////////////////////////////////////////////////

// Ethernet Configuration Standard Byte
// (not needed, it's possible to use the configuration of MACON)



#endif /* ENC_REGISTER_H_ */
