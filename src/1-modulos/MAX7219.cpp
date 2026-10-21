#include "Max7219.h"

// Direcciones de los registros INTERNOS DEL MAX7219, segun su datasheet.
// No son direcciones de memoria del LPC845.
#define MAX_REG_DECODE       0x09
#define MAX_REG_INTENSIDAD   0x0A
#define MAX_REG_SCAN_LIMIT   0x0B
#define MAX_REG_SHUTDOWN     0x0C
#define MAX_REG_DISPLAY_TEST 0x0F

Max7219::Max7219(ComunicacionSincronica *comunicacion, Outputs *load)
{
    m_comunicacion = comunicacion;
    m_load = load;
    m_inicializado = false;
    Limpiar(); // Solo borra nuestra RAM. El constructor no transmite nada.
}

bool Max7219::Inicializar(uint8_t intensidad)
{
    m_inicializado = false;
    if (m_comunicacion == nullptr || m_load == nullptr || intensidad > 15)
        return false;

    // Con actividad LOW: ClrPin deja LOAD fisicamente en ALTO (reposo).
    m_load->ClrPin();

    // Desactivamos el modo de prueba, que podria forzar todos los LED encendidos.
    if (!EnviarATodos(MAX_REG_DISPLAY_TEST, 0))
        return false;

    // Mantenemos la pantalla apagada mientras la configuramos.
    if (!EnviarATodos(MAX_REG_SHUTDOWN, 0))
        return false;

    // Sin decodificacion BCD: cada bit controla un LED de la matriz.
    if (!EnviarATodos(MAX_REG_DECODE, 0))
        return false;

    // Se recorren los ocho digitos/filas: el registro lleva cantidad - 1.
    if (!EnviarATodos(MAX_REG_SCAN_LIMIT, 7))
        return false;

    if (!EnviarATodos(MAX_REG_INTENSIDAD, intensidad))
        return false;

    // Borramos tanto nuestra imagen como la memoria de los cuatro MAX7219.
    Limpiar();
    for (uint8_t fila = 0; fila < 8; fila++)
    {
        // Las filas 0 a 7 se escriben en los registros 1 a 8.
        if (!EnviarATodos(fila + 1, 0))
            return false;
    }

    // Funcionamiento normal: el MAX7219 se ocupa del barrido de los LED.
    if (!EnviarATodos(MAX_REG_SHUTDOWN, 1))
        return false;

    m_inicializado = true;
    return true;
}

bool Max7219::SetIntensidad(uint8_t intensidad)
{
    if (!m_inicializado || intensidad > 15)
        return false;
    return EnviarATodos(MAX_REG_INTENSIDAD, intensidad);
}

void Max7219::Limpiar()
{
    for (uint8_t modulo = 0; modulo < 4; modulo++)
    {
        for (uint8_t fila = 0; fila < 8; fila++)
            m_imagen[modulo][fila] = 0;
    }
}

bool Max7219::SetPixel(uint8_t fila, uint8_t columna, bool encendido)
{
    if (fila >= 8 || columna >= 32)
        return false;

    // Columnas 0..7: modulo 0. Columnas 8..15: modulo 1, etc.
    // Modulo 0 es el conectado directamente al DIN de entrada de la placa.
    uint8_t modulo = columna / 8;
    uint8_t columnaLocal = columna % 8;

    // Convencion logica: columna local 0 corresponde al bit 7.
    // La orientacion fisica depende de como esten cableados los LED del modulo.
    uint8_t mascara = 1U << (7 - columnaLocal);

    if (encendido)
        m_imagen[modulo][fila] |= mascara;
    else
        m_imagen[modulo][fila] &= ~mascara;

    return true;
}

bool Max7219::Actualizar()
{
    if (!m_inicializado)
        return false;

    for (uint8_t fila = 0; fila < 8; fila++)
    {
        uint8_t datos[8]; // Cuatro chips por dos bytes (registro + dato).

        for (uint8_t posicion = 0; posicion < 4; posicion++)
        {
            // Los primeros bytes avanzan hasta el chip mas lejano.
            // Por eso transmitimos en el orden: modulo 3, 2, 1, 0.
            uint8_t modulo = 3 - posicion;
            datos[2 * posicion] = fila + 1;
            datos[2 * posicion + 1] = m_imagen[modulo][fila];
        }

        // Los cuatro chips reciben su fila con un unico pulso de LOAD.
        if (!Enviar(datos))
            return false;
    }

    return true;
}

bool Max7219::EnviarATodos(uint8_t registro, uint8_t dato)
{
    uint8_t datos[8];
    for (uint8_t modulo = 0; modulo < 4; modulo++)
    {
        datos[2 * modulo] = registro;
        datos[2 * modulo + 1] = dato;
    }
    return Enviar(datos);
}

bool Max7219::Enviar(const uint8_t *datos)
{
    // Esta salida debe estar configurada con actividad LOW.
    m_load->SetPin(); // Activa LOAD: nivel fisico BAJO.

    // Ocho bytes = cuatro palabras de 16 bits = 64 pulsos de reloj.
    // Transmitir espera hasta que haya terminado de salir el ultimo bit.
    bool ok = m_comunicacion->Transmitir(datos, 8);

    // El flanco ascendente carga los registros de los cuatro MAX7219.
    // Liberamos LOAD tambien cuando falla el bus; puede quedar una imagen parcial.
    m_load->ClrPin(); // Desactiva LOAD: nivel fisico ALTO.

    if (!ok)
        m_inicializado = false; // Habra que reinicializar el bus y la matriz.

    return ok;
}
