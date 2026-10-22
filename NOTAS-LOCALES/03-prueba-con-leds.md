# 03 – Prueba de joysticks con los LEDs del kit (21/10/2026)

> No tengo la matriz, así que la prueba usa los 3 LEDs rojos del kit.

## Qué cambió

- **pines.h**: P1_0, P1_1 y P1_2 pasan de "libres" a **LED0, LED1, LED2**.
  En el kit van a un ULN2003 que prende los LEDs: pin en 3,3 V → LED prendido (activos en ALTO).
- **hardware.h / .cpp**: tres objetos nuevos `g_led0`, `g_led1`, `g_led2` (Gpio de salida).
  Se apagan en `InicializarHardware()`.
- **main.cpp**: nueva prueba. La matriz sigue inicializándose (si no está conectada no pasa nada).

## Cómo leer los LEDs

Se muestra **un joystick a la vez**:

| Palanca    | LED0 | LED1 | LED2 |
|------------|:----:|:----:|:----:|
| suelta     |      |      |      |
| IZQUIERDA  |  ●   |      |      |
| ARRIBA     |      |  ●   |      |
| DERECHA    |      |      |  ●   |
| ABAJO      |  ●   |  ●   |  ●   |

- **Pulsador 1 (P0_26)**: cambia de joystick. Parpadea 1 vez = J1, 2 veces = J2.
  Al arrancar parpadea 1 vez: significa que inició bien y muestra J1.
- **Pulsador 2 (P0_15)**: recalibra el centro (palancas sueltas).
- La terminal (9600) sigue mostrando `#JOY,x1,y1,x2,y2$` con los números.

## Qué revisar si no anda

- No parpadea al arrancar → falló `InicializarHardware()` o el programa no arrancó (ver con el depurador).
- LEDs prendidos con la palanca suelta → recalibrar con el pulsador 2, o el eje está mal conectado.
- Izquierda/derecha o arriba/abajo al revés → en `hardware.cpp`:
  `Joystick g_joy1( &g_adcJoy1X , &g_adcJoy1Y , true , false );` (invierte X).
- X e Y cambiados → intercambiar los cables VRx y VRy (o los canales en `pines.h`).
- Con los números de la terminal se ve todo: palanca suelta ≈ 2048, extremos ≈ 0 y ≈ 4095.

## Cómo funciona `MostrarDireccion()`

`g_joy1.GetDireccion()` devuelve un `enum` (CENTRO, ARRIBA, …) y un `switch` decide qué
LEDs prender. `g_led0 = true;` funciona porque `Gpio` tiene sobrecargado el `operator=`:
llama a `SetPin()` o `ClrPin()`.

---

## Actualización: botones de los joysticks

Los pulsadores conectados son los **SW de cada joystick** (apretar la palanca):
HW-504 en **C0 (P0_26)** y MH en **C1 (P0_15)**.

- `pines.h`: `BOTON1/2` pasan a llamarse `JOY1_SW` y `JOY2_SW`. P0_28 y P0_8 quedan propuestos para los 2 pulsadores sueltos.
- `hardware`: `g_boton1/2` → `g_botonJoy1` y `g_botonJoy2`.
- `main.cpp`:
  - SW del J1 → muestra J1 (1 parpadeo).
  - SW del J2 → muestra J2 (2 parpadeos).
  - **Los dos juntos** → al **soltar** recalibra (3 parpadeos). Se espera a soltar porque
    apretar la palanca la puede correr del centro y se calibraría mal.

Antes, el botón de C1 "no hacía nada" porque recalibraba, y eso no se ve.
Si ahora el de C1 no parpadea 2 veces → revisar el cable SW del módulo MH y que el GND del módulo esté conectado.

---

## Actualización: ejes de los joysticks (descubierto con el display de 7 segmentos)

Las 4 líneas de los joysticks (P0_23…P0_20) son también las entradas BCD del CD4511, así que
el display de 7 segmentos muestra un número según qué líneas están en "1" (tensión alta):

| Línea | Pin | Bit | Se prende con |
|---|---|---|---|
| BCDA | P0_23 | 1 | HW-504 arriba |
| BCDB | P0_22 | 2 | HW-504 derecha |
| BCDC | P0_21 | 4 | MH arriba |
| BCDD | P0_20 | 8 | MH derecha |

Suelto, abajo o izquierda → tensión baja o media → el 4511 lo lee como 0.

**Conclusiones:**
1. Los 4 cables están bien conectados.
2. En la posición de uso, **VRx del módulo es el eje vertical y VRy el horizontal**.
3. Arriba y derecha dan tensión alta → no hay que invertir nada.

**Cambio:** en `pines.h` se intercambiaron los canales (sin tocar cables):
`JOY1_X = canal 4`, `JOY1_Y = canal 3`, `JOY2_X = canal 6`, `JOY2_Y = canal 5`.

Truco útil: el display sirve como tester rápido de esas 4 líneas mientras no se use para otra cosa.
