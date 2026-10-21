/*******************************************************************************************************************************//**
 *
 * @file		tipos.h
 * @brief		Tipos de datos enteros de tamano fijo y calificadores de los registros
 *
 * Antes se definian a mano (typedef char int8_t, etc.). En ARM "char" es SIN signo, asi que
 * int8_t terminaba siendo unsigned. Ahora se usan los del compilador (<stdint.h>), que son correctos.
 *
 **********************************************************************************************************************************/

#ifndef TIPOS_H_
#define TIPOS_H_

#include <stdint.h>

#define		__I		volatile const		// registro de solo lectura
#define		__O		volatile			// registro de solo escritura
#define		__IO	volatile			// registro de lectura / escritura

#endif /* TIPOS_H_ */
