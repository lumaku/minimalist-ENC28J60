/*
 * NetzStrg.h
 *
 *      Autor: Ludwig K�rzinger
 *		SS 2011, WS 2011/12, 2014
 *
 *      Im Rahmen einer Studienarbeit
 *		Implementierung eines sicheren Bootloaders mit SecTFTP
 *		auf dem Atxmega32A4
 *
 *
 *		Am
 *		Lehrstuhl f�r Sicherheit in der Informationstechnik
 *		Technische Universit�t M�nchen
 */


#ifndef NETZSTRG_H_
#define NETZSTRG_H_

#include <avr/io.h>
#include "config.h"
#include "ENCTreiber.h"
#include "SPIzuENC.h"
#include "ENC_Register.h"



// aus NetzStrg.c
void Paketsendung(uint8_t *Nutzlast);



#endif /* NETZSTRG_H_ */
