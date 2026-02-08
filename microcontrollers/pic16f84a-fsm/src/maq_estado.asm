; * *************************************** *
; * Descripcion de los estados del automata *
; * Daniel Sojo España                      *
; * Alejandro Grande Gómez                  *
; * Manuel Lorente Almán                    *
; * *************************************** *
;==================================
; Estado reposo
;==================================
reposo                                ; void resposo()

   btfsc  PORTA,4                     ; ¿RA4 == 0? SI <> Saltamos al codigo y cambiamos estado
   goto   reposo_0                    ;            NO <> Seguimos en reposo
   
   clrf centesimas                    ; Pongo a cero la variable centesimas
   clrf segundos                      ; Pongo a cero la variable segundos
   clrf fin                           ; Pongo a cero la variable fin
   movlw d'5'                         ;
   movwf cuenta5                      ; cuenta5=5;
   movlw d'100'                       ;
   movwf cuenta100                    ; cuenta100=100;
   movlw d'6'                         ;
   movwf TMR0                         ; TMR0=6;  // Valor ajustado para que el TIMER0 avise cada 500Hz según la 
									  ; 		 // configuracion que le hemos asignado en iniciaRegistros
   
   bcf INTCON,T0IF                    ; Pongo a cero el bit T0IF
   bcf INTCON,RBIF                    ; Pongo a cero el bit RBIF
   bsf INTCON,T0IE                    ; Pongo a uno el bit T0IE
   bsf INTCON,RBIE                    ; Pongo a uno el bit T0IE
   bsf INTCON,GIE                     ; Pongo a uno el bit GIE
   
   
; ---------- Borrar el LCD ---------- ;
   call LCD_irLineaSup
   movlw d'32'                        ; 32 es del codigo ASCII del espacio en blanco

   call LCD_enviaCaracter             ; Enviamos 16 veces el caracter en blanco pues no podemos usar la funcion LCD_lineaEnBlanco
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter

   call LCD_irLineaInf                ; Vamos a la linea superior y hacemos lo mismo 
   
   call LCD_enviaCaracter             ;  Enviamos 16 veces el caracter en blanco pues no podemos usar la funcion LCD_lineaEnBlanco
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
   call LCD_enviaCaracter
;-------------------------------------;


; --- Cambio de estado a Contando --- ;
   movlw   contando / 0x100           ; 
   movwf   estadoSig+0                ; // Iniciamos la variable estadoSig      
   movlw   contando % 0x100           ; // para que apunte a la rutina "contando"
   movwf   estadoSig+1                ; estadoSig= contando;     // estadoSig=(estadoSig+0,estadoSig+1)
;-------------------------------------;

reposo_0
   return                             ; } 

;==================================
; Estado contando
;==================================
contando                              ; void  contando()
                                      ; {
   movlw d'1'                         ;	
   subwf fin,0                        ;
   btfss STATUS,Z                     ; ¿fin == 1? SI <> Saltamos al codigo. Ha terminado la carrera (detenemos interrupciones)
   goto contando_0                    ;            NO <> Seguimos contando
   
   bcf INTCON,GIE                     ; GIE  = 0 --> Mascara global interrupciones (deshabilitamos todas las interrupciones)
   bcf INTCON,T0IE                    ; T0IE = 0 --> Mascara de interrupcion del TIMER0
   bcf INTCON,RBIE                    ; RBIE = 0 --> Mascara interrupcion cambio en RB<4:7>
   
; --- Cambio de estado a Resultado -- ;
   movlw   resultado / 0x100          ; 
   movwf   estadoSig+0                ; // Iniciamos la variable estadoSig      
   movlw   resultado % 0x100          ; // para que apunte a la rutina "resultado"
   movwf   estadoSig+1                ; estadoSig= resultado;     // estadoSig=(estadoSig+0,estadoSig+1)
;-------------------------------------;

contando_0
   return                             ; } 

    
;==================================
; Estado resultado
;==================================
resultado                              ; void  resultado()
      
   btfss  PORTA,4
   goto resultado_0	  ; {              ; Comprobamos si RA4=1 (carrera ha acabado o no)
                                       ; Ahora escribimos el resultado de la carrera
   ; ESCRIBIMOS LA LINEA SUPERIOR
   call LCD_irLineaSup

   movlw 'T'				; Escribimos en el LCD "T.R.: "
   call LCD_enviaCaracter
   movlw '.'
   call LCD_enviaCaracter
   movlw 'R'
   call LCD_enviaCaracter
   movlw '.'
   call LCD_enviaCaracter
   movlw ':'
   call LCD_enviaCaracter
   movlw ' '
   call LCD_enviaCaracter

   movf tr_segundos,0       ; Escribimos en el LCD el tiempo de reacción seguido
   call decena_unidad       ; de la cadena " s"
   movf  decena,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movf  unidad,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movlw '.'
   call LCD_enviaCaracter
   movf tr_centesimas,0
   call decena_unidad
   movf  decena,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movf  unidad,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movlw ' '
   call LCD_enviaCaracter
   movlw 's'
   call LCD_enviaCaracter
   
   ; ESCRIBIMOS LA LINEA INFERIOR
   call LCD_irLineaInf

   movlw 'T'                ; Escribimos en el LCD "T.T.: "
   call LCD_enviaCaracter
   movlw '.'
   call LCD_enviaCaracter
   movlw 'T'
   call LCD_enviaCaracter
   movlw '.'
   call LCD_enviaCaracter
   movlw ':'
   call LCD_enviaCaracter
   movlw ' '
   call LCD_enviaCaracter
	 
   movf tt_segundos,0       ; Escribimos en el LCD el tiempo total seguido
   call decena_unidad       ; de la cadena " s"
   movf  decena,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movf  unidad,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movlw '.'
   call LCD_enviaCaracter
   movf tt_centesimas,0
   call decena_unidad
   movf  decena,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movf  unidad,0
   addwf var_aux1,0
   call LCD_enviaCaracter
   movlw ' '
   call LCD_enviaCaracter
   movlw 's'
   call LCD_enviaCaracter
	 
; ---- Cambio de estado a Reposo ---- ;
   movlw   reposo / 0x100             ; 
   movwf   estadoSig+0                ; // Iniciamos la variable estadoSig      
   movlw   reposo % 0x100             ; // para que apunte a la rutina "reposo"
   movwf   estadoSig+1                ; estadoSig= reposo;     // estadoSig=(estadoSig+0,estadoSig+1)
;-------------------------------------;

resultado_0
   return                             ; } 

    
;==================================
; Rutina decena_unidad
;==================================
decena_unidad                  ; void  uni_dec_segundos()
    
   ; {
   
   clrf unidad                 ; unidad = 0
   clrf decena                 ; decena = 0
	
bucle	
   movwf unidad                ; unidad = (w);
   movwf var_aux1              ; w' = w;          // Almacenamos temporalmente el acumulador
   movlw d'10'                 ;                  // Cargamos un 10 para hacer la resta
   subwf var_aux1,0            ; w = w' - 10;
                               ;                  // Si la operacion anterior es "=0" ó ">0" decenas++ y seguimos en el bucle
							   
   btfsc STATUS,Z              ; Si el resultado fue CERO     => Z=1 => decena_unidad_1
   goto  decena_unidad_1       ; 
   btfsc STATUS,C              ; Si el resultado fue POSITIVO => C=1 => decena_unidad_1
   goto  decena_unidad_1
   goto  decena_unidad_0       ; En otro caso, el resultado de la operacion fue "<0" => decena_unidad_0
   
decena_unidad_1
   incf decena,1               ; Incrementamos las decenas y volvemos al bucle
   goto bucle
   

decena_unidad_0	               ; Fin de la funcion
   movlw 0x30         ; 
   movwf var_aux1     ; Almacenamos en var_aux1 48 (30 en hexadecimal)
                      ; Lo usaremos para poder imprimir los numeros en el LCD

   return                      ; }
   
   
   