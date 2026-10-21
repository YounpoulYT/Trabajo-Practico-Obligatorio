/*******************************************************************************************************************************//**
 *
 * @file		main.cpp
 * @brief		PRUEBA DE JOYSTICKS con los 3 LEDs del kit (sin matriz)
 *
 * Se muestra UN joystick a la vez en los LEDs (P1_0, P1_1, P1_2):
 *
 *     Palanca         LED0   LED1   LED2
 *     -----------     ----   ----   ----
 *     suelta           .      .      .
 *     IZQUIERDA        X      .      .
 *     ARRIBA           .      X      .
 *     DERECHA          .      .      X
 *     ABAJO            X      X      X
 *
 *  - Boton del joystick 1 (apretar la palanca, C0 = P0_26): muestra el J1. Parpadea 1 vez.
 *  - Boton del joystick 2 (apretar la palanca, C1 = P0_15): muestra el J2. Parpadea 2 veces.
 *  - Los DOS botones juntos: al soltarlos recalibra el centro de los dos joysticks. Parpadea 3 veces.
 *    (Se calibra al SOLTAR porque apretar la palanca la puede correr un poco del centro.)
 *  - Cada 250 ms manda a la PC (UART0, 9600) la linea  #JOY,x1,y1,x2,y2$  con los valores crudos (0..4095).
 *    Se ve con la terminal de MCUXpresso.
 *
 * Las conexiones estan en pines.h. Los objetos (g_joy1, g_led0, g_botonJoy1, ...) estan en hardware.cpp.
 *
 **********************************************************************************************************************************/

#include "hardware.h"

// ============================================================ LEDS

static void PonerLeds( bool led0 , bool led1 , bool led2 )
{
	g_led0 = led0;
	g_led1 = led1;
	g_led2 = led2;
}

static void MostrarDireccion( Joystick::direccion_t direccion )
{
	switch ( direccion )
	{
		case Joystick::IZQUIERDA:	PonerLeds( true  , false , false );	break;
		case Joystick::ARRIBA:		PonerLeds( false , true  , false );	break;
		case Joystick::DERECHA:		PonerLeds( false , false , true  );	break;
		case Joystick::ABAJO:		PonerLeds( true  , true  , true  );	break;
		default:					PonerLeds( false , false , false );	break;		// CENTRO
	}
}

// Parpadea los 3 LEDs "veces" veces (bloquea un momento; solo se usa al cambiar de joystick)
static void Parpadear( uint8_t veces )
{
	for ( uint8_t i = 0 ; i < veces ; i++ )
	{
		PonerLeds( true , true , true );
		Esperar( 150 );
		PonerLeds( false , false , false );
		Esperar( 150 );
	}
}

// ============================================================ PC

// Agrega un numero en texto al final de "texto" (sin sprintf, que ocupa mucha memoria)
static void AgregarNumero( char *texto , uint32_t &largo , int32_t numero )
{
	char cifras[ 12 ];
	uint8_t n = 0;

	if ( numero < 0 )
	{
		texto[ largo++ ] = '-';
		numero = -numero;
	}
	do
	{
		cifras[ n++ ] = '0' + ( numero % 10 );
		numero /= 10;
	} while ( numero > 0 );

	while ( n > 0 )
		texto[ largo++ ] = cifras[ --n ];
}

// Arma y envia:  #JOY,x1,y1,x2,y2$
static void EnviarJoysticks( void )
{
	char linea[ 48 ];
	uint32_t largo = 0;
	const char *encabezado = "#JOY,";

	while ( *encabezado )
		linea[ largo++ ] = *encabezado++;

	AgregarNumero( linea , largo , g_adcJoy1X.GetResultado() );	linea[ largo++ ] = ',';
	AgregarNumero( linea , largo , g_adcJoy1Y.GetResultado() );	linea[ largo++ ] = ',';
	AgregarNumero( linea , largo , g_adcJoy2X.GetResultado() );	linea[ largo++ ] = ',';
	AgregarNumero( linea , largo , g_adcJoy2Y.GetResultado() );
	linea[ largo++ ] = '$';
	linea[ largo++ ] = '\r';
	linea[ largo++ ] = '\n';

	g_uartPC.Transmitir( (const uint8_t *) linea , largo );
}

// ============================================================ MAIN

int main( void )
{
	if ( !InicializarHardware() )
	{
		while ( 1 )			// algo fallo al iniciar
		{
		}
	}

	Parpadear( 1 );			// arranco bien, mostrando el joystick 1

	Joystick *joyActual = &g_joy1;

	const uint32_t PERIODO_LEDS = 20;		// ms
	const uint32_t PERIODO_PC   = 250;		// ms

	uint32_t tLeds = Milisegundos();
	uint32_t tPC   = Milisegundos();

	bool calibracionPendiente = false;

	while ( 1 )
	{
		// ---------------- Botones de los joysticks (el antirrebote lo hace el SysTick) ----------------
		bool apretoJ1 = g_botonJoy1.FuePresionado();
		bool apretoJ2 = g_botonJoy2.FuePresionado();

		if ( g_botonJoy1.EstaPresionado() && g_botonJoy2.EstaPresionado() )
		{
			// Los dos juntos: se recalibra cuando se suelten
			calibracionPendiente = true;
		}
		else if ( calibracionPendiente )
		{
			if ( !g_botonJoy1.EstaPresionado() && !g_botonJoy2.EstaPresionado() )
			{
				Esperar( 200 );				// que las palancas vuelvan al centro
				g_joy1.Calibrar();
				g_joy2.Calibrar();
				Parpadear( 3 );
				calibracionPendiente = false;
			}
		}
		else if ( apretoJ1 )
		{
			joyActual = &g_joy1;
			Parpadear( 1 );
		}
		else if ( apretoJ2 )
		{
			joyActual = &g_joy2;
			Parpadear( 2 );
		}

		// ---------------- LEDs ----------------
		if ( Milisegundos() - tLeds >= PERIODO_LEDS )
		{
			tLeds += PERIODO_LEDS;
			MostrarDireccion( joyActual->GetDireccion() );
		}

		// ---------------- PC ----------------
		if ( Milisegundos() - tPC >= PERIODO_PC )
		{
			tPC += PERIODO_PC;
			EnviarJoysticks();
		}
	}
}
