/*
 * SPIzuENC.h
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


#ifndef SPIZUENC_H_
#define SPIZUENC_H_

#include <avr/io.h>
#include "config.h"
#include "ENC_Register.h"


// from SPIzuENC.c
void InitialisiereSPI();
void SPI_SendeByte(char paket);
uint8_t SPI_EmpfangeByte();
uint8_t SPI_SendeCMD(uint8_t Byte1, uint8_t Byte2);


#endif /* SPIZUENC_H_ */
