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

;Rutinas de control del sonido para Sound Blaster.

PUBLIC soundblaster_presente,out211_blaster

soundblaster_presente	 db	0 ;A 1 si se detecta sound blaster

;Puertos de la Sound Blaster
puerto_sb				dw		?
irq_sb					db		?
dma_ch               db		?

dsp_reset				dw		06h
dsp_read_data  		dw    0ah
dsp_write				dw		0ch
dsp_data_available	dw		0eh
ct_index					dw		04h
ct_data					dw    05h
pic_or_mask				db		?
pic_and_mask				db		?

dma_page					db		?
dma_address				db		?
dma_lenght				db		?

;Puertos de la Sound Blaster

COMMENT !
Obsoleto
CANAL_IZQ				equ	0220h
CANAL_DER				equ	0222h
CANAL_CEN				equ	0388h

adlib_address			dw		CANAL_CEN
!

FRECUENCIA_BLASTER	equ 	15384
AMPLITUD_SPEAKER		equ	50
AMPLITUD_AY				equ	24
;25*3+50=125

;Reproduccion del sonido
FRAMES_BUFFER			equ	10 ;frames de pantalla que ocupa el buffer de audio



si_enviado_blaster   db 0 ;se pone a 1 desde write_sblaster

seg_buffer_blaster	dw		?	;segmento del buffer de reproduccion del sonido
;seg_buffer_blaster_irq	dw		?	;segmento del buffer de reproduccion del sonido
;que recogera la irq
puntero_buffer_blaster	dw 	0 ;posicion del puntero dentro del buffer de audio
frames_audio_actual	db		FRAMES_BUFFER	;frames que quedan para llenar el buffer
;cuando se llega a 0, enviar el sonido (o enviarlo cuando llegue la IRQ? )
blaster_old_irq			dd		?
temp_dma_play			dw		?

seg_blaster_write		db		0 ;buffer usado en la escritura
seg_blaster_play		db		0 ;buffer usado en la reproduccion (lo que coje la DMA)

;Inicialmente, mono
;puerto_canal_a       dw    CANAL_CEN
;puerto_canal_b       dw    CANAL_CEN
;puerto_canal_c			dw		CANAL_CEN
last_speaker_value	db		0 ;Ultimo valor enviado al pcspeaker (0 o 255)

irq_event            db    0 ;Se pone a 1 cuando se produce una irq de Sound Blaster
inicializa_int_irq_event   db 0
;Se pone a 1 cuando se llama a la rutina inicializa_int_irq
;es usado por restaura_int_irq, para saber
;si se ha llamado dos veces seguidas (sin un inicializa_int_irq por medio),
;con lo cual, no debemos esperar una irq de sblaster

;La rutina restaura_int_irq lo pondra a 0


;En la emulacion del chip de sonido, si hay ruido, este siempre sale
;por el centro, ya que se usa un solo canal para cualquiera de los tres


men_si_blaster  		db		"Sound Blaster detectada A2$"
men_si_blaster2 		db    "0 I$"
men_si_blaster3		db		" D$"
men_si_blaster4      db		10,13,"$"

men_no_blaster 		db		"Sound Blaster no detectada",10,13
							db		"$"

cadena_blaster			db		"BLASTER=",0

write_sound_nombre            db              80 dup (0)
							
inicializar_soundblaster:

					;Si se especifica el parametro /nosb, no inicializarla
					test	parametros_flags,00000100b

					jz		inicializar_soundblaster2
					ret

inicializar_soundblaster2:

					;Detectar puerto de la Sound Blaster
					;Leerlo de las variables del entorno

					xor	di,di
					mov	ax,word ptr cs:[2ch]
					mov	es,ax
					mov     cx,65535
					cld

inicio_SB_bucle:					
					mov	si,offset cadena_blaster
					repz  cmpsb
					cmp	byte ptr [si-1],0
					jz		inicio_SB_bucle_si
					cmp	byte ptr es:[di-1],0
					jnz  	inicio_SB_bucle

					;Ver si hay otro cero. En ese caso, no hay variable
					cmp	byte ptr es:[di],0
					jz    inicio_SB_no
					jmp     short inicio_SB_bucle

inicio_SB_bucle_si:
					;Esta la variable. Detectar puerto, irq, dma
					;Formato variable: BLASTER=A2x0 Iy Dz
					;Leer puerto: Solo la x

					mov   al,byte ptr es:[di+1]
					sub	al,'0'
					mov	cl,4
					shl   al,cl
					mov	ah,2
					mov	puerto_sb,ax

					mov   al,byte ptr es:[di+5]
					sub	al,'0'
					mov	irq_sb,al
					
					;Poner mascara PIC
					mov	cl,al
					inc	cl
					mov	al,10000000b ; bit 0 a 1 = IRQ 0
					
					rol	al,cl
					mov	pic_or_mask,al
					
					not	al
					mov	pic_and_mask,al
					

					mov   al,byte ptr es:[di+8]
					sub	al,'0'
					mov	dma_ch,al

					;Mirar si se inicializa bien

					mov	dx,puerto_sb
					add	dx,6


					cli

					mov	al,1
					out   dx,al
					mov	cx,0
inicio_SB:		loop	inicio_SB ;Pausa
					xor	al,al
					out	dx,al



inicio_SB2:		loop	inicio_SB2 ;Pausa

					add	dx,0eh-06h

					in		al,dx

					and	al,080h
					jz		inicio_SB_no
					sub	dx,0eh-0ah

					in		al,dx

					cmp	al,0aah
					jz		inicio_SB_si

inicio_SB_no:
					sti
					mov	soundblaster_presente,0
					mov	ah,9
					mov	dx,offset men_no_blaster
					int	21h

					ret


inicio_SB_si:	mov	soundblaster_presente,1
					or		bits_estado3,00001000b ;Sonido altavoz a traves Sblaster
					
					or     bits_estado6,11000000b ;Sincronismo a traves sblaster+timer

					;Inicializar valores puertos Sound Blaster
					mov	ax,puerto_sb
					add   dsp_reset,ax
					add   dsp_read_data,ax
					add	dsp_write,ax
					add   dsp_data_available,ax
					add	ct_index,ax
					add	ct_data,ax

					;Inicializar valores control DMA
					mov   al,dma_ch
					mov	ah,al
					add	al,al
					mov	dma_address,al
					inc	al
					mov	dma_lenght,al
					or		ah,ah
					jnz	inicio_DMAno0
					mov	dma_page,87h
					jmp	short inicio_DMAsigue
inicio_DMAno0:
					cmp	ah,1
					jnz	inicio_DMAno1
					mov	dma_page,83h
					jmp	short inicio_DMAsigue

inicio_DMAno1:
					cmp	ah,2
					jnz	inicio_DMAno2
					mov	dma_page,81h
					jmp	short inicio_DMAsigue

inicio_DMAno2:
					mov	dma_page,82h
inicio_DMAsigue:

					
														
					;Reset Mixer
					xor	ax,ax
					call	write_ct

					;Selecccionar Output en Estereo
					mov	ah,0eh
					mov	al,00100010b  ;Bypass O/P Filter
					call	write_ct

					;Selecccionar Volumen Maximo (Master)
					mov	ah,22h
					mov	al,255
					call	write_ct
					
					;Selecccionar Volumen Maximo (DSP)
					mov	ah,04h
					mov	al,255
					call	write_ct
					
					;Seleccion de Linea
					mov	ah,0ch
					mov	al,00000110b
					call	write_ct
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

					;Volumen 15 de Linea(izq. y der.)
					mov	ah,2eh
					mov	al,255
					call	write_ct
					
					;Activar altavoz
					mov	al,0d1h
					call  write_dsp


					;mov 	dx,offset int_irq_blaster
					call	inicializa_int_irq
;Cambia vector de interrupcion IRQ
;Entrada: DX=Direccion gestor interrupcion

					sti

					mov	ah,9
					mov	dx,offset men_si_blaster
					int	21h


					mov	dl,byte ptr puerto_sb
					mov	cl,4
					shr	dl,cl
					add	dl,'0'
					mov	ah,2
					int	21h

					mov	ah,9
					mov	dx,offset men_si_blaster2
					int	21h

					mov	dl,irq_sb
					add	dl,'0'
					mov	ah,2
					int	21h

					mov	ah,9
					mov	dx,offset men_si_blaster3
					int	21h

					mov	dl,dma_ch
					add	dl,'0'
					mov	ah,2
					int	21h

					mov	ah,9
					mov	dx,offset men_si_blaster4
					int	21h


					ret

out211_blaster: ;Sonido de puerto Sound Blaster a traves Speaker
					cmp	ah,2
					jnz	out211_blaster2
					mov   ah,-AMPLITUD_SPEAKER

					jmp	short out211_blaster3
out211_blaster2:
					mov   ah,AMPLITUD_SPEAKER

out211_blaster3:
      			mov	last_speaker_value,ah
      			ret
      	

write_dsp:		;Llama a espera_dsp y escribe valor AL.
					mov	cl,al
					call  espera_dsp
					mov	al,cl
					out	dx,al
					ret


write_ct:		;Escribe en el CT_Mixer , indice AH y valor AL
					mov	dx,ct_index
					xchg	al,ah
					out	dx,al

					mov	dx,ct_data
					xchg	al,ah
					out	dx,al
					ret

read_ct:		   ;Lee del CT_Mixer , indice AL y devuelve en AL
					mov	dx,ct_index
					out	dx,al

					mov	dx,ct_data
					in		al,dx
					ret


espera_dsp:    ;Esperar a poder escribir en DSP. Devuelve en dx=dsp_write
					mov	dx,dsp_write
espera_dsp2:
					in		al,dx
					shl	al,1
					jc    espera_dsp2
					ret
					


inicializa_int_irq:

					cmp	soundblaster_presente,1
					jz		inicializa_int_irq2
					ret
					
					
inicializa_int_irq2:					

 					mov 	dx,offset int_irq_blaster

inicializa_int_irq3:

 					mov   inicializa_int_irq_event,1

					push	es
;Cambia vector de interrupcion IRQ

					;Calcular direccion de memoria
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
					
				           ; CHANGE PIC MASK :
           in      al,021h
           and     al,pic_and_mask   ; CLEAR MASK REGISTER BIT TO ENABLE INTERRUPT
           out     021h,al
	

					ret

restaura_int_irq:

					cmp	soundblaster_presente,1
					jz		restaura_int_irq2
					ret
					
					
restaura_int_irq2:					

               cmp   inicializa_int_irq_event,0
               jz    restaura_int_irq4
               mov   inicializa_int_irq_event,0

               ;cmp   irq_event,0
               ;jmp   short restaura_int_irq4
               ;Si esta a 0, quiere decir que desde que se ha restaurado
               ;la irq hasta ahora, no se
               
               ;Esperar a que se produzca una irq antes de cambiar nada
               mov   irq_event,0
               sti
restaura_int_irq3:
               cmp   irq_event,0
               jz    restaura_int_irq3
               cli
               
restaura_int_irq4:

           ; CHANGE PIC MASK :
           in      al,021h
           or     al,pic_or_mask
           out     021h,al


					push	es
;Restaura vector de interrupcion IRQ
					;Calcular direccion de memoria
					mov	bl,irq_sb
					add   bl,8  ;irq 0 -> int 8
					xor	bh,bh
					;Multiplicar *4
					add	bx,bx
					add	bx,bx

					xor	ax,ax
					mov	es,ax

					mov	ax,word ptr old_irq
					mov	word ptr es:[bx],ax

					mov	ax,word ptr old_irq+2
					mov	word ptr es:[bx+2],ax

					pop	es
					
					

					ret


int_irq_blaster:
					push	ax
					push	bx
					push	cx
					push	dx

					push	ds

					push	cs
					pop	ds
					
					
					mov   irq_event,1

					mov	dx,dsp_data_available
					in		al,dx
					
					mov	al,32
					out	32,al
					

					call	write_blaster_sound
					
					xor	seg_blaster_play,1 ;cambiar bloque
					
					
					pop	ds

					pop	dx
					pop	cx
					pop	bx
					pop	ax
					nop
					nop
					iret
					
inicializa_dma_play:

					mov	ax,FRAMES_BUFFER*2 ;
					mov	bx,sync_lineas
					
					mul	bx
	
					mov	temp_dma_play,ax

					mov	al,dma_ch
					add	al,4
					out	0ah,al

					xor	al,al
					out	0ch,al

					mov	al,dma_ch
					add	al,48h ;49h ;49=play
					out	0bh,al


					;mov	ax,seg_buffer_blaster_irq
					call	da_seg_blaster_play

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




					mov	dl,dma_lenght
					xor	dh,dh
					mov	ax,temp_dma_play
	
					dec	ax      ;Hay que decir 1 byte menos

					out	dx,al

					mov	al,ah
					out	dx,al

               mov   dl,dma_page
               xor   dh,dh
               mov   al,bl
               out   dx,al

					mov	al,dma_ch
					out	0ah,al
					
					ret
					
					
inicializa_dsp_play:


					mov	al,40h
					call	write_dsp
					
					;;mov	ax,50*2 ;Frecuencia siempre sync_lineas*50*2
					;el *2 viene pq es estereo
					
					;15550 o 15600 Hz reales
					;;mov	bx,sync_lineas
					;;mul	bx

;Esta funcion calcula la frecuencia que se le enviara a la Sound Blaster
;Se calcula:
;  frecuencia real=256-1000 000L/frecuencia
;Entrada: AX=frecuencia
;Salida: AL=frecuencia real
	

					;;call	calcula_frecuencia_real ;en todos casos, AL=223
					;;Frecuencia real= 15384

					mov	al,224 ;la z80 va mas rapida... :-((
					;223

					call	write_dsp
					
					mov	ax,FRAMES_BUFFER*2 ;
					mov	bx,sync_lineas
					
					mul	bx
					
					mov	al,14h ;8 bits
					call	write_dsp

					dec	ax      ;Hay que decir 1 byte menos
					push	ax

					call	write_dsp

					pop	ax

					mov	al,ah

					call	write_dsp
					
					;Activar altavoz
					mov	al,0d1h
					call  write_dsp
					

					ret
					

					
primera_vez		db		0		

da_seg_blaster_play:
					cmp	seg_blaster_play,0
;;Devuelve AX=seg_buffer_sonido si Z o AX=seg_buffer_sonido+1024 si NZ
					mov	ax,seg_buffer_blaster
					
					jz    da_seg_blaster_play2
					add	ax,975 ;312*25*2/16  ;16384/16
da_seg_blaster_play2:
					ret


da_seg_blaster_write:
					cmp	seg_blaster_write,0
;;Devuelve AX=seg_buffer_sonido si Z o AX=seg_buffer_sonido+1024 si NZ
					mov	ax,seg_buffer_blaster
					
					
					jz    da_seg_blaster_write2
					add	ax,975; 312*25*2/16  ;16384/16
da_seg_blaster_write2:
					ret
			
write_blaster_sound0:
					;Solo se llama una vez, las siguientes, se llamara desde la irq
					cmp	primera_vez,0
					mov	primera_vez,1
					jz		write_blaster_sound
					ret

write_blaster_sound:
					cmp	soundblaster_presente,1
					jz		write_blaster_sound2
write_blaster_sound_ret:
					ret
					
write_blaster_sound2:

				;Esta el sonido silenciado ?
			;		test	bits_estado8,00000001b
			;		jnz	write_blaster_sound_ret
					

					call	di_refrescar_sonido
					jz    write_blaster_sound3
					ret
					
					
write_blaster_sound3:					
	
					push	ax

					push	dx
					
					cli

					call	inicializa_dma_play
					call	inicializa_dsp_play
					
					;FIN
					
					mov   si_enviado_blaster,1
					
					pop	dx

					pop	ax
					
					sti
					
					ret
					




;poner en el buffer de sonido si se envia a sblaster o se graba
incrementa_puntero_blaster:					
					
incrementa_puntero_blaster2:				
					;Ponerlo en el buffer de sonido, de envio a soundblaster y de grabacion archivo .raw

					push	es
					push	bx
					push	ax
					
					call	da_seg_blaster_write
					mov	es,ax
					
					mov	bx,puntero_buffer_blaster
;					mov	ah,last_speaker_value ;CON SIGNO
;					add	ah,128

;					mov	al,ah

					mov	al,last_speaker_value ;CON SIGNO
					add	al,128

					call	da_amplitud_ay
					;Entrada: AL = Beeper (se tiene el valor del beeper)
					;Salida: AL,AH=Canales (valores del beeper mezclados con chip AY)

					;si se graba, dejar tal cual
						test	bits_estado7,10000000b
						jnz	incrementa_puntero_blaster3  ;se graba

;no se graba.
					cmp	soundblaster_presente,1
					jnz		incrementa_puntero_blaster_acero



; sonido silenciado?

					test	bits_estado8,00000001b
					jz		incrementa_puntero_blaster3  ;sonido no silenciado

					
incrementa_puntero_blaster_acero:
					xor	ax,ax

incrementa_puntero_blaster3:

					mov	es:[bx],al
					mov	es:[bx+1],ah  ;Se pone dos veces porque es estereo
				
					pop	ax
					pop	bx	
					pop	es
					
					inc	puntero_buffer_blaster
					inc	puntero_buffer_blaster
					
incrementa_puntero_blaster00:				
					ret
					
di_refrescar_sonido:
;Retorna Z si la rutina de refresco de sonido (IRQ) debe enviar sonido
					test	bits_estado4,00001000b
					jz		di_refrescar_sonido2
					ret
					
di_refrescar_sonido2:
					;Ver si esta en el menu y con modo pausa
					test	bits_estado4,00100000b
					jnz	di_refrescar_sonido3
					xor	al,al ;Poner Z
					ret
					
di_refrescar_sonido3:					

					test	bits_estado5,00000001b					
					ret
					
COMMENT !
					test	bits_estado8,00000001b
					jz	di_refrescar_sonido4
					ret
di_refrescar_sonido4:
					test	bits_estado5,00000001b					
					ret
!

;write_sound_nombre db "prueba.raw",0
;temporal db 0

write_sound_file:



COMMENT !
  Byte de estado no 7. Codificaci�n:
  bit
  ---
  7   A 1 indica que hay cinta insertada y activada para la grabacion de sonido en archivo raw
!

						test	bits_estado7,10000000b
						jnz	write_sound_file0
						ret

write_sound_file0:
						push dx
						push ax
						call write_sound_file_nolon0
						pop	ax
						pop	dx
						ret
;						test	bits_estado3,2
;						jnz	write_sound_file_
						;No hay cinta insertada
;						jmp	fin_cinta_mal
write_sound_file_:
;						cmp	cinta_longitud,0
;						jnz	write_sound_file_nolon0
;						jmp	fin_cinta_mal

write_sound_file_nolon0:

						call	inicio_cinta_no_int9

						mov	dx,offset write_sound_nombre
						mov	ax,3d01h ;Abrir fichero escritura
						int	21h
						jc		write_sound_file_fin0

						;jmp inicio_cinta

						mov	bx,ax
						xor	cx,cx
						xor	dx,dx
						mov	ax,4202h ;Mover puntero al final
						int	21h
						jc		write_sound_file_fin

						;En BX file handle
						push	bx
					mov	ax,FRAMES_BUFFER*2 ;
					mov	bx,sync_lineas
					
					mul	bx
					pop		bx
					mov	cx,ax
					;en cx, longitud

						call	da_seg_blaster_write
				
						
						push  ax
						pop	ds

						mov	dx,0

						mov	ah,40h
						int	21h
						jc	  	write_sound_file_fin

						push	cs
						pop	ds
						jmp short write_sound_file_fin

write_sound_file_fin0:
;						mov temporal,al
						push	ax
						pushf
						jmp	write_sound_file_fin2

write_sound_file_fin:
						push	cs
						pop	ds
						push	ax ;Guardar c�digo de error (si hay)
						pushf
						mov	ah,3eh
						int	21h	;Cerrar fichero

write_sound_file_fin2:
						call	write_sound_fin_cinta
						popf
						pop	ax
						jc		write_sound_file_fin_mal



						;mov	posicion_cadena,0
						jmp	write_sound_fin_cinta_bien
write_sound_file_fin_mal:



						jmp	write_sound_fin_cinta_mal        



write_sound_fin_cinta:			cli
						call	cambia_vectores
						mov	contador_disco,50*segundos
						mov	si,reg_pc         ;SI=PC
						mov	di,reg_sp			;DI=SP
						mov	es,seg_spectrum


write_sound_fin_cinta2:

						sti
						ret

write_sound_fin_cinta_bien:	;and	bits_estado2,11111110b
						ret

write_sound_fin_cinta_mal:		;or		bits_estado2,1
						;mov	posicion_cadena,0

;Error. extraemos cinta
						and	bits_estado7,01111111b

;Avisamos error io 
						or		bits_estado8,00000010b
;Numero de error
						mov error_io_num,0

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
