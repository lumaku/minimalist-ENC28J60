/*
 * config.h
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


#ifndef CONFIG_H_
#define CONFIG_H_


// specific PIN definitions for the communication to the ENC28J60
#define SPIzuENC_PORT		PORTC
#define SPIzuENC_SPIMODUL	SPIC
#define SPIzuENC_SS		PIN4_bm
#define SPIzuENC_MOSI		PIN5_bm
#define SPIzuENC_MISO		PIN6_bm
#define SPIzuENC_SCK		PIN7_bm
#define SPIzuENC_AN		( SPIzuENC_PORT.OUTCLR = SPIzuENC_SS )
#define SPIzuENC_AUS		( SPIzuENC_PORT.OUTSET = SPIzuENC_SS )

// do you want to reconfigure the LED behaviour on the ENC28J60?
//#define _LED_Konfiguration

// MAC address host
#define ENC_MAADR1_INIT			0x02
#define ENC_MAADR2_INIT			0x02
#define ENC_MAADR3_INIT			0x03
#define ENC_MAADR4_INIT			0x04
#define ENC_MAADR5_INIT			0x05
#define ENC_MAADR6_INIT			0x06

// MAC address server
#define Server_MAADR1				0xFF
#define Server_MAADR2				0xFF
#define Server_MAADR3				0xFF
#define Server_MAADR4				0xFF
#define Server_MAADR5				0xFF
#define Server_MAADR6				0xFF

// IP addresses
#define IPBootLoader = 192, 168, 1, 42
#define IPBootServer = 192, 168, 1, 43

// precalculated IP packet checksums
#define IPChecksum1		0xb2
#define IPChecksum2		0x1f //IP.230
//#define IPChecksum2		0x9e //IP.103


#endif /* CONFIG_H_ */
