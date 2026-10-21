# 02 – Simplificación del proyecto (21/10/2026)

> Notas personales. No se suben a GitHub.
> Esta nota **reemplaza** la parte de "archivos" de la nota 01: los joysticks siguen igual en
> concepto, pero ahora están dentro de la estructura nueva.

---

## 0. Estado

- **No está commiteado.** Los archivos nuevos ya están en la carpeta, pero las carpetas viejas
  las tenés que borrar vos (ver punto 6).
- Compila **y linkea** completo con el compilador ARM: 11 KB de flash (de 64) y 1,4 KB de RAM (de 16).
- **No se probó en la placa.** Como cambió el clock, hay que volver a probar también la matriz.

---

## 1. Decisiones

| Tema | Decisión |
|---|---|
| Lo que no se usa | Se mueve a `ejemplos-catedra/` (fuera de `src`, no se compila) |
| Clock | FRO interno a **30 MHz** para todo (antes había 3 valores distintos) |
| Estilo | Se mantienen las **clases** de la cátedra, simplificadas |
| Pines | Todo en **`pines.h`**; los objetos en **`hardware.cpp`** y se usan con **`extern`** |

---

## 2. Estructura nueva

```
src/
├── micro/          Lo del LPC845: registros, arranque, reloj, SysTick, switch matrix
│   ├── LPC845.h, tipos.h
│   ├── clock.h/.cpp            NUEVO: FRO a 30 MHz (reemplaza dr_pll)
│   ├── systick.h/.cpp
│   ├── swm.h/.cpp
│   └── cr_startup_lpc84x.cpp, crp.c, mtb.c, cmsis_*.h, ...   (arranque, sin cambios)
├── drivers/        Una carpeta por periférico, cada uno es una clase
│   ├── gpio/       Gpio + interfaces Inputs / Outputs
│   ├── timer/      PerifericoTemporizado + Timer (timers por software)
│   ├── intext/     Interrupciones por pin (lista para usar con los botones)
│   ├── adc/        Adc
│   ├── uart/       Uart + interfaz ComunicacionAsincronica
│   ├── spi/        Spi + interfaz ComunicacionSincronica
│   ├── max7219/    Matriz LED
│   └── joystick/   Joystick + Pulsador
└── aplicacion/     Lo nuestro
    ├── pines.h     QUÉ HAY EN CADA PIN (usados, libres, no tocar)
    ├── hardware.h  Declaraciones "extern" de todos los objetos
    ├── hardware.cpp  Creación de los objetos + InicializarHardware() + SysTick
    └── main.cpp    Programa
ejemplos-catedra/   Lo que no se usa (no se compila)
```

**Regla para saber dónde va algo:**
- ¿Toca registros de un periférico? → `drivers/`
- ¿Es del micro en sí (arranque, reloj)? → `micro/`
- ¿Es del juego / del TPO? → `aplicacion/`

---

## 3. `pines.h` + `hardware.h` + `hardware.cpp` (el cambio principal)

### 3.1 `pines.h`
Una tabla (en comentario) con **todos los pines del kit**: cuáles usamos, cuáles quedan libres
y cuáles no tocar (SWD del depurador y RESET). Abajo, los `#define` de cada conexión:

```cpp
#define JOY1_X_CANAL     3      // P0_23
#define BOTON1_PIN       26
```

Si cambia una conexión, se cambia **solo ahí**.

### 3.2 Qué es `extern`
- En **`hardware.cpp`** se **crea** el objeto (una sola vez):
  `Joystick g_joy1( &g_adcJoy1X , &g_adcJoy1Y );`
- En **`hardware.h`** se **declara**: `extern Joystick g_joy1;`
  Significa: "existe un objeto `g_joy1` de tipo `Joystick`, está creado en otro archivo".
- Cualquier `.cpp` que haga `#include "hardware.h"` puede usar `g_joy1`.
- Sin `extern`, cada archivo que incluyera el `.h` crearía **su propia copia** → error de linker
  "multiple definition".

El prefijo `g_` indica que es **global**.

### 3.3 Objetos globales y el clock
Los objetos globales se construyen **antes** de `main()`, cuando el clock todavía no está
configurado. Por eso:
- Los constructores de `Adc`, `Uart`, `Spi` y `Max7219` **solo guardan datos**.
- El hardware se configura en `InicializarHardware()`, **después** de `InicializarClock()`.
- `Gpio` sí configura el pin en el constructor (no depende del clock).

### 3.4 `InicializarHardware()` paso a paso
1. `InicializarClock()` → 30 MHz.
2. `Adc::Inicializar()` → configura el ADC con los canales que se crearon.
3. SysTick cada 1 ms → `TickDeSistema()`.
4. SPI + matriz.
5. UART a la PC.
6. Espera 100 ms y calibra el centro de los joysticks.

### 3.5 `TickDeSistema()` (cada 1 ms, dentro de la interrupción)
- Suma 1 a los milisegundos.
- `Adc::Disparar()` → convierte los joysticks.
- `PerifericoTemporizado::AtenderTodos()` → descuenta los `Timer` (antes era `Sheduller()`).
- Cada 10 ms: antirrebote de los pulsadores.

Así `main` no tiene que acordarse de nada de esto.

---

## 4. Cambios en cada driver

| Driver | Qué cambió |
|---|---|
| **tipos.h** | Usa `<stdint.h>`. Antes `int8_t` era `char`, que en ARM es **sin signo**. |
| **clock** | Nuevo. FRO a 30 MHz. Reemplaza `Inicializar_PLL()`, que dejaba otro valor. |
| **systick** | Calcula con `FREQ_CLOCK`; devuelve `bool`. |
| **Gpio** | Tabla de IOCON corregida (P0_8, P0_29, P0_30, P1_2 estaban mal). `IndiceIOCON()` pública (la usa el ADC). Ya no pisa todo el registro IOCON, solo los bits de pull-up. Open-drain implementado. |
| **Timer** | Sin cambios de lógica. La lista de periféricos ahora tiene límite y `AtenderTodos()`. |
| **Intext** | Borra el flag con `=` (con `\|=` borraba todas las pendientes). Chequea que el objeto exista. PININT6/7 libres. |
| **Adc** | Reescrito. Los canales se habilitan solos según los objetos creados. Constructor sin hardware + `Adc::Inicializar()`. Una interrupción por secuencia, disparada cada 1 ms. Sin pull-up. Calibración corregida. Sin NTC/callback. |
| **Uart** | Baudios con `FREQ_CLOCK` (antes 48 MHz fijo). Sin modo LOOP. No manda `'\0'`. Buffers fijos (sin `new`). Solo UART 0–2. `Recibir(dato)` en vez de `Message()`. |
| **Spi** | Sin cambios de lógica (ya estaba bien hecho). Archivo renombrado a `Spi.h` (el .cpp ya lo incluía así). |
| **Max7219** | Incluye la interfaz `ComunicacionSincronica.h` en vez de una ruta absoluta. |
| **Joystick** | Usa `GetResultado()`. |
| **Pulsador** | Variables `volatile` porque ahora lo actualiza el SysTick. |

---

## 5. `main.cpp`

El mismo programa de prueba de la nota 01 (modo puntos / modo barras), pero:
- Solo llama a `InicializarHardware()` y usa los objetos `g_...`.
- **Nuevo:** cada 250 ms manda a la PC `#JOY,x1,y1,x2,y2$` por la UART (9600 baudios).
  Se ve con la **terminal de MCUXpresso**. Sirve para probar la UART y el clock nuevo, y es
  el primer mensaje del protocolo con Qt.
- Para armar el texto no se usa `sprintf` (ocupa mucha memoria); hay una función chica `AgregarNumero()`.

---

## 6. Lo que tengo que hacer

1. **Cerrar MCUXpresso.**
2. Borrar de `src/`: **`1-modulos`**, **`2-aplicacion`**, **`3-firmware`**, **`LPC845.h`** y **`tipos.h`**.
   (Lo nuevo está en `src/micro`, `src/drivers` y `src/aplicacion`. Lo viejo está copiado en
   `ejemplos-catedra/` y en el historial de Git.)
   ⚠️ Si no se borran, no compila: habría dos `main`, dos `Gpio`, etc.
3. Abrir MCUXpresso → clic derecho en el proyecto → **Refresh (F5)** → **Clean** → **Build**.
4. Verificar en *Properties → C/C++ Build → Settings → MCU C++ Compiler → Includes* que estén
   las carpetas nuevas (las cambié en el `.cproject`).
5. Probar en la placa:
   - ¿La matriz prende? (cambió el clock de la SPI)
   - Terminal serie a 9600: ¿llegan las líneas `#JOY,...$`? Si llega basura, el problema es el clock o los baudios.
   - Joysticks en modo barras.
6. Si todo anda: commit en GitHub Desktop (sin tildar `NOTAS-LOCALES`).

---

## 7. Pendientes

- Release no tiene rutas de include configuradas (nunca las tuvo). Solo usamos Debug.
- Si no se ven las líneas en la terminal: confirmar que P0_24/P0_25 son el puerto serie USB de la LPC845-BRK.
- Botones SW de los joysticks: propuestos P0_28 y P0_8 (ver `pines.h`).
- Servo / solenoide del premio: falta elegir pin y hacer el driver de PWM (SCT).
