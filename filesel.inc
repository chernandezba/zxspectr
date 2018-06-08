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


;Rutina de seleccion de ficheros
;Entrada: BX=Offset cadena de ficheros a buscar. Si hay mas de uno,
;se puede separar con el codigo 0. El ultimo acaba con dos ceros.
;Ej: db "*.zx",0,"*.sp",0,0
;		    DX=Offset cadena a guardar fichero seleccionado, con la ruta
;de acceso completa.
;
;Salida: En DX, fichero seleccionado. Si se pulsa ESC, Carry a 1

FICHEROS_FILESEL equ 18 ;17 ;Numero de ficheros visibles cada vez
ANCHO_FILESEL	  equ 8+1+3+2+3+1 ;Ancho de la ventana visible
LINEA_FILESEL	  equ	3 ;Linea donde se empieza a escribir la lista
COLUMNA_FILESEL  equ 14-ANCHO_FILESEL/2
UNIDADES_FILESEL equ 26 ;Unidades maximas posibles

;seg_buffer_carga se usa al cargar datos desde cinta real de Spectrum y
;tambien se usa aqui para almacenar la lista de ficheros
;Se dispone de 64k
;La estructura que presenta es:
;Byte 0: Tipo de entrada
; 0=No hay mas ficheros
; 1=Unidad
; 2=Directorio
; 3=Fichero
;Byte 1..15 (8+1+3+2+1 bytes)
; Texto que se vera en pantalla, acabado en codigo 255
; (si es un directorio, debe estar escrito como
; <Director.ext>
; si es una unidad, debe estar escrito como
; [A:]
;Total cada entrada: 16 bytes

;Como minimo, siempre habra una entrada, la que indica la unidad actual

posicion_inicio_lista dw ? ;Offset donde se encuentra la primera linea
;de seleccion de fichero visible en pantalla
posicion_lista			db	? ;Linea relativa al inicio donde se encuentra
;el cursor de seleccion de fichero


fichero_seleccionado	dw ? ;Aqui se guarda el registro DX en entrar
ficheros_buscar		dw	? ;Aqui se guarda el registro BX en entrar

unidad_actual	db ? ;Se guarda el numero de la unidad actual
directorio_actual db 64 dup (?) ;Se guarda el directorio actual
directorio_barra db "\",0
cadena_porpuntopor db "*.*",0

filesel_buffer_sort	db 16 dup (?) ; Buffer para ordenacion de entradas

filesel_flags			db	0
COMMENT !
Bit
---
7-2 Reservado
1	 A 1 si se ha llegado al maximo de archivos en un directorio
0	 A 1 si la unidad B se puede seleccionar

!

COMMENT ! El DTA se situa a partir de la 80h
filesel_DTA_reservado 	db 21 dup (?)
filesel_DTA_atributo  	db ?
filesel_DTA_hora		 	dw ?
filesel_DTA_fecha   		dw	?
filesel_DTA_tamanyo		dd	?
filesel_DTA_nombre		db 13 dup (?)
!

unidades_posibles db UNIDADES_FILESEL+1 dup (?) ;Letras de las unidades
;posibles, acabadas en codigo 0

filesel:			
			       mov	fichero_seleccionado,dx
					mov	ficheros_buscar,bx

					;Obtener unidad y directorio actual
					call	restaurar_vectores
					mov	ah,19h
					int	21h
					mov	unidad_actual,al

					mov   ah,47h
					mov 	si,offset directorio_actual
					xor	dl,dl ;Unidad actual
					int	21h

					call  cambiar_vectores

					call	actualiza_y_espera
					mov	word ptr cursor_x,0
					call	print_cadena
					db		TESP,"Elija el fichero y pulse ENTER "
					db		TING,"Select the file and press ENTER"
					db		TCOM,13
					db		"-------------------------------",255

					;Mirar si se puede seleccionar la unidad B
					call	restaurar_vectores

					int	11h

					COMMENT !
					(E)AX=
					 0	floppy disk(s) installed (number specified by bits 7-6)
 1	80x87 coprocessor installed
 3-2	number of 16K banks of RAM on motherboard (PC only)
	number of 64K banks of RAM on motherboard (XT only)
 2	pointing device installed (PS)
 3	unused (PS)
 5-4	initial video mode
	00 EGA, VGA, or PGA
	01 40x25 color
	10 80x25 color
	11 80x25 monochrome
 7-6	number of floppies installed less 1 (if bit 0 set)
 8	DMA support installed (PCjr, Tandy 1400LT)
	DMA support *not* installed (Tandy 1000's)
 11-9	number of serial ports installed

 12	game port installed
 13	serial printer attached (PCjr)
	internal modem installed (PC/Convertible)
 15-14	number of parallel ports installed
---Compaq, Dell, and many other 386/486 machines--
 23	page tables set so that Weitek coprocessor addressable in real mode
 24	Weitek math coprocessor present
---Compaq Systempro---
 25	internal DMA parallel port available
 26	IRQ for internal DMA parallel port (if bit 25 set)
	0 = IRQ5
	1 = IRQ7
 28-27	parallel port DMA channel
	00 DMA channel 0
	01 DMA channel 0 ???
	10 reserved
	11 DMA channel 3


Copied from Ralf Brown's Interrupt List
!
					test	al,1
					jz    filesel_no_B
					;Hay unidades flexibles, veamos cuantas
					and   al,11000000b
					cmp   al,01000000b
					jc    filesel_no_B
					or		filesel_flags,1
					jmp	short filesel_no_B2

filesel_no_B:
					and	filesel_flags,11111110b
filesel_no_B2:

					;Empecemos a seleccionar cada unidad, y luego hay que
					;ver que la unidad seleccionada sea la actual, con
					;lo cual sabremos que la unidad es valida

					mov	bx,offset unidades_posibles
					xor	dl,dl
filesel_unidades:
					mov	ah,0eh ;Cambiar unidad
					int	21h

					mov	dh,dl
					mov	ah,19h ;Obtener unidad
					int	21h
					cmp	dh,al
					jnz	filesel_unidades_no
					;Ver si es la unidad B y se puede seleccionar
					cmp	dh,1
					jnz	filesel_unidades2
					test	filesel_flags,1
					jz    filesel_unidades_no
filesel_unidades2:
					add	al,"A"
					mov	byte ptr [bx],al
					inc	bx
					;No se restaura DL, pues el DH vale lo mismo

filesel_unidades_no:
					mov	dl,dh
					inc	dl
					cmp	dl,UNIDADES_FILESEL
					jnz	filesel_unidades

					mov	byte ptr [bx],0

					;Poner la unidad actual
					mov	ah,0eh
					mov	dl,unidad_actual
					int	21h


					call	cambiar_vectores

filesel_inicio0:

					call  liberar_teclas
					mov	posicion_inicio_lista,0
					mov	posicion_lista,0

filesel_inicio:
					;Aqui hay que buscar:
					;Los directorios, los ficheros y las unidades

					mov	es,seg_buffer_carga

					xor	bx,bx ;Apunta a la siguiente entrada de lista a
					;escribir
					and filesel_flags,11111101b

					;Leer directorios
					mov	dx,offset cadena_porpuntopor
					mov   cx,16

					call	filesel_findfirst
					jc		filesel_inicio1
					jmp	short filesel_inicio_directorio_si


filesel_inicio_directorio:
					mov	cx,16
					call	filesel_findnext
					jc		filesel_inicio1

filesel_inicio_directorio_si:

					mov	byte ptr es:[bx],2 ;Decir que es directorio
					mov	byte ptr es:[bx+1],"<"

					mov	cx,12
					push	bx
					add	bx,2
					mov	ax,80h+30
filesel_inicio_directorio_si_2:
					xchg	bx,ax
					mov	dl,byte ptr [bx]
					xchg	bx,ax
					cmp	dl,0
					jz    filesel_inicio_directorio_si_3
					mov	byte ptr es:[bx],dl
					inc	bx
					inc	ax
					loop	filesel_inicio_directorio_si_2

filesel_inicio_directorio_si_3:

					mov	byte ptr es:[bx],">"
					mov	byte ptr es:[bx+1],255

					pop	bx

					add	bx,16
					jmp	short filesel_inicio_directorio


filesel_inicio1:


					;Buscar los ficheros
					mov	dx,ficheros_buscar

filesel_ficheros:
					xchg	bx,dx
					cmp	byte ptr [bx],0
					xchg	bx,dx
					jz    filesel_inicio20

					;Leer ficheros
					mov   cx,0

					call	filesel_findfirst
					jc		filesel_ficheros_siguiente
					jmp	short filesel_ficheros_si

filesel_ficheros_buc:
					mov	cx,0
					call	filesel_findnext
					jc		filesel_ficheros_siguiente

filesel_ficheros_si:

					mov	byte ptr es:[bx],3 ;Decir que es fichero

					push	bx
					inc	bx
					mov	ax,80h+30
filesel_ficheros_si_2:
					xchg	bx,ax
					mov	cl,byte ptr [bx]
					xchg	bx,ax
					cmp	cl,0
					jz    filesel_ficheros_si_3
					mov	byte ptr es:[bx],cl
					inc	bx
					inc	ax
					loop	filesel_ficheros_si_2

filesel_ficheros_si_3:

					mov	byte ptr es:[bx],255

					pop	bx

					add	bx,16

;se ha llenado el buffer entero?
					cmp	bx,65534-16
					jc 	filesel_ficheros_buc		

					or		filesel_flags,00000010b
					;al llenarse el buffer no se muestran en el listado las unidades_posibles, no queda mas espacio!

					jmp	short filesel_inicio3
	
;filesel_ficheros_si_4:
;					jmp	short filesel_ficheros_buc

filesel_ficheros_siguiente:
					xchg	bx,dx
					cmp	byte ptr [bx],0
					xchg	bx,dx
					jz    filesel_ficheros_siguiente2
					inc	dx
					jmp	short filesel_ficheros_siguiente

filesel_ficheros_siguiente2:
					inc 	dx
					xchg	bx,dx
					cmp	byte ptr [bx],0
					xchg  bx,dx
					jnz   filesel_ficheros


filesel_inicio20:

					;Leer las unidades posibles de unidades_posibles
					mov	dx,offset unidades_posibles

filesel_inicio2:
					xchg	dx,bx
					mov	al,byte ptr [bx]
					xchg	dx,bx
					or		al,al
					jz		filesel_inicio3
					mov	byte ptr es:[bx],1 ;Indicar que es una unidad
					mov	byte ptr es:[bx+1],"["
					mov	byte ptr es:[bx+2],al
					mov	byte ptr es:[bx+3],":"
					mov	byte ptr es:[bx+4],"]"
					mov	byte ptr es:[bx+5],255
					add	bx,16
					inc	dx
					jmp	short filesel_inicio2


filesel_inicio3:

					mov	byte ptr es:[bx],0 ;Final de lista


					call	filesel_sort

filesel_inicio_mostrar:


					call	filesel_mostrar

filesel_inicio_mostrar_cursor:

					call	filesel_pon_posicion

					mov	es,seg_spectrum
					call	tecla_menu_print_cursor


filesel_tecla:

					mov   es,seg_spectrum
					call	espera_no_tecla
					call	espera_tecla

					;Mirar si tecla ESC
					test 	puerto_especial3,16
					jnz	filesel_tecla_noESC
					;Volver
					jmp	filesel_fin_esc

filesel_tecla_noESC:
					;Mirar si tecla arriba
					test	puerto_221,8
					jz	filesel_tecla_arr

                                        test    puerto_61438,8   ; 6    7    8    9    0     ;4
                                        jnz     filesel_tecla_noarr
filesel_tecla_arr:

					call  filesel_mueve_arriba
					jmp	short filesel_tecla_aba1

filesel_tecla_noarr:
					;Mirar si tecla abajo
					test	puerto_221,4
					jz	filesel_tecla_aba

                                        test    puerto_61438,16   ; 6    7    8    9    0     ;4
                                        jnz     filesel_tecla_noaba

filesel_tecla_aba:

					call  filesel_mueve_abajo
filesel_tecla_aba1:
;Punto de entrada comun tambien para filesel_tecla_arr
					jnz	filesel_tecla_aba2
					jmp	filesel_tecla
filesel_tecla_aba2:
					jnc   filesel_tecla_aba3
					jmp	filesel_inicio_mostrar


filesel_tecla_aba3:
					jmp	filesel_inicio_mostrar_cursor

filesel_tecla_noaba:
					;Mirar si tecla enter
					test	puerto_49150,1
					jz		filesel_tecla_enter
					jmp   filesel_tecla_noenter

filesel_tecla_enter:
					;Mirar que se selecciona (si es unidad,directorio o fichero)
					mov	al,posicion_lista
					xor	ah,ah
					mov	bl,16
					mul	bl
					add	ax,posicion_inicio_lista
					mov	bx,ax
					mov	es,seg_buffer_carga
					mov	al,byte ptr es:[bx]
; 0=No hay mas ficheros
; 1=Unidad
; 2=Directorio
; 3=Fichero
					cmp	al,1
					jz		filesel_tecla_enter_unidad
					cmp	al,2
					jz		filesel_tecla_enter_directorio
					;Se selecciona un fichero

					;Obtener unidad y directorio actual
					push	bx
					call	restaurar_vectores
					pop	bx

					inc	bx ;Posicionarse en el nombre del fichero
					mov	ah,19h
					int	21h
					mov	dx,fichero_seleccionado
					add	al,"A"
					xchg  bx,dx
					mov	byte ptr [bx],al
					mov	byte ptr [bx+1],":"
					mov	byte ptr [bx+2],"\"
					add	bx,3

					mov   ah,47h
					mov 	si,bx
					push	dx
					xor	dl,dl ;Unidad actual
					int	21h
					pop	dx

					mov	al,byte ptr [bx]
					or		al,al
					jz    filesel_tecla_enter_fic3 ;Si el directorio es el
					;principal, no poner \

					;Buscar codigo 0

filesel_tecla_enter_fic:
					mov	al,byte ptr [bx]
					or		al,al
					jz    filesel_tecla_enter_fic2
					inc	bx
					jmp	short filesel_tecla_enter_fic

				

filesel_tecla_enter_fic2:
					mov	byte ptr [bx],"\"
					inc 	bx

					;Poner nombre del fichero

filesel_tecla_enter_fic3:
					xchg  bx,dx
					mov	al,byte ptr es:[bx]
					xchg  bx,dx
					mov	byte ptr [bx],al
					cmp	al,255
					jz    filesel_tecla_enter_fic4

					inc   dx
					inc	bx
					jmp	short filesel_tecla_enter_fic3


filesel_tecla_enter_fic4:
					mov	byte ptr [bx],0
					jmp   filesel_fin

filesel_tecla_enter_unidad:
					push	bx
					call	restaurar_vectores
					pop	bx
					mov	dl,byte ptr es:[bx+2]
					sub	dl,"A"
					mov	ah,0eh
					int	21h
					call	cambiar_vectores
					jmp	filesel_inicio0

filesel_tecla_enter_directorio:
					inc	bx
					inc	bx
					push	bx
					;Buscar caracter >
filesel_tecla_enter_directorio_buc:
					cmp	byte ptr es:[bx],">"
					jz    filesel_tecla_enter_directorio2
					inc	bx
					jmp	short filesel_tecla_enter_directorio_buc

filesel_tecla_enter_directorio2:
					mov 	byte ptr es:[bx],0 ;Como se va a cambiar el
					;directorio, podemos tocar el texto y asi hacer cd
					;leyendo el directorio del mismo texto
					call	restaurar_vectores

					pop	dx
					mov	ds,seg_buffer_carga
					mov	ah,3bh
					int	21h
					push	cs
					pop	ds

					call	cambiar_vectores
					jmp	filesel_inicio0


filesel_tecla_noenter:
					;Mirar si tecla Pg Dn

					test	puerto_especial7,1
					jnz   filesel_tecla_nopgdn

					mov	cx,FICHEROS_FILESEL*2-1
					mov	posicion_lista,0
filesel_tecla_pgdn2:
					push	cx
					call	filesel_mueve_abajo
					pop	cx
					jz		filesel_tecla_pgdn3
					loop  filesel_tecla_pgdn2
					mov     posicion_lista,0	
					jmp   filesel_inicio_mostrar

filesel_tecla_pgdn3:

					jmp   filesel_inicio_mostrar

filesel_tecla_nopgdn:
					;Mirar si tecla Pg Up

					test	puerto_especial6,1
					jnz   filesel_tecla_nopgup

					mov	cx,FICHEROS_FILESEL
					mov	posicion_lista,0
filesel_tecla_pgup2:
					push	cx
					call	filesel_mueve_arriba
					pop	cx
					jz		filesel_tecla_pgup3
					loop  filesel_tecla_pgup2

filesel_tecla_pgup3:

					mov	posicion_lista,0
					jmp   filesel_inicio_mostrar

filesel_tecla_nopgup:

					jmp   filesel_tecla


filesel_mueve_abajo:
;Rutina que mueve el cursor 1 posicion hacia abajo (si se puede)
;Salida: Z si no se puede bajar mas (no hace nada)
;        NZ y C si se ha de mostrar la lista (saltar a filesel_inicio_mostrar)
;			NZ y NC si solo se ha de mover el cursor (saltar a
;			  filesel_inicio_mostrar_cursor)

					mov	al,posicion_lista
					xor	ah,ah
					mov	bl,16
					mul	bl
					add	ax,posicion_inicio_lista
					mov	bx,ax
					mov	es,seg_buffer_carga
					cmp	byte ptr es:[bx+16],0
					jnz 	filesel_mueve_abajo1
					ret

filesel_mueve_abajo1:

					;Ver si el cursor esta abajo del todo
					cmp	posicion_lista,FICHEROS_FILESEL-1
					jnz	filesel_mueve_abajo2
					add	posicion_inicio_lista,16
					;Se activa NZ
					;Poner C
					stc
					;jmp	filesel_inicio_mostrar
					ret

filesel_mueve_abajo2:

					call	filesel_borra_cursor
					inc	posicion_lista
					;Se activa NZ
					;Poner NC
					clc
					ret
					;jmp	filesel_inicio_mostrar_cursor

filesel_mueve_arriba:
;Rutina que mueve el cursor 1 posicion hacia arriba (si se puede)
;Salida: Z si no se puede subir mas (no hace nada)
;        NZ y C si se ha de mostrar la lista (saltar a filesel_inicio_mostrar)
;			NZ y NC si solo se ha de mover el cursor (saltar a
;			  filesel_inicio_mostrar_cursor)

					;Ver si el cursor esta arriba del todo
					cmp   posicion_lista,0
					jnz	filesel_mueve_arriba2
					;Esta arriba del todo. Ver si se puede retroceder mas
					cmp	posicion_inicio_lista,0
					jnz   filesel_mueve_arriba1
					ret

filesel_mueve_arriba1:
					;No esta arriba del todo. Retroceder
					sub	posicion_inicio_lista,16
					or		al,1 ;NZ
					stc
					ret
					;jmp   filesel_inicio_mostrar

filesel_mueve_arriba2:
					call	filesel_borra_cursor
					dec	posicion_lista
					or		al,1 ;NZ
					clc
					ret
					;jmp	filesel_inicio_mostrar_cursor



filesel_fin:	;Poner la unidad y el directorio con el que se entro
					clc ;Carry=0
					jmp	short filesel_fin2

filesel_fin_esc:
					stc
filesel_fin2:
					pushf
					call	restaurar_vectores

					mov	ah,0eh
					mov	dl,unidad_actual
					int	21h

					;Ahora hay que poner el directorio. Como la funcion
					;que se usa para obtenerlo, no pone la barra \ al principio
					;entonces haremos primero un "cd \"

					mov	dx,offset directorio_barra
					mov	ah,3bh
					int	21h

					mov	dx,offset directorio_actual
					mov	ah,3bh
					int	21h

					call	cambiar_vectores
					call  liberar_teclas
					popf
					ret


filesel_dibuja_ventana:
;Rutina que crea una cuadrado en la pantallla, situado en la posicion
;actual del cursor
;Entrada: CL=Ancho, CH=Alto
					mov	al,cursor_x
					push	ax
					push	cx
filesel_dibuja_ventana_buc:

					push	cx
					mov	al,32
					call	print_caracter
					pop	cx
					dec	cl
					jnz	filesel_dibuja_ventana_buc

					pop	cx
					pop	ax
					dec	ch
					jz		filesel_dibuja_ventana_fin
					inc	cursor_y
					mov	cursor_x,al
					jmp	short filesel_dibuja_ventana

filesel_dibuja_ventana_fin:
					ret

filesel_mostrar:
;Muestra el cuadro de seleccion de fichero. Si se ha llegado al maximo de archivos por directorio, se avisa
					mov	cursor_x,0
					mov	cursor_y,LINEA_FILESEL-1

					test	filesel_flags,00000010b
					jz		filesel_mostrar0

					
					call	print_cadena
					db		TESP,"Muchos archivos en directorio! "
					db		TING,"Too many files in directory!   "
					db		255	
					jmp	short filesel_mostrar00

filesel_mostrar0:
					call	print_cadena
					db		"                               "
					db		255	

					
filesel_mostrar00:
					mov	cursor_x,COLUMNA_FILESEL
					mov	cursor_y,LINEA_FILESEL
					mov   cl,ANCHO_FILESEL
					mov	ch,FICHEROS_FILESEL
					call	filesel_dibuja_ventana

					mov	cursor_x,COLUMNA_FILESEL
					mov	cursor_y,LINEA_FILESEL

					mov	bx,posicion_inicio_lista
					mov	es,seg_buffer_carga
					mov	cx,FICHEROS_FILESEL

filesel_mostrar_buc:

					mov	al,byte ptr es:[bx]
					or		al,al
					jz    filesel_mostrar_fin

					push	bx
					inc	bx
					mov	cursor_x,COLUMNA_FILESEL+3
filesel_mostrar_buc2:
					mov	al,byte ptr es:[bx]
					cmp	al,255
					jz		filesel_mostrar_siguiente
					push	bx
					push	cx
					push	es
					mov	es,seg_spectrum
					call	print_caracter
					pop	es
					pop	cx
					pop	bx
					inc	bx
					jmp	short filesel_mostrar_buc2

filesel_mostrar_siguiente:
					mov	cursor_x,COLUMNA_FILESEL
					inc	cursor_y

					pop	bx
					add	bx,16
					loop	filesel_mostrar_buc


filesel_mostrar_fin:

					ret

filesel_pon_posicion:
					mov	cursor_x,COLUMNA_FILESEL
					mov	al,posicion_lista
					add	al,LINEA_FILESEL
					mov	cursor_y,al

					ret

filesel_findfirst:
;Entrada: Igual que la funcion del DOS, solo que esta funcion
;si que mira los atributos
					mov	ah,4eh
					push	cx
					;xor	cx,cx
					int	21h
					pop	cx

					jnc	filesel_findfirst2
					ret

filesel_findfirst2:
					mov	al,byte ptr cs:[80h+21] ;Atributo
					and	al,cl
					cmp	al,cl
					jnz	filesel_findnext

					ret


filesel_findnext:
;Entrada: Igual que la funcion del DOS, solo que esta funcion
;si que mira los atributos, mas CX=Atributo a buscar

					mov	ah,4fh
					push	cx
					
					int	21h
					pop	cx

					jnc	filesel_findnext2
					ret

filesel_findnext2:
					mov	al,byte ptr cs:[80h+21] ;Atributo
					and	al,cl
					cmp	al,cl
					jnz	filesel_findnext
					ret

filesel_borra_cursor:
					call	filesel_pon_posicion

					mov	es,seg_spectrum
					call	print_cadena
					db		"   ",255
					ret






;Rutina que compara dos cadenas de texto acabadas en codigo 255
;Entrada: bx, dx Cadenas, y ES como segmento de datos
;Salida: NC si BX es mayor
;			C si BX es menor
;Primer byte:
; 0=No hay mas ficheros
; 1=Unidad
; 2=Directorio
; 3=Fichero

;comparar el primer byte, tambien, conservar orden
					

filesel_strcmp:
					mov	al,byte ptr es:[bx]
					xchg	dx,bx
					cmp	al,byte ptr es:[bx]
					xchg	dx,bx
					jnz	filesel_strcmp_fin

					inc	bx
					inc	dx
filesel_strcmp0:
					mov	al,byte ptr es:[bx]
					cmp 	al,255
					;fin de cadena. es menor
					jz		filesel_strcmp_menor
					xchg	dx,bx
					mov	cl,byte ptr es:[bx]
					xchg	dx,bx
					cmp	cl,255
					;fin de cadena. es mayor
					jz		filesel_strcmp_mayor

					cmp	al,cl
					jz		filesel_strcmp_sigue
filesel_strcmp_fin:
					ret
filesel_strcmp_sigue:
					inc 	bx
					inc 	dx
					jmp	short filesel_strcmp0
filesel_strcmp_mayor:
					;Ponemos NC
					or		al,al
					ret
filesel_strcmp_menor:
					;Ponemos C
					stc
					ret


filesel_strcpy_from_es:
;Rutina que copia la cadena BX en DX. Acabada en codigo 255. Con ES source
					mov	al,byte ptr es:[bx]
					cmp	al,255
					jz		filesel_strcpy_from_es_fin
					xchg	dx,bx
					mov 	byte ptr [bx],al
					xchg	dx,bx
					inc	bx
					inc 	dx
					jmp	short filesel_strcpy_from_es
filesel_strcpy_from_es_fin:
					xchg	dx,bx
					mov	byte ptr [bx],al
					xchg	dx,bx
					ret

filesel_strcpy_to_es:
;Rutina que copia la cadena BX en DX. Acabada en codigo 255. Con ES destino
					mov	al,byte ptr [bx]
					cmp	al,255
					jz		filesel_strcpy_to_es_fin
					xchg	dx,bx
					mov 	byte ptr es:[bx],al
					xchg	dx,bx
					inc	bx
					inc 	dx
					jmp	short filesel_strcpy_to_es
filesel_strcpy_to_es_fin:
					xchg	dx,bx
					mov	byte ptr es:[bx],al
					xchg	dx,bx
					ret

filesel_strcpy_es:
;Rutina que copia la cadena BX en DX. Acabada en codigo 255. Con ES origen y destino
					mov	al,byte ptr es:[bx]
					cmp	al,255
					jz		filesel_strcpy_es_fin
					xchg	dx,bx
					mov 	byte ptr es:[bx],al
					xchg	dx,bx
					inc	bx
					inc 	dx
					jmp	short filesel_strcpy_es
filesel_strcpy_es_fin:
					xchg	dx,bx
					mov	byte ptr es:[bx],al
					xchg	dx,bx
					ret


filesel_sort:
;Ordena las entradas del listado. primero con todos. segundo con todos, etc.

					xor	bx,bx
					mov	es,seg_buffer_carga


filesel_sort1:
					mov	al,byte ptr es:[bx]
					or 	al,al
					jz		filesel_sort_fin  ;fin de origen
					
					mov 	dx,bx
					add	dx,16

filesel_sort2:

					xchg	dx,bx
					mov	al,byte ptr es:[bx]
					or 	al,al
					xchg	dx,bx
					jz		filesel_sort_prox_segmento ;fin de destino. siguiente bloque

			
					;comparamos las dos entradas
					push	bx
					push	dx

					call	filesel_strcmp
;Rutina que compara dos cadenas de texto acabadas en codigo 255
;Entrada: bx, dx Cadenas, y ES
;Salida: NC si BX es mayor
;			C si BX es menor

					pop	dx
					pop	bx
					jc		filesel_sort_no_mayor

					;intercambiar

					;copiar destino en buffer temporal

					push	bx
					push	dx
					mov	bx,dx
					mov	dx,offset filesel_buffer_sort
					call	filesel_strcpy_from_es
					pop	dx
					pop 	bx

					;copiar origen en destino
					push	bx
					push	dx
					call	filesel_strcpy_es
					pop	dx
					pop	bx
		
					;copiar buffer temporal en origen
					
					xchg	dx,bx
					push	dx
					push	bx

					mov	bx,offset filesel_buffer_sort
					call	filesel_strcpy_to_es
					pop	bx
					pop	dx
					xchg	dx,bx	


filesel_sort_no_mayor:					
					;incrementamos destino
					add 	dx,16
					jmp	short filesel_sort2

filesel_sort_prox_segmento:
					;incrementamos origen
					add	bx,16
					jmp	short filesel_sort1


filesel_sort_fin:
					ret
