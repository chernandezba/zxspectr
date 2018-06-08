;Este fichero contiene las posiciones donde se almacenan los valores
;de los puertos del teclado y la transformacion de los codigos de teclas
;enviados por el puerto 60h a estos "puertos"

;Variables publicas
PUBLIC puerto_65278,puerto_65022,puerto_64510,puerto_63486
PUBLIC puerto_61438,puerto_57342,puerto_49150,puerto_32766
PUBLIC puerto_221

;Rutinas publicas
PUBLIC puerto_in_254


;                    Bits:  4    3    2    1    0     ;desplazamiento puerto
puerto_65278   db    255  ; V    C    X    Z    Sh    ;0
puerto_65022   db    255  ; G    F    D    S    A     ;1
puerto_64510	db		255  ; T    R    E    W    Q     ;2
puerto_63486	db		255  ; 5    4    3    2    1     ;3
puerto_61438	db		255  ; 6    7    8    9    0     ;4
puerto_57342	db		255  ; Y    U    I    O    P     ;5
puerto_49150	db		255  ; H		J	  K	 L    Enter ;6
puerto_32766	db		255  ; B    N    M    Simb Space ;7


;Puertos no leidos por el spectrum. Algunas teclas usadas en el emulador: ESC, +, -, PGUp, PGDown
puerto_especial1  db		255  ; F1   F2   F3   F4   F5    ;8
puerto_especial2	db		255  ; F6   F7   F8   F9   F10   ;9
puerto_especial3	db		255  ; ESC  '?   ­¨   BKSP TAB   ;10
puerto_especial4	db    255  ; `^   +*   CPLK ¥    'ù    ;11
puerto_especial5	db		255  ; Del   <>   ,;   .:   -_    ;12
puerto_especial6	db		255  ;  ^   NumL ScrL Home PgUp  ;13
puerto_especial7	db		255  ; -    5    +    End  PgDn  ;14

puerto_221			db		31	  ; Ins  Arr  Aba  Izq  Der  Cuando se lea
								  ; el puerto 221 o 31 se toma este valor,se
								  ; hace un NOT y luego un AND 31  ;15
; La tecla ‡€ genera el mismo c¢digo que Simb
; La tecla ALT ser  igual que CTRL, o sea, Simb
; ^ = No usado (Reservado anteriormente para ALT)
;PrtsC o * en teclados extendidos genera el c¢digo Ins

;Tabla de transformaci¢n de teclas del puerto 60h a los puertos del
;Spectrum y los puertos especiales
;Formato:
;bbbpppp :   b: n£mero del bit de la tecla(1,2,3,4 o 5)
;				 p: desplazamiento desde puerto_65278 del puerto de la tecla
;Nuevo formato: Se permite dos pulsaciones de tecla. Si el segundo valor es 0, no se pulsa segunda tecla
;Eliminado de "puerto_especialXX" :
; BLOQ MAYUS
; antiguo: 11+3*16,0,
;nuevo: 0+1*16,  3+2*16

;BCSP: Backspace:
;antiguo: 10+2*16,0
;nuevo: 0+1*16,4+1*16

;TAB
;Antiguo:  10+1*16,0,
;Nuevo:  0+1*16,7+2*16

tabla_puertos_spectrum:
					db   10+5*16,0,3+1*16,0,3+2*16,0,3+3*16,0,3+4*16,0,3+5*16,0 ;ESC 12345
					db   4+5*16,0,4+4*16,0,4+3*16,0,4+2*16,0,4+1*16,0 ;67890
					db   10+3*16,0,10+3*16,0, 0+1*16,4+1*16,  0+1*16,7+2*16, 2+1*16,0 ;'? ­¨ BCSP TAB Q
					db   2+2*16,0,2+3*16,0,2+4*16,0,2+5*16,0,5+5*16,0 ;WERTY
					db   5+4*16,0,5+3*16,0,5+2*16,0,5+1*16,0,11+5*16,0 ;UIOP `^
					db   11+3*16,0,6+1*16,0,7+2*16,0,1+1*16,0,1+2*16,0 ;+* Ent Ctrl A S
					db   1+3*16,0,1+4*16,0,1+5*16,0,6+5*16,0,6+4*16,0 ;DFGHJ
					db   6+3*16,0,6+2*16,0,11+2*16,0,11+1*16,0,7+2*16,0 ;KL¥ 'ù ‡€
					db   0+1*16,0,12+3*16,0,0+2*16,0,0+3*16,0,0+4*16,0 ;Mays <> ZXC
					db   0+5*16,0,7+5*16,0,7+4*16,0,7+3*16,0,12+3*16,0 ;VBNM ,;
					db   12+2*16,0,12+1*16,0,0+1*16,0,15+5*16,0,7+2*16,0 ;.: -_ Mays * Alt
					db   7+1*16,0, 0+1*16,3+2*16, 8+5*16,0,8+4*16,0,8+3*16,0 ;SPC CPLK F1 F2 F3
					db   8+2*16,0,8+1*16,0,9+5*16,0,9+4*16,0,9+3*16,0 ;F4 F5 F6 F7 F8
					db   9+2*16,0,9+1*16,0,13+3*16,0,13+3*16,0,13+2*16,0 ;F9 F10 Num Scr Home
tabla_puerto_arr:			db   4+4*16,0+1*16 ;Arr
					db   13+1*16,0,14+5*16,0 ;PgUp - 
tabla_puerto_izq:			db   3+5*16,0+1*16, 14+3*16,0 ;Izq 5
tabla_puerto_der:			db   4+3*16,0+1*16; Der 
					db   14+3*16,0,14+2*16,0; + End
tabla_puerto_aba:			db   4+5*16,0+1*16,14+1*16,0 ;Aba PgDn
					
					db   15+5*16,0,12+5*16,0 ;Ins Del

;Antes, con joystick kempston:
;tabla_puerto_arr:                       db   15+4*16,0 
;tabla_puerto_izq:                       db   15+2*16,0
;tabla_puerto_der:                       db   15+1*16,0
;tabla_puerto_aba:                       db   15+3*16,0


puerto_in_254:		;Rutina de lectura del puerto 254
						;Salida=AL=Valor
					mov	bx,offset puerto_65278
					xor	al,al
					mov	cx,8
puerto_in_254_buc:
					mov	ah,byte ptr [bx]
					not	ah
					or		al,ah
					inc	bx
					loop	puerto_in_254_buc
					not	al
					ret

lee_todas_teclas:	;Rutina de lectura de todas las puertas
						;Actua igual que puerto_in_254 pero
						;considerando tambi‚n los puertos especiales
						;y que sin considerar pulsaci¢n de disparo (tecla Ins)
						;pues sino podr¡a actuar el disparador autom tico
						;y tampoco las may£sculas, pues es usado para decrementar
						;valores
						;Salida=AL=Valor
					mov	bx,offset puerto_65278
					xor	al,al
					mov	cx,16
lee_todas_teclas_buc:
					mov	ah,byte ptr [bx]
					cmp	cx,1 						;si est  en puerto_221,poner
														;bit de disparo a 1 (no pulsado)
					jnz	no_en_off_221
					or		ah,16
					jmp	short lee_todas_sigue

no_en_off_221:
					cmp	cx,16
					jnz	lee_todas_sigue
					or		ah,1

lee_todas_sigue:
					not	ah
					or		al,ah
					inc	bx
					loop	lee_todas_teclas_buc
					not	al
					ret
