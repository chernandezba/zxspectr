COMMENT !
Tabla de los colores del Spectrum:
Formato:
Valor de Rojo, Valor de Verde, Valor de Azul
(La codificaci¢n para los colores en Spectrum es:
	bit  2      1      0
		  GREEN  RED    BLUE

 )
						R  G  B
!
bri0                    equ     42+5
bri1                    equ     16

;Tabla para los colores reales

;                 RED       GREEN     BLUE                 G R B
brillo0_real    db		0,	       0,        0			; 0 En SP: 0 0 0 Negro
				db    0,        0,        bri0      ; 1        0 0 1 Azul
				db    bri0,     0,		  0         ; 2        0 1 0 Rojo
				db		bri0,	    0,		  bri0      ; 3        0 1 1 Magenta
				db    0,		    bri0,	  0			; 4        1 0 0 Verde
				db		0,		    bri0,	  bri0		; 5        1 0 1 Cyan
				db		bri0,	    bri0,	  0			; 6        1 1 0 Amarillo
				db		bri0,	    bri0,	  bri0		; 7        1 1 1 Blanco
;Con brillo1

brillo1_real	db		0,	       0,        0			; 0        0 0 0 Negro
				db    0,        0,        bri0+bri1 ; 1        0 0 1 Azul
				db    bri0+bri1,0,		  0         ; 2        0 1 0 Rojo
				db		bri0+bri1,0,		  bri0+bri1 ; 3        0 1 1 Magenta
				db    0,		    bri0+bri1,0			; 4        1 0 0 Verde
				db		0,		    bri0+bri1,bri0+bri1	; 5        1 0 1 Cyan
				db		bri0+bri1,bri0+bri1,0			; 6        1 1 0 Amarillo
				db		bri0+bri1,bri0+bri1,bri0+bri1	; 7        1 1 1 Blanco

bri0_t          equ   8
bri1_t          equ     5

;Tabla de los colores con tonos: Grises, Verdes, etc.

;                 RED       GREEN     BLUE                 G R B
brillo0_tono    db		0,	       0,        0			; 0 En SP: 0 0 0 Negro
				db    bri0_t,   bri0_t,   bri0_t    ; 1        0 0 1 Azul
				db    bri0_t*2, bri0_t*2, bri0_t*2  ; 2        0 1 0 Rojo
				db		bri0_t*3, bri0_t*3, bri0_t*3  ; 3        0 1 1 Magenta
				db    bri0_t*4, bri0_t*4, bri0_t*4  ; 4        1 0 0 Verde
				db		bri0_t*5, bri0_t*5, bri0_t*5	; 5        1 0 1 Cyan
				db		bri0_t*6, bri0_t*6, bri0_t*6  ; 6        1 1 0 Amarillo
				db		bri0_t*7, bri0_t*7, bri0_t*7  ; 7        1 1 1 Blanco
;Con brillo1

brillo1_tono    db		0,	       0,        0			; 0 En SP:      0 0 0 Negro
				db    bri0_t+bri1_t,  bri0_t+bri1_t,   bri0_t+bri1_t  ; 1 0 0 1 Azul
				db    bri0_t*2+bri1_t,bri0_t*2+bri1_t,bri0_t*2+bri1_t ; 2 0 1 0 Rojo
				db		bri0_t*3+bri1_t,bri0_t*3+bri1_t,bri0_t*3+bri1_t ; 3 0 1 1 Magenta
				db    bri0_t*4+bri1_t,bri0_t*4+bri1_t,bri0_t*4+bri1_t ; 4 1 0 0 Verde
				db		bri0_t*5+bri1_t,bri0_t*5+bri1_t,bri0_t*5+bri1_t	; 5 1 0 1 Cyan
				db		bri0_t*6+bri1_t,bri0_t*6+bri1_t,bri0_t*6+bri1_t ; 6 1 1 0 Amarillo
				db		bri0_t*7+bri1_t,bri0_t*7+bri1_t,bri0_t*7+bri1_t ; 7 1 1 1 Blanco


brillo0		dw		brillo0_real
brillo1		dw		brillo1_real

tinta_menu	db		bri0+bri1,bri0+bri1,0    		;Color de la tinta
				db		0,			 0,		  bri0  		;Color del papel
;El color de tinta_menu sera el 9

buffer_paleta db	16*3 dup (?)
;Aqui se copiara la paleta original para luego poder modificar la paleta
;actual (brillo0 y brillo1) de acorde al control de brillo del menu
