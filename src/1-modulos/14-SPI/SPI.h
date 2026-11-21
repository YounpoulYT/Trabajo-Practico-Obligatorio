#ifndef SPI_H_
#define SPI_H_

#include "LPC845.h"                   // Registros del micro, como en Uart.h.
#include "ComunicacionSincronica.h"   // Interfaz que implementa esta clase.

// Bus SPI maestro: 8 bits por dato y bit mas significativo primero.
// Esta version consulta STAT; no utiliza interrupciones ni buffers internos.
// El controlador del dispositivo se encarga de su seleccion (CS/LOAD).
class Spi : public ComunicacionSincronica
{
private:
    // Igual que USART_Type *m_usart en la clase Uart del profesor.
    // Apunta al bloque de registros del periferico, no a un objeto Spi.
    SPI_Type *m_spi;

    uint8_t m_numeroSPI;   // 0: SPI0. 1: SPI1.

    uint8_t m_puertoSCK;   // SCK: reloj que sale del micro.
    uint8_t m_pinSCK;

    uint8_t m_puertoMOSI;  // MOSI: datos que salen del micro.
    uint8_t m_pinMOSI;

    uint8_t m_puertoMISO;  // MISO: datos que entran al micro.
    uint8_t m_pinMISO;

    bool m_inicializado;  // true despues de configurar correctamente el SPI.

public:
    // Guarda el periferico elegido y los pines. Todavia no toca el hardware.
    // Omitir MISO deja su pin en 0xFF, que usaremos como "sin conexion".
    Spi(uint8_t numeroSPI,
        uint8_t puertoSCK,  uint8_t pinSCK,
        uint8_t puertoMOSI, uint8_t pinMOSI,
        uint8_t puertoMISO = 0, uint8_t pinMISO = 0xFF);

    // Configura el hardware despues de establecer el reloj de la placa.
    // relojHz: main_clk real, antes de SYSAHBCLKDIV. No modifica el PLL.
    // velocidadHz: frecuencia maxima deseada para SCK.
    // modo: 0, 1, 2 o 3. Se utiliza 0 si se omite.
    bool Inicializar(uint32_t relojHz, uint32_t velocidadHz, uint8_t modo = 0);

    // Envia tx y guarda en rx los bytes que llegan al mismo tiempo.
    // rx puede ser nullptr cuando solamente queremos enviar.
    // Se hereda Transmitir() de ComunicacionSincronica.
    bool Transferir(const uint8_t *tx, uint8_t *rx, uint32_t cantidad) override;

private:
    // Ayuda a Transferir: espera una bandera de STAT con un limite de consultas.
    // Si falla, detiene el periferico y exige volver a inicializarlo.
    bool Esperar(uint32_t bandera);
};

#endif
