# 01 – Firmware de los joysticks (21/10/2026)

> Notas personales. Esta carpeta NO se sube a GitHub (está en `.git/info/exclude`, que es un
> `.gitignore` que vale solo para esta PC).

---

## 0. Estado

- Archivos **modificados**: `src/1-modulos/10-adc/Adc.cpp`, `src/1-modulos/10-adc/Adc.h`, `src/2-aplicacion/main.cpp`
- Archivos **nuevos**: `src/1-modulos/15-joystick/` → `Joystick.h/.cpp`, `Pulsador.h/.cpp`
- **No está commiteado.** Para deshacer un archivo: GitHub Desktop → clic derecho → *Discard changes*.
- Compila sin errores ni warnings (probado con arm-none-eabi-g++). **Todavía no se probó en la placa.**

---

## 1. Objetivo

Leer 2 joysticks analógicos (X, Y) con el ADC y 2 pulsadores, y verlo funcionando en la matriz 8×32.

---

## 2. Hardware: por qué esos pines

Cada canal del ADC está atado a **un pin fijo** del LPC845, no se puede elegir cualquiera.
Revisando el esquemático del kit (`Kit/kit22025.pdf`):

| Canal | Pin   | En el kit hay…                        | ¿Sirve?                  |
|-------|-------|---------------------------------------|--------------------------|
| 1     | P0_6  | POT1 / NTC (DIP S1)                   | Reserva                  |
| 2, 10 | P0_14, P0_13 | LCD, con **pull-up 10k a 5V**  | ❌ arruina la lectura    |
| 7, 8  | P0_19, P0_18 | CK / RST del 4017              | ❌                       |
| 9     | P0_17 | RX1 y LOAD de la matriz               | ❌                       |
| **3–6** | **P0_23…P0_20** | Entradas BCD del CD4511 (display 7 seg) | ✅ (no usamos el display) |

Las entradas del 4511 son CMOS (no consumen corriente), por eso no alteran la lectura.
Consecuencia: **el display de 7 segmentos no se puede usar** (muestra basura).

### Conexiones

| Qué               | Pin LPC845 | Conector |
|-------------------|------------|----------|
| J1 VRx            | P0_23 (ADC3) | CN12-1 |
| J1 VRy            | P0_22 (ADC4) | CN12-2 |
| J2 VRx            | P0_21 (ADC5) | CN12-3 |
| J2 VRy            | P0_20 (ADC6) | CN12-4 |
| "+5V" del módulo  | **3,3 V** (CN8-2 o CN10-5) | |
| GND               | GND        | |
| Pulsador "modo"   | P0_26 ↔ GND | |
| Pulsador "calibrar" | P0_15 ↔ GND | |

⚠️ **El joystick va a 3,3 V, no a 5 V.** Cada eje es un potenciómetro: la salida llega hasta la
tensión de alimentación, y el ADC aguanta máximo 3,3 V.

Valores esperados: 0 V → 0 cuentas, 3,3 V → 4095, palanca suelta ≈ 1,65 V ≈ 2048.

---

## 3. Cambios en el driver del ADC (`Adc.cpp`)

### 3.1 Tabla de canales
`CanalesHabilitados[]` dice qué canales convierte el ADC. Se pusieron en `true` los 3, 4, 5 y 6
(antes estaba solo el 1, el NTC). Se agregó un campo `iocon` con la posición de cada pin en el
registro IOCON (sirve para el paso 3.2).

### 3.2 `inicializar()` paso a paso
Se ejecuta **una sola vez**, con el primer objeto `Adc` que se crea:

1. **Encender el ADC**: `PDRUNCFG` (activo bajo → poner 0) y darle clock en `SYSAHBCLKCTRL0`.
2. **Pines en modo analógico**:
   - `SWM0->PINENABLE0`: habilita la función fija `ADC_n` en su pin (activo bajo).
   - `IOCON->PIO[...]`: **se apaga el pull-up** (bits 4:3 = 00). *Nuevo.* Por defecto los pines
     arrancan con pull-up, que tira la tensión hacia 3,3 V y falsea la lectura.
3. **Calibración** (`calibrateADC()`).
4. **Clock del ADC**: modo sincrónico, clock del sistema dividido por 13.
5. **Secuencia A** (`SEQ_CTRL[0]`):
   - `CHANNELS` = bits de los canales 3 a 6.
   - `MODE = 1` → interrumpe **al terminar toda la secuencia** (antes interrumpía en cada conversión).
   - `SEQ_ENA = 1` → habilita la secuencia.
6. **Interrupción**: `INTEN` bit 0 y el NVIC (`ISER[0]` bit 16).

### 3.3 Quién dispara las conversiones (cambio importante)
- **Antes**: la interrupción volvía a arrancar el ADC → convertía sin parar, decenas de miles de
  interrupciones por segundo, comiéndole CPU al juego.
- **Ahora**: `Adc::Disparar()` pone `START = 1` y hace **una pasada** por los 4 canales.
  Se llama **cada 1 ms desde el SysTick** (ver `main.cpp`). → 1000 interrupciones/seg, nada más.

### 3.4 La interrupción (`ADC_seqA_IRQHandler`)
1. Para cada canal habilitado lee `ADC0->DAT[canal]` (cada canal tiene su registro de resultado).
2. El resultado de 12 bits está en los bits 15:4 → se enmascara y se corre 4 lugares.
3. Lo guarda en un **buffer circular de 16 muestras** por canal (`g_result`).
4. Borra el flag escribiendo 1 en `FLAGS` bit 28. Se usa `=` y no `|=`, porque en `FLAGS` escribir
   un 1 *borra*: con `|=` borraríamos también otros flags que estuvieran activos.

`g_result` se declaró `volatile` porque lo escribe la interrupción y lo lee el programa principal.

### 3.5 Lectura (`getResultado()`)
- `PROMEDIO`: promedio de las últimas 16 muestras → filtra el ruido del potenciómetro.
- `CRUDO`: la última muestra (antes leía siempre la posición 0, que podía estar vieja).

### 3.6 Bug corregido en la calibración
```cpp
while( ADC0->CTRL &= 0x40000000 )   // ANTES: "&=" ESCRIBE el registro en cada vuelta
while( ADC0->CTRL & ADC_CTRL_CALMODE_MASK )   // AHORA: solo lee y compara
```

### 3.7 `Adc.h`
Solo se agregó la declaración `static void Disparar(void);`. Es `static` porque el ADC es uno solo:
no hace falta un objeto para dispararlo.

---

## 4. Clase `Joystick` (nueva, no toca registros)

Usa **dos objetos `Adc`** (uno por eje) y traduce números a algo útil para el juego.

- `Calibrar()`: guarda dónde queda **de verdad** la palanca suelta (nunca es exactamente 2048).
- `GetX()` / `GetY()`: posición relativa al centro, aprox. −2048 … +2047.
- `GetDireccion()`:
  1. Si |X| y |Y| son menores que la **zona muerta** (600 cuentas ≈ 15%) → `CENTRO`.
  2. Si no, gana el eje que esté más lejos del centro → `DERECHA/IZQUIERDA` o `ARRIBA/ABAJO`.
- `invertirX` / `invertirY` (en el constructor): por si el joystick queda montado al revés.

---

## 5. Clase `Pulsador` (nueva)

- Usa `Gpio` con **pull-up interno** y actividad **LOW**: suelto = 3,3 V, apretado = 0 V → `GetPin()` da 1.
- **Antirrebote**: un cambio se acepta recién cuando la lectura se repite **3 veces seguidas**.
  Llamando `Actualizar()` cada 10 ms → 30 ms estable.
- `EstaPresionado()`: mientras está apretado.
- `FuePresionado()`: `true` **una sola vez** por cada apretada (detecta el flanco).

---

## 6. `main.cpp` (programa de prueba)

1. `Inicializar_PLL()` → clock.
2. **SysTick cada 1 ms** → `TickDeSistema()`: suma 1 a `g_ms` y llama a `Adc::Disparar()`.
   Es una interrupción: tiene que ser corta.
3. Crea la matriz (igual que el main de las caritas).
4. Crea los 4 `Adc` (canales 3–6, `PROMEDIO`) **dentro de `main`**, después del PLL.
5. Crea `joy1`, `joy2` y los 2 pulsadores.
6. Espera 100 ms (se llena el promedio) y **calibra el centro** → no tocar los joysticks al arrancar.
7. `while(1)` con **tareas por tiempo** (nada se queda bloqueado esperando):

| Cada   | Hace |
|--------|------|
| 10 ms  | Lee los pulsadores. P0_26 cambia de modo; P0_15 recalibra y resetea los puntos |
| 120 ms | Mueve cada punto según la dirección de su joystick (velocidad del punto) |
| 40 ms  | Redibuja la matriz (25 cuadros/seg) |

**Modo puntos**: un punto por jugador.
**Modo barras**: 4 barras (J1-X, J1-Y, J2-X, J2-Y). Palanca suelta = barra por la mitad.

El main anterior (caritas) está en el historial de Git.

---

## 7. Lo que falta hacer (yo)

1. **Excluir de la compilación** los ejemplos de la cátedra: `aplicacion.cpp`, `inicializarInfotronic.cpp`,
   `Puerta.cpp`, `PuertaCorrefiza.cpp` → clic derecho → *Resource Configurations → Exclude from Build*
   (Debug y Release).
   **Motivo**: `inicializarInfotronic.cpp` crea objetos globales que ponen P0_20–P0_23 como **salidas**
   (para el display) → pelean con los joysticks.
2. **F5** sobre el proyecto (para que aparezca `15-joystick`) → **Build**.
3. Conectar según la tabla del punto 2.
4. Probar: modo barras → mover cada palanca → cada barra sube y baja.
   - Barra fija arriba/abajo → cable mal o pin configurado como salida.
   - Eje al revés → `Joystick joy1(&j1x, &j1y, true, false)`.

---

## 8. Cosas a revisar más adelante (no urgentes)

- `FREQ_CLOCK` en `LPC845.h` dice 30 MHz, pero `Inicializar_PLL()` no deja el clock en 30 MHz, y la UART
  calcula los baudios con 48 MHz. Hay que averiguar el clock real: afecta al SysTick (1 ms puede no ser
  1 ms exacto) y a la UART.
- `Uart.cpp` tiene activado el bit `LOOP` (modo de prueba en lazo). Revisar antes de comunicar con Qt.
- `Gpio.cpp`, tabla `IOCON_INDEX_PIO0[8]` = 4, debería ser 14 (afecta a P0_8).

---

## 9. Glosario rápido

- **ADC**: convierte una tensión (0 a 3,3 V) en un número (0 a 4095, 12 bits).
- **Secuencia**: lista de canales que el ADC convierte uno atrás del otro con un solo disparo.
- **SWM (Switch Matrix)**: decide qué función tiene cada pin.
- **IOCON**: configura la parte eléctrica del pin (pull-up, pull-down, etc.).
- **NVIC**: controlador de interrupciones del Cortex-M0+.
- **SysTick**: timer del núcleo; acá interrumpe cada 1 ms.
- **Zona muerta**: margen alrededor del centro donde la palanca se considera suelta.
- **Rebote**: el contacto mecánico abre y cierra varias veces en pocos ms al apretar.
