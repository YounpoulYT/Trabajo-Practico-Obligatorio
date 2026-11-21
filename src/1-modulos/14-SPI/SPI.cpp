#include "Spi.h"
#include "swm.h"

// Igual que el arreglo USARTS[] del profesor.
// Contiene DIRECCIONES DE REGISTROS de hardware, no objetos de la clase Spi.
SPI_Type *SPIS[] = { SPI0, SPI1 };

// Limite de consultas para evitar quedar atrapados en una espera.
// Es una cantidad de iteraciones, NO un tiempo expresado en microsegundos.
#define SPI_LIMITE_ESPERA 1000000UL

Spi::Spi(uint8_t numeroSPI,
         uint8_t puertoSCK, uint8_t pinSCK,
         uint8_t puertoMOSI, uint8_t pinMOSI,
         uint8_t puertoMISO, uint8_t pinMISO)
{
    // Guardamos los argumentos para usarlos despues en Inicializar().
    m_numeroSPI = numeroSPI;
    m_puertoSCK = puertoSCK;
    m_pinSCK = pinSCK;
    m_puertoMOSI = puertoMOSI;
    m_pinMOSI = pinMOSI;
    m_puertoMISO = puertoMISO;
    m_pinMISO = pinMISO;

    m_spi = nullptr;
    m_inicializado = false;
}

bool Spi::Inicializar(uint32_t relojHz, uint32_t velocidadHz, uint8_t modo)
{
    // 1. Revisamos los datos antes de acceder al hardware.
    if (m_numeroSPI > 1 || modo > 3 || relojHz == 0 ||
        relojHz > 30000000UL || velocidadHz == 0)
        return false;

    // Puerto 0: pines 0 a 31. Puerto 1: pines 0 a 21.
    // Ademas hay que verificar su disponibilidad en la placa real.
    if (m_puertoSCK > 1 || m_pinSCK > 31 ||
        (m_puertoSCK == 1 && m_pinSCK > 21))
        return false;
    if (m_puertoMOSI > 1 || m_pinMOSI > 31 ||
        (m_puertoMOSI == 1 && m_pinMOSI > 21))
        return false;
    if (m_pinMISO != 0xFF &&
        (m_puertoMISO > 1 || m_pinMISO > 31 ||
         (m_puertoMISO == 1 && m_pinMISO > 21)))
        return false;

    // Las tres senales deben usar pines distintos.
    if (m_puertoSCK == m_puertoMOSI && m_pinSCK == m_pinMOSI)
        return false;
    if (m_pinMISO != 0xFF &&
        ((m_puertoMISO == m_puertoSCK && m_pinMISO == m_pinSCK) ||
         (m_puertoMISO == m_puertoMOSI && m_pinMISO == m_pinMOSI)))
        return false;

    // Frecuencia SCK = relojHz / (DIV + 1).
    // Redondeamos el divisor hacia arriba para no superar velocidadHz.
    uint32_t divisor = relojHz / velocidadHz;
    if (relojHz % velocidadHz != 0)
        divisor++;
    if (divisor == 0 || divisor > 65536UL)
        return false;

    // 2. Elegimos el bloque de registros, como m_usart = USARTS[usart].
    m_spi = SPIS[m_numeroSPI];
    m_inicializado = false;

    // 3. Habilitamos el clock y reiniciamos SOLO el SPI elegido.
    // SPI0 utiliza el bit 11. SPI1 utiliza el bit 12.
    uint32_t mascaraSPI = 1UL << (11 + m_numeroSPI);
    SYSCON->SYSAHBCLKCTRL0 |= mascaraSPI;
    SYSCON->PRESETCTRL0 &= ~mascaraSPI;  // 0: mantener en reset.
    SYSCON->PRESETCTRL0 |= mascaraSPI;   // 1: liberar el reset.

    // FCLKSEL[9] corresponde a SPI0; FCLKSEL[10] a SPI1.
    // El valor 1 selecciona main_clk como fuente de reloj.
    SYSCON->FCLKSEL[9 + m_numeroSPI] = 1;

    // 4. Configuramos el periferico todavia deshabilitado.
    m_spi->INTENCLR = 0x3F; // Deshabilita las interrupciones del SPI.
    m_spi->DIV = divisor - 1;
    m_spi->DLY = 0;        // Sin retardos adicionales programados.
    m_spi->CFG = 1UL << 2; // MASTER=1, ENABLE=0, MSB primero.

    // CPHA (bit 4) y CPOL (bit 5) definen el modo de comunicacion.
    if (modo == 1 || modo == 3)
        m_spi->CFG |= 1UL << 4;
    if (modo == 2 || modo == 3)
        m_spi->CFG |= 1UL << 5;

    // 5. Encendemos la Switch Matrix para asignar las senales.
    // Guardamos su estado para devolver el clock como estaba al entrar.
    bool swmEstabaEncendida = (SYSCON->SYSAHBCLKCTRL0 & (1UL << 7)) != 0;
    SYSCON->SYSAHBCLKCTRL0 |= 1UL << 7;

    switch (m_numeroSPI)
    {
        case 0:
            PINASSIGN_Config(PA_SPI0_SCK, m_puertoSCK, m_pinSCK);
            PINASSIGN_Config(PA_SPI0_MOSI, m_puertoMOSI, m_pinMOSI);
            if (m_pinMISO != 0xFF)
                PINASSIGN_Config(PA_SPI0_MISO, m_puertoMISO, m_pinMISO);
            else
                PINASSIGN_Config(PA_SPI0_MISO, 0, 0xFF);

            // CS se manejara por separado, con una salida digital.
            PINASSIGN_Config(PA_SPI0_SSEL0, 0, 0xFF);
            PINASSIGN_Config(PA_SPI0_SSEL1, 0, 0xFF);
            PINASSIGN_Config(PA_SPI0_SSEL2, 0, 0xFF);
            PINASSIGN_Config(PA_SPI0_SSEL3, 0, 0xFF);
            break;

        case 1:
            PINASSIGN_Config(PA_SPI1_SCK, m_puertoSCK, m_pinSCK);
            PINASSIGN_Config(PA_SPI1_MOSI, m_puertoMOSI, m_pinMOSI);
            if (m_pinMISO != 0xFF)
                PINASSIGN_Config(PA_SPI1_MISO, m_puertoMISO, m_pinMISO);
            else
                PINASSIGN_Config(PA_SPI1_MISO, 0, 0xFF);

            PINASSIGN_Config(PA_SPI1_SSEL0, 0, 0xFF);
            PINASSIGN_Config(PA_SPI1_SSEL1, 0, 0xFF);
            break;
    }

    if (!swmEstabaEncendida)
        SYSCON->SYSAHBCLKCTRL0 &= ~(1UL << 7);

    // 6. Habilitamos el SPI. El clock del SPI queda encendido.
    m_spi->CFG |= 1UL << 0;
    m_inicializado = true;
    return true;
}

bool Spi::Esperar(uint32_t bandera)
{
    // Se consulta el hardware hasta que aparece la bandera solicitada.
    for (uint32_t i = 0; i < SPI_LIMITE_ESPERA; i++)
    {
        uint32_t estado = m_spi->STAT;

        // RXOV y TXUR son indicadores de error (bits 2 y 3).
        if (estado & ((1UL << 2) | (1UL << 3)))
            break;
        if (estado & bandera)
            return true;
    }

    // Hubo un error o se agoto la espera. El reset detiene el SPI.
    // Antes de volver a utilizarlo, hay que llamar otra vez a Inicializar().
    uint32_t mascaraSPI = 1UL << (11 + m_numeroSPI);
    SYSCON->PRESETCTRL0 &= ~mascaraSPI;
    SYSCON->SYSAHBCLKCTRL0 &= ~mascaraSPI;
    m_inicializado = false;
    return false;
}

bool Spi::Transferir(const uint8_t *tx, uint8_t *rx, uint32_t cantidad)
{
    if (!m_inicializado)
        return false;
    if (cantidad == 0)
        return true;
    if (tx == nullptr && rx == nullptr)
        return false;
    if (rx != nullptr && m_pinMISO == 0xFF)
        return false; // Para recibir necesitamos haber asignado MISO.

    // LEN (bits 27:24) = cantidad de bits - 1. Por eso 7 significa 8 bits.
    // TXSSEL (bits 19:16) = 1111 deja inactivos los selectores hardware.
    uint32_t control = (7UL << 24) | (0xFUL << 16);
    if (rx == nullptr)
        control |= 1UL << 22; // RXIGNORE: descartar la recepcion.

    for (uint32_t i = 0; i < cantidad; i++)
    {
        // TXRDY (bit 1): el registro de transmision puede aceptar otro dato.
        if (!Esperar(1UL << 1))
            return false;

        uint8_t dato = 0xFF;
        if (tx != nullptr)
            dato = tx[i];

        uint32_t controlDelByte = control;
        if (i == cantidad - 1)
            controlDelByte |= 1UL << 20; // EOT: este es el ultimo byte.

        // Escribir TXDATCTL carga el dato y su control, e inicia el envio.
        m_spi->TXDATCTL = controlDelByte | dato;

        if (rx != nullptr)
        {
            // RXRDY (bit 0): hay un dato recibido para leer.
            if (!Esperar(1UL << 0))
                return false;
            rx[i] = (uint8_t)m_spi->RXDAT;
        }
    }

    // MSTIDLE (bit 8): ya termino de salir el ultimo bit.
    // Esperar solamente TXRDY no alcanza para confirmar ese final.
    return Esperar(1UL << 8);
}
