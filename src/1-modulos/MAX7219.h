#ifndef MAX7219_H_
#define MAX7219_H_

#include "ComunicacionSincronica.h"
#include "Outputs.h"

// Tamano de la matriz completa: dos tiras de 8x32 una arriba de la otra.
#define MATRIZ_FILAS        16
#define MATRIZ_COLUMNAS     32
#define MATRIZ_MODULOS      8     // 8 MAX7219 encadenados (4 arriba + 4 abajo)

// ===== ORIENTACION =====
// Las tiras estan montadas GIRADAS 180 grados respecto de como vienen de fabrica
// (en los FC-16 el DIN entra por la derecha y la primera fila esta arriba).
// Una pieza fisica no se puede "espejar", solo girar: girada 180 grados quedan al
// reves las filas, las columnas y el orden de los modulos. Por eso los valores de abajo.

// Cual de las dos tiras de 8x32 es la primera de la cadena (la que recibe el DIN del micro):
//   1 = la de ABAJO.   0 = la de ARRIBA.
// Sintoma de que esta mal: el punto recorre bien cada tira, pero la mitad de arriba
// de la imagen aparece abajo y viceversa.
#define MATRIZ_TIRAS_AL_REVES 0

// Orden de las filas dentro de cada modulo de 8x8:
//   1 = la primera fila del MAX7219 (DIG0) es la de ABAJO del modulo (modulos puestos al reves).
//   0 = la primera fila del MAX7219 es la de arriba.
// Sintoma de que esta mal: SetPixel( 0 , ... ) prende la fila de abajo de un modulo.
#define MATRIZ_FILAS_AL_REVES 1

// Orden de los modulos dentro de cada tira:
//   1 = el primer modulo de la cadena (el que recibe el DIN) queda a la DERECHA.
//       Es lo normal en los modulos FC-16: los datos entran por la derecha.
//   0 = el primer modulo de la cadena queda a la IZQUIERDA.
// Sintoma de que esta mal: el punto anda bien dentro de un modulo pero al pasar al
// siguiente salta al otro extremo.
#define MATRIZ_MODULOS_AL_REVES 0

// Orden de las columnas dentro de cada modulo de 8x8:
//   0 = normal.  1 = espejado.
// Sintoma de que esta mal: dentro de cada modulo el punto se mueve al reves.
#define MATRIZ_COLUMNAS_AL_REVES 1

// Como esta puesta la tira de ABAJO respecto de la de arriba:
//   0 = igual que la de arriba (su DIN queda del mismo lado que el DIN de la de arriba).
//   1 = girada 180 grados (el cable sale de la punta de la tira de arriba y entra
//       por la punta de al lado de la tira de abajo).
// Si la mitad de abajo se ve espejada o cabeza abajo, cambiar este valor.
#define MATRIZ_ABAJO_GIRADA 0

// Una matriz de 16 filas por 32 columnas: ocho MAX7219 encadenados.
//
// Vista de frente, con los valores de arriba (modN = N-esimo chip de la cadena):
//
//                        columna 0 ............ columna 31
//                        +------+------+------+------+
//   DIN del micro -->    | mod0 | mod1 | mod2 | mod3 |    filas 0 a 7  (tira de ARRIBA)
//                        +------+------+------+------+
//   DOUT de arriba -->   | mod4 | mod5 | mod6 | mod7 |    filas 8 a 15 (tira de ABAJO)
//                        +------+------+------+------+
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
