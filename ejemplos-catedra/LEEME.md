# Ejemplos y drivers originales de la cátedra

Esta carpeta **no se compila** (está fuera de `src/`). Queda solo para consultar.

- `modulos/`: drivers que el TPO no usa (display 7 segmentos, teclado matricial, DAC,
  entradas/salidas digitales con antirrebote).
- `aplicacion/`: ejemplos de clase (control de temperatura con NTC, máquinas de estado de la puerta)
  e `inicializarInfotronic`, que creaba todos los objetos del kit.
- `originales/`: versiones originales de los drivers que se simplificaron para el TPO
  (ADC, UART, PLL, interrupciones externas, GPIO), para comparar con las de `src/drivers/`.
