; * ********************************************************************************* *
; * ************                   Fichero: LCD_LM016.h                  ************ *
; * ********************************************************************************* *
IFNDEF     LCD_LM016_h          ;  ENDIF al final del fichero
#define    LCD_LM016_h          ;  Si se llama dos veces al fichero, la 2ª no se incluye
;
;---------------------
; Listado de rutinas:
;        - LCD_ini
;        - LCD_enviaCaracter
;        - LCD_irLineaInf     ; El cursor lo pone al principio
;        - LCD_irLineaSup     ; El cursor lo pone al principio
;        - LCD_lineaEnBlanco  ; Borra la línea y sitúa el curso al principio de ella
;        - LCD_1_blanco
;        - LCD_2_blancos
;        - LCD_3_blancos
;        - LCD_4_blancos
;          ----------------
;        - LCD_irFilCol
;        - LCD_enciende
;        - LCD_apaga
;        - LCD_enviaComando
     
;
; La librería está preparada para que los 4 pines de datos (salidas para el PIC) puedan
; ser compartidos por otro periférico pero como entradas (ej.: filas de un teclado).
;
; Librería de rutinas para la gestión de un LCD de 2 líneas x 16 caracteres gestionado 
; con un bus de datos 4 líneas para optimizar el número de pines empleados.
;
; El bus de datos del LCD (configurado para 4 bits) se conectará bien a las 4 líneas 
; inferiores de un puerto del PIC (en este caso la constante LCD_BUS_PINES=b'00001111')
; o bien a los 4 bits superiores del puerto (en este caso la constante 
; LCD_BUS_PINES=b'11110000'). Estos pines se ponen en alta impedancia cuando el pin E del
; LCD se pone a '0'. Por tanto, pueden conectarse a otros dispositivos y tener más
; de una función ya que el pin E solo se pone a '1' cuando el pic se dirige al LCD.
;     Los pines E y RS pueden asignarse a cualquier otro pin libre 
; del mismo puerto o de cualquier otro puerto del PIC. Las posibilidades de conexión del
; 16F84 son múltiples. En el caso de un pic con más de 2 puertos aumentan aun más. 
; En esta librería se pueden elegir entre 5 opciones cambiando el valor de la constante 
; LCD_OPCION para un pic de dos puertos (PORTA y PORTB).
;     
; Todos los pines del PIC que van a l LCD se configurarán de salida. 
;  - El único pin dedicado es el pin E (su dirección será siempre de salida).
;  - El resto de pines (RS y <DB7:DB4>) pueden tener otra función alternativa bien
;      como entradas bien como salidas.        
;



ENDIF    ; LCD_LM016_h
