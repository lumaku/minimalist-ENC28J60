/*
 * ENCTreiber.c
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
 *-----------------------------------------------------
 *
 *	Initialization, sending and receiving routines are based
 *      on the ENC28J60 datasheet and the B7 Errata Rev.
 *	
 *	This driver suits the ENC28J60 Rev. 7.
 *
 *	To find out which revision you own, read the chips register
 *	EREVID that contains his product number.
 *      (this function is already programmed and in a comment)
 *
 *      in this version: ENC_EREVID = 0x06
 *			-> Revision 7
 */


// Remember !!
//  ... to adapt the functions EmpfangePaket() and PaketsendungStart() to your needs
// these functions have been optimized to send and receive packets only of a specific length


// programming this driver was a very frustrating experience.

#include "ENCTreiber.h"

// pointer to the next packet
static uint16_t NaechstPkt = ENC_ERXST_INIT;


// initialize the ENC
void InitialisiereENC()
{
	// Soft Reset
	SPI_SendeCMD(ENC_RESET,0);


	//       wait at least    300 us
	// B7 Errata Sheet: wait    1 ms
	// calculated at 2 MHz: 3 us for every nop 
	// 0xFF --> 765 us waiting time
	uint8_t i = 0xFF;
	while (i > 0) {
		__asm__ volatile("nop");
		i--;
	}


	// for further details about the cmd's --> see documentation
	uint8_t Initialisierung[] = {	// Initialization:
						// Bank0 is set at the beginning
						// first step: setting allocation pointer to the RX buffer
						// Errata Sheet B7: TX buffer *has* to be at the end of the total buffer
						// The automatic allocation of the TX buffer (as described in the datasheet)
						//   doesn't seem to work when the RX buffer is allocated from 0x0000 to n.
					ENC_WRITECTRL_bm | ENC_ERXSTL, ENC_ERXSTL_INIT,
					ENC_WRITECTRL_bm | ENC_ERXSTH, ENC_ERXSTH_INIT,
					// RX END - has to be set at first to the correct value
					ENC_WRITECTRL_bm | ENC_ERXNDL, ENC_ERXNDL_INIT,
					ENC_WRITECTRL_bm | ENC_ERXNDH, ENC_ERXNDH_INIT,
/*						// TX Start	//--- TX will be set automatically
 					ENC_WRITECTRL_bm | ENC_ETXSTL, ENC_ETXSTL_INIT,
 					ENC_WRITECTRL_bm | ENC_ETXSTH, ENC_ETXSTH_INIT,
						// TX End
 					ENC_WRITECTRL_bm | ENC_ETXNDL, ENC_ETXNDL_INIT,
 					ENC_WRITECTRL_bm | ENC_ETXNDH, ENC_ETXNDH_INIT,
*/						// Lese-Pointer setzen
					ENC_WRITECTRL_bm | ENC_ERDPTL, ENC_ERXSTL_INIT,
					ENC_WRITECTRL_bm | ENC_ERDPTH, ENC_ERXSTH_INIT,
						// ERXFCON: you need a RX filter?
					//ENC_WRITECTRL_bm | ENC_ERXFCON, ENC_ERXFCON_INIT,
						// if you want to check for ESTAT:CLKRDY, here is a good place for it.
						// MAC - Konfiguration
						// step into Bank 2
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK2_bm,
						// MACON1 (0x01)
					ENC_WRITECTRL_bm | ENC_MACON1, ENC_MACON1_INIT,
						// MACON3 (0xF0)
					ENC_WRITECTRL_bm | ENC_MACON3, ENC_MACON3_INIT,
						// MACON4 (0x40)
					ENC_WRITECTRL_bm | ENC_MACON4, ENC_MACON4_INIT,
						// MAMXFL Maximum Frame Length
						// the current value is totally sufficient, change at own gusto
// 					ENC_WRITECTRL_bm | ENC_MAMXFLL, ENC_MAMXFLL_INIT,
// 					ENC_WRITECTRL_bm | ENC_MAMXFLH, ENC_MAMXFLH_INIT,
						// MABBIG = 0x12 for half duplex
					ENC_WRITECTRL_bm | ENC_MABBIG, ENC_MABBIG_INIT,
						// half duplex? -> MAIPG = 0x0C12
					ENC_WRITECTRL_bm | ENC_MAIPGL, ENC_MAIPGL_INIT,
					ENC_WRITECTRL_bm | ENC_MAIPGH, ENC_MAIPGH_INIT,
						// MAC address
						// change to Bank3
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK3_bm,
						// address from the configuration files
					ENC_WRITECTRL_bm | ENC_MAADR1, ENC_MAADR1_INIT,
					ENC_WRITECTRL_bm | ENC_MAADR2, ENC_MAADR2_INIT,
					ENC_WRITECTRL_bm | ENC_MAADR3, ENC_MAADR3_INIT,
					ENC_WRITECTRL_bm | ENC_MAADR4, ENC_MAADR4_INIT,
					ENC_WRITECTRL_bm | ENC_MAADR5, ENC_MAADR5_INIT,
					ENC_WRITECTRL_bm | ENC_MAADR6, ENC_MAADR6_INIT,
						// Promiscous Mode (only needed for testing)
					//ENC_WRITECTRL_bm| ENC_ERXFCON, 0x00,
					// ---------
					#ifdef _LED_Konfiguration
					// configure LEDs on RX
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK2_bm,
					ENC_WRITECTRL_bm | ENC_MIREGADR, ENC_PHLCON,
					ENC_WRITECTRL_bm | ENC_MIWRL, ENC_PHLCONL_INIT, // write the low byte first
					ENC_WRITECTRL_bm | ENC_MIWRH, ENC_PHLCONH_INIT,
					//-----------
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK3_bm,
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK2_bm,
					ENC_WRITECTRL_bm | ENC_MIREGADR, ENC_PHCON2,
					ENC_WRITECTRL_bm | ENC_MIWRH, ENC_PHCON2H_INIT, // writing only the high byte
					//-----------
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK3_bm,
					ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm,
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK2_bm,
					ENC_WRITECTRL_bm | ENC_MIREGADR, ENC_PHCON1,
					ENC_WRITECTRL_bm | ENC_MIWRH, ENC_PHCON1H_INIT, // only the high byte
					#endif // _LED_Konfiguration
						// RX enable, startet den Empfang
					ENC_BFS_bm | ENC_ECON1, ENC_ECON1_RXEN_bm
					};
	i = 0;

	while (i < sizeof(Initialisierung)) {
		if ((i%2)==0) SPIzuENC_AN;
		SPI_SendeByte(Initialisierung[i]);
		if (i%2) SPIzuENC_AUS;
		i++;
	}

	// if you want to immediately start RX:
	// SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_RXEN_bm);

	// Check revision:
	// uint8_t revision_id = SPI_SendeCMD(ENC_READCTRL_bm | ENC_EREVID, 0x00);
}


/////////// sending
// ENC28J60 Manual:
// 	To achieve the example layout shown in Figure 7-2 and to transmit a packet, the host controller should:
// 	1. Appropriately program the ETXST Pointer to point to an unused location in memory. It will
// 	point to the per packet control byte. In the example, it would be programmed to 0120h. It is
// 	recommended that an even address be used for ETXST.
// 	2. Use the WBM SPI command to write the per packet control byte, the destination address, the
// 	source MAC address, the type/length and the data payload.
// 	3. Appropriately program the ETXND Pointer. It should point to the last byte in the data payload.
// 	4. Clear EIR.TXIF, set EIE.TXIE and set EIE.INTIE to enable an interrupt when done (if desired).
// 	5. Start the transmission process by setting ECON1.TXRTS.


// prepare the ENC to send a packet
void PaketsendungStart()
{
	// wait until ENC is done sending the previous packet?
	//while((SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, 0x00)) & (ENC_ECON1_TXRTS_bm)) {}

	// ECON1.TXRST - Transmit Logic Reset - B7 Errata Sheet
	// ECON1.TXRST set and erase
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_TXRST_bm);
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_TXRST_bm);

	// If you want to be truly conservative about your program
	// space, reset the ENC28J60 (but then again, use another ethernet controller)
	// InitialisiereENC();

	// Bank 0
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK0_bm);
	// then, again ETXST = ENC_ETXST_INIT
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ETXSTL, ENC_ETXSTL_INIT);
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ETXSTH, ENC_ETXSTH_INIT);
	// set ETXND, TX buffer pointer
	// packet length:
	// (1 byte Per Packet Control) + 2 byte Ethernet Header + 12 byte MAC address + 20 bytes IP Header + 8 bytes UDP Header + 48 bytes payload
	//		= 90 bytes = 0x5A
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ETXNDL, ENC_ETXSTL_INIT + 0x5A);
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ETXNDH, ENC_ETXSTH_INIT);

	// EWRPT has to be initialized, too (but it is not written in the datasheet)
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_EWRPTL, ENC_ETXSTL_INIT);
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_EWRPTH, ENC_ETXSTH_INIT);

	// packet contents can now be transmitted
	SPIzuENC_AN;
}


// packet contents written to ENC buffer -> send the packet over the ether
void PaketsendungEnde()
{
	// packet done
	SPIzuENC_AUS;

	// EIR.TXIF = 0;
	SPI_SendeCMD(ENC_BFC_bm | ENC_EIR, ENC_EIR_TXERIF | ENC_EIR_TXIF);

	// ECON1.TXRTS: "Ready to send"
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_TXRTS_bm);
}


/////////// Receiving

// read in received packets
// returns true if there was a packet
_Bool EmpfangePaket(uint8_t *Zwischenspeicher, uint8_t *Paketlaenge)
{
	uint16_t k; // length of received packet
	uint16_t naechst, MaxByte;
	uint8_t i; // number of received packets
        uint8_t  m,n, *Ziel = Zwischenspeicher;
	_Bool SetzeZurueck = 0; // if true, reset the ENC after reading the packet
	// Bank 1 (should already be set, but to be sure...)
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK1_bm);

	// B7 Errata Sheet:
	// values from EPKTCNT are more reliable than from EIR.PKTIF
	i = SPI_SendeCMD( ENC_READCTRL_bm | ENC_EPKTCNT, 0x00);

	// is there a packet?
	if (i == 0) return 0;

	// start reading the RX'd packet
	SPIzuENC_AN;
	SPI_SendeByte(ENC_READBUF);

	// pointer to the next packet
	naechst = SPI_EmpfangeByte();
	naechst |= SPI_EmpfangeByte() << 8;
	NaechstPkt = naechst;

	// watch out for little endianness
	// read packet length here 
	m = SPI_EmpfangeByte();
	n = SPI_EmpfangeByte();

	// reset RX logic if the received packet length is odd (best solution to that RX pointer problem)
	SetzeZurueck |= m % 2;

	// byte count of packet
	k = (uint16_t)((n << 8) | (m));
	MaxByte = PROGMEM_PAGE_SIZE + 42 /* Eth Header */ + 2* AES_Blocklaenge /* Protokolloverhead */;

	// ckeck for expected packet length k
	if (k > MaxByte | k < 42)
	{
		k = 0;
		*Paketlaenge = 0;
	} else *Paketlaenge = k - 42;

	m = SPI_EmpfangeByte() & 0x80;
	SPI_EmpfangeByte();

	i = 0;
	if (m) // has the packet been rx'd successfully? -> read packet from ENC
	{
		while (k)
		{
			k--;
			// i > 42 -> protocol header (IP / UDP) finished

			if (i > 42)
			{
				// receive packet contents
				*(Ziel++) = SPI_EmpfangeByte();
			}
			else
			{
				// read header
				n = SPI_EmpfangeByte();

				// protocol header could be checked here (if you'd like it to check it on-the-fly)
				// e.g. for the correct port at i == 36
				i++;
			}
		}
	}
	else
	{
		// packet RX error? -> reset ENC
		SetzeZurueck |= 1;
	}

	SPIzuENC_AUS;

	// Bank 0 
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK0_bm);

	// set the read pointer to the beginning of the next packet
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERDPTL, (uint8_t) (naechst & 0xFF));
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERDPTH, (uint8_t) (naechst >> 8));

	// free buffer
		// Rev. B7 Errata Sheet
		// wihtout this, there will be a RX buffer error and the ENC will hang up
		// Errata Sheet: if (naechst == ENC_ERXST_INIT)
	if (((naechst - 1) < ENC_ERXSTL_INIT)|((naechst -1) > ENC_ERXND_INIT)) {
			// ERXRDPT = ERXND
		SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERXRDPTL, ENC_ERXNDL_INIT);
		SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERXRDPTH, ENC_ERXNDH_INIT);
	} else
	{
		//ERXRDPT = Next Packet Pointer – 1
		naechst--;
		SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERXRDPTL, (uint8_t) ((naechst-1) & 0xFF));
		SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERXRDPTH, (uint8_t) ((naechst-1) >> 8)  );
	}

	// packet read -> decrement packet counter
	SPI_SendeCMD( ENC_BFS_bm | ENC_ECON2, ENC_ECON2_PKTDEC_bm);

	// reset RX logic (errata sheet)
	SetzeZurueck |= ((SPI_SendeCMD( ENC_READCTRL_bm | ENC_EIR, 0x00)) & ENC_EIR_RXERIF);

	// remember to reset the ENC when the received packet was invalid.
	// when the next pckt pointer is bigger than the end of the RX buffer -> reset.

	if (SetzeZurueck) {
		InitialisiereENC();
		return 0;
	}

	return 1;

} // EmpfangePaket




/////////// Debug


#ifdef _MacAdresseSeriellAusgeben
void GebeMACAdresseAus()
{
	// Bank 3
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK3_bm);

	uint8_t MAC[6];
	MAC[5] = SPI_SendeCMD(ENC_READCTRL_bm | ENC_MAADR6, ENC_MAADR6_INIT);
	MAC[4] = SPI_SendeCMD(ENC_READCTRL_bm | ENC_MAADR5, ENC_MAADR5_INIT);
	MAC[3] = SPI_SendeCMD(ENC_READCTRL_bm | ENC_MAADR4, ENC_MAADR4_INIT);
	MAC[2] = SPI_SendeCMD(ENC_READCTRL_bm | ENC_MAADR3, ENC_MAADR3_INIT);
	MAC[1] = SPI_SendeCMD(ENC_READCTRL_bm | ENC_MAADR2, ENC_MAADR2_INIT);
	MAC[0] = SPI_SendeCMD(ENC_READCTRL_bm | ENC_MAADR1, ENC_MAADR1_INIT);

	// Bank 1 again
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK1_bm);

	// well, you have to do something with the received MAC?
}
#endif // _MacAdresseSeriellAusgeben

#ifdef _ENCRegisterSeriellAusgeben
// reading registers from the ENC28J60
void CheckRegister()
{
	printf("Inhalte der Register:");
	printf("\r\nECON1: ");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ECON1, 0x00));
	printf("\r\nECON2: ");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ECON2, 0x00));
	printf("\r\nENC_ESTAT: ");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ESTAT, 0x00));
	printf("\r\nENC_EPKTCNT: ");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_EPKTCNT, 0x00));
	printf("\r\nENC_EIR: ");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_EIR, 0x00));
	printf("\r\nENC_EIE: ");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_EIE, 0x00));
	printf("\r\nERXRDPT: 0x");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ERXRDPTH, 0x00));
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ERXRDPTL, 0x00));
	printf("\r\nERDPT: 0x");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ERDPTH, 0x00));
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ERDPTL, 0x00));
	printf("\r\nEWRPT: 0x");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_EWRPTH, 0x00));
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_EWRPTL, 0x00));
	printf("\r\nERXWRPT: 0x");
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ERXWRPTH, 0x00));
	printf_byte(SPI_SendeCMD( ENC_READCTRL_bm | ENC_ERXWRPTL, 0x00));
	printf("\r\n");

	// LED status:
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK2_bm);
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_MIREGADR, ENC_PHLCON);
	SPI_SendeCMD(ENC_BFS_bm | ENC_MICMD, ENC_MICMD_MIIRD);
	// you have to wait if you want to access PHY registers.... that's why:
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK1_bm);
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK3_bm);
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK2_bm);
	// finished waiting.
	SPI_SendeCMD(ENC_BFC_bm | ENC_MICMD, ENC_MICMD_MIIRD);
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_MIRDH, 0x00));
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_MIRDL, 0x00));

}
#endif // _ENCRegisterSeriellAusgeben


#ifdef _TxPaketStatusSeriellAusgeben
void LesePaketSendeStatus()
{
	uint8_t n,m, k;
	// let's assume that you just received a packet
	// Bank 0
	SPI_SendeCMD(ENC_BFC_bm | ENC_ECON1, ENC_ECON1_BNKSEL_bm);
	SPI_SendeCMD(ENC_BFS_bm | ENC_ECON1, ENC_ECON1_BANK0_bm);

	// set the read pointer to the desired position
	n = SPI_SendeCMD(ENC_READCTRL_bm | ENC_ERDPTL, 0x00);
	m = SPI_SendeCMD(ENC_READCTRL_bm | ENC_ERDPTH, 0x00);

	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERDPTL, ENC_ETXSTL_INIT);
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERDPTH, ENC_ETXSTH_INIT);

	printf("WritePtr:");
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_EWRPTL, 0x00));
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_EWRPTH, 0x00));

	printf("Von:");
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_ETXSTL, 0x00));
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_ETXSTH, 0x00));

	printf("Bis:");
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_ETXNDL, 0x00));
	printf_byte(SPI_SendeCMD(ENC_READCTRL_bm | ENC_ETXNDH, 0x00));



	// there you have your seven bytes receive status!
	printf(", Status: ");
	SPIzuENC_AN;
	SPI_SendeByte(ENC_READBUF);
	// read seven bytes
	k = 0x07;
	while (k)
	{
		k--;
		printf_byte(SPI_EmpfangeByte());
	}
	SPIzuENC_AUS;
	printf(";\r\n");

	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERDPTL, n);
	SPI_SendeCMD(ENC_WRITECTRL_bm | ENC_ERDPTH, m);
}

#endif // _TxPaketStatusSeriellAusgeben
