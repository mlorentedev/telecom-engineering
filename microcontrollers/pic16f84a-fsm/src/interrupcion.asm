; * *************************************** *
; *   Rutina de atención a la interrupción  *
; * Daniel Sojo España                      *
; * Alejandro Grande Gómez                  *
; * Manuel Lorente Almán                    *
; * *************************************** *
; * *************************************** *
; =======================================================
; interrupcion
;     - Si T0IF='1' llama a la rutina interrupcion_T0I
; =======================================================
interrupcion                      ; void interrupcion()
                                  ; {
   btfsc INTCON,T0IF              ;    if (INTF==1)
                                  ;    {
   call  interrupcion_T0I         ;       INTERRUPCION_RBO();
                                  ;    }
   btfsc INTCON,RBIF              ;    else if (RBIF==0)
                                  ;    {
   call  interrupcion_RB4_RB7     ;       INTERRUPCION_RB4_RB7();
                                  ;    }
                                  ;    else
                                  ;    {
   bcf    INTCON,INTF             ;       INTF=0;     
                                  ;    }
   return                         ; }   

     
; =================================================================
; interrupcion_T0I
;     - Ocurre cada vez que desborda TMR0 (pasa de 255 a 0)
;     - Cada 4 interrupciones se incrementará la cuenta del display
;     - Baja la bandera que originó esta interrupción (T0IF)
; =================================================================
interrupcion_T0I                   ; void interrupcion _T0I()
                                   ; {
   bcf     INTCON,T0IF             ;  T0IF=  0;    // Baja la bandera que originó la interrupción 
   movlw   d'6'                    ;
   movwf   TMR0                    ;  TMR0= 6;     // Valor ajustado para que el timer0 avise cada 500hz.
                                   ;
   decfsz  cuenta5,1               ;  if (--cuenta5  == 0) 
   goto    interrupcion_T0I_0      ;  {
                                   ;
   movlw   d'5'                    ;   
   movwf   cuenta5                 ;      cuenta5 = 5; // cada 5 interrupciones -> 1 centesima de segundo
                                   ;
   decfsz    cuenta100,1           ;      if (--cuenta100 == 0)
   goto    interrupcion_T0I_0      ;      {
   movlw   d'100'                  ;         
   movwf   cuenta100               ;         cuenta100 = 100;
   incf segundos,1                 ;         segundos++;
                                   ;      }
    	
	
interrupcion_T0I_0                 ;  } else {
   movlw d'100'                    ; 
   movwf centesimas                ;     centesimas = 100;  // Para poder usar la funcion subwf
   movf  cuenta100,0               ;                        // Cargamos cuenta100 en el acumulador
   subwf centesimas,1              ;     centesimas = 100 - cuenta100;
   return                          ;     return;       
                                   ; }
								   

interrupcion_RB4_RB7               ; void INTERRUPCION_RB4_RB7()
                                   ; {
   bcf     INTCON,RBIF 		       ; // Bajar bandera de interrupcion
                                   ;
								   ; COMPROBAMOS SI EL ATLETA HA SALIDO DEL TACO Y PASAMOS LOS TIEMPOS ACUTALES A LOS DE REACCIÓN
   btfsc   PORTB,4                 ; if (RB4 == 0)
   goto    meta                    ; {
   movf    centesimas,0            ;
   movwf   tr_centesimas           ;    tr_centesimas = centesimas;
   movf    segundos,0              ;
   movwf   tr_segundos             ;    tr_segundos = segundos;
   goto finish                     ; }
	
meta							   ; COMPROBAMOS SI EL ATLETA LLEGA AL FINAL Y PASAMOS LOS TIEMPOS ACTUALES A LOS FINALES.
   btfsc PORTB,5                   ; if (RB5 == 0)
   goto finish                     ; {
   movf    centesimas,0            ;
   movwf   tt_centesimas           ;    tt_centesimas = centesimas;
   movf    segundos,0              ;
   movwf   tt_segundos             ;    tt_segundos = segundos;
   movlw   d'1'                    ;
   movwf   fin                     ;    fin = 1;
                                   ; }
	
finish 
   return                          ; }
