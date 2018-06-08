;
;This file is part of ZXSpectr.
;
;ZXSpectr is free software; you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation; either version 2 of the License, or
;(at your option) any later version.

;ZXSpectr is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.

;You should have received a copy of the GNU General Public License
;along with ZXSpectr; if not, write to the Free Software
;Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;


COMMENT !

El siguiente texto esta extraido literalmente del Manual del Usuario
del ZX Spectrum +2A, las paginas 206 a 209. Nota: Esta escaneado y 
traducido a texto mediante Corel Trace, sin apenas fallos en las letras 
(unicamente algunos acentos).


Gestion de la memoria

Ya hemos mencionado antes que en el ordenador hay bastante mas memoria que la que la que
el microprocesador puede manejar comodamente. Este puede direccionar solo 64K de
memoria de una vez, pero la memoria adicional puede ser intercambiada a discrecion con
esos 64K. Observe un aparato de television; a pesar de que so1o podemos ocuparnos de
un canal a la vez, hay otros canales que pueden ser seleccionados pulsando los botones
adecuados. Asi, aunque hay mas informacion de la que somos capaces de asimilar a un
tiempo, podemos elegir la que nos interese.
Lo que ocurre en el ordenador es muy parecido. Colocando los bits adecuados en cierta
puerta de E/S, la maquina puede elegir que porciones quiere usar del total de 192K de
memoria. BASIC ignora casi permanentemente toda la memoria adicional. En cambio,en

los programas de juegos viene muy bien tener una RAM triplicada. Vuelva a echar un
vistazo al mapa de memoria del principio de esta seccion. Las paginas 2 y 5 de la RAM
estan siempre en la posicion indicada en el diagrama, y sin embargo no hay ninguna razon
por la que no puedan estar en la seccion conmutable (C000h a FFFFh), si bien esto tampo-
co tendria ninguna utilidad.

Los bancos (o paginas) de RAM son de dos tipos: las paginas 4 a 7, que son compartidas
(es decir, que comparten el tiempo con los circuitos de video), y las paginas 0 a 3, que
solo estan al servicio del microprocesador. Cualquier programa de codigo de maquina
que requiera un control muy preciso del tiempo (por ejemplo, los de musica o comunica-
ciones) debe guardar todas las rutinas de temporizacion en los bancos no compartidos.
Por ejemplo, ejecutando instrucciones NOP en la RAM compartida obtenemos una fre-
cuencia de reloj de 2.66 MHz, mientras que en la RAM no compartida obtendriamos 3.55
MHz; la velocidad se reduce, pues, en un 25%.

El conmutador de hardware esta en la direccion de E/S 7FFDh (32765). El campo del bit
para esta direccion es el siguiente:
DO a D2 	Seleccion de RAM
D3   		Seleccion de pantalla
D4			Seleccion de ROM
D5       Inhabilitacion de la paginacion

D2 a DO crean un numero de tres bits que selecciona que RAM entra en el hueco compren-
dido entre C000h y FFFFh. En BASIC normalmente esta seleccionada la pagina 0; duran-
te la edicion y el acceso a +3DOS se usa la pagina 7 para almacenar diversos tampones
y memorias transitorias. D3 conmuta pantallas; la pantalla 0 est en la pagina 5 de la
RAM (que normalmente empieza en 4000h) y es la que utiliza BASIC; la pantalla 1 esta
en la pagina 7 (a partir de COOOh) y solo puede ser usada por programas de codigo de ma-
quina. Es perfectamente factible preparar una pantalla en la pagina 7 y despues «descon-
mutarla»; de esta forma se deja los 48K libres para datos y programa. (Tengase en cuenta
que la orden de copia de ficheros, COPY, puede crear tampones en la zona de la segunda
pantalla, y por lo tanto destruir la imagen que hubieramos almacenado en ella.) D4 deter-
mina, junto con el bit 2 de la puerta 1FFDh, que ROM debe ser colocada en las direcciones
0000h a 3FFFh. D5 es un dispositivo de seguridad; en cuanto se pone a 1 este bit, quedan
imposibilitadas todas las operaciones de paginacion. Este sistema es el utilizado cuando
el ordenador adopta la configuracion del Spectrum de 48K estandar y los circuitos de pagi-
nacion de memoria estan bloqueados. A partir de ese momento no se lo puede volver a
convertir en un ordenador de 128K mas que apagandolo o pulsando el boton REINIC;
no obstante, el circuito de sonido puede seguir siendo controlado por OUT. Si se carga
desde el disco un programa de juego diseñado para el Spectrum de 48K y no funciona,
quiza lo haga si se da la orden SPECTRUM y luego OUT 32765,48 (que bloquea el bit
5 en esta puerta).

El +2A utiliza la puerta de E/S 1FFDh para realizar ciertas operaciones de conmutacion
de ROM y RAM. El campo del bit para esta direccion es el siguiente:
D0			Decide si D1 y D2 afectan a la ROM o a la RAM    (1)
D1 y D2 	Conmutacion de ROM/RAM                           (2)
D4			Motor del disco
D5			Señal STROBE en la puerta paralelo (activa a nivel bajo)

(1) Nota: En el manual se menciona erroneamente como bit D3
(2) Nota: En el manual se mencionan erroneamente como bits D0 y D1

Cuando el bit 0 esta a 0, el bit 1 es indiferente y el bit 2 constituye un conmutador 'vertical'
para la ROM (que elige entre ROM 0 y ROM 2, o entre ROM 1 y ROM 3). El bit 4 de
la puerta 7FFDh es un conmutador 'horizontal' para la ROM (que elige entre ROM 0 y
ROM 1, o entre ROM 2 y ROM 3). El siguiente diagrama ilustra las diferentes posibilidades:

 ----------			Bit 4 de 7FFDh (23388)         ------------
 | ROM 0  |       variable de sistema: BANKM)	 |	ROM 1    |
 |			 |			<-     horizontal		->        |          |
 | Editor |													 |	Sintaxis	|
 ----------	                                     ------------
	  ^                                                ^
	  |                                                |

Bit 2 de 1FFDh (23399)
(variable de sistema: BANK678)
	vertical														vertical

		|																|
		v                                               v

 ----------													 ------------
 |	ROM 2	 |													 |	ROM 3	   |
 |			 |			<-		horizontal		->        |          |
 |	DOS	 |													 |	48 BASIC |
 ----------                                      ------------

		Conmutacion horizontal y vertical de la ROM

Es util imaginar que el bit 4 de la puerta 7FFDh y el bit 2 de la puerta 1FFDh se combinan
para formar un numero de dos bits (margen de 0 a 3) que determina que ROM entra en
la zona de 0000h a 3FFFh. Al formar ese numero, el bit 4 de la puerta 7FFDh seria el
menos significativo, y el otro el mas significativo.


Bit 2 de 1FFDh        			 | Bit 4 de 7FFDh					 |	ROM que entra en
(Variable de sistema: BANK678) | (Variable de sistema: BANKM)|	0000h-3FFFh
-------------------------------|-----------------------------|-----------------
				  0					 |					0					 |			0
				  0					 |					1					 |			1
				  1					 |					0					 |			2
				  1					 |					1					 |			3

		Conmutacion de ROM (con el bit 0 de 1FFDh puesto a 0)

Cuando el bit 0 de la puerta 1FFDh esta a 1, los bits 1 y 2 controlan que combinacion
de paginas de RAM ocupan los 64K posibles. Estas combinaciones no son utilizadas por
BASIC, pero pueden serlo por los autores de sistemas operativos y programas de juegos.
Cuando se invoca la rutina 'DOS CARGAR', el cargador inicial es transferido al «entor-
no» de las paginas 4, 7, 6, 3. Las opciones de paginacion de RAM del +2A son:

Bit 2 de 1FFDh		|		Bit 1 de 1FFDh		|		Paginas de RAM que
						|								|			entran en
						|								|			0000h-3FFFh,
						|								|			4000h-4FFFh, etc.
------------------|-----------------------|--------------------------																	
						|                       |
		0				|				0				|			0,1,2,3
		0				|				1				|			4,5,6,7
		1				|				0				|			4,5,6,3
		1				|				1				|			4,7,6,3																									

		Conmutacion de paginas de RAM (con el bit 0 de 1FFDh puesto a 1)
!

COMMENT !

La configuracion normal de paginas en modo 48k es la siguiente:

0000h-3fffh: 	ROM 3
4000h-7fffh:	RAM 5
8000h-bfffh:	RAM 2
c000h-ffffh:	RAM 0

		
!

PUBLIC out_p32765,out_p8189


;
puerto_32765	db		0  ;Valor del ultimo OUT al puerto 32765
puerto_8189		db		0  ;Valor del ultimo OUT al puerto 8189
;
;No modificar el orden consecutivo de puerto_32765 ni puerto_8189
;para que asi se pueda leer con un word


segmento_ROM	dw		?  ;Segmento a partir del cual empiezan
								;las ROMS: 0,1,2,3
								;(Van antes de las RAMS)

paginas_actuales db 0,5+4,2+4,0+4
;Contiene para cada parte de memoria la pagina(ROM o RAM) asignada

;

si_despaginar_rom db 0
;Si ese valor no es 0, la ROM se despagina, es decir,
;se copia en segmento_ROM
;Usado por insertar cinta,
;al cargar las ROMS en el menu
;Despues de llamar a paginar, se pone
;a 0 automaticamente

out_p32765_si dw ? ;Temporal
out_p32765_di dw ? ;Temporal
out_p32765_es dw ? ;Temporal
out_p32765_ax dw ? ;Temporal

comprueba_si_128k:
;Rutina que comprueba si hay 128k de memoria. Para ello mira
;si el puerto no esta bloqueado y si al cargar el emulador habia memoria
;suficiente
;Devuelve: Z si hay memoria 128k
					;Mirar si se tiene suficiente memoria

comprueba_si_128k_2:  ;Mirar si el puerto no esta bloqueado
					test	puerto_32765,00100000b
					ret




out_p32765:

					call	comprueba_si_128k

					jz		out_p32765_2
					ret  ;Puerto bloqueado
out_p32765_2:
					mov   out_p32765_si,si
					mov   out_p32765_di,di
					mov   out_p32765_es,es
					mov   al,bl
					mov   out_p32765_ax,ax

					;Mirar si se esta en el modo de paginacion RAM a ROM
					test	puerto_8189,1
					jnz 	out_p32765_ramrom

					mov	ah,puerto_32765
					and	ax,0000011100000111b
					cmp	al,ah
					jz    out_p32765_iguales

					call	pagina_ram


out_p32765_iguales:
					mov   al,byte ptr out_p32765_ax

					;Mirar si se cambia de ROM horizontal
					mov	ah,puerto_32765
					and	ax,0001000000010000b
					cmp	al,ah
					jz		out_p32765_ramrom
					shr	al,1
					shr	al,1
					shr	al,1
					shr	al,1
					mov	ah,puerto_8189
					and	ah,00000100b
					shr	ah,1
					or		al,ah

					call	pagina_rom

out_p32765_ramrom:

					mov   al,byte ptr out_p32765_ax
					mov	puerto_32765,al

					call  pone_seg_pant

					mov   es,out_p32765_es
					mov   di,out_p32765_di
					mov   si,out_p32765_si
					ret


out_p32765_128:


					call	comprueba_si_128k

					jz    out_p32765_128_2
					ret  ;Puerto bloqueado
out_p32765_128_2:
					mov   out_p32765_si,si
					mov   out_p32765_di,di
					mov   out_p32765_es,es
					mov   al,bl
					mov   out_p32765_ax,ax


					mov	ah,puerto_32765
					and	ax,0000011100000111b
					cmp	al,ah
					jz    out_p32765_128_iguales

					call	pagina_ram


out_p32765_128_iguales:


					mov   al,byte ptr out_p32765_ax

					;Mirar si se cambia de ROM horizontal
					mov	ah,puerto_32765
					and	ax,0001000000010000b
					cmp	al,ah
					jz    out_p32765_128_ramrom
					shr	al,1
					shr	al,1
					shr	al,1

																										 ;

					mov   ah,al
					;Poner que si es ROM0, pagina0
					;Si es ROM1, pagina3

					shr	al,1
					or		al,ah

					call	pagina_rom

out_p32765_128_ramrom:


					mov   al,byte ptr out_p32765_ax
					mov	puerto_32765,al

					call  pone_seg_pant
					mov   es,out_p32765_es
					mov   di,out_p32765_di
					mov   si,out_p32765_si


					ret

actualiza_buffer_57:
;Punto de entrada al cambiar de pantalla (5 o 7)


					push	es

					;Poner pagina correspondiente
					;(5 o 7) en zona 4000h-7fffh

					mov	al,1
					mov   ah,bl


					mov	bl,byte ptr paginas_actuales+1
					push	bx
					call	paginar

					mov   si,16384
					mov   di,16384
					mov   cx,6912/2
					mov   es,seg_pant_57
					mov   ds,seg_spectrum ;pant

					call  hace_movsw

					push  cs
					pop   ds


					pop	bx
					mov	ah,bl
					mov	al,1

					call	paginar
					pop	es
					mov   ax,seg_pant_57
					mov   seg_pant,ax
					ret



out_p8189:
					call	comprueba_si_128k

					jz		out_p8189_2
					ret  ;Solo hay 48k
out_p8189_2:
					mov   out_p32765_si,si
					mov   out_p32765_di,di
					mov   out_p32765_es,es
					mov   al,bl
					mov   out_p32765_ax,ax


					;Mirar si se esta en el modo de paginacion RAM a ROM
					test	puerto_8189,1
					jz 	out_p8189_3
					jmp	out_p8189_ramrom
out_p8189_3:

					;Mirar si cambia al modo de paginacion RAM a ROM
					test	al,1
					jnz	out_p8189_4
					jmp	out_p8189_nocambiaramrom

out_p8189_4:

					;Cambia al modo de paginacion RAM a ROM

out_p8189_paginaramrom:

					;Guardar las paginas RAM activas (5,2,paginada en C000h)

					;Ahora ya se tienen guardadas las RAMS en su sitio
					;Mirar las paginas que entran

					mov   al,byte ptr out_p32765_ax

					and	al,00000110b
					cmp	al,00000000b
					jnz	out_p8189_noentra0123

					;Entran las paginas 0,1,2,3
					xor	al,al
					mov	ah,4 ;Saltar las 4 ROMS

out_p8189_entra0123_buc:
					push	ax
					call	paginar
					pop	ax
					inc	al
					inc	ah
					cmp	al,4
					jnz   out_p8189_entra0123_buc

					jmp	out_p8189_romiguales


out_p8189_noentra0123:
					cmp	al,00000010b
					jnz	out_p8189_noentra4567

					;Entran las paginas 4,5,6,7
					xor	al,al
					mov	ah,4+4 ;Saltar las 4 ROMS

out_p8189_entra4567_buc:
					push	ax
					call	paginar
					pop	ax
					inc	al
					inc	ah
					cmp	al,4
					jnz   out_p8189_entra4567_buc

					jmp	out_p8189_romiguales

out_p8189_noentra4567:
					cmp	al,00000100b
					jnz	out_p8189_noentra4563

					;Entran las paginas 4,5,6,3

					xor	al,al
					mov	ah,4+4

out_p8189_entra4563_buc:
					push	ax
					call	paginar
					pop	ax
					inc	al
					inc	ah
					cmp	al,3
					jnz   out_p8189_entra4563_buc

					mov	ah,3+4
					call	paginar

					jmp	out_p8189_romiguales

out_p8189_noentra4563:

					;Entran las paginas 4,7,6,3
					xor	al,al
					mov	ah,4+4
					push	ax
					call	paginar
					pop	ax
					inc	al

					mov	ah,7+4
					push	ax
					call	paginar
					pop	ax
					inc	al

					mov	ah,6+4
					push	ax
					call	paginar
					pop	ax
					inc	al

					mov	ah,3+4
					call	paginar

					jmp	out_p8189_romiguales

out_p8189_ramrom:
					;Mirar si se pone a 0 la ram-rom
					test	al,1
					jz		out_p8189_ramroma0

					;Mirar si se cambia de pagina
					mov	ah,puerto_8189
					and	ax,0000011000000110b
					cmp	al,ah
					jnz	out_p8189_ramrom1
					jmp	out_p8189_romiguales

out_p8189_ramrom1:
					;Se mantiene igual, seguir
					jmp	out_p8189_paginaramrom

out_p8189_ramroma0:

					;Hay cambio

					mov	al,1
					mov	ah,5+4
					push	ax
					call	paginar
					pop	ax
					inc	al

					mov	ah,2+4
					push	ax
					call	paginar
					pop	ax
					inc	al

					mov	ah,puerto_32765
					and	ah,7
					add	ah,4
					call	paginar

					;Hacer que ponga la ROM
					pop	ax
					push	ax
					and	al,00000100b
					jmp	out_p8189_nocambiaramrom2


out_p8189_nocambiaramrom:

					mov   al,byte ptr out_p32765_ax
					;Mirar si se cambia de ROM vertical
					mov	ah,puerto_8189
					and	ax,0000010000000100b
					cmp	al,ah
					jz		out_p8189_romiguales
out_p8189_nocambiaramrom2:

					shr	al,1
					mov	ah,puerto_32765
					and	ah,00010000b
					shr	ah,1
					shr	ah,1
					shr	ah,1
					shr	ah,1
					or		al,ah
					call	pagina_rom

out_p8189_romiguales:

					mov   al,byte ptr out_p32765_ax
					mov	puerto_8189,al

					call  pone_seg_pant

					mov   es,out_p32765_es
					mov   di,out_p32765_di
					mov   si,out_p32765_si

					ret

out_nada: ;Usado por las outs de 48k,128k(8189)
					ret

paginar:
;Rutina que pone en la zona de los 64k indicada la pagina indicada
;Entrada: AL=zona de 64k:
;			AL=0 para direccion 0000h-3fffh
;			AL=1 para direccion 4000h-7fffh
;			AL=2 para direccion 8000h-bfffh
;			AL=3 para direccion c000h-ffffh
;			AH=pagina de ROM (0..3) o RAM (4..11)
;Si no hay EMS, previamente se mueve la pagina anterior
					xor	bh,bh
					mov	bl,al
					add	bx,offset paginas_actuales

					;Mirar si hay EMS

					test	bits_estado3,00010000b
					jz		paginar0

					mov	byte ptr [bx],ah

					mov	bl,ah
					xor	bh,bh

					jmp	mapear_ems

paginar0:
					mov	paginar_es,es


					;Sacar la pagina actual
					;Excluir las paginas ROM

					mov	paginar_bx,bx


					mov	paginar_ax,ax


					mov	al,byte ptr [bx]
					mov   temporal_paginar,al
					call	calcula_segmento_ROM_pagina
					mov	es,ax
					;En ES se tiene el destino

					mov	ax,paginar_ax

					call	calcula_seg_spectrum_pagina

					push	ax ;Guardar segmento
					mov	ds,ax


					mov	cx,8192 ;Como se hace con MOVSW, es 16384/2
					xor	si,si
					mov	di,si

					cmp   cs:si_despaginar_rom,0
					jnz   paginar_
					 ;Si ese valor no es 0, la ROM se despagina, es decir,
					 ;se copia en segmento_ROM
					 ;Usado por insertar cinta,
					 ;al cargar las ROMS en el menu

					cmp	cs:temporal_paginar,4
					jc		paginar00
paginar_:
					mov   cs:si_despaginar_rom,0

					call	hace_movsw

paginar00:

					push	cs
					pop	ds

					xor	di,di

					xor	si,si



					mov	cx,8192

					pop	es ;Obtener destino



					mov 	ax,paginar_ax

					call	calcula_origen_pagina

					mov	dx,paginar_ax

					mov	bx,paginar_bx


					mov	byte ptr [bx],dh ;Poner pagina actual
					mov	ds,ax

					call	hace_movsw

					push	cs
					pop	ds

					mov	es,paginar_es

					ret

temporal_paginar db ? ;Pagina que habia anteriormente
paginar_ax		dw		?
paginar_bx		dw		?
paginar_cx		dw		?
paginar_es		dw		?

calcula_segmento_ROM_pagina:
;Dado el valor de la pagina AL, calcula el segmento donde se encuentra
;ubicada esa pagina
					multiplica_AL_por_1024
					add	ax,segmento_ROM
					ret

calcula_seg_spectrum_pagina:
					multiplica_AL_por_1024
					add	ax,seg_spectrum
					ret

calcula_origen_pagina:
;Dado el valor de la pagina AH y zona fisica AL, dice en AX de donde se
;ha de sacar la pagina
					;Mirar primero si la pagina a poner ya esta en alguna zona fisica

					xor	dl,dl
					mov	bx,offset paginas_actuales
calcula_origen_pagina_bucle:
					cmp	byte ptr [bx],ah
					jz		calcula_origen_pagina_si
					inc	bx
					inc	dl
					cmp	dl,4
					jnz   calcula_origen_pagina_bucle
					mov	al,ah
					jmp	calcula_segmento_ROM_pagina

calcula_origen_pagina_si:
					mov	al,dl
					jmp	calcula_seg_spectrum_pagina

hace_movsw:
					cld
					 ;Mirar si hay 386 o superior
					test  byte ptr cs:bits_estado3,01000000b
					jz		hace_movsw_no386
					 ;Dividir CX en 2
					shr	cx,1
					db		0f3h,66h,0a5h ;REP MOVSD
					 ;Con el movsd es un 13% mas rapido
					ret

hace_movsw_no386:
					rep	movsw
					ret

pagina_ram:
;Rutina que pone en la zona de memoria c000h-ffffh la RAM indicada
;Entrada: AL=Pagina que entra
					mov	ah,al
					add	ah,4 ;Saltar las ROMS
					mov	al,3 ;Zona 49152-65535
					jmp	paginar

pagina_rom:
;Rutina que pone en la zona de memoria 0000h-3fffh la ROM indicada
;Entrada: AL=Pagina que entra
					mov	ah,al
					xor	al,al
					jmp	paginar

pone_seg_pant:
;Rutina que dado el valor de puerto_32765 en AL
;y el de puerto_8189 en AH pone seg_pant adecuadamente

					mov   ax,word ptr puerto_32765

					mov   bl,5+4
					test  al,8
					jz    pone_seg_pant0

					inc   bl
					inc   bl  ;Pantalla 7
pone_seg_pant0:

					push  bx
					call	donde_pantalla
					pop   bx
					sub   ax,1024
					cmp   ax,seg_pant
					jnz   pone_seg_pant1
					;Queda igual, Volver
					ret
pone_seg_pant1:
					;Si hay EMS, y sale fuera de seg_spectrum, no tocar
					test  bits_estado3,00010000b
					jz    pone_seg_pant3
					;Hay EMS. Ver si sale
					add     ax,1024

					cmp   byte ptr paginas_actuales+1,bl
					jz    pone_seg_pant2
					cmp   byte ptr paginas_actuales+2,bl
					jz    pone_seg_pant2
					cmp   byte ptr paginas_actuales+3,bl
					jz    pone_seg_pant2

					call  actualiza_buffer_57
					ret

pone_seg_pant2:
					sub   ax,1024 ;Poner que empiece en desplazamiento 16384
pone_seg_pant3:
					mov	seg_pant,ax
					ret

donde_pantalla:
;Rutina que devuelve en AX el segmento donde esta la pantalla activa
;Entrada: AL=valor del puerto 32765
;			 AH=valor del puerto 8189

					 ;Mirar si es paginacion de RAM en ROM
					test	ah,1
					jnz	donde_pantalla0
					jmp   donde_pantalla_noramrom

donde_pantalla0:
					 ;Ver la combinacion
					and	ah,00000110b
					cmp	ah,00000000b
					jnz	donde_pantalla_no0123rom
					;Paginas 0,1,2,3
					;Ver si es la pantalla 5 o la 7
					mov	bx,segmento_ROM
					add	bx,5120+4096  ;5*16384/16+las ROMS
					test	al,8
					jz		donde_pantalla_0123_2
					add	bx,2048  ;2*16384 Poner en RAM7
donde_pantalla_0123_2:
					mov	ax,bx
					ret
donde_pantalla_no0123rom:
					cmp	ah,00000010b
					jnz	donde_pantalla_no4567rom
					;Paginas 4,5,6,7
					;Ver si es la pantalla 5 o la 7
					mov	bx,seg_spectrum
					add	bx,1024  ;16384/16
					test	al,8
					jz		donde_pantalla_0123_2
					add	bx,2048  ;2*16384 Poner en RAM7
					jmp	short donde_pantalla_0123_2

donde_pantalla_no4567rom:
					cmp	ah,00000100b
					jnz	donde_pantalla_no4563rom
					;Paginas 4,5,6,3
					;Ver si es la pantalla 5 o la 7
					test	al,8
					jnz	donde_pantalla_4563_2

donde_pantalla_4000:

					;Pantalla 5
					mov	bx,seg_spectrum
					add	bx,1024
					jmp	short donde_pantalla_0123_2

donde_pantalla_4563_2:
					;Pantalla 7
					mov	bx,segmento_ROM
					add	bx,7168+4096   ;7*16384/16+las ROMS
					jmp	short donde_pantalla_0123_2

donde_pantalla_no4563rom:
					;Paginas 4,7,6,3
					;Ver si es la pantalla 5 o la 7
					test	al,8
					jnz	donde_pantalla_4000

					;Pantalla 5
					mov	bx,segmento_ROM
					add	bx,5120+4096  ;5*16384/16+las ROMS
					jmp	short donde_pantalla_0123_2

donde_pantalla_noramrom:
					;Ver si es la pantalla 5 o la 7
					test	al,8
					jz 	donde_pantalla_4000

					;Ver si esta paginada la RAM 7
					and 	al,7
					cmp	al,7
					jnz	donde_pantalla_4563_2 ;No esta paginada la 7

					;Esta paginada la 7
					mov	ax,seg_spectrum
					add   ax,3072 ;3*16384/16
					ret

donde_rom3:
;Rutina que devuelve en AX el segmento donde este la RAM3
;O sea, en seg_spectrum (si no hay 128k o si esta paginada)
;o en segmento_ROM+... si hay 128k y no esta paginada
					;Mirar si hay 128k, la ROM esta en pagina_ROM,
					;si no, esta en seg_spectrum

					test	bits_estado3,00100000b
					jnz	donde_rom3_2

					test	puerto_8189,8
					jnz	donde_rom3_1

					mov	ax,word ptr puerto_32765
					and	ax,0000010000010000b
					cmp	ax,0000010000010000b
					jz		donde_rom3_2

donde_rom3_1:

					mov	ax,segmento_ROM
					add	ax,3072 ;49152/16
					ret

donde_rom3_2:

					mov	ax,seg_spectrum
					ret


