# TPO Informática II

Proyecto de MCUXpresso para la placa **LPC845-BRK**: matriz LED de 16x32 (8 x MAX7219), dos joysticks analógicos y comunicación con la PC (Qt) por UART.

Todas las conexiones (pines y colores de cables) y el mensaje que se manda a la PC están en **[CONEXIONES.txt](CONEXIONES.txt)**.

## Cómo bajarlo y compilarlo

1. Clonar el repositorio (GitHub Desktop: *File → Clone repository*).
2. En MCUXpresso: *File → Import → General → Existing Projects into Workspace*.
3. En *Select root directory* elegir la carpeta del repositorio. Tiene que aparecer el proyecto **R2002**.
4. **Destildar "Copy projects into workspace"**. Si queda tildado, MCUXpresso trabaja sobre una copia y los cambios no llegan al repositorio.
5. *Finish*. Compilar con el martillo (configuración **Debug**).
6. Para grabar la placa: conectarla por el micro USB y *Debug* (el bicho verde). La primera vez MCUXpresso pregunta qué placa usar: elegir la que aparece.

## Estructura (la de la cátedra)

| Carpeta | Qué hay |
|---|---|
| `src/1-modulos` | Clases de los periféricos (Gpio, Adc, SPI, Uart, Timer, MAX7219, Joystick...) |
| `src/2-aplicacion` | `inicializarInfotronic.cpp`: se crean los objetos con sus pines. `aplicacion.cpp`: lo que hace el programa. `main.cpp` |
| `src/3-firmware` | Arranque del micro, PLL, SysTick, switch matrix |

No se suben `Debug/` ni los archivos `.launch`: MCUXpresso los genera solo en cada compu.
