;Rutinas para la carga de ficheros a traves
;del casette
;Nota: Esta tarjeta de sonido no admite otra frecuencia que no
;sea 11111 Hz. Convendria poder hacer la rutina de carga a 22222 Hz

volumen_maxveces equ    11111/10 ;Veces que se llama a load_lee_valor hasta escribir
								  ;el volumen

volumen_cursor_x equ	9
volumen_franja	  equ 32-volumen_cursor_x

volumen_max		db		? ;Volumen maximo leido hasta siguiente escritura de
							  ;volumen

volumen_veces	dw	? ;Numero de veces leido el volumen. Cuando llega
						  ;a volumen_maxveces, se escribe en pantalla

load_tono      db       14
load_ceros     db       6
load_unos      db   	  	11
;antes load_unos db 	   12
;load_onda_falsa db      7 -> Siempre es la mitad de load_tono

load_ondas_guia dw 		100


load_byte_cambio	db	?

baudios        dw    1500
constante_frec	dd		1000000

seg_buffer_sonido	dw	?
seg_buffer_carga	dw	?

old_irq			dd		?

longitud_carga	dw		?  ;Longitud de lo cargado. Usado temporalmente


offset_sonido	dw		?  ;Offset dentro de seg_buffer_sonido del dato de
								;sonido siguiente a leer

load_flags		db		0

COMMENT !
  Flags varios para rutina de carga. Codificacion:
  bit
  ---
  7-3 Reservado
  2   Indica el bloque de buffer de sonido usado (0 o 1)
  1	A 1 indica que ha habido cambio de onda
  0	A 1 indica que se debe corregir el checksum de la carga si hay error
!


load_byte		db	?
load_byte_anterior	db ?
load_veces		db	?

load_longitud  db ?
load_amplitud  db	?
irq_event_load  db ?

COMMENT !
  Input Setting Register (0Ch):
	 ADC - input source:
	 d[2] d[1]
		0    0    Microphone (default)
		0    1    CD audio
		1    0    Microphone
		1    1    Line-in
	 In Filter - filter select:
	 d[5] d[4] d[3]
		0    x    0   Low Filter (default)
		0    x    1   High Filter
		1    x    x   No Filter
!


filtros			db		TESP,"Bajo   "
					db		TING,"Low    "
					db		255
					db		TESP,"Alto   "
					db		TING,"High   "
					db		255
					db		TESP,"Ninguno"
					db		TING,"None   "
					db		255

calcula_frecuencia:
;Esta funcion calcula la frecuencia necesaria a la que se deben recoger
;los datos, teniendo en cuenta que 1500 baudios equivale a 11111 Hz
;Devuelve en AX el resultado de :
;baudios/1500*11111 = baudios * 11111/1500
					mov   ax,11111
					mul	baudios
					mov	bx,1500
					div	bx
					ret

calcula_frecuencia_real:
;Esta funcion calcula la frecuencia que se le enviara a la Sound Blaster
;Se calcula:
;  frecuencia real=256-1000 000L/frecuencia
;Entrada: AX=frecuencia
;Salida: AL=frecuencia real
					mov	bx,ax
					mov	dx,word ptr constante_frec+2
					mov	ax,word ptr constante_frec
					div	bx
					neg	al
					ret
load:
;Rutina principal de carga desde casete
					

					call  vga_pon_marca_borde0

					mov	border,1
					
					call	inicializa_dos_buffers

          ;Esperar a que se produzca una irq antes de cambiar nada, si es que estamos en
          ;modo menu multitarea
          test bits_estado5,1
          jnz load1
          
               mov   irq_event,0
load0:
               cmp   irq_event,0
               jz    load0
load1:
               cli
					
					call	inicializa_irq
					call	inicializa_frecuencia

					sti

					mov	volumen_veces,0

;load2:

					call	load_inicio_print

					cli
					mov	offset_sonido,0
					and	load_flags,11111001b ;Poner a 0 buffer de sonido usado,
														;cambio de onda a 0

					in		al,21h

					mov	cl,irq_sb
					mov	ah,11111110b
					rol	ah,cl

					and	al,ah
					out	21h,al

					mov	al,0d0h
					call	write_dsp


					call	inicializa_dma
					sti

load_bucle:


					xor	cx,cx ;Veces

load_bucle_tono:

					test	puerto_especial3,16		;Tecla ESC
					jnz	load_bucle_tono0
					jmp	load_fin

load_bucle_tono0:

					push	cx
					call	load_lee_onda
					pop	cx

					test	puerto_especial3,16		;Tecla ESC
					jnz	load_bucle_tono00
					jmp	load_fin

load_bucle_tono00:
					cmp	load_amplitud,0
					jz		load_bucle
					mov	ah,load_tono
					mov	al,load_longitud
					call	load_lee_margen
					jnc	load_bucle
					inc	cx
					cmp	cx,load_ondas_guia
					jnz   load_bucle_tono
					call  print_cadena
					db		TESP,"Leyendo tono guia..."
					db    TING,"Reading ld signal..."
					db		255
					mov	cursor_x,0
load_bucle_tono2:
					call	load_lee_onda
					cmp	load_amplitud,0
					jz		load_bucle_tono3
					mov	ah,load_tono
					mov	al,load_longitud
					call	load_lee_margen
					jc		load_bucle_tono2
load_bucle_tono3:

					mov	cl,load_tono
					shr	cl,1
					cmp	al,cl

					jc		load_bucle_tono4
					and	load_flags,11111101b
					mov	cl,load_byte_cambio
load_bucle_tono30:
					push	cx
					call	load_lee_valor_onda
					pop	cx
					call	load_da_signo
					mov	ah,al
					mov	al,cl
					call	load_da_signo
					cmp	al,ah
					jz		load_bucle_tono30

load_bucle_tono4:

					call	print_cadena
					db		TESP,"Leyendo datos...    "
					db		TING,"Reading data...     "
					db		255
					mov	word ptr cursor_x,0c00h

					xor	bx,bx ;offset_carga,0
					xor	cx,cx ;CL=Carry, CH=Carry anterior

load_bucle_datos:
					push	cx
					push	bx
					call	load_lee_8_bits
					pop	bx
					pop	cx
					jc		load_bucle_datos2

					test	puerto_especial3,16		;Tecla ESC
					jnz	load_bucle_datos1
					jmp	load_fin

load_bucle_datos1:

					push	es
					mov	es,seg_buffer_carga
					mov	byte ptr es:[bx],al
					inc	bx
					pop	es

					mov	ch,cl
					xor	cl,al ;Actualizar Carry

					push	cx
					push	bx
					call	print_byte
					pop	bx
					pop	cx
					jmp	short load_bucle_datos

load_bucle_datos2:
;Fin de la carga.
;Stop DMA
					push	cx
					push	bx
					call	load_stop_dma
					pop	bx
					pop	cx


;Ver si hay error
					;Si longitud=0, empezar a cargar de nuevo
					or		bx,bx
					jnz	load_bucle_datos20
					jmp   load_bucle_datos_sigue

load_bucle_datos20:
					or		cl,cl
					jz		load_bucle_datos_noerr
					;Hay error
					mov	word ptr cursor_x,0700h
					push	bx
					push	cx
					call	print_cadena
					db		TESP,"Error de carga!"
					db		TING,"Loading error! "
					db		255
					pop	cx
					pop	bx

load_bucle_datos_noerr:
					;Escribir datos
					push  es
					push  cx
					push  bx
					mov	word ptr cursor_x,0200h
					mov	ah,20
					mov	al,32
					call	print_caracter_veces
					mov	cursor_x,0
					mov	es,seg_buffer_carga
					pop   cx
					push  cx
					xor	bx,bx
					call	load_print_datos
					pop   bx
					pop   cx
					pop   es

load_bucle_datos_grabar:
					mov	word ptr cursor_x,0800h
					push  bx
					push	cx
					call	print_cadena
					db		TESP,"Pulse G para grabar"
					db		TING,"Press G to save"
					db		255
					push  es
					mov   es,seg_spectrum
					call	espera_no_tecla
					call	espera_tecla
					pop   es
					pop   cx
					pop   bx
					test	puerto_65022,16
					jz	   load_bucle_datos_grabar1
					jmp	load_bucle_datos_sigue

load_bucle_datos_grabar1:
					push  bx
					push	cx
					mov	cursor_x,0
					mov	ax,2020h
					call	print_caracter_veces
					pop	cx
					pop   bx

					;Escribir datos
					;Escribir checksum leido (si no hay autocorreccion)
					;o escribir checksum calculado (si hay autocorreccion)
					test	load_flags,1
					jz		load_bucle_datos_grabar2

					push	es
					mov	es,seg_buffer_carga
					mov	byte ptr es:[bx-1],ch
					pop	es
					jmp	short load_bucle_datos_grabar2


load_bucle_datos_grabar2:
					mov	longitud_carga,bx

					call	inicio_cinta

					mov	dx,offset fichero_TAP_salida
					mov	ax,3d01h ;Abrir fichero escritura
					int	21h
					jc		load_bucle_datos_errgra0

					mov	bx,ax
					xor	cx,cx
					xor	dx,dx
					mov	ax,4202h ;Mover puntero al final
					int	21h
					jc		load_bucle_datos_errgra


					;Escribir longitud
					mov   cx,2
					mov   dx,offset longitud_carga
					mov   ah,40h
					int   21h

					jc    load_bucle_datos_errgra

					mov	cx,longitud_carga
					xor	dx,dx
					mov	ds,seg_buffer_carga
					mov	ah,40h
					int	21h

					jmp	short load_bucle_datos_errgra


load_bucle_datos_errgra0:
					pushf
					jmp	short load_bucle_datos_errgra2

load_bucle_datos_errgra:
					push	cs
					pop	ds
					pushf
					mov	ah,3eh
					int	21h	;Cerrar fichero

load_bucle_datos_errgra2:
					call	fin_cinta
					popf
					jnc	load_bucle_datos_sigue

					;Error al grabar
					mov	word ptr cursor_x,0700h
					call	print_cadena
					db		TESP,"Error al grabar!"
					db		TING,"Saving error!"
					db		255
					push  es
					mov   es,seg_spectrum
					call	espera_no_tecla
					call	espera_tecla
					pop   es


load_bucle_datos_sigue:
;Leer siguiente bloque

					jmp   load

load_fin:
					jmp	load_stop_dma


load_stop_dma:
          ;;testing para poder hacer debug call restaurar_vectores
          jmp short load_stop_dma00

;para buscar en debug........
db 0,1,2,3,4,5


load_stop_dma00:

					cli
					
				          ;Esperar a que se produzca una irq antes de cambiar nada
               mov   irq_event_load,0
load_stop_dma1:
               cmp   irq_event_load,0
               ;jz    load_stop_dma1

					
					mov	al,0d0h
					call	write_dsp

					mov	al,dma_ch
					add	al,4
					out	0ah,al


					
           ; CHANGE PIC MASK :
           in      al,021h
           or     al,pic_or_mask
           out     021h,al

           ;test in		al,21h
					;test mov	cl,irq_sb
					;test mov	ah,00000001b
					;test shl	ah,cl

					;test or		al,ah
					;test out	21h,al

					;test mov	al,32
					;test out	32,al
					

     			call	restaura_irq
					sti
					
					
					mov	primera_vez,0
					call write_blaster_sound0

					ret


load_inicio_print:

					mov	word ptr cursor_x,0
					mov	al,32
					mov	cx,32*24
					call  print_caracter_xveces
					mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901"
					db		TESP,"Cargando...  (ESC) Para Cancelar"
					db		TING,"Loading...          (ESC) Cancel"
					db		255
					mov	word ptr cursor_x,0a00h
					call	print_cadena
					db		"Volume"
					db		TESP,"n:"
					db		TING,": "
					db		TCOM,"                        "
					db		TESP,"Lectura Actual:"
					db		TING,"Data Being Read:"
					db		255
					mov   word ptr cursor_x,0200h

					ret
load_print_datos:
;Rutina que imprime los datos del bloque de bytes cargado, si hay cabecera,
;etc.
;Entrada: Direccion ES:BX, CX=Longitud total (con flag+checksum)
					cmp	cx,19 ;Cabecera
					jnz	load_print_datos_nocab
					cmp	byte ptr es:[bx],0 ;Cabecera: Flag=0
					jz		load_print_datos_cab
load_print_datos_nocab:
					push	cx
					push	bx
					call	print_cadena
					db		"Flag: ",255
					pop	bx
					mov	al,byte ptr es:[bx]
					push	bx
					xor	ah,ah
					mov	bl,128 ;No imprimir espacios
					call	print_num
					call  print_codigo13
					pop	bx
					pop	cx

					push	cx

					cmp   cx,3
					jc    load_print_datos_nocab1
					sub   cx,2

load_print_datos_nocab1:
					call	load_print_longitud
					pop	cx
					cmp	cx,3
					jc	   load_print_datos_nocab2
					call	print_cadena
					db		"+2",255

load_print_datos_nocab2:
					call	print_codigo13
					ret

load_print_datos_cab:
					inc	bx
					mov	al,byte ptr es:[bx]
					push	bx
					push	cx
					or		al,al
					jnz	load_print_datos_cab_no0
					call	print_cadena
					db		"Program"
					db		TESP,"a"
					db		TING," "
					db		255
					jmp	short load_print_datos_cab_sigue

load_print_datos_cab_no0:
					cmp	al,1
					jnz	load_print_datos_cab_no1
					call	print_cadena
					db		TESP,"Matriz Num."
					db		TING,"Array Num. "
					db		255
					jmp	short load_print_datos_cab_sigue

load_print_datos_cab_no1:
					cmp	al,2
					jnz	load_print_datos_cab_no2
					call	print_cadena
					db		TESP,"Matriz Car."
					db		TING,"Array Char."
					db		255
					jmp	short load_print_datos_cab_sigue

load_print_datos_cab_no2:
					cmp	al,3
					jnz	load_print_datos_cab_no3
					call	print_cadena
					db		"Bytes.",255
					jmp	short load_print_datos_cab_sigue

load_print_datos_cab_no3:
					pop	cx
					pop	bx
					dec	bx
					jmp	load_print_datos_nocab

load_print_datos_cab_sigue:
					call	print_cadena
					db		": ",255
					pop	cx
					pop	bx

					mov	cx,10 ;Escribir cabecera
					inc	bx

load_print_datos_cab_buc:
					mov	al,byte ptr es:[bx]
					push	bx
					push	cx
					call  da_caracter_leible
					call	print_caracter

					pop	cx
					pop	bx
					inc	bx
					loop  load_print_datos_cab_buc

					call  print_codigo13

					cmp	byte ptr es:[bx-11],3
					jnz	load_print_datos_cab_buc2
					push	bx
					call	print_cadena
					db		TESP,"Inicio: "
					db		TING,"Start:  "
					db		255
					pop	bx
					mov	ax,word ptr es:[bx+2]
					push	bx
					mov	bl,128
					call	print_num
					pop	bx
					call  print_codigo13

load_print_datos_cab_buc2:
					mov	cx,word ptr es:[bx]
					push	bx
					call	load_print_longitud
					pop	bx
					cmp	byte ptr es:[bx-11],0
					jnz	load_print_datos_fin
					inc	bx
					inc	bx
					;p[1]
					mov	ax,word ptr es:[bx-2]
					sub	ax,word ptr es:[bx+2]

					push	bx
					push	ax
					call	print_cadena
					db		13,"Variables: ",255
					pop	ax
					mov	bl,128
					call	print_num
					call  print_cadena
					db    13,"AutoRun: ",255
					pop	bx

					mov	ax,word ptr es:[bx]
					cmp   ax,32768
					jnc   load_print_datos_cab_norun

					mov	bl,128
					call	print_num
					jmp	short load_print_datos_fin

load_print_datos_cab_norun:
					call	print_cadena
					db		TESP,"Ninguno"
					db		TING,"Nothing"
					db		255

load_print_datos_fin:
					call  print_codigo13
					ret
load_print_longitud:
					push	cx
					call	print_cadena
					db		TESP,"Longitud: "
					db		TING,"Length:   "
					db		255
					pop	cx
					mov	ax,cx
					mov	bl,128
					call	print_num
					ret


inicializa_irq:

					;Restaurar el vector antiguo (el que habia en ms-dos)
					mov    inicializa_int_irq_event,0 ;sino, no sale nunca
					call	restaura_int_irq

					mov	dx,offset int_irq
					jmp inicializa_int_irq3

restaura_irq:


          call inicializa_int_irq
          ret

					;Ponemos el vector de la soundblaster
					;Asi, la variable, old_irq tendra el valor del gestor en msdos
					mov 	dx,offset int_irq_blaster

					push	es

					mov	bl,irq_sb
					add   bl,8  ;irq 0 -> int 8
					xor	bh,bh
					;Multiplicar *4
					add	bx,bx
					add	bx,bx

					xor	ax,ax
					mov	es,ax

					mov	word ptr es:[bx],dx
					mov	ax,cs
					mov	word ptr es:[bx+2],ax
					pop	es

					ret
	
inicializa_frecuencia:
					mov	al,40h
					call	write_dsp

					call	calcula_frecuencia
					call	calcula_frecuencia_real

					call	write_dsp
					ret

da_seg_bloque:
;Devuelve AX=seg_buffer_sonido si Z o AX=seg_buffer_sonido+1024 si NZ
					mov	ax,seg_buffer_sonido
					jz              da_seg_bloque2
					add	ax,1024  ;16384/16
da_seg_bloque2:
					ret
					
					
inicializa_dma:



					mov	al,dma_ch
					add	al,4
					out	0ah,al


					xor	al,al
					out	0ch,al


					mov	al,dma_ch
					add	al,44h
					out	0bh,al


					test	load_flags,00000100b
					call	da_seg_bloque

					xor	bl,bl

					shl	ax,1 ;Poner en BL los 4 bits altos de AH
					rcl   bl,1
					shl	ax,1
					rcl   bl,1
					shl	ax,1
					rcl   bl,1
					shl	ax,1
					rcl   bl,1

					mov	dl,dma_address
					xor	dh,dh
					out	dx,al


					mov	al,ah

					out	dx,al


					mov	dl,dma_page
					xor	dh,dh
					mov	al,bl
					out	dx,al


					mov	dl,dma_lenght
					xor	dh,dh
					mov   al,255  ;Hay que decir 16383 bytes
					out	dx,al


					mov   al,63

					out	dx,al


					mov	al,dma_ch
					out	0ah,al


					mov	al,24h
					call	write_dsp


					mov   al,255 ;Hay que decir 1 byte menos
					call	write_dsp


					mov   al,63
					call	write_dsp


					ret

inicializa_buffer:
;Inicializa buffer siguiente
					mov	al,load_flags
					not	al ;Inicializar buffer contrario
					test	al,00000100b ;Poner Z o NZ
inicializa_buffer2:
;Punto de entrada alternativo para inicializa_dos_buffers
					call	da_seg_bloque

					push	es

					mov	es,ax
					xor	bx,bx
					mov	cx,16384

inicializa_buffer_buc:

					mov	byte ptr es:[bx],0
					inc	bx
					loop	inicializa_buffer_buc

					pop	es

					ret

inicializa_dos_buffers:
					xor	al,al ;Z=0
					call	inicializa_buffer2
					or		al,1  ;Z=1
					jmp	short inicializa_buffer2

load_lee_valor:
;Da el valor de sonido siguiente en AL

					mov	bx,offset_sonido
					cmp	bx,16384
					jnz	load_lee_valor2

					xor	load_flags,00000100b

					call	inicializa_buffer

					xor	bx,bx


load_lee_valor2:
					push	es
					test	load_flags,00000100b
					call	da_seg_bloque
					mov	es,ax

load_lee_valor_espera:
					test	puerto_especial3,16		;Tecla ESC
					jz		load_lee_valor_espera0

					mov   al,byte ptr es:[bx]
					or		al,al
					jz    load_lee_valor_espera

load_lee_valor_espera0:

					inc	bx
					mov   offset_sonido,bx

					pop	es

					mov	ah,al
					call	load_da_abs
					xchg	al,ah

					cmp	ah,volumen_max
					jc		load_lee_valor4
					mov	volumen_max,ah
load_lee_valor4:
					mov	bx,volumen_veces
					inc	bx
					mov	volumen_veces,bx
					cmp	bx,volumen_maxveces
					jnz	load_lee_valor_fin

					;Escribir Volumen
					push	ax
					mov	ax,word ptr cursor_x
					push	ax

					mov	cursor_x,volumen_cursor_x
					mov	cursor_y,10

					mov	ah,volumen_max
					or		ah,ah
					jz		load_lee_valor5

					shr	ah,1
					inc	ah
load_lee_valor5:
					cmp	ah,volumen_franja
					jc    load_lee_valor6
					mov	ah,volumen_franja
load_lee_valor6:
					push	ax
					mov   al,'>'
					call	print_caracter_veces
					pop	ax
					mov	al,volumen_franja
					sub	al,ah
					mov	ah,al
					mov	al,'.'
					call	print_caracter_veces

					mov	volumen_veces,0
					mov	volumen_max,0

					pop	ax
					mov	word ptr cursor_x,ax
					pop	ax

load_lee_valor_fin:

					ret

da_caracter_leible:
;Convierte AL en caracter imprimible (32..127)
					and	al,01111111b ;Borrar bit 7
					cmp	al,32
					jnc  	da_caracter_leible_si
					mov	al,'.'
da_caracter_leible_si:
					ret

print_byte:
;Imprime el caracter AL en pantalla de bytes
					call	da_caracter_leible
					call	print_caracter
					mov	al,cursor_y
					cmp	al,24
					jnz	print_byte2
					mov	cursor_y,0ch

print_byte2:
					ret

load_lee_onda:
;Funcion que lee una onda completa de sonido
;Da la maxima amplitud (en positivo) en AH y la longitud de esa
;onda en AL

					mov	load_veces,0
					mov   load_longitud,1

					call	load_lee_valor_onda
					mov   load_byte_anterior,al
					call	load_da_abs
					mov	load_amplitud,al

load_lee_onda_buc:
					test	puerto_especial3,16		;Tecla ESC
					jnz	load_lee_onda_buc0
          ret

load_lee_onda_buc0:
					call	load_lee_valor_onda
					mov	load_byte,al
					call	load_da_abs
					cmp	al,load_amplitud
					jc		load_lee_onda_buc2
					mov	al,load_byte
					call	load_da_abs
					mov   load_amplitud,al

load_lee_onda_buc2:
					mov	al,load_byte
					call	load_da_signo
					mov	ah,al
					mov   al,load_byte_anterior
					call	load_da_signo
					cmp	al,ah
					jz		load_lee_onda_buc3

					;La onda ha pasado por el cero. Cambiar el borde de la
					;pantalla (si conviene)

					test	bits_estado5,00001000b
					jnz   load_lee_onda_buc20
					;Ver si hay VGA
					;cmp	vga_presente,1
					;jnz   load_lee_onda_buc20
					mov	al,border
					inc	al
					cmp	al,3
					jnz   load_lee_onda_buc200
					mov	al,1

load_lee_onda_buc200:
					mov	border,al
					push	ax
					push	cx
					push	dx
					push	si
					call  vga_pon_color_borde0
					pop	si
					pop	dx
					pop	cx
					pop	ax

load_lee_onda_buc20:

					cmp	load_veces,1
					jnz	load_lee_onda_buc21
					or	   load_flags,00000010b
					mov	al,load_longitud
					mov	ah,load_amplitud
					ret
load_lee_onda_buc21:
					inc	load_veces
load_lee_onda_buc3:
					inc	load_longitud
					mov	al,load_byte
					mov	load_byte_anterior,al
					jmp	short load_lee_onda_buc


load_da_abs:
;Funcion que transforma un valor de sonido de AL en un valor con signo:
;Si es hacia arriba, positivo. Si es hacia abajo, negativo

					sub   al,128
					jns	load_da_abs2

					neg	al
load_da_abs2:
					ret

load_da_signo:
;Devuelve el signo de AL en AL: -1,+1
					or		al,al
					jns	load_da_signo2
					mov	al,-1
					ret
load_da_signo2:
					mov	al,1
					ret

load_lee_valor_onda:
;Devuelve en AL valor de la onda
;Mira si hay un byte de cambio de onda, en cuyo caso lo devuelve

					test	load_flags,00000010b
					jz		load_lee_valor_onda2
					and	load_flags,11111101b
					mov	al,load_byte_cambio
					ret
load_lee_valor_onda2:
					call	load_lee_valor
					mov	load_byte_cambio,al
					ret

load_lee_margen:
;Funcion que devuelve C si el valor AL esta dentro del margen:
;[AH-2,AH+2]
					sub     ah,2
					cmp	al,ah
					jc		load_lee_margen_fin0

					add     ah,5
					cmp	al,ah
					jnc	load_lee_margen_fin
					ret

load_lee_margen_fin0:
					clc
load_lee_margen_fin:
					ret

load_dice_bit:
					mov	ah,load_ceros
					call	load_lee_margen
					jnc	load_dice_bit2
					mov	al,0
					ret
load_dice_bit2:
					mov	ah,load_unos
					call	load_lee_margen
					jnc	load_dice_bit3
					mov	al,1
					ret
load_dice_bit3:
					mov	al,-1
					ret

;int dice_bit(char longitud)
;Dice si el bit es 0 o 1 segun su longitud en AL
;Devuelve -1 si no es un bit aceptado
;(Valor de retorno en AL)

load_lee_8_bits:
;Devuelve en AL 8 bits leidos
;Devuelve C si se encuentran datos sin sentido

					mov	ah,0
					mov	cx,8

load_lee_8_bits_buc:
					push	ax
					push	cx
					call	load_lee_onda
					cmp	load_amplitud,0
					jz		load_lee_8_bits_error
					call	load_dice_bit
					cmp	al,-1
					jz		load_lee_8_bits_error
					mov	bl,al
					pop	cx
					pop	ax
					shl	ah,1
					or		ah,bl
					loop	load_lee_8_bits_buc
					mov   al,ah
					clc
					ret

load_lee_8_bits_error:
					pop	cx
					pop	ax
					stc
					ret


int_irq:
					push	ax
					push	bx
					push	cx
					push	dx

					push	ds

					push	cs
					pop	ds
					
					mov irq_event_load,1

					mov	dx,dsp_data_available
					in		al,dx

					mov	al,load_flags
					push	ax
					
					xor   load_flags,00000100b ;Cambiar momentaneamente de bloque

					call	inicializa_dma

					pop	ax
					mov	load_flags,al
					
					mov	al,32
					out	32,al
					
					pop	ds

					pop	dx
					pop	cx
					pop	bx
					pop	ax
					nop
					nop
					iret

					

