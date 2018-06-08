COMMENT !
Fichero que contiene las rutinas a la hora de leer o escribir bytes
en memoria por parte de la CPU, las cuales deben compartir el tiempo
con la ULA. De momento, no usado.


poke_byte_bx	dw		? ;Puntero a la rutina de poke byte a BX con AL
poke_word_bx   dw		? ;Puntero a la rutina de poke word a BX con AX

peek_byte_bx	dw		? ;Puntero a la rutina de peek byte con AL a BX
peek_word_bx	dw		? ;Puntero a la rutina de peek word con AX a BX

peek_byte_pc	dw		? ;Puntero a la rutina de peek byte con AL a SI
peek_word_pc	dw		? ;Puntero a la rutina de peek word con AX a SI
!
COMMENT !
;Tablas de los ciclos compartidos
;Si hay una s en el nombre, es el borde superior. Si hay una i, es el borde
;inferior. Y si hay una p, es la pantalla. Todos estan repetidos para cuando
;una instruccion sobrepasa una linea, pueda seguir leyendo los ciclos.

tabla_compartida_s_48 dw 224 dup (0)
							 dw 224 dup (0)
tabla_compartida_p_48 dw 16 dup ( 6,5,4,3,2,1,0,0 )
							 dw 24+48+24 dup ( 0 )
							 dw 16 dup ( 6,5,4,3,2,1,0,0 )
							 dw 24+48+24 dup ( 0 )
tabla_compartida_i_48 dw 224 dup (0)
							 dw 224 dup (0)

tabla_compartida_s_128 dw 228 dup (0)
							  dw 228 dup (0)
tabla_compartida_p_128 dw 16 dup ( 6,5,4,3,2,1,0,0 )
							  dw 100 dup ( 0 )
							  dw 16 dup ( 6,5,4,3,2,1,0,0 )
							  dw 100 dup ( 0 )
tabla_compartida_i_128 dw 228 dup (0)
							  dw 228 dup (0)

tabla_compartida_s_p2a dw 228 dup (0)
							  dw 228 dup (0)
tabla_compartida_p_p2a dw 16 dup ( 7,6,5,4,3,2,1,0 )
							  dw 100 dup ( 0 )
							  dw 16 dup ( 7,6,5,4,3,2,1,0 )
							  dw 100 dup ( 0 )
tabla_compartida_i_p2a dw 228 dup (0)
							  dw 228 dup (0)
!
