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


;Impresion de caracter para VGA.
;Las llamadas que cambian para VGA con respecto a CGA empiezan 
;con vga_ 


vga_print_char:    ;Rutina de impresi¢n de un car cter en pantalla
					;Entrada: AL=Caracter
					;			 x,y=cursor_x,cursor_y
					;			 color si tinta=color_tinta


					;Avisar, si conviene, al buffer de multitarea
					call	print_char_inicio

					;Offset en pantalla=320*Linea*8+Columna+320*4+32
					push	es
					mov	bh,cursor_y
					xor	bl,bl  ;BX=Linea*320
					mov	dl,bh
					xor	dh,dh
					rol   dx,1 ;*2
					rol	dx,1 ;*4
					rol	dx,1 ;*8
					rol   dx,1 ;*16
					rol	dx,1 ;*32
					rol	dx,1 ;*64
					add	bx,dx  ;BX=320*Linea
					rol   bx,1 ;*2
					rol   bx,1 ;*4
					rol   bx,1 ;*8 ;BX=320*Linea*8
					mov	dl,cursor_x
					xor	dh,dh
					rol	dx,1
					rol	dx,1
					rol	dx,1
					add	bx,dx ;BX=320*Linea*8+columna*8
					add	bx,320*4+32 ;En BX=Offset en pantalla
					;Calcular direcci¢n en "ROM" del Spectrum=60*256+Caracter*8
					xor	ah,ah
					rol	ax,1
					rol	ax,1
					rol	ax,1
					add	ax,offset char_set-256
					mov	dl,color_tinta

					mov	es,seg_vga
					mov	cx,8

vga_print_char_bucle:
					push	cx
					xchg	ax,bx
					mov	dh,byte ptr [bx] ;En DH byte letra
					xchg	ax,bx

					mov	cx,8
vga_print_char_bucle2:
					rol	dh,1
					jnc	letra_color_cero
					mov   byte ptr es:[bx],dl
					jmp	short vga_print_char_sigue
letra_color_cero:
					push	dx
					mov	dl,color_papel
					mov   byte ptr es:[bx],dl
					pop	dx
vga_print_char_sigue:
					inc	bx
					loop  vga_print_char_bucle2
					pop	cx
					add	bx,320-8
					inc	ax
					loop	vga_print_char_bucle
					pop	es
					ret
