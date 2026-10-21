/*******************************************************************************************************************************//**
 *
 * @file		ComunicacionSincronica.cpp
 * @brief		Interfaz de una comunicacion serie sincronica (SPI)
 *
 **********************************************************************************************************************************/

#include "ComunicacionSincronica.h"

ComunicacionSincronica::ComunicacionSincronica()
{
}

ComunicacionSincronica::~ComunicacionSincronica()
{
}

// Enviar sin guardar lo que se recibe
bool ComunicacionSincronica::Transmitir( const uint8_t *datos , uint32_t cantidad )
{
	return Transferir( datos , nullptr , cantidad );
}
