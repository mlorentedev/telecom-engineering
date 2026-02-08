; * *************************************** *
; *       Función iniciaRegistros           *
; * Daniel Sojo España                      *
; * Alejandro Grande Gómez                  *
; * Manuel Lorente Almán                    *
; * *************************************** *
;----------------------------------------
; Rutina: iniciaRegistros
;----------------------------------------
iniciaRegistros                          ; void iniciaRegistros()
                                         ; {
   clrw                         ;
   movwf   PORTA                ;      PORTA= 0b00000000;      // Antes de habilitarlo.        
   movwf   PORTB                ;      PORTB= 0b00000000;      // Antes de habilitarlo.    
   ;------ --------------       ;      // ------------------ 
   bsf     STATUS,RP0           ;      // Acceso al Banco 1 -> Status[RP0]=1 
   ;------ --------------       ;      // ------------------ 
   movlw  b'00010000'           ;
   movwf  TRISA                 ;      TRISA= 0b00010000;      // ENTRADAS => RA4
   movlw  b'00110000'           ;
   movwf  TRISB                 ;      TRISB= 0b00110000;      // ENTRADAS => RB4 y RB5
   
   ; * ************************************************************************ *
   ; * Configuración Timer 0: temporización a 2 milisegundo (Fosc=500hz)
   ; *
   ; * Tiempo= 4.Tosc.(256-TMR0).Preescalador
   ; *      Si Tosc=0.25us (Fosc=4Mhz),  TMR0=6  y Preescalador= 1:8  ([PS2:PS0]=010)->
   ; *      Tiempo= 4.(2000us).(256-6).8= 2000useg. -> (500hz)
   ; * 
   ; * Cada 5 interrupciones se tiene 5x 2000useg.= 0.01 seg. -> Incrementa las centesimas cada 10.095ms probado en proteus y con punto de parada
   ; * ************************************************************************ *
   movlw b'10000010'                    ;
         ; 76543210                     ;
         ; ||||||||                     ;
         ; |||||||+- bit0 PS0   = '0'   ;       // [PS2:PS0]=010.  Preescalador  1:8
         ; ||||||+-- bit1 PS1   = '1'   ;  
         ; |||||+--- bit2 PS2   = '0'   ;  
         ; ||||+---- bit3 PSA   = '0'   ;       // El Divisor de frecuencia se asigna al TMR0
         ; |||+----- bit4 T0SE  = '0'   ; 
         ; ||+------ bit5 T0CS  = '0'   ;       // Utiliza Fosc/4 para temporizar
         ; |+------- bit6 INTEDG= '0'   ;
         ; +-------- bit7 RBPU  = '1'   ;       // No activa los pullups internos de PORTB
   movwf OPTION_REG                     ;       OPTION_REG= 0b10000010;
   ; * ************************************************************************ *

   ;------ --------------       ;      // ------------------ 
   bcf     STATUS,RP0           ;      // Acceso al Banco 0. -> Status[RP0]=0
   ;------ --------------       ;      // ------------------ 
   
   movlw   reposo / 0x100       ; 
   movwf   estadoSig+0          ;      // Iniciamos la variable estadoSig      
   movlw   reposo % 0x100       ;      // para que apunte a la rutina "reposo"
   movwf   estadoSig+1          ;      estadoSig= reposo;     // estadoSig=(estadoSig+0,estadoSig+1)
   
   ;--------------------------------------------------------------------
   ; LCD: debe iniciarse al final del bloque "inicio" usa la config. de TRISx
   ;--------------------------------------------------------------------
   #define  _XTAL_FREQ     4000000                ; Reloj a 4Mhz
   #define  LCD_BUS_PORT   PORTB
   #define  LCD_BUS_PINES  b'00001111'            ; Bits <0:3> de LCD_PORT_BUS=PORTB
   #define  LCD_RS_PORT    PORTA
   #define  LCD_RS_PIN     0                      ; RS= RA0
   #define  LCD_E_PORT     PORTA
   #define  LCD_E_PIN      1                      ; E = RA1
   ;--------------------------------------------------------------------
   
   ;--------------------------------------------------------------------
   call LCD_ini                  ; Inicia el display 
   ;--------------------------------------------------------------------
   
   return                               ; }