/*
 * ComunicacionSincronica.h
 *
 *  Created on: Sep 28, 2026
 *      Author: User
 */
#ifndef COMUNICACION_SINCRONICA_H_
#define COMUNICACION_SINCRONICA_H_

#include "tipos.h"


class ComunicacionSincronica {
public:
	ComunicacionSincronica();

	virtual bool Transferir(const uint8_t *tx, uint8_t *rx, uint32_t cantidad) = 0;
	//permite enviar datos y guardar los que se reciben
	bool Transmitir(const uint8_t *datos, uint32_t cantidad);
	//sirve para enviar datos cuando no interesa guardar lo recibido
	virtual ~ComunicacionSincronica();
};




#endif
