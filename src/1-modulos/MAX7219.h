#ifndef MAX7219_H_
#define MAX7219_H_

#include "1-modulos\14-SPI\SPI.h"
#include "Outputs.h"

// Una matriz de 8 filas por 32 columnas: cuatro MAX7219 encadenados.
// Esta clase conoce los comandos del MAX7219, pero no los registros del LPC845.
class Max7219
{
private:
    // Son punteros a OBJETOS de nuestras clases, no a registros de hardware.
    ComunicacionSincronica *m_comunicacion;
    Outputs *m_load;

    // Imagen: cuatro modulos, con ocho bytes de filas en cada uno.
    // Cada bit del byte representa un LED. En total ocupa 32 bytes.
    uint8_t m_imagen[4][8];
    bool m_inicializado;

public:
    // Recibe el bus ya creado y la salida LOAD configurada como activa en LOW.
    // Ambos objetos deben seguir existiendo mientras se utilice la matriz.
    Max7219(ComunicacionSincronica *comunicacion, Outputs *load);

    // Configura los cuatro chips, borra la imagen y habilita la visualizacion.
    // Antes hay que inicializar el SPI en modo 0, MSB primero, 8 bits por dato.
    // intensidad: 0 a 15. El valor 0 es el brillo minimo, no significa apagado.
    bool Inicializar(uint8_t intensidad = 1);
    bool SetIntensidad(uint8_t intensidad);

    // Estas dos operaciones modifican la imagen guardada en RAM.
    // Para ver el cambio en la matriz hay que llamar a Actualizar().
    void Limpiar();
    bool SetPixel(uint8_t fila, uint8_t columna, bool encendido = true);

    // Envia la imagen completa y espera hasta terminar.
    // Devuelve false si el bus falla o la matriz no fue inicializada.
    bool Actualizar();

private:
    // Envia un par registro/dato a cada uno de los cuatro chips (8 bytes).
    bool Enviar(const uint8_t *datos);

    // Usado para dar el mismo comando de configuracion a los cuatro chips.
    bool EnviarATodos(uint8_t registro, uint8_t dato);
};

#endif
