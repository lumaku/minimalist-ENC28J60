/*
 * ENCTreiber.h
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


#ifndef ENCTREIBER_H_
#define ENCTREIBER_H_

#include "config.h"
#include "ENCTreiber.h"
#include "ENC_Register.h"
#include "SPIzuENC.h"
#include <avr/io.h>


// from ENCTreiber.c
void InitialisiereENC();
void PaketsendungStart();
void PaketsendungEnde();
_Bool EmpfangePaket(uint8_t *Zwischenspeicher, uint8_t *Paketlaenge);
void GebeMACAdresseAus();
void CheckRegister();
void LesePaketSendeStatus();


#endif /* ENCTREIBER_H_ */
