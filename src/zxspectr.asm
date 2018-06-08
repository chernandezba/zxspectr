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


.8086
.MODEL TINY

;Variables publicas

;PUBLIC sonido,flag_n,reg_r_bit7,interrumpir,bits_estado2,bits_estado3
PUBLIC flag_n,reg_r_bit7,interrumpir,bits_estado2,bits_estado3
PUBLIC bits_estado4,si_interrumpir,bits_estado5,bits_estado6,bits_estado8
PUBLIC previo_puerto254,previo_salida_sonido
PUBLIC valor_poke_rom,puerto_254,p_out_32765,p_out_8189,p_in_255
PUBLIC poke_word_ax,poke_word_bx,call_ax_ax
PUBLIC sync_estados_actual,sync_lineas_actual,sync_borde_sup
PUBLIC pantalla_tabla,seg_pant,sync_lineas,ordenador_emulado,puerto_32765,bits_estado7,bits_estado8


;


;DIRECCIONES EXTERNAS
extrn t_codigos_sin_prefijo:WORD
extrn t_codigos_prefcb:WORD
extrn t_codigos_prefed:WORD
extrn t_codigos_221:WORD
extrn t_codigos_253:WORD
extrn t_codigos_xycb:WORD

extrn t_codigos_sin_prefijo_estados:WORD
extrn t_codigos_prefcb_estados:WORD
extrn t_codigos_prefed_estados:WORD
extrn t_codigos_221253_estados:WORD
extrn t_codigos_xycb_estados:WORD

;RUTINAS
extrn puerto_no_usado_255:near

;PUNTEROS A RUTINAS
extrn in_255_48:near,in_255_p2a:near




codigo			SEGMENT BYTE PUBLIC
					assume cs:codigo,ds:codigo
					org   100h
empezar:

					jmp	inicio

include cabecera.inc

disparador_defecto  db		2 ;contador por defecto del disparador
								;para el joystick kempston
disparador		db		2 ;contador actual del disparador
interrumpir		db		0  ;este byte si no es cero indica al int�rprete
								;de c�digos que se ha de interrumpir la
								;ejecucion para atender a una interrupci�n o
								;una pulsaci�n de la tecla ESC
								;Es la �nica manera de parar al int�rprete
								;de c�digos

velocidad		dw		0 ;Contiene el valor de retardo para el interprete
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

;

bits_estado3	 db	 0
COMMENT;
  Byte de estado n� 3. Codificaci�n:
  bit
  ---
  7   A 1 indica que no es posible cambiar el color del borde mediante
		OUT 211,valor - no usado
  6   A 1 indica que se ha detectado procesador 386 o superior
  5	A 1 indica que solo hay 48k de memoria, por lo tanto,
		no hay paginacion posible
  4	A 1 indica que se encuentra controlador de Memoria Expandida
  3   Obsoleto: A 1 indica que la salida de sonido es por Sound Blaster, si no, Speaker
  2	A 1 indica que hay cinta insertada para cargar
  1	A 1 indica que hay cinta insertada para grabar
  0	A 1 indica que ha habido error al cargar programa, y por lo tanto
		en el menu saldra un mensaje de error. 


;

bits_estado4	 db	 0+1 ;No permitir cargar cualquier flag
COMMENT !
  Byte de estado n� 4. Codificaci�n:
  bit
  ---
  7   A 1 indica que la rutina de escribir caracter en pantalla no debe
		poner a 1 los bytes indicados en el buffer de multitarea, ni se debe
		hacer multitarea en lee_teclas_and
  6	A 1 se pone cada vez que hay una interrupcion 8 (50 cada segundo)
  5	A 1 indica al interprete (y a las rutinas de pantalla) que el menu esta
		activo, por lo tanto, hay que POKEar en pantalla de manera especial, el
		puerto 254 no lee tecla, etc...
  4	Obsoleto: A 1 indica que la salida de sonido AY va a Nada. Si es 0, va a Sblaster
  3	A 1 indica que no se debe refrescar el sonido (usado al aparecer el
		menu).
  2   A 1 indica que no se debe refrescar el sonido (opcion manipulable
		por el usuario). Obsoleto. Se usara cuando se implemente de nuevo el ruido
		por soundblaster
  Nota: Para la rutina de refresco, si bits 3 y 2 son 0 y bit 0 de
  bits_estado5 es 0 refrescar sonido;
  en caso contrario, no refrescarlo
  1   A 1 indica que no existe el chip ay-3-8912. El puerto 49149
		devuelve 255
  0	A 1 indica que la rutina de load (1378) no permite la carga de
		ficheros con cualquier flag .
!

bits_estado5	 db	 0+2 ;Idioma ingles
COMMENT !
  Byte de estado n� 5. Codificaci�n:
  bit
  ---
  7   A 1 si se ha especificado fichero snapshot a cargar
  6   A 1 indica que al ejecutar la instruccion el SI esta en una RAM "lenta",
      o se lee/escribe un puerto par
  5	A 1 indica que esta el emulador en modo Inves+ 48k
  4   A 1 indica que se esta ejecutando un halt
  3   A 1 indica que no se ven las franjas del border al cargar directo
		desde casette
  2   A 1 indica que no se imprime texto (con print_cadena) ya que
		esta en otro idioma distinto del seleccionado
  1   A 1 indica que los textos del menu salen en ingles
  0   A 1 indica que esta en modo pausa

!

bits_estado6	 db	 0
COMMENT !
  Byte de estado n� 6. Codificaci�n:
  bit
  ---
  7   A 1 indica metodo de sincronismo por Sound Blaster. A 0, sincronizar por Timer
  6   Solo tiene sentido si bit 7 esta a 1. A 0 , el sincronismo es SoundBlaster. A 1, el sincronismo
  es soundblaster+timer
  6   Obsoleto. A 1 indica que la actualizacion de pantalla es baja; si no,alta
  5   Obsoleto. Indica el modo de salida de sonido del puerto 254 a la Sound Blaster:
		0: Se hace en modo interrupciones , desde la rutina de sincronizar_cpu
		1: Se hace en modo directo, cuando se hace un OUT
  4   A 1 indica que se emula un Spectrum 128k o +2
  3   A 1 indica que se esta en modo cargar desde casette, por
		lo tanto, la rutina de int 8, actua diferente
  2	A 1 indica que se ha saltado a la INT 9 (pulsar tecla)
  1   Obsoleto: Valor anterior del bit 0, es decir, si es distinto es que: o se ha
		perdido sincronismo, o se ha ganado, y por lo tanto se ha de poner
		(o quitar) el indicador de la pantalla
  0   Obsoleto: A 1 indica que se ha perdido sincronismo

!

bits_estado7	db		0+8
COMMENT !
  Byte de estado no 7. Codificaci�n:
  bit
  ---
  7   A 1 indica que hay cinta insertada y activada para la grabacion de sonido en archivo raw
  6   A 1 indica que no se ha llegado ni a un frame por segundo. Y se ha forzado un solo frame.
Util solo para mostrarlo en el menu
  5   A 1 indica que hay frameskip. que no se debe actualizar la pantalla, puesto
      que se ha llamado a int8 cuando no se habia acabado las 311 lineas
  4   Bit de espera para actualizacion de pantalla (cada 50Hz)
  (lo pone a 1 cada int8)
  3   A 0 indica que se pseudo-emula la memoria compartidaa
  2   A 1 indica que no se emula el ruido.
  1-0 Indica el modo de stereo del chip AY:
	  0=Mono
	  1=ACB Stereo (Canal A=Izq,Canal C=Centro,Canal B=Der)
	  2=ABC Stereo (Canal A=Izq,Canal B=Centro,Canal C=Der)

!

bits_estado8	db		0   
COMMENT !
  Byte de estado no 7. Codificaci�n:
  bit
  ---
  7-4 No usados
  3	A 1 indica que los cursores emulan el joystick kempston. Sino, emulan los cursores de spectrum (SHIFT+5/6/7/8)
  2   A 1 indica que hay que grabar video en archivo de salida
  1   A 1 indica que ha habido un error al grabar, cargar, output file sound,etc. Ver variable error_io_num. 
Intentare reconvertir la gestion de errores a este formato nuevo. Ver output file sound
  0   A 1 indica que el sonido esta silenciado
!

frames_video_file db 5 ; Cada cuantos frames se debe guardar archivo
frames_video_file_actual db 0 ;Poner a 0 al insertar archivo

error_io_num db ?

p_menu_activo	dw		? ;Puntero que indica la direccion de memoria a la que
							  ;hay que saltar cada vez que se ejecuta una instruccion
							  ;del Z80, pues el menu esta activo en pantalla


carga_programa_error	db ?
COMMENT !
Cuando hay error al cargar programa (bit 0 de bits_estado3)
sale el mensaje de error aqui indicado:
1: Error de disco
2: Fichero es de 128k y solo hay 48k - NO usado a partir version 3.0
3: Tipo desconocido (ni SP ni ZX)
4: Error al abrir fichero: Posiblemente, no encontrado
!

;sonido			db		1 ;Si est� a 1 indica que se escucha el sonido,
;							  ;si est� a 0, no se escucha


flag_n			db		0 ;Contiene el flag N del Z80 (0=Suma, 2=Resta)

reg_r_bit7		db		0 ;Contiene el bit 7 del registro R


antigua_int8	dd    0

antigua_int9	dd    0

antigua_int24	dd    0

seg_spectrum	dw		0

seg_pant			dw		?
;Apunta al segmento donde esta la pantalla actual:
;(con el desplazamiento= 16384..23295)
;Si es la RAM5 = seg_spectrum
;Si es la RAM7 = segmento_RAM+6144 (si no esta paginada)
;					= seg_spectrum+2048 (si esta paginada)

seg_pant_57  dw ?
;Apunta al segmento donde se pone la pantalla 5 o 7 que no esta
;paginada dentro de los 64k, y siempre que haya EMS
;(con el desplazamiento= 16384...23295)

seg_pantalla_multitarea	dw		?
;Indica el segmento de memoria donde se ubican los 6912 bytes para hacer
;la multitarea menu-emulacion. Indica si la pantalla del Spectrum se esta
;usando por el menu. Contiene 0 si el byte no lo usa para escribir el menu,
;o 1 si el byte lo usa para escribir el menu.

puerto_49149				db		255 ;Valor del ultimo OUT al puerto 49149
											 ;(valor enviado al registro (65533)

previo_puerto254	db 0

previo_salida_sonido	db 0 ;0 o 2 si salida altavoz a 0 o 1

puerto_254              db 0 ;Ultimo valor enviado a puerto 254
;frecuencia_sonido db 2 ;Frecuencia de sonido del puerto 254 a traves Sblaster
;frecuencia_sonido_actual db 2 ;Valor actual

valor_poke_rom		db 255 ;16+8+7 ;Valor usado en el modo de emulacion Inves
;Contiene el valor del POKE a la ROM


;Los siguientes datos se usan en la sincronizacion del Z80:
;sync_pit_freq_128			equ   77
sync_lineas_128  			equ	311 ;Lineas totales
sync_estados_linea_128	equ   228 ;Duracion en T estados de una linea
sync_borde_sup_128		equ   63  ;Lineas del borde superior
sync_borde_inf_128		equ   63+192  ;Linea donde empieza el borde inferior

;sync_pit_freq_48			equ   76
sync_lineas_48				equ	312 ;Lineas totales
sync_estados_linea_48	equ   224 ;Duracion en T estados de una linea
sync_borde_sup_48		   equ   64  ;Lineas del borde superior
sync_borde_inf_48		   equ   64+192  ;Linea donde empieza el borde inferior

sync_borde_sup_inves    equ   0  ;Lineas del borde superior
sync_borde_inf_inves    equ   192  ;Linea donde empieza el borde inferior


;Constantes que dependen del modo: 48k o 128k
;sync_pit_freq				dw		?
sync_lineas					dw		?
sync_borde_sup				dw		? ;Lineas del borde superior
sync_borde_inf				dw		? ;Linea donde empieza el borde inferior

;sync_lineas_actual_Z80	dw		0 ;Cuando llega a 311 (o 312) el interprete
;realiza una interrupcion (si esta a EI)

sync_estados_linea		dw		? ;(224 o 228)*sync_factor/100
sync_estados_linea_original		dw		? ;224 o 228
sync_estados_actual		dw		0 ;Estados por linea ejecutados. Cuando
;llega a 224 o 228, sincronizar


;sync_fin_linea				dw		0 ;Numero de veces que se ha finalizado una
;linea y no se ha sincronizado el Z80. Cuando se llegan a los estados de
;una linea, el Z80 mira este valor. Si es 0, espera a que se incremente.
;Si es >0, lo decrementa en 1 y continua ejecutando instrucciones
sync_factor					dw		100 ;Porcentaje de velocidad del procesador
sync_lineas_actual	dw		0 ;Cuando llega a 311 (o 312) la int8
;hace parpadeo (si conviene), apaga el motor del disco, etc.
sync_frames			db 0 ;frames realizados actuales
sync_frames_ult	db 0 ;frames realizados ultimos


BUFFER_DISCO	equ	4096
seg_buffer_disco			dw		?
posicion_buffer_disco	dw		?
bytes_buffer_disco		dw		? ;Bytes leidos en el buffer

;pantalla_frameskip	      db 1
;pantalla_frameskip_actual	db 1

ordenador_emulado       db             3
COMMENT !

Ordenador que se esta emulando:
0=Sinclair 16k
1=Sinclair 48k
2=Inves Spectrum+
3=Sinclair 128k
4=Amstrad +2
5=Amstrad +2 - Frances
6=Amstrad +2 - Espa�ol
7=Amstrad +2A (ROM v4.0)
8=Amstrad +2A (ROM v4.1)
9=Amstrad +2A - Espa�ol
10=Sinclair 128k Español
Nota: La posicion normal del 128k Español seria la 4, pero este modelo se añadio en la version 3.5,
cuando ya estaban definidos los valores. Dichos valores se guardan en las cabeceras de archivos .zx y por eso no combiene cambiarlos
para poder tener compatibilidad hacia atrás

!

p_out_32765             dw ?
;Direccion de la rutina que se salta cuando se hace un out a 32765
p_out_8189              dw ?
;Direccion de la rutina que se salta cuando se hace un out a 8189
p_in_255						dw ?
;Direccion de la rutina que se salta cuando se lee un puerto no usado

interprete_ax dw ? ;Temporal para los distintos interpretes
poke_word_ax dw ? ;Temporal para macro poke_word,push_ax,pop_ax...
poke_word_bx dw ? ;Temporal para macro poke_word,push_ax,pop_ax...
call_ax_ax  dw ? ;Temporal para macro call_ax


if1
  include macros.mac  ;Cargar definiciones de macros

  macro_version	MACRO
						db   "3.5"
						ENDM
  macro_fecha 		MACRO
						db   "15/09/2013"
						ENDM


endif

copyright		db		10,13
					db		"ZXSpectr"
					db		" V"
					macro_version
					db    ". Emulador de ZX Spectrum",10,13
					db		"(c) Cesar Hernandez ("
					macro_fecha
					db		")",10,13
					db		"Distribuido bajo la GNU General Public License",10,13
					db    10,13,"$"
					

COMMENT !

65536 emulador+ 
4096 buffer disco
65536 carga cassete / lista ficheros filesel
768 multitarea menu
6912 buffer cambio pantalla 5/7
12496 buffer rainbow
6240*2 grabacion audio a archivo raw

si hay sb
32768 buffer lectura sample cassette 

si no hay EMS
65536 ram spectrum
128k 7 RAMS
64 k 4 ROMS

167824 +
+32768 (SB)
+64k+128k+64k (NOEMS)

=462736



!

menconems		db		"Usando Memoria Expandida (EMS)",10,13,"$"

mensinems		db		"Memoria Expandida (EMS) no disponible",10,13,"$"

mennohaymemo	db		"No hay memoria suficiente para el emulador"
					db		10,13,10,13,7,"$"

menerr_rom     db    "Error al cargar ROM",13,10,7,'$'
mennospec_scr  db	   "Error al abrir fichero ZXSPECTR.SCR",13,10,7,'$'

men_parametro_erroneo db "Parametro no valido",10,13,10,13,7,"$"

men_ayuda		db		"Sintaxis: ZXSpectr [parametros] [fichero a cargar]"
					db		10,13,10,13
					db		"Los parametros validos son:",10,13
					db		" /?     Muestra la pantalla de ayuda",10,13
					db    " /Red   Forzar modo de pantalla con tono Rojo",10,13
					db    " /Green Forzar modo de pantalla con tono Verde",10,13
					db    " /Blue  Forzar modo de pantalla con tono Azul",10,13
					db    " /Nosb  No utilizar tarjeta Sound Blaster",10,13
					db    " /No386 No detectar presencia de procesador 386",10,13
					db    " /Noems No utilizar memoria expandida (EMS)",10,13
					db    " /Eng   Mostrar textos del menu en Ingles",10,13
               db    " /Esp   Mostrar textos del menu en Espanol",10,13
					db    10,13
					db    "Pulsa una tecla...","$"
men_ayuda2:
					db    13
					db    "Ordenadores posibles a emular:",10,13,10,13
					db    " /16k   Emular Spectrum 16k",10,13
					db    " /48k   Emular Spectrum 48k",10,13
					db    " /Inves Emular Inves Spectrum+",10,13
					db    " /128k  Emular Spectrum 128k",10,13
					db    " /128ks Emular Spectrum 128k (Espanol)",10,13
					db    " /P2    Emular Spectrum Plus 2",10,13
					db    " /P2F   Emular Spectrum Plus 2 (Frances)",10,13
					db    " /P2S   Emular Spectrum Plus 2 (Espanol)",10,13
					db    " /P2A40 Emular Spectrum Plus 2A (ROM v4.0)",10,13
					db    " /P2A41 Emular Spectrum Plus 2A (ROM v4.1)",10,13
					db    " /P2AS  Emular Spectrum Plus 2A (Espanol)",10,13

					db		10,13,"$"

file_handle		dw		?   ; Aqu� se almacena el "file handle" al abrir ficheros

cadrom_48		db		"48.ROM",0
cadrom_Inves	db		"INVES.ROM",0
cadrom_128  	db		"128.ROM",0
cadrom_P2  	   db		"P2.ROM",0
cadrom_P2F 	   db		"P2F.ROM",0
cadrom_P2S 	   db		"P2S.ROM",0
cadrom_P2A40   db		"P2A40.ROM",0
cadrom_P2A41   db		"P2A41.ROM",0
cadrom_P2AS    db		"P2AS.ROM",0
cadrom_128S    db		"128S.ROM",0

cadfichero_scr	db		"ZXSPECTR.SCR",0
cadgrabar		db		80 dup (0)
cadcargar		db		80 dup (0)
fuente			db		0 ;Usado para la instruccion lods es:fuente

control_brillo	db		0 ;Valor del control de brillo para VGA


parametros_flags	db		0
COMMENT !
Flags usados en la introduccion de parametros:

  bit
  ---
  7-6 Reservado
  5  Se ha especificado algun /vgaRGB
  4	Parametro /no386
  3   Parametro /noems
  2	Parametro /nosb
  1-0	Modo de video forzado: (no usado)
		00 : Ninguno
		01	: VGA
		10 : CGA
		11 : Hercules
!

parametro_vgaR    db    0 ;Mascara usada a la hora de poner la paleta
parametro_vgaG    db    0 ;Mascara usada a la hora de poner la paleta
parametro_vgaB    db    0 ;Mascara usada a la hora de poner la paleta


inicio:

					mov	ah,9
					mov	dx,offset copyright
					int	21h

					;Leer parametros

					mov	bx,81h
inicio_bucle:	cmp	byte ptr [bx],13
					jz		inicio_bucle_fin
					cmp	byte ptr [bx],32
					jz		inicio_bucle_inc
					cmp	byte ptr [bx],"/"
					jnz	inicio_bucle_fin
					call	compara_un_parametro
					jnz	inicio_bucle_no_parametro
					jmp	inicio_bucle

inicio_bucle_inc:
					inc	bx
					jmp	short inicio_bucle


;compara_un_parametro: ;Compara el siguiente parametro escrito
;en la linea de comandos con todos los posibles
;Si coincide, se llama a su rutina y se devuelve Z y BX al siguiente parametro
;Si no coincide, se devuelve NZ
;Entrada: BX=Linea comandos

inicio_bucle_no_parametro:
					mov	ah,9
					mov	dx,offset men_parametro_erroneo
					int	21h
					call	parametro_ayuda ;Acaba con INT 20h

inicio_bucle_fin:
					mov	ax,81h
					mov	bp,bx
					sub	bx,ax

					;Borrar de la linea de comandos los parametros

					mov	bx,ax
					lahf
					mov	byte ptr [bx],32

					inc	bx
					sahf
					jz		inicio_bucle_fin2
inicio_bucle_fin1:
					mov	al,byte ptr [bp]
					cmp	al,13
					mov	byte ptr [bx],al
					jz		inicio_bucle_fin2

					inc	bx
					inc	bp
					jmp	short	inicio_bucle_fin1

inicio_bucle_fin2:
					mov	byte ptr [bx],0
					call	inicializar_soundblaster

					call	inicializar_ems
					call	detectar_386
					

;Mirar si hay memoria disponible
;Cuando se carga el emulador el DOS le asigna toda la memoria disponible
;y en la direcci�n cs:0002 hay una palabra que indica el p�rrafo
;del primer byte no utilizable

COMMENT !

65536 emulador+ 
4096 buffer disco
65536 carga cassete / lista ficheros filesel
768 multitarea menu
6912 buffer cambio pantalla 5/7
12496 buffer rainbow
6240*2 grabacion audio a archivo raw

si hay sb
32768 buffer lectura sample cassette 

si no hay EMS
65536 ram spectrum
128k 7 RAMS
64 k 4 ROMS

167824 +
+32768 (SB)
+64k+128k+64k (NOEMS)

=462736



!


					mov	ax,cs
					mov	bx,word ptr cs:[0002]
					sub	bx,ax
					mov	ax,4096  ;65536/16 64k del emulador
					add	ax,BUFFER_DISCO/16
					add	ax,4096	  ;65536/16  Para la carga desde casette

					add	ax,48 ;768/16  Para la "multitarea" menu-emulacion
					add   ax,432 ;6912/16 Para la pantalla 5 o 7 cuando sale de los 64k
					
					add	ax,781 ;Para el buffer rainbow
					

					
;					cmp	soundblaster_presente,1
;					jnz	memo_no_sb
					
					;Poner el buffer maximo de la soundblaster pro, buffer grabacion sonido .raw
					;312 lineas*10 frames*2 canales
					;FRAMES_BUFFER			equ	10 ;frames de pantalla que ocupa el buffer de audio
					
					add 	ax,(312*FRAMES_BUFFER*2)/16   ;6240 bytes
					add 	ax,(312*FRAMES_BUFFER*2)/16 ;6240 bytes. doble buffer
					
					;Ver si hay soundblaster
					cmp	soundblaster_presente,1
					jnz	memo_no_sb

					
					;buffer de grabacion
					add	ax,2048    ;32768/16
					
memo_no_sb:

					;Ver si hay EMS
					test	bits_estado3,00010000b
					jz    memo_noems
					cmp   bx,ax
					jnc   memo_hay
					jmp   short memo_nohay

memo_noems:

					;Emular sin EMS
					add	ax,4096  ;65536/16 64k del Spectrum

					add   ax,8192 	;131072/16 128k de las paginas de RAM
					add	ax,4096	;65536/16  64k  de las paginas de ROM

					cmp	bx,ax
					jnc   memo_hay

memo_nohay:

					call  liberar_ems

					mov	ah,9
					mov	dx,offset mennohaymemo
					int	21h
					int	20h

memo_hay:
					;Mirar si hay EMS
					test	bits_estado3,00010000b
					jz		memo_noems1
					mov	dx,offset menconems
					jmp	short memo_noems2

memo_noems1:
					mov	dx,offset mensinems

memo_noems2:
					mov	ah,9
					int   21h

memo_sigue:

					mov 	ah,7
					int	21h	;esperar tecla


;Ajustar el ES en el parrafo disponible
					mov	ax,cs
					add	ax,4096    ; 65536/16 Los 64k del emulador

					test	bits_estado3,00010000b
					jz		sihaymemo22

					;Hay EMS
					mov	es,seg_spectrum
					jmp	short sihaymemo_ems


sihaymemo22:
					;No hay EMS
					mov	seg_spectrum,ax
					mov	es,ax


					add	ax,4096 	  ;65536/16	Los 64k del Spectrum

					mov	segmento_ROM,ax
					add	ax,12288 	;196608/16 192k de las paginas de ROM,RAM
sihaymemo_ems:

					mov	seg_buffer_disco,ax  ;Para el buffer de disco
           		add	ax,BUFFER_DISCO/16
           		
;					cmp	soundblaster_presente,1
;					jnz	sihaymemo_no_sb
					mov	seg_buffer_blaster,ax
					;Poner el buffer de la soundblaster pro, buffer de grabacion sonido raw
					;312 lineas*10 frames*2 canales
				  ;FRAMES_BUFFER			equ	10 ;frames de pantalla que ocupa el buffer de audio

					add 	ax,(312*FRAMES_BUFFER*2)/16
					add 	ax,(312*FRAMES_BUFFER*2)/16 ;doble buffer

					cmp	soundblaster_presente,1
					jnz	sihaymemo_no_sb

					mov	seg_buffer_sonido,ax ;buffer de grabacion de audio input a .tap
					add	ax,2048    ;32768/16

           		
sihaymemo_no_sb:           		
           		

					mov	seg_buffer_rainbow,ax
					add	ax,781
					
					mov   seg_buffer_carga,ax
					add	ax,4096	  ;65536/16
;seg_buffer_carga se usa al cargar datos desde cinta real de Spectrum y
;tambien se usa en filesel.inc para almacenar la lista de ficheros
					
					mov	seg_pantalla_multitarea,ax
					mov   seg_pant_57,ax ;El buffer empieza a partir del desplazamiento 16384
					add   ax,432 ;6912/16  ???
					 ;add	ax,432 ;6912/16


;Ahora el ES y seg_spectrum apuntan al bloque de 64k de
;memoria emulable del SPECTRUM

					call	inicializa_ordenador
					jnc   no_error_ord
					mov   dx,offset menerr_rom
					mov	ah,9
					int	21h
					int	20h
no_error_ord:

					;Cambia vector 24h
					cli
					push	es
					xor	ax,ax
					mov	es,ax
					mov	ax,es:[144]
					mov	bx,es:[146]
					mov	word ptr antigua_int24,ax
					mov   word ptr antigua_int24+2,bx

					mov	ax,offset int24
					mov	word ptr es:[144],ax
					mov	word ptr es:[146],cs
					
					;La rutina de cargar_programa llama a inicializa_ordenador,
					;y necesita cambiar los vectores 8 y 9. Para ello, debe
					;saber las direcciones originales de los vectores 8 y 9

					call  guarda_vectores_old
										
					pop	es

;Abrir fichero SP si es que se especifica alguno

					cmp	byte ptr cs:[0082h],0
					jz		nocargafic

					or		bits_estado5,10000000b

					mov	dx,82h
					
					call	carga_programa0
					jnc	nocargafic2
					or		bits_estado2,8
					mov	interrumpir,1 ;Para que salga el error y luego el menu
					jmp	nocargafic2

nocargafic:
					mov	byte ptr cs:[82h],0
					;Cargar pantalla

					mov   ax,3d00h  ; Funci�n 3d, 0: Lectura
					mov	dx,offset cadfichero_scr
					int	21h
					jnc	si_spec_scr

;Error al abrir ZXSPECTR.SCR
cargar_scr_err:
					mov	ah,9
					mov	dx,offset mennospec_scr
					int	21h
					int	20h

si_spec_scr:	mov	bx,ax  ; BX=File handle
					mov	cx,6912  ;Longitud 6912
					mov	dx,16384  ; Direcci�n 16384
					push 	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds
					jc    cargar_scr_err

					mov	ah,3eh
					int	21h   ; Cerrar fichero
					jc		cargar_scr_err

nocargafic2:


;Modificar vectores de interrupci�n 8 y 9
					push	es
					
					call	cambia_vectores


;Establecer modo de video

					call	vga_inicializa_pantalla

inicio_no128k:


					pop	es

					;Si no se especifica ningun comando
					;/vgaRGB, poner mascara a 255
					test  parametros_flags,00100000b
					jnz   inicio_RGB

					mov   parametro_vgaR,255
					mov   parametro_vgaG,255
					mov   parametro_vgaB,255
					jmp   short inicio_RGB2

inicio_RGB:
;Cambiar direccion paleta
					mov   brillo0,offset brillo0_tono
					mov   brillo1,offset brillo1_tono

inicio_RGB2:

					call	crea_pantalla_tabla
					call	vga_copia_paleta

					call  actualiza_pantalla ;;;Al final de esta rutina se hace sti
					sti
					call	vga_actualiza_brillo

					cld ;Para que lodsb incremente

					call	inicializa_rom_cinta

					;Esperar tecla si es que no se ha cargado snapshot
					test	bits_estado5,10000000b
					jnz   inicio_notecla
					and	bits_estado6,11111011b

inicio_tecla_buc:
					test	bits_estado6,00000100b
					jz    inicio_tecla_buc

inicio_notecla:

					;Actualizar registros para el interprete
					mov   al,reg_f
					mov	ah,al
					and	ah,2 ;Aislar flag_n
					mov	flag_n,ah
					cargar_flags_con_f
					mov	si,reg_pc         ;SI=PC
					mov	di,reg_sp			;DI=SP

					jmp	interprete

si_interrumpir:

					test	bits_estado4,00100000b
					jz 	no_activo_menu
					jmp   multitarea_mirar

no_activo_menu:

					mov	al,bits_estado2
					test	al,8
					jz	   no_pulsado_esc
					mov	interrumpir,0
					and	bits_estado2,11110111b
					mov	ah,dh
					sahf ;Se ha pulsado ESC
					jmp	menu
no_pulsado_esc:
					test	al,4
					jz		no_salvapan

					mov	ah,dh
					sahf
					pushf
					push	si
					push	di
					call	salvapan_inicio
					call	act_pantalla_border
					pop	di
					pop	si
					popf
					lahf
					mov	dh,ah
					jmp	short no_interrumpir

no_salvapan:
					test	al,16
					jz	   no_interrumpir
					;Hay interrupcion

					test	bits_estado4,00100000b
					jnz	no_salvapan2
					mov	interrumpir,0

no_salvapan2:
					and	bits_estado2,11101111b ;interrupcion liberada

					test	bits_estado5,00010000b
					jz		no_halt
					inc	si
					and   bits_estado5,11101111b

no_halt:

					dec   di
					dec   di
					mov   bx,di  ;BX=SP
					mov	ax,si

					push	bx
					push	ax
					call	vga_poke_byte
					pop	ax
					pop	bx
					mov	al,ah
					inc	bx
					call	vga_poke_byte

					;Poner DI
					and   bits_estado,11111110b
					;

					test	bits_estado,00000010b ;Tipo de interrupci�n
					jnz	int_modo2
					mov	si,56
					jmp	short no_interrumpir

int_modo2:		mov	bl,255
					mov	bh,reg_i
					;Leer direcci�n
					mov	al,byte ptr es:[bx]
					mov	ah,byte ptr es:[bx+1]
					mov	si,ax
					;Continuar
					jmp	short no_interrumpir

si_interrumpir_inicio:
					jmp   si_interrumpir


interprete:		;Al entrar los registros deben estar as�:
					;SI corresponde al registro PC del Z80
					;flag_n debe de estar actualizado con el flag N del Z80
					;   N=0 suma  flag_n=0
					;	 N=1 resta flag_n=2
					;Los flags del 8088 deben corresponder con los flags del Z80


					;Mirar si se debe interrumpir
					lahf

interprete1:

					mov	dh,ah
					cmp	interrumpir,0
					jnz   si_interrumpir_inicio


no_interrumpir:
					;Ver si SI esta en memoria compartida,
					;esto es, RAMS 4,5,6,7
					test	bits_estado7,00001000b
					jnz	no_compartido


					mov   bx,offset paginas_actuales
					mov   ax,si
					mov   al,ah
					and   al,11000000b ;Ver segmento de memoria
					rol   al,1
					rol   al,1
					xlat
					cmp   al,8
					jc    no_compartido
					;RAM 4=Valor 8, RAM 7=Valor 11

					or    bits_estado5,01000000b

no_compartido:

;Ver si esta en la ROM de carga o grabacion
					cmp	si,1222
					jnz	no_compartido_no_save

					;Ver si hay cinta insertada

					test	bits_estado3,00000010b
					jnz	no_compartido_save0


					jmp no_compartido_no_load

no_compartido_save0:

					test	bits_estado3,00100000b
					jz		no_compartido_save_128k

			
					;Hay 48k. por lo tanto, vamos a grabar

no_compartido_save:
					mov	ah,dh
					sahf
					cargar_reg_f
					call	save_1222
					cargar_flags_con_f
					ret_pc

					;mov   ax,interprete_ax ?????

					jmp	sincronizar_cpu			

no_compartido_save_128k:
					;Ver como estan las paginas. Esperar ROM3
					mov	al,byte ptr paginas_actuales
					cmp	al,3
					jz		no_compartido_save
					jmp	short no_compartido_no_load

					
					
no_compartido_no_save:
					cmp	si,1378
					jnz	no_compartido_no_load

					;Ver si hay cinta insertada

					test	bits_estado3,00000100b
					jz		no_compartido_no_load


					test	bits_estado3,00100000b
					jz		no_compartido_load_128k

					;Hay 48k. por lo tanto, vamos a cargar
no_compartido_load:
					mov	ah,dh
					sahf
					cargar_reg_f
					call	load_1378
					cargar_flags_con_f
					ret_pc

					;mov   ax,interprete_ax ?????

					jmp	sincronizar_cpu			
;;;;;;;;;;
no_compartido_load_128k:

					;Ver como estan las paginas. Esperar ROM3
					mov	al,byte ptr paginas_actuales
					cmp	al,3
					jz		no_compartido_load

no_compartido_no_load:
					lods 	es:fuente
					mov	bl,al
					xor	bh,bh
					shl	bx,1
					add	bx,offset tabla_interprete
					jmp	[bx]

interprete_253:
					lods 	es:fuente
					cmp	al,203
					jz    interprete_253_2
					jmp	interprete253_sigue

interprete_253_2:
					add   reg_r,2

COMMENT !
	 About the R register.  This is not really an undocumented feature,
	 although I have never seen any thorough description of it anywhere.  The
	 R register is a counter that is updated every instruction, where DD, FD,
	 ED and CB are to be regarded as separate instructions.  So shifted
	 instruction will increase R by two.  There's an interesting exception:
	 doubly-shifted opcodes, the DDCB and FDCB ones, increase R by two too.
	 LDI increases R by two, LDIR increases it by 2 times BC, as does LDDR
	 etcetera.  The sequence LD R,A / LD A,R increases A by two, except for
	 the highest bit: this bit of the R register is never changed.  This is
	 because in the old days everyone used 16 Kbit chips.  Inside the chip
	 the bits where grouped in a 128x128 matrix, needing a 7 bit refresh
	 cycle.  Therefore ZiLOG decided to count only the lowest 7 bits. Anyway,
	 if the R register emulation is switched on the R register will behave as
	 is does on a real Spectrum; if it is off it will (except for the upper
	 bit) act as a random generator.

!
					mov	ch,dh
					mov	al,byte ptr es:[si]
					;En AL=Desp
					cbw
					;EN AX=Desp
					mov	bx,word ptr reg_iyl
					add	bx,ax ;BX=XY+D
					peek_byte ;AL=Contenido de BX
					inc	si

					mov   dl,al
					mov   interprete_ax,bx

					mov	bx,offset t_codigos_xycb_estados
					mov	al,byte ptr es:[si]
					xlat

					mov   bx,interprete_ax
					mov   interprete_ax,ax

					mov	al,dl
					mov	dl,byte ptr es:[si]
					inc	si

					xor	dh,dh
					rol	dx,1 ;DX=DX*2
					mov	reg_sp,di
					mov	di,offset t_codigos_xycb
					add	di,dx
					mov	ah,ch
					sahf
					call	[di]
					mov	di,reg_sp
					mov   ax,interprete_ax

					jmp	sincronizar_cpu


interprete_203:
					add   reg_r,2

					mov	bx,offset t_codigos_sin_prefijo_estados
					xlat
					mov   interprete_ax,ax

					mov	bx,offset t_codigos_prefcb
					lods 	es:fuente
					xor	ah,ah
					rol	ax,1 ;AX=AX*2
					add	bx,ax
					mov	ah,dh
					sahf
					call	[bx]
					mov   ax,interprete_ax

					jmp	sincronizar_cpu

					mov	al,cl
interprete_sinpref:
					inc   reg_r

					sub   bx,offset tabla_interprete
					;Poner BX=Codigo*2
					mov   interprete_ax,bx

					mov	bx,offset t_codigos_sin_prefijo_estados
					mov	cl,al
					xlat
					mov   bx,interprete_ax
					mov   interprete_ax,ax

					add   bx,offset t_codigos_sin_prefijo
					mov	ah,dh
					sahf
					call	[bx]
					mov   ax,interprete_ax

					jmp	sincronizar_cpu

interprete_237:
					add   reg_r,2

					mov	bx,offset t_codigos_prefed_estados
					xlat
					mov   interprete_ax,ax

					mov	bx,offset t_codigos_prefed
					lods  es:fuente
					xor	ah,ah
					rol	ax,1 ;AX=AX*2
					add	bx,ax
					mov	ah,dh
					sahf
					call	[bx]

					mov   ax,interprete_ax
					jmp	sincronizar_cpu


interprete253_sigue:

					add   reg_r,2

					mov	bx,offset t_codigos_221253_estados
					mov	cl,al
					xlat
					mov   interprete_ax,ax

					mov	bx,offset t_codigos_253
					xor	ch,ch
					rol	cx,1 ;CX=CX*2
					add	bx,cx
					mov	ah,dh
					sahf
					call	[bx]
					mov   ax,interprete_ax

					jmp	sincronizar_cpu


interprete_221:
					lods 	es:fuente
					cmp	al,203
					jnz	interprete221_sigue
					add   reg_r,2

					mov	ch,dh
					mov	al,byte ptr es:[si]
					;En AL=Desp
					cbw
					;EN AX=Desp
					mov	bx,word ptr reg_ixl
					add	bx,ax ;BX=XY+D
					peek_byte ;AL=Contenido de BX
					inc	si

					mov   dl,al
					mov   interprete_ax,bx

					mov	bx,offset t_codigos_xycb_estados
					mov	al,byte ptr es:[si]
					xlat

					mov   bx,interprete_ax
					mov   interprete_ax,ax

					mov	al,dl
					mov	dl,byte ptr es:[si]
					inc	si

					xor	dh,dh
					rol	dx,1 ;DX=DX*2
					mov	reg_sp,di
					mov	di,offset t_codigos_xycb
					add	di,dx
					mov	ah,ch
					sahf
					call	[di]
					mov	di,reg_sp
					mov   ax,interprete_ax

					jmp	sincronizar_cpu

interprete221_sigue:

					add   reg_r,2

					mov	bx,offset t_codigos_221253_estados
					mov	cl,al
					xlat
					mov   interprete_ax,ax

					mov	bx,offset t_codigos_221
					xor	ch,ch
					rol	cx,1 ;CX=CX*2
					add	bx,cx
					mov	ah,dh
					sahf
					call	[bx]

					mov   ax,interprete_ax
					jmp	sincronizar_cpu

sincronizar_cpu:

					lahf

					;Si memoria compartida, instruccion ocupa al+al/6=al+al/(2+4)
					test	bits_estado7,00001000b
					jnz	sincronizar_cpu_no_compartida

					test  bits_estado5,01000000b
					jz    sincronizar_cpu_no_compartida
					and   bits_estado5,10111111b
					push  ax

					xor   ah,ah
					mov   bl,6
					div   bl
					mov   dl,al
					pop   ax
					add   al,dl


sincronizar_cpu_no_compartida:


					mov   dx,sync_estados_actual
					mov	bl,al
					xor	bh,bh
					add	dx,bx
					cmp	dx,sync_estados_linea
					jnc   sincronizar_cpu_si

					;Volver
					mov	sync_estados_actual,dx
					jmp   interprete1

sincronizar_cpu_si:

					;Cambio de linea

					sub	dx,sync_estados_linea
					mov   sync_estados_actual,dx
					
					call	incrementa_puntero_blaster
					
					call 	copia_buffer_rainbow

					mov	bx,sync_lineas_actual
					inc	bx
					cmp	bx,sync_lineas
					jnc   sincronizar_cpu_int
sincronizar_cpu_no_int:

					;No se produce interrupcion

					mov	sync_lineas_actual,bx

					jmp	interprete1


sincronizar_cpu_int:
COMMENT !
Metodo de sincronizacion:
-Si no hay targeta sounblaster, se sincroniza mediante el timer. Frecuencia=50 Hz
-Si hay targeta sounblaster, si se elige el modo de timer, es el metodo anterior
-  Si se elige el metodo sounblaster, al final de cada frame no se sincroniza.
-  Si se elige el metodo sounblaster+timer, al final de cada frame se sincroniza mediante el timer.
-  Si se llega al final del buffer de soundblaster (normalmente, cada 10 frames), se espera a
   que se produzca la irq de la soundblaster

!

					;Enviar audio a archivo raw, si toca........
					dec	frames_audio_actual
					jnz	 sincronizar_cpu_no_fin_audio

					;enviar sonido a archivo raw
					call	write_sound_file					




sincronizar_cpu_no_fin_audio:
					cmp	soundblaster_presente,1
					jnz	sincronizar_cpu_no_blaster
					
				
					cmp	frames_audio_actual,0
					jz	 sincronizar_cpu_int2
					
					;No se llega al final del buffer de audio

					test bits_estado6,10000000b
					jz sincronizar_cpu_no_blaster ;Sincronismo Timer


COMMENT !
bits_estado6	 db	 0
  Byte de estado n� 6. Codificaci�n:
  bit
  ---
  7   A 1 indica metodo de sincronismo por Sound Blaster. A 0, sincronizar por Timer
  6   Solo tiene sentido si bit 7 esta a 1. A 0 , el sincronismo es SoundBlaster. A 1, el sincronismo
  es soundblaster+timer
!
          test  bits_estado6,01000000b
          jnz    sincronizar_cpu_no_blaster ;Sincronismo  SBlaster+Timer

					;Sincronismo SBlaster
					jmp short sincronizar_cpu_esp0
					

sincronizar_cpu_int2:					
					;mov	frames_audio_actual,FRAMES_BUFFER
					;mov	puntero_buffer_blaster,0		

				
					               										
					;Enviar sonido a la sound blaster
					;



					call	write_blaster_sound0

					xor	seg_blaster_write,1
					;cambiamos el bloque, de momento. si luego sincroniza
					;por sblaster, ya se modificara
					
					cmp   si_enviado_blaster,1
					jz    sincronizar_cpu_no_blaster00
					
					cmp   primera_vez,0
					jz    sincronizar_cpu_esp0
					
					test bits_estado6,10000000b
					jz sincronizar_cpu_no_blaster
					call wait_cpu_blaster

					
sincronizar_cpu_blaster00:					
					
					mov al,seg_blaster_play
					mov seg_blaster_write,al					
					
sincronizar_cpu_no_blaster0:

					mov   si_enviado_blaster,0
					jmp short sincronizar_cpu_esp0
					
sincronizar_cpu_no_blaster00:
;se ha llegado mas tarde que la irq de sblaster

					mov   si_enviado_blaster,0
					or bits_estado7,00100000b
					test 	bits_estado6,10000000b
					jz 	sincronizar_cpu_esp2
					
					mov al,seg_blaster_play
					mov seg_blaster_write,al					
					
					jmp short sincronizar_cpu_esp2
					
	
COMMENT !
  Byte de estado n� 6. Codificaci�n:
  bit
  ---
  7   A 1 indica metodo de sincronismo por Sound Blaster. A 0, sincronizar por Timer
!

sincronizar_cpu_no_blaster:
					call wait_cpu_timer
sincronizar_cpu_esp0:					
					;inc	sync_frames
sincronizar_cpu_esp2:
					and bits_estado7,11101111b


;Ver si esta la grabacion de video de archivo activada
COMMENT !
bits_estado8	db		0+4   ;temporal

  Byte de estado no 7. Codificaci�n:
  bit
  ---
  7-3 No usados
  2   A 1 indica que hay que grabar video en archivo de salida
  1   A 1 indica que ha habido un error al grabar, cargar, output file sound,etc. Ver variable error_io_num. 
Intentare reconvertir la gestion de errores a este formato nuevo. Ver output file sound
  0   A 1 indica que el sonido esta silenciado

frames_video_file db 50 ; Cada cuantos frames se debe guardar archivo
frames_video_file_actual db 0 ;Poner a 0 al insertar archivo
!
					test	bits_estado8,00000100b
					jz		sincronizar_cpu_esp_novideo

					inc   frames_video_file_actual
					mov	al,frames_video_file
					cmp	al,frames_video_file_actual
					jnz	sincronizar_cpu_esp_novideo
					
					;Enviar frame de video a archivo
					mov	frames_video_file_actual,0


					;actualizar pantalla
					push	si
					push	di
					push	ax
					push	es
					call	actualiza_pantalla_rainbow
					pop	es
					pop	ax
					pop	di
					pop	si

					call write_video_file

;ya se ha actualizado pantalla. 
					inc	sync_frames  ;necesario?????????

jmp short sincronizar_cpu_noactualiza


sincronizar_cpu_esp_novideo:

;;
;Ver si en el ultimo segundo no se ha alcanzado ningun frame, pues actualizar como minimo una vez por segundo la pantalla
					cmp sync_frames,0
					jnz sincronizar_cpu_esp3
					;estamos al final de un primer frame en un intervalo de segundo

					and bits_estado7,10111111b
					;la ultima vez no se ha alcanzado ni un frame?
					cmp sync_frames_ult,0
					jnz sincronizar_cpu_esp3

					;actualizar pantalla
					push	si
					push	di
					push	ax
					push	es
					call	actualiza_pantalla_rainbow
					pop	es
					pop	ax
					pop	di
					pop	si

					;avisamos que es un frame forzado
					or bits_estado7,01000000b

;ya se ha actualizado pantalla. 
;??????? volvemos a pintar el frame? jmp short sincronizar_cpu_noactualiza


sincronizar_cpu_esp3:
;;

          test bits_estado7,00100000b
          
          ;hay frameskip
          jnz sincronizar_cpu_noactualiza

          inc sync_frames
					;actualizar pantalla
					push	si
					push	di
					push	ax
					;call	actualiza_pantalla00
					push	es
					call	actualiza_pantalla_rainbow
					pop	es
					pop	ax
					pop	di
					pop	si

sincronizar_cpu_noactualiza:

					;Se produce interrupcion
					and bits_estado7,11011111b

					mov	pantalla_origen,offset pantalla_tabla
					mov	pantalla_destino,320*4
					
					mov	puntero_buffer_rainbow,0


;Puntero de audio blaster y archivo .raw
					cmp	frames_audio_actual,0
					jnz	sincronizar_cpu_noactualiza2

					mov	frames_audio_actual,FRAMES_BUFFER
					mov	puntero_buffer_blaster,0		

				
					;xor	seg_blaster_write,1
					;cambiamos el bloque, de momento. si luego sincroniza
					;por sblaster, ya se modificara

sincronizar_cpu_noactualiza2:
;sincronizar_cpu_no_punto:
					xor	bx,bx

					;Ver si esta EI

					test	bits_estado,1
					jnz   sincronizar_cpu_ei
					jmp   sincronizar_cpu_no_int

sincronizar_cpu_ei:
					or		bits_estado2,16 ;Avisar que hay una interrupci�n
					;Decir que se debe atender la interrupci�n
					mov	interrumpir,1
					jmp   sincronizar_cpu_no_int


sincronizar_reset:
;Poner a 0 sync_fin_linea, decir que no hay perdida de sincronismo


					mov	sync_lineas_actual,0
					mov	pantalla_origen,offset pantalla_tabla
					mov	pantalla_destino,320*4
					
					mov	puntero_buffer_rainbow,0

					ret


wait_cpu_timer:
					test bits_estado7,00010000b
					jz wait_cpu_timer1
					
					;se ha llegado mas tarde

					or bits_estado7,00100000b
					ret
wait_cpu_timer1:
					test bits_estado7,00010000b
					jnz wait_cpu_timer_end

					hlt
					jmp short wait_cpu_timer1
wait_cpu_timer_end:
					

					ret

wait_cpu_blaster:
					cmp   si_enviado_blaster,0
					jz    wait_cpu_blaster1
					;se ha llegado mas tarde
          mov si_enviado_blaster,0

          or bits_estado7,00100000b
          ret
;Esperar a una IRQ de Sound Blaster					
wait_cpu_blaster1:
					cmp   si_enviado_blaster,0
					jz    wait_cpu_blaster1
					mov si_enviado_blaster,0

					ret

;Restaurar vectores de interrupci�n

fin_spectrum:
					or		bits_estado4,00001000b ;No refrescar el sonido

					;Desactivar canales 0,1,2,7
					;call	silenciar_canales

					call	actualiza_pantalla
					call	vga_apaga_pantalla

					;Liberar memoria expandida
					call    liberar_ems

fin_spectrum2:

					cli
					push  es
					mov    inicializa_int_irq_event,0 ;sino, no sale nunca
					call	restaura_vectores
					call	restaura_int_irq

					xor	ax,ax
					mov	es,ax
					mov	ax,word ptr antigua_int24
					mov	bx,word ptr antigua_int24+2
					mov	word ptr es:[144],ax
					mov   word ptr es:[146],bx

					call  pon_estados_a0

					pop	es
					sti

;Volver al modo texto
					mov	ax,0003h
					int	10h

					int   20h ;FIN

cambiar_vectores:
					cli
					call	cambia_vectores
					mov	contador_disco,50*segundos
					mov 	es,seg_spectrum
					sti
					ret

cambia_vectores:
					xor	ax,ax
					mov	es,ax
					mov	ax,offset int8
					mov	word ptr es:[32],ax
					mov	word ptr es:[34],cs
					mov	ax,offset int9
					mov	word ptr es:[36],ax
					mov	word ptr es:[38],cs
					
					mov	primera_vez,0 ;Volver a iniciar la transferencia de sonido
					;call	inicializa_int_irq					
				
					;Ajustar frecuencia convenientemente
					;(que se llame a la INT 8 cada vez que se finalice
					;el refresco de una linea -> Obsoleto

cambia_vectores_pit:
					mov	al,34h
					out	43h,al
					;si esta cargando, cambiar la frecuencia
					;a 50Hz
					;mov   ax,sync_pit_freq
					;test  bits_estado6,00001000b
					;jz    cambia_vectores_pit_nocarga
					mov   ax,5d37h
;cambia_vectores_pit_nocarga:

					out	40h,al
					mov   al,ah
					out	40h,al
					
 					ret

restaurar_vectores: ;Es la llamada mas comun de las rutinas que usan
;las interrupciones del DOS
					push	es
					cli
					call	restaura_vectores
					sti
					pop	es
					ret
restaura_vectores:
					xor	ax,ax
					mov	es,ax


					mov	ax,word ptr antigua_int9
					mov	bx,word ptr antigua_int9+2
					mov	word ptr es:[36],ax
					mov   word ptr es:[38],bx

restaura_vectores_no_int9:
;Se llama aqui en streaming de audio y video para no restaurar int9-teclado
;En streaming suele suceder que la pulsacion/liberacion de teclas "se va" a la int9 del msdos
;y no al emulador

					xor	ax,ax
					mov	es,ax


					mov	ax,word ptr antigua_int8
					mov	bx,word ptr antigua_int8+2
					mov	word ptr es:[32],ax
					mov   word ptr es:[34],bx


					;Restaurar contador del temporizador
					mov	al,34h
					out	43h,al
					xor	al,al
					out	40h,al
					out	40h,al
					
					;call	restaura_int_irq
					

					ret
					
guarda_vectores_old:
;Guardar vectores de interrupci�n 8 y 9 antiguos
					push  es
					xor	ax,ax
					mov	es,ax
					mov	ax,word ptr es:[32]
					mov	bx,word ptr es:[34]
					mov	word ptr antigua_int8,ax
					mov   word ptr antigua_int8+2,bx

					mov	ax,es:[36]
					mov	bx,es:[38]
					mov	word ptr antigua_int9,ax
					mov   word ptr antigua_int9+2,bx

					;IRQ de soundblaster
					
					mov	bl,irq_sb
					add   bl,8  ;irq 0 -> int 8
					xor	bh,bh
					;Multiplicar *4
					add	bx,bx
					add	bx,bx

					mov	ax,word ptr es:[bx]
					mov	word ptr old_irq,ax
					mov	ax,word ptr es:[bx+2]
					mov	word ptr old_irq+2,ax
					
					
					pop	es
					ret

					;call	cambia_vectores
					

pon_estados_a0:
					push  es
					push  ax
					;Poner estado de las teclas MAYS,CTRL,ALT a 0 (no pulsadas)
					mov   ax,40h
					mov   es,ax
					and   word ptr es:[17h],0000111111110000b
					pop   ax
					pop   es
					ret


int24:			;Gestor de errores de disco
					mov	al,0
					iret

carga_programa:
					mov	dx,offset cadcargar
carga_programa0:  ;Usado al cargar al inicio
					mov	es,seg_spectrum
					mov   ax,3d00h  ; Funci�n 3d, 0: Lectura

					int	21h
					jnc	si_fic_sp

					mov	carga_programa_error,4
					jmp	short errorfic_sp0

;Error al abrir Fichero SP o ZX
errorfic_sp:
					mov	carga_programa_error,1
errorfic_sp0:
					or		bits_estado3,1

					stc
					ret

si_fic_sp:		mov	bx,ax  ; BX=File handle
					mov	fic_zx_handle,bx
					mov	cx,2	 ; Longitud total: 38
					mov	dx,offset signatura
					mov	ah,3fh
					int	21h
					jc		errorfic_sp

					call	fic_dice_tipo
					or		al,al
					jz		no_fic_tipo ;Tipo desconocido

					mov	cx,36	 ; 36 Longitud total: 38
					mov	dx,offset signatura+2
					mov	ah,3fh
					int	21h

					jc		errorfic_sp

					mov	al,reg_r
					and	al,10000000b
					mov	reg_r_bit7,al

					;Mirar si es un fichero de formato ZX o SP
					call	fic_dice_tipo
					cmp	al,2
					jz		fic_zx

					;Tipo SP
					;Establecemos version a 1. Para engañarle y decirle que es casi como un fichero zx version 1
					mov	version,1
					jmp	fic_zx_48k

no_fic_tipo:
					mov	carga_programa_error,3
					jmp	short errorfic_sp0

si_fic_sp_fin:
					ret

si_fic_sp_cerrar:

					mov	ah,3eh
					mov	bx,fic_zx_handle
					int	21h   ; Cerrar fichero

					jc		errorfic_sp
					ret

fic_zx:	      mov	dx,offset version
					mov   cx,256
					mov	ah,3fh
					mov	bx,fic_zx_handle
					int	21h
					jc		errorfic_sp

					;Poner datos de la cabecera extendida

					cli
					mov	bl,bits_estado2
					and	bl,10111101b
					mov	bh,cabecera_bits_estado
					push	bx
					and	bh,01000010b
					or		bl,bh
					mov	bits_estado2,bl
					pop	ax
					and	ah,128
					ror	ah,1

					mov	cx,8
					mov	bx,offset puerto_65278
fic_zx_2:
					mov	al,byte ptr [bx]
					and	al,10111111b
					or		al,ah
					mov	byte ptr [bx],al
					inc	bx
					loop  fic_zx_2

					mov	bl,cabecera_control_brillo
					mov	control_brillo,bl

					mov	bl,cabecera_disparador_defecto
					mov	disparador_defecto,bl

					mov	bl,cabecera_sonido
					xor	bl,1
					and	bits_estado8,11111110b
					or		bits_estado8,bl

;					mov	sonido,bl

					sti

					;Ver si es version 2+
					cmp	version,2
					jnc	fic_zx_128k

					jmp	fic_zx_48k
fic_zx_128k:
					mov	bl,cabecera_bits_estado
					push	bx
					shl	bl,1
					shl	bl,1
					shl	bl,1
					shl	bl,1
					mov	bh,bits_estado3
					and	bx,0111111110000000b
					or		bh,bl
					mov	bits_estado3,bh
					pop	bx

					;Ver si es un programa de 128k
					test	cabecera_bits_estado,00010000b
					jz		fic_zx_48k

fic_zx_128k_si:

					;Hay 128k y el programa es de 128k

					;Paginar RAMS 0,1,2,3

					xor	al,al
					mov	ah,4

					call	menu_grabar_3_128k_pagina

					;Ahora hay que cargar 65536 bytes
					mov	bx,0
					mov	posicion_buffer_disco,BUFFER_DISCO ;Inicializar puntero
					call  fic_zx_bucle0

					;Paginar RAMS 4,5,6,7

					xor	al,al
					mov	ah,8

					call	menu_grabar_3_128k_pagina

					;Ahora hay que cargar 65536 bytes mas
					mov	bx,0
					;mov	posicion_buffer_disco,BUFFER_DISCO ;Inicializar puntero
					call  fic_zx_bucle0

					call	si_fic_sp_cerrar


					;Inicializa ordenador
					;Ver si es version 3+
					cmp   version,3
					jnc	fic_zx_128k_v3
					mov	cabecera_ordenador_emulado,9 ;Spectrum +2A Espa�ol
fic_zx_128k_v3:
					call	fic_zx_48k_solo4

					;Poner posicion normal de las paginas

					xor	al,al
					mov	bx,offset cabecera_paginas_actuales


fic_zx_128k_si_buc:
					mov	ah,byte ptr [bx]
					push	ax
					push	bx
					call	paginar
					pop	bx
					pop	ax
					inc	bx
					inc	al
					cmp	al,4
					jnz   fic_zx_128k_si_buc

					mov	ax,word ptr cabecera_puerto_32765
					mov	word ptr puerto_32765,ax

					ret

					
fic_zx_48k:
					;Ver si solo se permite 48k
					test	bits_estado3,00100000b
					jnz	fic_zx_48k_solo

					mov   word ptr puerto_32765,0430h

					;Poner ROM 3, RAMS normales
					xor	al,al
					mov	ah,3
					call	paginar
					mov	al,1
					mov	ah,5+4
					call	paginar
					mov	al,2
					mov	ah,2+4
					call	paginar
					mov	al,3
					mov	ah,0+4
					call	paginar

					mov	ax,seg_spectrum
					mov	seg_pant,ax
fic_zx_48k_solo:
					call	fic_dice_tipo
					cmp	al,2
					jz		fic_zx_48k_solo2

					;Tipo SP

					mov	dx,16384
					push	ds
					mov	cx,49152
					mov	bx,fic_zx_handle
					mov   ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds

					jnc	fic_zx_48k_solo3
					jmp	errorfic_sp


fic_zx_48k_solo2:
					cmp	version,2
					jnc	fic_zx_48k_solo20
					mov   al,reservado20
					mov   bits_estado,al
fic_zx_48k_solo20:
					call	fic_zx_4
fic_zx_48k_solo3:

					call	si_fic_sp_cerrar
					;Inicializa ordenador
					;Ver si es version 3+
					cmp   version,3
					jnc	fic_zx_48k_solo4
					mov	cabecera_ordenador_emulado,1 ;Sinclair 48k
fic_zx_48k_solo4:
					mov	al,cabecera_ordenador_emulado
					mov	ordenador_emulado,al
					call	restaurar_vectores
					call  inicializa_ordenador
					cli
					;jc   menu_ordenador_cambia2 ;Cuando hay error de carga

					call	cambia_vectores
					sti

					ret

fic_zx_4:
					mov	bx,16384
					mov	posicion_buffer_disco,BUFFER_DISCO ;Inicializar puntero

fic_zx_bucle:	cmp	bx,0

					jnz	fic_zx_bucle0
					jmp	si_fic_sp_fin


fic_zx_bucle0:

					call	fic_zx_lee_byte
					cmp	al,221
					jnz	fic_zx_bucle_no
					;Ver si no es final
					cmp	bx,65535
					jz    fic_zx_bucle_no
					cmp	bx,65534
					jz		fic_zx_bucle_no
					call	fic_zx_lee_byte
					cmp	al,221
					jz		fic_zx_bucle_si_repe
					;No hay dos 221. Escribir el anterior 221 y el actual byte
					mov	byte ptr es:[bx],221
					inc	bx
					mov	byte ptr es:[bx],al
					inc	bx
					jmp	short fic_zx_bucle

fic_zx_bucle_si_repe:
					;Leer el byte a repetir
					call	fic_zx_lee_byte
					mov	ah,al
					call	fic_zx_lee_byte ;Leer numero de veces
fic_zx_bucle_si_repe2:
					mov	byte ptr es:[bx],ah
					inc	bx
					dec	al
					jnz	fic_zx_bucle_si_repe2
					jmp	short fic_zx_bucle

fic_zx_bucle_no:
					;Escribir byte
					mov	byte ptr es:[bx],al
					inc	bx
					jmp	short fic_zx_bucle

fic_zx_lee_byte:
					cmp	posicion_buffer_disco,BUFFER_DISCO
					jnz	fic_zx_lee_byte2
					push	ax
					push	bx
					mov	cx,BUFFER_DISCO
					xor	dx,dx
					mov	bx,fic_zx_handle
					push	ds
					mov  	ds,seg_buffer_disco
					mov	ah,3fh
					int	21h
					pop	ds
					mov	posicion_buffer_disco,0
					mov	bytes_buffer_disco,ax

					pop	bx
					pop	ax

fic_zx_lee_byte2:
					mov	dx,posicion_buffer_disco
					cmp	dx,bytes_buffer_disco
					jnz	fic_zx_lee_byte3
					;Se ha leido el final del fichero, error
					stc
					ret
fic_zx_lee_byte3:
					push	bx
					mov	bx,posicion_buffer_disco
					push	ds
					mov   ds,seg_buffer_disco
					mov	al,byte ptr [bx]
					pop	ds
					inc	bx
					mov	posicion_buffer_disco,bx
					pop	bx
					clc
					ret

fic_dice_tipo:
;Devuelve el tipo de fichero leido desde la cabecera
;Salida: AL=0 Desconocido
;			AL=1 SP
;			AL=2 ZX
					cmp	byte ptr signatura,'Z'
					jz		fic_dice_tipo_zx
					cmp	byte ptr signatura,'S'
					jz		fic_dice_tipo_sp

fic_dice_tipo_no:

					xor	al,al ;Desconocido
					ret

fic_dice_tipo_zx:
					cmp	byte ptr signatura+1,'X'
					jnz	fic_dice_tipo_no
					mov	al,2
					ret

fic_dice_tipo_sp:
					cmp	byte ptr signatura+1,'P'
					jnz	fic_dice_tipo_no
					mov	al,1
					ret


compara_parametro:  ;Dando BX=Linea de comandos y BP=Tabla
;Devuelve Z si son iguales y BX y BP al siguiente caracter
;Tabla debe ser del tipo:
;db "parametro",255
;Linea de comandos debe ser: "/parametro " o "/parametro/" o "parametro",13
					inc	bx ;Saltar "/"
compara_parametro2:

					mov	al,byte ptr [bx]
					call	haz_minusculas
					cmp	al,"/"
					jz		compara_parametro_fin
					cmp	al," "
					jz		compara_parametro_fin
					cmp	al,13
					jz		compara_parametro_fin
					cmp	al,byte ptr [bp]
					jnz	compara_parametro_diferente
					inc	bp
					inc	bx
					jmp	short compara_parametro2

compara_parametro_fin:
					cmp	byte ptr [bp],255
compara_parametro_diferente:
					ret

compara_un_parametro: ;Compara el siguiente parametro escrito
;en la linea de comandos con todos los posibles
;Si coincide, se llama a su rutina y se devuelve Z y BX al siguiente parametro
;Si no coincide, se devuelve NZ
;Entrada: BX=Linea comandos

					mov	bp,offset tabla_parametros
					xor	al,al
					
compara_un_parametro1:
					push	bx
					push	ax
					call	compara_parametro
					pop	ax
					jnz	compara_un_parametro_fin
					add	sp,2 ;Eliminar bx de la pila
					push	bx
					mov	bx,offset tabla_parametros_saltos
					xor	ah,ah
					add	ax,ax
					add	bx,ax
					call	[bx]
					pop	bx
					xor	al,al
					ret

compara_un_parametro_fin:
					pop	bx
					inc	al
					cmp   al,20 ;Parametros posibles
					jz		compara_un_p_no
					;Siguiente parametro de la tabla
compara_un_p_fin2:
					cmp	byte ptr [bp],255
					jnz	compara_un_p_fin3
					inc	bp
					jmp	short compara_un_parametro1
compara_un_p_fin3:
					inc	bp
					jmp	short compara_un_p_fin2

compara_un_p_no:
					or		al,al ;Devolver NZ
					ret

tabla_parametros  db "?",255
					db		"red",255
					db		"green",255
					db		"blue",255
					db    "nosb",255
					db    "noems",255
					db		"no386",255
					db		"eng",255
					db		"esp",255
					db    "16k",255
					db    "48k",255
					db		"inves",255
					db    "128k",255
					db    "128ks",255
					db    "p2",255
					db    "p2f",255
					db    "p2s",255
					db    "p2a40",255
					db    "p2a41",255
					db    "p2as",255

tabla_parametros_saltos  dw	parametro_ayuda
					dw		parametro_vga_R,parametro_vga_G,parametro_vga_B
					dw		parametro_nosb,parametro_noems
					dw    parametro_no386,parametro_ingles,parametro_espanyol
					dw    parametro_16k,parametro_48k,parametro_inves
					dw    parametro_128k,parametro_128ks,parametro_p2
					dw    parametro_p2f,parametro_p2s
					dw    parametro_p2a40,parametro_p2a41,parametro_p2as

parametro_ayuda:
					mov	ah,9
					mov	dx,offset men_ayuda
					int	21h

					mov   ah,7 ;Esperar tecla
					int   21h

					mov	ah,9
					mov   dx,offset men_ayuda2
					int	21h
					int	20h


parametro_vga_R:
					mov	parametro_vgaR,255
parametro_vgaR2:
					or    parametros_flags,00100000b

					ret
parametro_vga_G:
					mov	parametro_vgaG,255
					jmp   short parametro_vgaR2
parametro_vga_B:
					mov	parametro_vgaB,255
					jmp   short parametro_vgaR2

parametro_nosb:
					or		parametros_flags,00000100b
					ret

parametro_noems:

					or		parametros_flags,00001000b
					ret

parametro_no386:
					or		parametros_flags,00010000b
					ret

parametro_ingles:
					or		bits_estado5,00000010b
					ret
					
parametro_espanyol:
					and	bits_estado5,11111101b
					ret
					

parametro_16k:
					mov   ordenador_emulado,0
					ret
parametro_48k:
					mov   ordenador_emulado,1
					ret

parametro_inves:
					mov   ordenador_emulado,2
					ret

parametro_128k:
					mov   ordenador_emulado,3
					ret
parametro_128ks:
					mov   ordenador_emulado,10
					ret

parametro_p2:
					mov   ordenador_emulado,4
					ret

parametro_p2f:
					mov   ordenador_emulado,5
					ret

parametro_p2s:
					mov   ordenador_emulado,6
					ret

parametro_p2a40:
					mov   ordenador_emulado,7
					ret

parametro_p2a41:
					mov   ordenador_emulado,8
					ret

parametro_p2as:
					mov   ordenador_emulado,9
					ret

haz_minusculas:
;Rutina que convierte a minusculas la letra contenida en AL
					cmp	al,65
					jc		haz_minusculas_fin
					cmp	al,91
					jnc	haz_minusculas_fin
					add	al,32
haz_minusculas_fin:
					ret


inicializa_ordenador:
;Rutina para cargar la ROM segun el ordenador que se emula
;Se pagina en su sitio correspondiente (si se emula + de 48k)
;Tambien se ponen los estados, lineas, rutinas de pokeo, etc..
;Llamar al final a cambia_vectores_pit para cambiar la frecuencia
;(si es que se llama desde el menu y no al iniciar el emulador, pues
;�l solo cambiara los vectores y tambien el pit)
;Entrada: Ordenador en ordenador_emulado
;Salida: Carry si algo fue mal


COMMENT !

Ordenador que se esta emulando:
0=Sinclair 16k
1=Sinclair 48k
2=Inves Spectrum+
3=Sinclair 128k
4=Amstrad +2
5=Amstrad +2 - Frances
6=Amstrad +2 - Espa�ol
7=Amstrad +2A (ROM v4.0)
8=Amstrad +2A (ROM v4.1)
9=Amstrad +2A - Espa�ol
10=Sinclair 128k Español
Nota: La posicion normal del 128k Español seria la 4, pero este modelo se añadio en la version 3.5,
cuando ya estaban definidos los valores. Dichos valores se guardan en las cabeceras de archivos .zx y por eso no combiene cambiarlos
para poder tener compatibilidad hacia atrás
!
;Hay 3 tipos de carga ROM:
;0,1,2: Modo 48: Se carga un fichero ROM de 16k
;3,4,5,6,10: Modo 128: Se carga un fichero ROM de 32k, los primeros 16k
;al inicio del segmento de 64k, y los ultimos 16k al final
;7,8,9: Modo +2A: Se carga un fichero ROM de 64k, llenando el segmento de 64k

					call	sincronizar_reset
					;Asumir 128k (128,128s,+2,+2A), no inves, chip sonido
					and   bits_estado3,11011001b ;128k,
					;extraer cintas de entrada,salida
					and   bits_estado5,11011111b ;no inves
					and   bits_estado6,11101111b ;no 128k,+2
					and   bits_estado4,11111101b ;chip ay presente


					mov	al,ordenador_emulado
					cmp	al,11
					jc		inicializa_ordenador_2
					;Hay error, se selecciona un modelo superior
					;Poner aqui futuros modelos (+3, Pentagon...)
					stc
					ret

inicializa_ordenador_2:
					xor	ah,ah
					add	ax,ax
					mov	bx,offset t_inicializa_ordenador
					add	bx,ax
					jmp	[bx]

inicializa_16:
					mov	linea_menu_ordenador,0
					mov	dx,offset cadrom_48
					jmp	short inicializa_modo_48

inicializa_inves:
					mov   linea_menu_ordenador,2
					mov	dx,offset cadrom_Inves
					or		bits_estado5,00100000b
					mov   valor_poke_rom,255
					mov   sync_borde_sup,sync_borde_sup_inves
					mov   sync_borde_inf,sync_borde_inf_inves

					mov	bx,offset puerto_no_usado_255
					mov	[p_in_255],bx

					jmp   short inicializa_modo_48_1

inicializa_48:
					mov   linea_menu_ordenador,1
					mov	dx,offset cadrom_48

inicializa_modo_48:
					mov   sync_borde_sup,sync_borde_sup_48
					mov	sync_borde_inf,sync_borde_inf_48
					mov	bx,offset in_255_48
					mov	[p_in_255],bx


inicializa_modo_48_1:

					push  dx

					xor	al,al
					mov   ah,3
					call	paginar
					mov	al,1
					mov	ah,5+4
					call	paginar
					mov	al,2
					mov	ah,2+4
					call	paginar
					mov	al,3
					mov	ah,0+4
					call	paginar


					pop   dx


					;Aqui cargar la ROM de 16k, poner bits_estado3, syncs
					or		bits_estado3,00100000b

					;Poner Chip AY no presente
					or    bits_estado4,00000010b

					;mov 	sync_pit_freq,sync_pit_freq_48
					mov	sync_lineas,sync_lineas_48
					mov	sync_estados_linea,sync_estados_linea_48

					mov   ax,offset out_nada
					mov   [p_out_32765],ax
					mov   [p_out_8189],ax

					mov   ax,3d00h  ; Funcion 3d, 0: Lectura
					int	21h
					jnc	inicializa_modo_48_2
					ret

inicializa_modo_48_2:
					mov	bx,ax  ; BX=File handle
					mov	cx,16384  ;Longitud 16384
					xor	dx,dx  ; Direcci�n 0
					push 	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h

					pop	ds
					jnc   inicializa_modo_48_3

					ret

inicializa_modo_48_3:

					mov	ah,3eh
					int	21h   ; Cerrar fichero
					jmp   inicializa_ord_fin

inicializa_p2a41:
					mov	linea_menu_ordenador,9
					mov	dx,offset cadrom_P2A41
					jmp	short inicializa_modo_p2a

inicializa_p2as:
					mov	linea_menu_ordenador,10
					mov	dx,offset cadrom_P2AS
					jmp	short inicializa_modo_p2a

inicializa_p2a40:
					mov	dx,offset cadrom_P2A40
					mov	linea_menu_ordenador,8
inicializa_modo_p2a:
					push	dx

					;Mapear las paginas 0,1,2,3 para poder cargar las ROMS
					xor	al,al ;Pagina fisica
					xor	ah,ah ;Pagina logica

inicializa_modo_p2a_buc:
					push	ax
					call	paginar
					pop	ax
					inc   ah
					inc	al
					cmp	al,4
					jnz	inicializa_modo_p2a_buc

					mov   ax,3d00h  ; Funci�n 3d, 0: Lectura
					pop	dx
					int	21h
					jnc	inicializa_modo_p2a_2
					ret

inicializa_modo_p2a_2:
					mov	bx,ax  ; BX=File handle
					mov	cx,32768  ;Longitud 65536 (Se hace en 2 veces)
					xor	dx,dx  ; Direcci�n 0
					push 	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds
					jnc   inicializa_modo_p2a_3
					ret

inicializa_modo_p2a_3:

					mov	dx,32768
					mov	cx,dx
					push 	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds
					jnc   inicializa_modo_p2a_4
					ret

inicializa_modo_p2a_4:

					mov	ah,3eh
					int	21h   ; Cerrar fichero
					jnc   inicializa_modo_p2a_5
					ret

inicializa_modo_p2a_5:


					xor	al,al
					xor	ah,ah
					mov   si_despaginar_rom,1
					call	paginar
					mov	al,1
					mov	ah,5+4
					mov   si_despaginar_rom,1
					call	paginar
					mov	al,2
					mov	ah,2+4
					mov   si_despaginar_rom,1
					call	paginar
					mov	al,3
					mov	ah,0+4
					mov   si_despaginar_rom,1
					call	paginar

					call	inicializa_sync_128

					mov   ax,offset out_p32765
					mov   [p_out_32765],ax
					mov   ax,offset out_p8189
					mov   [p_out_8189],ax

					mov	ax,offset in_255_p2a
					mov	[p_in_255],ax

					clc
					ret


inicializa_p2:
					mov	linea_menu_ordenador,5
					mov	dx,offset cadrom_p2
					jmp	short inicializa_modo_128

inicializa_p2f:
					mov	linea_menu_ordenador,6
					mov	dx,offset cadrom_p2F
					jmp	short inicializa_modo_128

inicializa_p2s:
					mov	linea_menu_ordenador,7
					mov	dx,offset cadrom_p2S
					jmp	short inicializa_modo_128

inicializa_128:
					mov	linea_menu_ordenador,3
					mov	dx,offset cadrom_128
					jmp	short inicializa_modo_128

inicializa_128s:
					mov	linea_menu_ordenador,4
					mov	dx,offset cadrom_128s

inicializa_modo_128:
					push	dx

					or		bits_estado6,00010000b ;Decir modo 128 o +2

					;Mapear las paginas 0,3 para poder cargar las ROMS
					xor	al,al ;Pagina fisica
					xor	ah,ah ;Pagina logica

					call	paginar
					mov	al,3
					mov	ah,al
					call	paginar

					mov   ax,3d00h  ; Funci�n 3d, 0: Lectura
					pop	dx
					int	21h
					jnc	inicializa_modo_128_2
					ret

inicializa_modo_128_2:
					mov	bx,ax  ; BX=File handle
					mov	cx,16384  ;Longitud 32768 (Se hace en 2 veces)
					xor	dx,dx  ; Direcci�n 0
					push 	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds
					jnc   inicializa_modo_128_3
					ret

inicializa_modo_128_3:
					mov	dx,49152
					mov	cx,16384
					push 	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds
					jnc   inicializa_modo_128_4
					ret

inicializa_modo_128_4:

					mov	ah,3eh
					int	21h   ; Cerrar fichero
					jnc   inicializa_modo_128_5
					ret

inicializa_modo_128_5:

					xor	al,al
					xor	ah,ah
					mov   si_despaginar_rom,1
					call	paginar
					mov	al,1
					mov	ah,5+4
					mov   si_despaginar_rom,1
					call	paginar
					mov	al,2
					mov	ah,2+4
					mov   si_despaginar_rom,1
					call	paginar
					mov	al,3
					mov	ah,0+4
					mov   si_despaginar_rom,1
					call	paginar

					call	inicializa_sync_128

					mov   ax,offset out_p32765_128
					mov   [p_out_32765],ax
					mov   ax,offset out_nada
					mov   [p_out_8189],ax

					mov	ax,offset in_255_48
					mov	[p_in_255],ax

					clc
					ret

inicializa_sync_128:
					mov   word ptr puerto_32765,0
					;32765,8189=0

					;mov 	sync_pit_freq,sync_pit_freq_128
					mov	sync_lineas,sync_lineas_128
					mov	sync_estados_linea,sync_estados_linea_128
					mov   sync_borde_sup,sync_borde_sup_128
					mov	sync_borde_inf,sync_borde_inf_128


inicializa_ord_fin:

					mov   sync_lineas_actual,0
					mov   ax,sync_estados_linea
					mov   sync_estados_linea_original,ax
					;mov   sync_fin_linea,0
					mov   sync_estados_actual,0
					mov   sync_factor,100

					mov   ax,seg_spectrum
					mov   seg_pant,ax

					ret


;Tabla para saltos de los diferentes ordenadores emulados
t_inicializa_ordenador dw	inicializa_16,inicializa_48,inicializa_inves

					dw		inicializa_128,inicializa_p2
					dw		inicializa_p2f,inicializa_p2s

					dw		inicializa_p2a40,inicializa_p2a41,inicializa_p2as
					dw		inicializa_128s


fic_zx_handle	dw		?


;Tabla para saltar a la hora de leer un prefijo
;(se hace asi para que sea mas rapido y mas estructurado, pero ocupa
;mas memoria)
tabla_interprete dw 203 dup (interprete_sinpref)

					dw		interprete_203

					dw	   221-204 dup (interprete_sinpref)

					dw		interprete_221

					dw		237-222 dup (interprete_sinpref)

					dw		interprete_237

					dw		253-238 dup (interprete_sinpref)

					dw		interprete_253

					dw    interprete_sinpref,interprete_sinpref


include t_rgb.inc
include pantalla.inc
include char.inc

include int8.inc
include t_teclad.inc
include int9.inc

include menu.inc
include filesel.inc

include tape.inc
include salvapan.inc
include sblaster.inc
include 128k.inc
include ems.inc
include load.inc
include tipo_cpu.inc
include ay38912.inc
include charset.inc
include contende.inc
include t_daa.inc

codigo			ends
					end 	empezar
