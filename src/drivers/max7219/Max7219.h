#ifndef MAX7219_H_
#define MAX7219_H_

#include "ComunicacionSincronica.h"
#include "Outputs.h"

// Tamano de la matriz completa: dos tiras de 8x32 una arriba de la otra.
#define MATRIZ_FILAS        16
#define MATRIZ_COLUMNAS     32
#define MATRIZ_MODULOS      8     // 8 MAX7219 encadenados (4 arriba + 4 abajo)

// Como esta puesta la tira de ABAJO respecto de la de arriba:
//   0 = igual que la de arriba (su DIN queda del mismo lado que el DIN de la de arriba).
//   1 = girada 180 grados (el cable sale de la punta de la tira de arriba y entra
//       por la punta de al lado de la tira de abajo).
// Si la mitad de abajo se ve espejada o cabeza abajo, cambiar este valor.
#define MATRIZ_ABAJO_GIRADA 0

// Una matriz de 16 filas por 32 columnas: ocho MAX7219 encadenados.
//
//      columna 0 ............ columna 31
//      +------+------+------+------+
//      | mod0 | mod1 | mod2 | mod3 |    filas 0 a 7    (tira de ARRIBA: la primera,
//      +------+------+------+------+                    su DIN va al micro)
//      | mod4 | mod5 | mod6 | mod7 |    filas 8 a 15   (tira de ABAJO: su DIN va al
//      +------+------+------+------+                    DOUT de la tira de arriba)
//
// Esta clase conoce los comandos del MAX7219, pero no los registros del LPC845.
class Max7219
{
private:
    // Son punteros a OBJETOS de nuestras clases, no a registros de hardware.
    ComunicacionSincronica *m_comunicacion;
    Outputs *m_load;

    // Imagen: ocho modulos, con ocho bytes de filas en cada uno.
    // Cada bit del byte representa un LED. En total ocupa 64 bytes.
    uint8_t m_imagen[MATRIZ_MODULOS][8];
    bool m_inicializado;

public:
    // Recibe el bus ya creado y la salida LOAD configurada como activa en LOW.
    // Ambos objetos deben seguir existiendo mientras se utilice la matriz.
    Max7219(ComunicacionSincronica *comunicacion, Outputs *load);

    // Configura los ocho chips, borra la imagen y habilita la visualizacion.
    // Antes hay que inicializar el SPI en modo 0, MSB primero, 8 bits por dato.
    // intensidad: 0 a 15. El valor 0 es el brillo minimo, no significa apagado.
    bool Inicializar(uint8_t intensidad = 1);
    bool SetIntensidad(uint8_t intensidad);

    // Estas dos operaciones modifican la imagen guardada en RAM.
    // Para ver el cambio en la matriz hay que llamar a Actualizar().
    // fila: 0 (arriba) a 15 (abajo). columna: 0 (izquierda) a 31 (derecha).
    void Limpiar();
    bool SetPixel(uint8_t fila, uint8_t columna, bool encendido = true);

    // Envia la imagen completa y espera hasta terminar.
    // Devuelve false si el bus falla o la matriz no fue inicializada.
    bool Actualizar();

private:
    // Envia un par registro/dato a cada uno de los ocho chips (16 bytes).
    bool Enviar(const uint8_t *datos);

    // Usado para dar el mismo comando de configuracion a los ocho chips.
    bool EnviarATodos(uint8_t registro, uint8_t dato);
};

#endif
