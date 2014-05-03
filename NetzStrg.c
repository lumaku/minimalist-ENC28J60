/*
 * NetzStrg.c
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
 *		
 *	----------------------------------------------------------------	
 *		
 *		
 *		
 *	Contains a minimalist IP/UDP implementation.
 *		
 *	The function Paketsendung() sends IP packets always with a	
 *	payload of 48 bytes. Please remember to adapt the values
 *	of packet length and the checksum to your needs.
 *		
 */




#include "NetzStrg.h"



// Send an IP/UDP packet
void Paketsendung(uint8_t *Nutzlast)
{
	// starting the transmission with signals to the ENC
	PaketsendungStart();
	// Header Length:
	// 2 bytes for the ENC, 12 bytes MAC-Adress, 2 bytes Packet Type (IPv4), 20 Byte IP-Header, 8 Byte UDP
	//		= 44 Byte
	// IP begins at the 16th byte

	// Header Senden:
	uint8_t Header[] = {
						// Der Befehl, das Paket zu schreiben, passt gut in den Header
						// Write Buffer Memory Command
						ENC_WRITEBUF,
						// "Per Packet Control Byte" for the ENC28J60
						0x00 ,
						// MAC address of the receiver
						// (multicast adresses possible)
						Server_MAADR1,
						Server_MAADR2,
						Server_MAADR3,
						Server_MAADR4,
						Server_MAADR5,
						Server_MAADR6,
						// own MAC address
						ENC_MAADR1,
						ENC_MAADR2,
						ENC_MAADR3,
						ENC_MAADR4,
						ENC_MAADR5,
						ENC_MAADR6,

						// -----

						// Ethernet header: packet type IPv4
						0x08, 0x00,

						// ----

						// IPv4 - RFC 798
						// 1 Byte "Version" and header length 0b01000101 = 0x45 (smallest possible)
						0x45,
						// 1B Type of Service
								0x00, // niedrigste Priorität?
						// 2B Total Length:
						// 20 Byte IP Header + 8B UDP Header + 16 Byte Daten + 16 Byte CMAC = 60 = 0x3C
						0x00,
								0x4C,
						// 2B Identifikation, can be anything
						0x40,
								0x40,
						// 2B Flags & Fragment Offset, 0x4000
						0x40,
								0x00,
						// 1B Time to Live
						// willkürlich gesetzter Wert
						0x80,
						// 1B Protocol UDP - RFC 790 Ass. Numbers
								0x11,
						// 2B Header Checksum
						IPChecksum1,
								IPChecksum2,
						// 4B Quell IP
						IPBootLoader1, IPBootLoader2,
						IPBootLoader3, IPBootLoader4, // in main.h
						// 4B Ziel IP
						IPBootServer1, IPBootServer2,
						IPBootServer3, IPBootServer4,
						// no options

						// ----

						// UDP - Header
						// 2B Quellport
						// temporärer Port 0xD903 = 55555
						0xD9,	0x03,
						// 2B Zielport
						0xD9,	0x03,
						// 2B length of UDP header (8 bytes) and UDP payload (48 bytes) = 56 = 0x28
						0x00,	0x38,
						// 2B Checksum of the UDP header and data.
						// can remain undefined
						0x00,	0x00

						// END OF HEADER
						};
	
	// preparing the payload
	uint8_t i = 0, *temp = Nutzlast;

	// Send the header byte-wise over SPI
	while (i < sizeof(Header)) {
		SPI_SendeByte(Header[i]);
		i++;
	}

	// Sending the payload ot the packet  == 48 bytes (without UDP/IP)
	// (This works only if the payload ot the packet is always 48 bytes long)
	for(i = 0; i < ( 48 ); i++){
		SPI_SendeByte(*(temp++));
	}
	
	// commandig the ENC to flush
	PaketsendungEnde();
}




