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


;Este fichero contiene las rutinas de control de la pantalla VGA

;Para la actualizacion de la pantalla con efecto rainbowing se utilizara
;el siguiente buffer: (buffer_rainbow)

;4 bytes : valores del borde en las 4 lineas superiores

;65 bytes = 1 byte de borde + 32 bytes de pixeles + 32 bytes de color
;Esto para todas las 192 lineas

;4 bytes: valores del borde en las 4 lineas superiores

;Nota: Dado que las franjas de borde laterales solo tienes 32 pixeles de ancho,
;cada una no puede tener mas de un color. Lo mismo para las superiores y
;inferiores

;El buffer total es de:
;4+65*192+4=12488

PUBLIC vga_poke_byte


vga_presente	db		? ;Contiene 1 si se encuentra tarjeta VGA,
							  ;si es 0=CGA/Hercules. Ya no se usa

seg_vga			dw		0a000h  ;es una constante
seg_buffer_rainbow  dw ? ;Buffer de todas las lineas
puntero_buffer_rainbow dw 0 ;Donde esta situado el puntero del buffer rainbow


;Variables usadas en la actualizacion de pantalla mediante lineas

pantalla_origen dw	offset pantalla_tabla ;Offset a pantalla_tabla de la direccion
;de memoria de pantalla Spectrum que se va a leer

pantalla_destino dw	? ;Direccion de memoria de pantalla PC que se va a escribir

pantalla_tabla		db 192*2*2 dup (?)
;Tabla de correspondencia de cada linea de pantalla con
;su direccion en pixels y atributos del Spectrum. Cada entrada
;tiene la direccion en pixels y luego atributos.
;Ejemplo: 16384,22528,16640,22528, etc

write_video_nombre            db              80 dup (0)


_poke_byte_rom00:
					;Mirar si esta en modo Inves
					test	bits_estado5,00100000b
					jz    _poke_byte_rom0
					;Ver si se pokea una direccion del tipo XXFEH
					cmp   bl,0feh
					jnz   _poke_byte_rom0
					mov	valor_poke_rom,al ;Guardar valor del poke

_poke_byte_rom0:
					;Mirar si esta paginacion de RAM en ROM
					test	puerto_8189,1
					jnz 	vga_poke_byte000

_poke_byte_rom:
					ret

vga_poke_byte:	;Hace un poke de byte comprobando si se est� en ROM
					;o en pantalla
					;Entrada: BX=Direcci�n
					;			 AL=Byte
					;Registros modificados:
					;AX,BX,CX,DX,BP,DI
COMMENT!
Esta rutina ha de ser la m�s r�pida de todo el emulador, por lo que se
eliminan las sentencias de bucles repetitivos.
!
					cmp	bh,64 ;Comprobar si se pokea en ROM
					jc		_poke_byte_rom00 ;Se hace as� porque es m�s probable
					;que no se pokee en ROM. Por lo tanto, al no pokear
					;en ROM es m�s r�pido

vga_poke_byte000:

					cmp	byte ptr es:[bx],al
					jz		_poke_byte_rom

vga_poke_byte00:

					poke128k
					;Devuelve C si debe actualizar la pantalla
					;NC si no debe

vga_poke_byte0:

					ret

actualiza_pantalla: ;Redibujar la pantalla. Comun a VGA y CGA

					push	es
					;Poner el buffer de multitarea a 0
					mov	es,seg_pantalla_multitarea
					xor	bx,bx
					mov	cx,768/2

actualiza_pantalla0_buc:

					mov	word ptr es:[bx],0
					inc	bx
					inc	bx
					loop  actualiza_pantalla0_buc

					pop	es

actualiza_pantalla00:

					;Guardar valores de pantalla_origen, pantalla_destino
					mov	ax,pantalla_origen
					push	ax
					mov	ax,pantalla_destino
					push	ax

					mov	pantalla_origen,offset pantalla_tabla
					mov	pantalla_destino,320*4

					mov	cx,192

actualiza_pantalla1:
					push	cx

					call	vga_actualiza_linea20

					pop	cx

					loop	actualiza_pantalla1

					pop	ax
					mov	pantalla_destino,ax

					pop	ax
					mov	pantalla_origen,ax

					ret


vga_inicializa_pantalla:

					;Poner modo VGA
					mov   ax,0013h    ;0013h ;320*200*256 colores
					int	10h
					ret

vga_apaga_pantalla:

COMMENT !

Informacion extraida de PCGPE


------------------------------------------------------------
Port-Index: -                Port: 03bah,03dah ; read only
usage:      d3  Vertical retrace
				d0  Horizontal retrace
!
					;Apagar pantalla
					mov	dx,03dah
apaga_pantalla0:
					in		al,dx
					test	al,00001000b
					jnz 	apaga_pantalla0
apaga_pantalla00:
					in		al,dx
					test	al,00001000b
					jz 	apaga_pantalla00
					mov	al,control_brillo
					or		al,al
					jns 	apaga_pantalla2
					cmp	al,-57
					jc		apaga_pantalla3
apaga_pantalla2:
					dec	al
					mov	control_brillo,al
					;Esperar una interrupcion 8 (y cada 50 Hz)
					and	bits_estado4,10111111b ;Eliminar int. pendiente

					call	vga_actualiza_brillo
apaga_pantalla2_esp:
					test	bits_estado4,01000000b
					jz		apaga_pantalla2_esp

					jmp	short vga_apaga_pantalla

apaga_pantalla3:
					ret




vga_copia_paleta:	;copia la paleta de brillo 0 y 1 a buffer_paleta
					mov		si,brillo0
					mov		di,offset buffer_paleta
					mov		cx,16*3
copia_paleta0:	lodsb
					mov		byte ptr [di],al
					inc		di
					loop		copia_paleta0
					ret

vga_inicializa_paleta:

					xor 		al,al
					mov		dx,3c8h
					out		dx,al
					inc		dx

					mov		cx,8
					mov		si,brillo0
					cld
ini_paleta1:	lodsb
					and		al,parametro_vgaR
					out		dx,al ;Red
					lodsb
					and		al,parametro_vgaG
					out		dx,al ;Green
					lodsb
					and		al,parametro_vgaB
					out		dx,al ;Blue
					loop		ini_paleta1

					mov		al,64
					mov		dx,3c8h
					out		dx,al
					inc		dx

					mov		cx,8
					mov		si,brillo1
ini_paleta2:	lodsb
					and		al,parametro_vgaR
					out		dx,al ;Red
					lodsb
					and		al,parametro_vgaG
					out		dx,al ;Green
					lodsb
					and		al,parametro_vgaB
					out		dx,al ;Blue
					loop		ini_paleta2

;Poner color de color_tinta y color_papel

					mov		al,9
					mov		dx,3c8h
					out		dx,al
					inc		dx

					mov		si,offset tinta_menu
					lodsb
					out		dx,al
					lodsb
					out		dx,al
					lodsb
					out		dx,al

					lodsb
					out		dx,al
					lodsb
					out		dx,al
					lodsb
					out		dx,al

					ret

vga_pon_color_borde0:
;Usado por load

					cli
					mov		al,8
					mov		dx,3c8h
					out		dx,al
					inc		dx
					mov		al,border
					mov		cl,3
					mul		cl
					xor		ah,ah
					mov		si,brillo0
					add		si,ax
					lodsb
					and		al,parametro_vgaR
					out		dx,al
					lodsb
					and		al,parametro_vgaG
					out		dx,al
					lodsb
					and		al,parametro_vgaB
					out		dx,al
					sti
					ret


vga_pon_marca_borde0:
;Usado por load

					push		es                 ;   -----------  <-
					mov		es,seg_vga         ;   --       --
					cld                         ;   --       --
					mov		ax,0808h           ;   -----------

					mov      di,320*4
					mov		bx,192             ;   -----------
pon_marca1:    mov		cx,32/2            ;   -- <- -> --
					rep		stosw              ;   --       --
					add      di,256             ;   --       --
					mov		cx,32/2 				 ;   -----------
					rep		stosw
					dec		bx
					jnz		pon_marca1
														 ;   -----------
					pop      es                 ;   --       --
					ret								 ;   --       --
														 ;   --       --
														 ;   -----------  <-

print_char_inicio:

;Buffer multitarea:
;768 posiciones

;Rutina comun a la print char de CGA y VGA para que indiquen al buffer
;de multitarea que las 9 direcciones de pantalla de Spectrum de 1 caracter
;estan ocupadas.

					test		bits_estado4,10000000b
					jz			print_char_inicio_si
					ret

print_char_inicio_si:
					;Convertir linea,columna a formato direcci�n de pixels
					;de Spectrum (16384-22527):
					;     byte alto        byte bajo
					;bit  7 6 5 4 3 2 1 0  7 6 5 4 3 2 1 0
					;     0 1 0 L L S S S  L L L C C C C C
					
					;push		bx
					
					;;
					;;ret
					;;
					
					push		ax

					mov		al,cursor_y
					;xor		ah,ah
					mov		bl,32
					mul		bl
					mov		bl,cursor_x
					xor		bh,bh
					add		ax,bx
					mov		bx,ax
					
					push		es
					mov		es,seg_pantalla_multitarea
					mov		byte ptr es:[bx],1
					pop		es
					
					pop		ax
					;pop		bx
					
					ret
					
					
COMMENT !					
					mov		bl,cursor_y
					mov		bh,bl
					ror		bl,1
					ror		bl,1
					ror		bl,1
					and		bl,11100000b
					or			bl,cursor_x
					and		bh,00011000b
					push		es
					mov		es,seg_pantalla_multitarea
					mov		cx,8

;print_char_inicio_bucle:

					mov		byte ptr es:[bx],1
					inc		bh
					;loop     print_char_inicio_bucle

					mov		bl,cursor_y
					xor		bh,bh
					shl		bx,1
					shl		bx,1
					shl		bx,1
					shl		bx,1
					shl		bx,1
					or			bl,cursor_x
					add		bx,6144
					mov		byte ptr es:[bx],1

					pop		es
					ret
					
!

poke_pantalla_inicio:
;Rutina comun a la poke_byte_pantalla de VGA para que miren si
;pueden pokear o no (si parte del menu esta encima de ese byte)
;Devuelve Z si pueden escribir
;Entrada: BX=Destino en pantalla VGA


					;Reservar BX,AL,ES
					
					;;
					;;ret
					;;
					
					push		es
					push		dx
					push     bx
					push		ax


					sub		bx,320*4+32
					
					mov		ax,bx
					xor		dx,dx
					mov		bx,320
					
					div		bx
					
					;Obtener linea y columna
					;Linea=AX
					;Columna=DX
					
					shr		ax,1
					shr		ax,1
					shr		ax,1
					
					mov		bx,32
					push		dx
					mul		bx
					pop		dx
					
					shr		dx,1 ;Dividir entre 8
					shr		dx,1					
					shr		dx,1	
					
					add     	ax,dx
					
					;Ya esta en el rango 0-767
					
					mov		bx,ax
							
					
					;sub		bx,16384
					mov		es,seg_pantalla_multitarea
					cmp		byte ptr es:[bx],0

					pop		ax
					pop		bx
					pop		dx
					pop		es
					ret

vga_actualiza_linea_ds dw ? ;Registro DS
vga_actualiza_linea_dx dw ? ;Registro DX
vga_actualiza_linea_cx dw ? ;Registro CX
vga_actualiza_linea_ax dw ? ;Registro AX
vga_actualiza_linea_si dw ? ;Registro SI
vga_actualiza_linea_di dw ? ;Registro DI

vga_actualiza_linea:

;Rutina para dibujar una linea en pantalla. Guardar AX
;Entrada: BX=sync_lineas_actual
					mov   bx,sync_lineas_actual
					cmp	bx,sync_borde_sup
					jnc	vga_actualiza_linea1
					ret ;Esta en el borde superior
vga_actualiza_linea1:
					cmp	bx,sync_borde_inf
					jc		vga_actualiza_linea2
					;Esta en el borde inferior
					ret

vga_actualiza_linea2:
					mov   vga_actualiza_linea_ax,ax
					mov   vga_actualiza_linea_dx,dx
					mov   vga_actualiza_linea_si,si
					mov   vga_actualiza_linea_di,di


vga_actualiza_linea200:

					add	pantalla_origen,4

					mov   di,pantalla_destino
					add   di,32+256

					jmp   vga_actualiza_linea31
vga_actualiza_linea20:
					;Dibujar borde izquierdo.

					mov	es,seg_vga         ;   --
					cld                      ;   --
					mov	di,pantalla_destino
					
					;
					add	di,32
					;
					
					;Preparar contadores
					mov	cx,32 ;bytes pantalla Spectrum

					mov   bx,pantalla_origen
					mov	si,word ptr [bx]   ;SI=direccion de los pixeles
					mov	bp,word ptr [bx+2] ;BP=direccion de los atributos
					add	bx,4
					mov	pantalla_origen,bx

					mov   vga_actualiza_linea_ds,ds

vga_actualiza_linea_buc:

					test	bits_estado4,00100000b
					jz    vga_actualiza_linea_buc0

					;Como esta el menu, mirar si se puede POKEar
					mov   bx,di
					call	poke_pantalla_inicio
					jz    vga_actualiza_linea_buc0
					jmp   vga_actualiza_linea30
vga_actualiza_linea_buc0:

					mov   vga_actualiza_linea_cx,cx
					mov	es,seg_pant
					;Leer atributo
					mov	dh,byte ptr es:[bp]
					;En DH atributo

					mov	al,byte ptr es:[si]
					;En AL byte

					mov	es,seg_vga

					;Poner color en caso de que sea 0 o 1

					;En AL byte a pokear en zona pixels
					;En DH color
					test	dh,128 ;Comprobar si tiene flash
					jz	   vga_actualiza_linea_nof

					test	bits_estado,00100000b
					jnz	vga_actualiza_linea_nof

					 ;Ahora DH contendra el color en el caso de que el bit
					 ;de pixel este a 1 (papel)
					 ;Ahora DL contendr� el color en el caso de que el bit
					 ;de pixel este a 0 (tinta)

					mov	dl,dh ;DH antes contenia el byte a pokear
					and	dl,01000111b
					mov	cl,dh
					and	cl,64 ;Conservar bit de brillo
					and	dh,00111000b
					ror	dh,1
					ror	dh,1
					ror	dh,1
					or		dh,cl
					jmp	short vga_actualiza_linea3

vga_actualiza_linea_nof:
					 ;Ahora DH contendra el color en el caso de que el bit
					 ;de pixel este a 1 (tinta)
					 ;Ahora DL contendr� el color en el caso de que el bit
					 ;de pixel este a 0 (papel)
					mov	dl,dh ;DH antes contenia el color
					and	dh,01000111b
					mov	cl,dl
					and	cl,64 ;Conservar bit de brillo
					and	dl,00111000b
					ror	dl,1
					ror	dl,1
					ror	dl,1
					or		dl,cl

vga_actualiza_linea3:
					;Registros:
					;DH contendra el color en el caso de que el bit sea 1
					;DL contendra el color en el caso de que el bit sea 0
					;AL byte
					;DI pantalla_destino
					;ES seg_vga

					mov ds,seg_vga

					test	al,10000000b
					jz	   bit_7_cero
					mov   byte ptr [di],dh
					jmp	short bit_6
bit_7_cero:    mov   byte ptr [di],dl
bit_6: 			test	al,01000000b
					jz	   bit_6_cero
					mov   byte ptr [di+1],dh
					jmp	short bit_5
bit_6_cero:    mov   byte ptr [di+1],dl
bit_5: 			test	al,00100000b
					jz	   bit_5_cero
					mov   byte ptr [di+2],dh
					jmp	short bit_4
bit_5_cero:    mov   byte ptr [di+2],dl
bit_4: 			test	al,00010000b
					jz	   bit_4_cero
					mov   byte ptr [di+3],dh
					jmp	short bit_3
bit_4_cero:    mov   byte ptr [di+3],dl
bit_3: 			test	al,00001000b
					jz	   bit_3_cero
					mov   byte ptr [di+4],dh
					jmp	short bit_2
bit_3_cero:    mov   byte ptr [di+4],dl
bit_2: 			test	al,00000100b
					jz	   bit_2_cero
					mov   byte ptr [di+5],dh
					jmp	short bit_1
bit_2_cero:    mov   byte ptr [di+5],dl
bit_1: 			test	al,00000010b
					jz	   bit_1_cero
					mov   byte ptr [di+6],dh
					jmp	short bit_0
bit_1_cero:    mov   byte ptr [di+6],dl
bit_0: 			test	al,00000001b
					jz	   bit_0_cero
					mov   byte ptr [di+7],dh
					jmp	short bit_next
bit_0_cero:    mov   byte ptr [di+7],dl
bit_next:      mov   ds,word ptr cs:[vga_actualiza_linea_ds]

					mov   cx,vga_actualiza_linea_cx
vga_actualiza_linea30:
					add   di,8
					inc	si
					inc	bp
					dec	cx
					jz    vga_actualiza_linea31
					jmp   vga_actualiza_linea_buc

vga_actualiza_linea31:

					add   di,32


vga_actualiza_linea_fin:
					mov   pantalla_destino,di

					mov	es,seg_spectrum
					mov   di,vga_actualiza_linea_di
					mov   si,vga_actualiza_linea_si
					mov   dx,vga_actualiza_linea_dx
					mov   ax,vga_actualiza_linea_ax
					ret

crea_pantalla_tabla:
;Rutina que crea la tabla de la pantalla
					mov	bx,offset pantalla_tabla
					mov	cx,24 ;Lineas
					mov	ax,16384
					mov	dx,22528
crea_pantalla_tabla1:
					push	cx
					mov	cx,8 ;Scans

crea_pantalla_tabla2:
					mov 	word ptr [bx],ax
					mov	word ptr [bx+2],dx
					add	bx,4
					dec	cx
					jz		crea_pantalla_tabla3
					add	ax,256
					jmp	short crea_pantalla_tabla2
crea_pantalla_tabla3:
					;Cambio de linea
					add	dx,32
					;Formato de una direcci�n en la zona de pantalla
					;de pixels (16384-22527):
					;     byte alto        byte bajo
					;bit  7 6 5 4 3 2 1 0  7 6 5 4 3 2 1 0
					;     0 1 0 L L S S S  L L L C C C C C
					mov	cl,3
					rol	al,cl
					and   ax,1111100000000111b
					or		al,ah
					inc	al
					mov	ah,al
					ror	al,cl
					and	ax,1111100011100000b
					pop	cx
					loop  crea_pantalla_tabla1
					ret

copia_buffer_rainbow:
;Aqui se llama cada vez que se cambia de linea
;Se debe copiar el color de borde y el contenido de la linea (pixeles y atributos)
;(si se esta en el centro) o solo el color del borde (si se esta en cualquiera
;de las 4 franjas superiores o inferiores)

;Tener en cuenta el modo inves de actualizar la pantalla:
;Interrupcion:
;	Pantalla por debajo del borde superior,
;  Borde inferior
;  Borde superior

					test	bits_estado5,00100000b
					jz		copia_buffer_rainbow_no_inves
					;Esta en modo inves
					
					;Se supone que de las 312 lineas, las 4 finales iran
					;al borde superior
					;O sea:
					;Interrupcion:
					;  192 lineas
					;  4 lineas de borde
					;  312-192-8=112 lineas sin nada
					;  4 lineas de borde
					
					;Como poner buffer ranbow para inves?
					;Hacerlo que se escructure igual, para no tener que
					;modificar la rutina de actualizacion
					
					;Esta en la zona de pixeles?
					push	ax
					mov	al,puerto_254
					push	ax
					and	al,valor_poke_rom
					mov	puerto_254,al ;enga�ar
	
					
					mov	ax,sync_lineas_actual
					cmp	ax,sync_borde_inf
					jnc	copia_buffer_rainbow_inves_nopix
					
					;Zona de pixeles

					;Saltarse los 4 bytes del borde superior
	
					add	puntero_buffer_rainbow,4
					call	copia_buffer_rainbow_centro2
					sub	puntero_buffer_rainbow,4
copia_buffer_rainbow_inves_ret:
					pop	ax
					mov	puerto_254,al
					pop	ax
					ret

copia_buffer_rainbow_inves_nopix:
					;Borde inferior?
					sub	ax,4
					cmp	ax,sync_borde_inf
					jnc	copia_buffer_rainbow_inves_inf
					
					;Borde inferior
					add	puntero_buffer_rainbow,4				
					call	copia_buffer_rainbow_sup2
					sub	puntero_buffer_rainbow,4
					jmp	copia_buffer_rainbow_inves_ret
					
					
					
copia_buffer_rainbow_inves_inf:					

					;Estara en las 4 lineas de borde superiores?
					;O sea, 308,309,310,311
					
					mov	ax,sync_lineas_actual
					add	ax,4
					cmp	ax,sync_lineas
					
					jnc	copia_buffer_rainbow_inves_sup
					jmp	copia_buffer_rainbow_inves_ret					

copia_buffer_rainbow_inves_sup:
					;Zona borde superior
					;El puntero debe estar en rango 0..3

					sub	puntero_buffer_rainbow,65*192+4			
					call	copia_buffer_rainbow_sup2
					add	puntero_buffer_rainbow,65*192+4
					jmp	copia_buffer_rainbow_inves_ret					
										
					
copia_buffer_rainbow_no_inves:
					push	ax
					mov	ax,sync_lineas_actual
					cmp	ax,sync_borde_sup
					jnc	copia_buffer_rainbow_no_sup
					
					;Aqui esta en el borde superior
					;esta en las 4 ultimas lineas del borde superior?
					add	ax,4
					cmp	ax,sync_borde_sup
					jnc	copia_buffer_rainbow_sup
					pop	ax
					
					ret	;No esta en las 4 ultimas
					
copia_buffer_rainbow_sup:
					call	copia_buffer_rainbow_sup2
					pop	ax
					ret
					
copia_buffer_rainbow_sup2:					
					mov	al,puerto_254
					
					push	es
					mov	es,seg_buffer_rainbow
					
					mov	bx,puntero_buffer_rainbow
					mov	es:[bx],al
					
					pop	es
					
					inc	bx
					mov	puntero_buffer_rainbow,bx
					
					ret
					
copia_buffer_rainbow_no_sup:
					;Esta o en el centro o en las lineas de borde
					cmp	ax,sync_borde_inf
					jc		copia_buffer_rainbow_centro
					;Esta en las lineas de borde
					;esta en las 4 primeras del borde inferior?
					sub	ax,4
					cmp	ax,sync_borde_inf
					jc		copia_buffer_rainbow_sup ;Esta en las lineas visibles
					pop	ax
					
					ret	;Esta por debajo de las 4 lineas de borde
					
copia_buffer_rainbow_centro:

					call	copia_buffer_rainbow_centro2
					pop	ax
					ret
copia_buffer_rainbow_centro2:

					push	di
					
					mov	di,puntero_buffer_rainbow
					push	es
					
					mov	es,seg_buffer_rainbow
					
					mov	al,puerto_254
					
					stosb  ;Guardar borde
					
					mov	bx,pantalla_origen
					
					push	si
					
					mov	si,word ptr [bx]
					
					inc	bx
					inc	bx
					push	bx
					
					mov	ds,seg_pant
					
					mov	cx,32
					
					repz	movsb
					
					push	cs
					pop	ds
					
					pop	bx
					
					mov	si,word ptr [bx]
					inc	bx
					inc	bx
		
					mov	pantalla_origen,bx
					
					mov	ds,seg_pant
					
					mov	cx,32
					
					repz 	movsb					
					

					
					push	cs
					pop	ds
					
					
					mov	puntero_buffer_rainbow,di
										
					pop	si
					
					pop	es
					
					pop	di
					
				
					ret																


actualiza_pantalla_rainbow:
;Redibujar toda la pantalla desde el buffer de rainbow:
;4 lineas borde sup
;192 lineas (borde izq+pixels/attr+borde der)
;4 lineas borde inf
;En modo inves, es diferente
;Los registros se guardan desde la rutina que se llama


					mov	ax,seg_pant
					push	ax
					

					mov	es,seg_vga
					mov	di,0
					
					mov	si,0
					
					mov	ds,seg_buffer_rainbow
					
			
					mov	word ptr cs:seg_pant,ds  ;Enga�amos a la rutina de actualiza_linea
					
					
;ES=SEG_VGA
;DS=SEG_BUFFER_RAINBOW
;SI=Puntero del buffer rainbow
;DI=Puntero de la pantalla
					
					
					mov	cx,4
				
actualiza_pantalla_rainbow1:
					
					lodsb
					and	al,7
					
					push	cx
					mov	cx,320
					
actualiza_pantalla_rainbow2:					
					
					stosb
					
					loop	actualiza_pantalla_rainbow2
					
					pop	cx
					
					loop 	actualiza_pantalla_rainbow1
					
					mov	cx,192
					
					
					
actualiza_pantalla_rainbow3:										

					push	cx
			
					;Ahora hacer las 192 lineas
					
					
					;32 pixels del borde izquierdo
					
					
;ES=SEG_VGA
;DS=SEG_BUFFER_RAINBOW
;SI=Puntero del buffer rainbow
;DI=Puntero de la pantalla

					push	es
					push	ds
					push	si
					push	di
					
					call	actualiza_pantalla_rainbow_lateral					
					
					
					push	cs
					pop	ds
					
					
					;Preparar contadores
					mov	cx,32 ;bytes pantalla Spectrum

					mov	bp,si ;SI=direccion de los pixeles
					add	bp,32 ;BP=direccion de los atributos
					add	bx,4
					mov	pantalla_origen,bx

					mov   vga_actualiza_linea_ds,ds
					
					call	vga_actualiza_linea_buc
					
					pop	di
					
					pop	si
					
					pop	ds
					
					pop	es
					
					;add	si,32*2
					add	di,256+32
					
					call	actualiza_pantalla_rainbow_lateral					
					
					add	si,32*2
									
					pop	cx
					
					loop	actualiza_pantalla_rainbow3
					
					;256 pixeles centrales
					
					
					;Borde inferior
					
;ES=SEG_VGA
;DS=SEG_BUFFER_RAINBOW
;SI=Puntero del buffer rainbow
;DI=Puntero de la pantalla
					
					
					mov	cx,4
				
actualiza_pantalla_rainbow4:
					
					lodsb
					and	al,7
					
					push	cx
					mov	cx,320
					
actualiza_pantalla_rainbow5:					
					
					stosb
					
					loop	actualiza_pantalla_rainbow5
					
					pop	cx
					
					loop 	actualiza_pantalla_rainbow4
					
				
					push	cs
					pop	ds
					
					pop	ax

               mov	seg_pant,ax
					
					ret
					
;Pone un borde lateral					
actualiza_pantalla_rainbow_lateral:					
					lodsb
					and	al,7
					
					mov	cx,32
					
actualiza_pantalla_rainbow_lateral2:					
					
					stosb
					
					loop	actualiza_pantalla_rainbow_lateral2
					
					
					ret





write_video_file:



COMMENT !
  Byte de estado no 7. Codificaci�n:
  bit
  ---
  7   A 1 indica que hay cinta insertada y activada para la grabacion de sonido en archivo raw
!


write_video_file0:
						push dx
						push ax
						call write_video_file_nolon0
						pop	ax
						pop	dx
						ret

write_video_file_nolon0:

						call	inicio_cinta_no_int9


						mov	dx,offset write_video_nombre
						mov	ax,3d01h ;Abrir fichero escritura
						int	21h
						jc		write_video_file_fin0

						;jmp inicio_cinta

						mov	bx,ax
						xor	cx,cx
						xor	dx,dx
						mov	ax,4202h ;Mover puntero al final
						int	21h
						jc		write_video_file_fin

						;En BX file handle
						
					;en cx, longitud

						mov	cx,320*200

						mov	ax,seg_vga
				
						
						push  ax
						pop	ds

						mov	dx,0

						mov	ah,40h
						int	21h
						jc	  	write_video_file_fin

						push	cs
						pop	ds
						jmp short write_video_file_fin

write_video_file_fin0:
;						mov temporal,al
						push	ax
						pushf
						jmp	write_video_file_fin2

write_video_file_fin:
						push	cs
						pop	ds
						push	ax ;Guardar c�digo de error (si hay)
						pushf
						mov	ah,3eh
						int	21h	;Cerrar fichero

write_video_file_fin2:
						call	write_video_fin_cinta
						popf
						pop	ax
						jc		write_video_file_fin_mal



						;mov	posicion_cadena,0
						jmp	write_video_fin_cinta_bien
write_video_file_fin_mal:



						jmp	write_video_fin_cinta_mal        



write_video_fin_cinta:			cli
						call	cambia_vectores
						mov	contador_disco,50*segundos
						mov	si,reg_pc         ;SI=PC
						mov	di,reg_sp			;DI=SP
						mov	es,seg_spectrum


write_video_fin_cinta2:

						sti
						ret



write_video_fin_cinta_bien:	;and	bits_estado2,11111110b
						ret

write_video_fin_cinta_mal:

;Error. extraemos cinta
						and	bits_estado8,11111011b

;Avisamos error io 
						or		bits_estado8,00000010b
;Numero de error
						mov error_io_num,8

;Y hacemos que salga el menu
						mov	interrumpir,1
						or		bits_estado2,00001000b


COMMENT !
bits_estado8	db		0

  Byte de estado no 7. Codificaci�n:
  bit
  ---
  7-2 No usados
  1   A 1 indica que ha habido un error al grabar, cargar, output file sound,etc. Ver variable error_io_num
  0   A 1 indica que el sonido esta silenciado

bits_estado2	db		0+2 ;Protector de pantallas activo
  COMMENT ;
  Byte de estado n� 2. Codificaci�n:
  bit
  ---
  7   A 1 indica a la interrupcion 9 que la tecla pulsada es extendida
		(el codigo anterior leido es 224)
  6   A 1 indica que ha de actuar el disparador autom�tico en puerto kempston
  5   A 0 indica que el refresco de Flash est� activo, sino a 1. Obsoleto
  4   A 1 indica que hay una interrupci�n enmascarable (Modos 0, 1 o 2)
		 pendiente de ejecutarse. (para que se pueda ejecutar una interrupci�n
		 enmascarable hay que activar este bit y tambi�n pokear con un
		 valor distinto de 0 en interrumpir para que se detenga la ejecuci�n de
		 c�digos.
  3   A 1 indica al ejecutador de c�digos(juntamente con byte interrumpir)
		 que se ha pulsado ESC y por lo tanto ha de salir el men�
  2   A 1 indica al men� o al int�rprete que se ha de activar
		 el salvapantallas
  1   A 1 indica que est� habilitado el protector de pantallas
  0	A 1 indica que ha habido error en cinta. Se devuelve este bit
		en el puerto 204=CCh

!

						ret
