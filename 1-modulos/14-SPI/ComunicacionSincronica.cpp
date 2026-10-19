/*
 * ComunicacionSincronica.cpp
 *
 *  Created on: Sep 28, 2026
 *      Author: User
 */

#include <1-modulos/14-SPI/ComunicacionSincronica.h>

ComunicacionSincronica::ComunicacionSincronica() {
	// TODO Auto-generated constructor stub

}

ComunicacionSincronica::~ComunicacionSincronica() {
	// TODO Auto-generated destructor stub
}

#include "ComunicacionSincronica.h"

bool ComunicacionSincronica::Transmitir(
    const uint8_t *datos,
    uint32_t cantidad
)
{
    return Transferir(datos, nullptr, cantidad);
}
