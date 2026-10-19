#include "dr_pll.h"
#include "Gpio.h"
#include "C:\Users\User\Downloads\R2002\src\1-modulos\14-SPI\SPI.h"
#include "C:\Users\User\Downloads\R2002\src\1-modulos\MAX7219.h"




static void DelayAproxMs(uint32_t ms)
{
    while (ms--)
    {
        for (volatile uint32_t i = 0; i < 300; i++)
        {
            __asm volatile ("nop");
        }
    }
}


// ============================================================
// CARITA 8x8
//
// 00111100     ****
// 01000010    *    *
// 10100101   * *  * *
// 10000001   *      *
// 10000001   *      *
// 10100101   * *  * *
// 01011010    * ** *
// 00111100     ****
//
// ============================================================

const uint8_t CARITA[8] =
{
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10000001,
    0b10100101,
    0b01011010,
    0b00111100
};


// ============================================================
// DIBUJAR UNA CARITA EN UN MODULO 8x8
//
// modulo:
// 0 -> columnas 0..7
// 1 -> columnas 8..15
// 2 -> columnas 16..23
// 3 -> columnas 24..31
// ============================================================

void DibujarCarita(Max7219 &matriz, uint8_t modulo)
{
    uint8_t columnaInicial = modulo * 8;

    for (uint8_t fila = 0; fila < 8; fila++)
    {
        for (uint8_t columna = 0; columna < 8; columna++)
        {
            bool encendido =
                CARITA[fila] & (1U << (7 - columna));

            matriz.SetPixel(
                fila,
                columnaInicial + columna,
                encendido
            );
        }
    }
}


// ============================================================
// MAIN
// ============================================================

int main(void)
{
    // --------------------------------------------------------
    // CLOCK
    // --------------------------------------------------------

    Inicializar_PLL();


    // --------------------------------------------------------
    // LOAD / CS
    //
    // P0_17
    // --------------------------------------------------------

    Gpio loadMatriz(
        Gpio::PORT0,
        17,
        Gpio::PUSHPULL,
        Gpio::OUTPUT,
        Gpio::LOW
    );


    // --------------------------------------------------------
    // SPI0
    //
    // P0_16 -> CLK
    // P0_27 -> DIN
    // --------------------------------------------------------

    Spi spiMatriz(
        0,
        Gpio::PORT0, 16,     // SCK / CLK
        Gpio::PORT0, 27      // MOSI / DIN
    );


    // --------------------------------------------------------
    // INICIALIZAR SPI
    // --------------------------------------------------------

    if (!spiMatriz.Inicializar(
            24000000UL,
            500000UL,
            0))
    {
        while (1)
        {
        }
    }


    // --------------------------------------------------------
    // MAX7219
    // --------------------------------------------------------

    Max7219 matriz(
        &spiMatriz,
        &loadMatriz
    );


    // Intensidad inicial minima
    if (!matriz.Inicializar(0))
    {
        while (1)
        {
        }
    }


    // ========================================================
    // DIBUJAR LAS CUATRO CARITAS
    // ========================================================

    matriz.Limpiar();


    DibujarCarita(matriz, 0);
    DibujarCarita(matriz, 1);
    DibujarCarita(matriz, 2);
    DibujarCarita(matriz, 3);


    // Mandamos la imagen UNA SOLA VEZ
    if (!matriz.Actualizar())
    {
        while (1)
        {
        }
    }


    // ========================================================
    // EFECTO DE INTENSIDAD
    // ========================================================

    uint8_t intensidad = 0;

    bool subiendo = true;


    // Intensidad baja.
    // El MAX7219 admite 0..15.
    // Nosotros usamos solamente 0..5.
    const uint8_t INTENSIDAD_MAXIMA = 5;


    while (1)
    {
        // Los cuatro MAX7219 cambian juntos
        if (!matriz.SetIntensidad(intensidad))
        {
            while (1)
            {
            }
        }


        // Velocidad del efecto de "respiracion"
        DelayAproxMs(120);


        // ----------------------------------------------------
        // SUBIR / BAJAR
        // ----------------------------------------------------

        if (subiendo)
        {
            if (intensidad < INTENSIDAD_MAXIMA)
            {
                intensidad++;
            }
            else
            {
                subiendo = false;
                intensidad--;
            }
        }
        else
        {
            if (intensidad > 0)
            {
                intensidad--;
            }
            else
            {
                subiendo = true;
                intensidad++;
            }
        }
    }
}
