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


;Este fichero contiene las rutinas del menu que aparece al pulsar la
;tecla ESC

PUBLIC menu

;salidas_altavoz	db	"PC Speaker   ",255
;					db		"Sound Blaster",255
;					db		TESP,"Nada"
;					db		TING,"None"
;					db		TCOM,"         ",255

;modo_salidas_altavoz db "Int.",255
;					db		"Dir.",255

modos_stereo	db		"Mono",255
					db		"ACB ",255
					db		"ABC ",255
					
modos_sincronismo	db	"Timer         ",255
					        db	"SBlaster      ",255
					        db  "SBlaster+Timer",255


;frecuencias_salidas_altavoz db "8",255
;					db		"5",255

;actualizaciones_pantalla db TESP,"Baja"
;					db		TING,"Low ",255
;					db		TESP,"Alta"
;					db		TING,"High"
;					db		255

buffer_numero	db		6 dup (?) ;Temporal para varias funciones
cad_pantalla	db    79 dup (?) ;Nombre de la pantalla cargada/grabada
cad_tap			db		"*.tap",0
					db		0

cad_zxsp			db		"*.sp",0
cad_zx			db		"*.zx",0
					db		0

cad_scr			db		"*.scr",0
					db		0

cad_raw_audio			db		"*.rwa",0
					db		0

cad_raw_video			db		"*.vga",0
					db		0


old_border		db	? ;Usado al cargar directo desde casette
old_logica0_temporal	db		? ;Temporal para cinta_ins_entrada_fin1

linea_menu_ordenador db 0 ;Linea que indica que opcion del menu de ordenador
;se seleccionara. Cuando se entra en menu_ordenador, se copia este valor
;a tabla_menu_ordenador

menu_inicio_mensajes_io_error db TESP,"Error escribiendo archivo sonido"
					db	TING,"Error writing output sound file"
					db 255
;1
					db TESP,"Error leyendo cinta de entrada"
					db	TING,"Error reading input tape"
					db 255
;2
					db TESP,"Error grabando cinta de salida"
					db	TING,"Error writing output tape"
					db 255
;3

COMMENT !
carga_programa_error	db ?

Cuando hay error al cargar programa (bit 0 de bits_estado3)
sale el mensaje de error aqui indicado:
1: Error de disco
2: Fichero es de 128k y solo hay 48k - NO usado a partir version 3.0
3: Tipo desconocido (ni SP ni ZX)
4: Error al abrir fichero: Posiblemente, no encontrado

					db		TESP,"Error al cargar programa!"
					db		TING,"Error when loading program!"
					db	255
;4
					db		255 ; no usado
;5
					db		TESP,"Tipo de fichero desconocido!"
					db		TING,"Unknown File Format!"
					db		255
;6
					db		TESP,"Fichero no encontrado!"
					db		TING,"File not found!"
					db		255
;7
					db		TESP,"Error grabando snapshot!"
					db		TING,"Error saving snapshot!"
					db 255
;8
					db TESP,"Error escribiendo archivo video"
					db	TING,"Error writing output video file"
					db 255


liberar_escape:
					mov	interrumpir,0 ;Liberar ESC
					and	bits_estado2,11110111b ;Liberar interrumpir
					ret

menu_inicio_error_io:
					;Quitamos el error
					and	bits_estado8,11111101b
					
					mov	al,error_io_num
menu_inicio_error_io2:
					mov	bx,offset menu_inicio_mensajes_io_error
					call	print_cadenas
					call	espera_tecla
					jmp	short menu_inicio


menu:          cargar_reg_f ;Guarda flags para poder grabar luego
					mov	reg_pc,si
					mov	reg_sp,di
					call	liberar_escape
					or	   bits_estado4,00100000b ;Decir que esta el menu activo
					; para no POKEar normal en pantalla...

					;Si esta en modo pausa, silenciar canales
;					test  bits_estado5,00000001b
;					jz    menu_inicio

;					call  silenciar_canales

menu_inicio:
					call	actualiza_y_espera
					call	liberar_teclas
menu_inicio00:
					mov	word ptr cursor_x,0
					test	bits_estado3,1

					jnz	menu_inicio000

					test	bits_estado8,00000010b
					jnz	menu_inicio_error_io

					jmp	menu_inicio0

menu_inicio000:
					and	bits_estado3,11111110b
					mov	al,carga_programa_error
					add	al,2
					jmp	short menu_inicio_error_io2

COMMENT *
					cmp	al,1
					jnz	menu_inicio_noer1
					call	print_cadena
					db		13
					db		TESP,"Error al cargar programa!"
					db		TING,"Error when loading program!"
					db		255
menu_inicio_er1:
					call	espera_tecla
					jmp	short menu_inicio

menu_inicio_noer1:

menu_inicio_noer2:
					cmp	al,3
					jnz	menu_inicio_noer3
					call	print_cadena
					db		13
					db		TESP,"Tipo de fichero desconocido!"
					db		TING,"Unknown File Format!"
					db		255
					jmp	short menu_inicio_er1

menu_inicio_noer3:
					call	print_cadena
					db		13
					db		TESP,"Fichero no encontrado!"
					db		TING,"File not found!"
					db		255
					jmp	menu_inicio_er1
*

menu_inicio0:
					call	print_cadena
					;      01234567890123456789012345678901
					db    TESP,"Emulador de ZX Spectrum   v."
					db    TING,"ZX Spectrum Emulator      v."
					db    TCOM
					macro_version
					db    " "
					db		"(C) Cesar Hernandez  "
					macro_fecha
					db		" "
					db		"------------------------------- "
					db		TESP
					db		"Menu principal                  "
					db		"                                "
					db		"    Cargar Snapshot             "
					db		"    Grabar Snapshot             "
					db    "    Seleccion de Ordenador      "
					db		"    Ajustes de Pantalla         "
					db		"    Menu Depuracion             "
					db		"    Emulacion de Cinta          "
					db		"    Ajustes de Hardware         "
					db		"    Ajustes de Sonido           "
					db		"    Seleccion de Idioma         "
					db		"    Multitarea:   "

					db		TING
					db		"Main Menu                       "
					db		"                                "
					db		"    Load Snapshot               "
					db		"    Save Snapshot               "
					db    "    Machine Selection           "
					db    "    Screen Settings             "
					db		"    Debug Menu                  "
					db		"    Tape Emulation              "
					db		"    Hardware Settings           "
					db		"    Audio Settings              "
					db		"    Language Selection          "
					db		"    Multitasking: "

					db		255
					test	bits_estado5,1
					call	print_siZ
					call	print_cadena
					db    "           "
					db		TESP,"    Velocidad CPU: "
					db		TING,"    CPU Speed:     ",255

					mov	ax,sync_factor
					call  print_num_spc

					call	print_cadena
					db		"%       "
					db		"                                "
					
					db		TESP,"    F1  Ayuda"
					db		TING,"    F1  Help "
					db		TCOM,"                   "

					db		TESP,"    F2  Volver al Emulador      "
					db		"    F10 Salir del Emulador      "
					db		TING,"    F2  Back to the Emulator    "
					db		"    F10 Exit Emulator           "
					db		255

					mov	bx,offset tabla_menu_menu
					call	tecla_menu
					jnz   menu_tecla_salvapan
					jmp	bx

menu_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_inicio

menu_pausa:		xor	bits_estado5,1
					and	bits_estado2,11101111b ;Decir que no ha habido INT Z80
					
					mov	primera_vez,0
;					call  silenciar_canales
					call  sincronizar_reset
					jmp	menu_inicio00

menu_idioma:	xor	bits_estado5,00000010b
					jmp	menu_inicio00
					
menu_ayuda:		call	actualiza_y_espera

menu_ayuda0:	mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901
					
					db		TESP,"Ayuda                           "
					db		"-----"
					
					db		TING,"Help                            "
					db		"---- ",255
					

					mov   ax,59*256+32
					call	print_caracter_veces
					
					call print_cadena
			
					db		TESP
					db		"Seleccion opcion:          Enter"
					db		"Incrementar/Decrementar:   +,-  "
					db		"                                "
					db		"Seleccion de archivo muestra F> "
					db		" Seleccion archivo:        Enter"
					db		" Escribir nombre arch.:  SPC o -"
					db    "                                "
					db    "Opcion no disponible muestra  -x"

					db		TING
					db		"Choose option:        Enter     "
					db		"Increase/Decrease:    +,-       "
					db		"                                "
					db		"Filename option shows F>        "
					db		" Choose file:         Enter     "
					db		" Write filename:      SPC or -  "
					db    "                                "
					db    "Option unavailable shows -x     "
					
					
					db    255
					

					mov   ax,32*256+32
					call	print_caracter_veces

					
menu_ayuda_lee:
					call	lee_teclas_and
					jnz   menu_ayuda_fin

					;Mirar si se ha de activar el salvapantallas
					test	bits_estado2,4
					jnz   menu_ayuda_lee2

					jmp   menu_ayuda0

menu_ayuda_lee2:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_ayuda


menu_ayuda_fin:
					jmp	menu_inicio
					
								

menu_velocidad:
					mov   word ptr cursor_x,0f13h
					mov	bx,offset buffer_numero
					mov   byte ptr [bx],0
					mov	cx,0505h
					push	bx
					call	input
					pop	bx
					jnc   menu_velocidad_fin

					call	convertir_ascii_num
					or		ax,ax
					jz		menu_velocidad_fin
					cmp   ax,20000 ;Ponerle un tope
					;de velocidad, pues si
					;ponemos vel. 30000: 30000*228/100=
					;=68400, y generaria INT 0
					jnc   menu_velocidad_fin


					mov	sync_factor,ax
					mul   sync_estados_linea_original
					mov	bx,100

					div	bx
					mov	sync_estados_linea,ax
					
					

menu_velocidad_fin:
					jmp	menu_inicio

menu_cargar_filesel:
					mov	bx,offset cad_zxsp
					mov	dx,offset cadcargar
					call	filesel
					pushf
					call	actualiza_y_espera
					popf
					mov	word ptr cursor_x,0
					cmc

					jmp	short menu_cargar0

menu_cargar:
					call	actualiza_y_espera
					mov	word ptr cursor_x,0
					call	print_cadena
					db		TESP,"Nombre del fichero:"
					db		TING,"File name:"

					db		255

					mov	bx,offset cadcargar
					mov	cl,70
					mov	ch,8+4
					call	input
menu_cargar0:
					jnc	menu_cargar_fin

					call	restaurar_vectores

					call	carga_programa

					and	bits_estado2,11101111b ;Decir que no ha habido INT Z80

					call	cambiar_vectores
					mov al,border
					mov puerto_254,al
					call	act_pantalla_border
					;Actualizar brillo
					call	vga_actualiza_brillo
					
					;mov	primera_vez,0


menu_cargar_fin:
					jmp	menu_inicio

menu_grabar_filesel:
					mov	bx,offset cad_zx
					mov	dx,offset cadgrabar
					call	filesel
					pushf
					call	actualiza_y_espera
					popf
					mov	word ptr cursor_x,0
					cmc

					jmp	short menu_grabar0


menu_grabar:	call	actualiza_y_espera
					mov	word ptr cursor_x,0
					call	print_cadena
					db		TESP,"Nombre del fichero:"
					db		TING,"File name:"
					db		255

					mov	bx,offset cadgrabar

					mov	cl,79
					mov	ch,8+4
					call	input
menu_grabar0:
					jc		menu_grabar1
					jmp	menu_inicio

menu_grabar1:
					cli
					call	restaura_vectores
					sti

					mov	ah,5bh
					xor	cx,cx
					mov	dx,offset cadgrabar
					int	21h


					jc		menu_grabar10
					jmp	menu_grabar2
menu_grabar10:
					cmp	ax,50h
					jnz	menu_grabar_no_pregunta

					call	menu_grabar_pregunta
					jz		menu_grabar_reemplaza
					clc
					jmp	menu_grabar_error


menu_grabar_no_pregunta:
					stc
					jmp	menu_grabar_error

menu_grabar_pregunta:
					cli
					call	cambia_vectores
					mov	contador_disco,50*segundos
					mov	es,seg_spectrum
					sti

					call	print_cadena
					db		13
					db		TESP,"El fichero ya existe,",13
					db		"desea reemplazarlo ? (S/N)"
					db		TING,"File already exists,",13
					db		"overwrite ? (S/N)"

					db		255
					call	lee_siono
					pushf

					cli
					call	restaura_vectores
					sti

					popf
					ret

menu_grabar_reemplaza:

					mov	ah,3ch ;Reemplazar fichero
					xor	cx,cx
					mov	dx,offset cadgrabar
					int	21h
					jnc	menu_grabar2
					jmp 	menu_grabar_error

menu_grabar2:  ;El fichero, como no existe, se ha creado

					;Cambiar signatura de cabecera a ZX
					mov	byte ptr signatura,'Z'
					mov 	byte ptr signatura+1,'X'
					;Poner numero de version de cabecera
					mov	version,VERS

					;Poner ordenador que se emula
					mov	bl,ordenador_emulado
					mov	cabecera_ordenador_emulado,bl

					;Poner datos en la cabecera extendida
					mov	bl,bits_estado2
					and	bl,01100010b
					mov	bh,puerto_65278
					and	bh,64
					rol	bh,1
					or		bl,bh


COMMENT !
					cabecera_bits_estado				db		0+2 ;Protector de pantallas activo
;Se guardan varios bits en el fichero ZX.El significado es el siguiente:
Bit	Significado
---	-----------
7     Contiene el bit 6 del puerto del teclado (Issue 1 o 2)
6		Indica si est� activado el disparador autom�tico
5		A 0 indica que el refresco de Flash est� activo, sino a 1 - no usado
4     A 1 indica que el programa a cargar es de 128k (version 2+)
3     A 1 indica que no es posible cambiar el color del borde mediante
		OUT 211,valor (version 2+)
2     No usado
1	 	A 1 indica que est� habilitado el protector de pantallas
0		Paleta CGA (version 2+)
		;0=Negro,Verde,Rojo,Amarillo
		;1=Cyan,Magenta,Blanco

!
					;Ahora hay que ver si el programa a grabar es de 48k o 128k
					;Si se esta en modo 48k,o si esta la ROM3 y
					;la paginacion bloqueada, tambien se considera modo 48k
					;En caso contrario, es modo 128k

					cmp   ordenador_emulado,3
					jc    menu_grabar_48k


menu_grabar_128k:
					;Es modo 128k

					or		bl,00010000b

					mov	cabecera_bits_estado,bl

					;Copiar valores de puertos
					mov	bx,word ptr puerto_32765
					mov	word ptr cabecera_puerto_32765,bx

					;Copiar paginas activas
					mov	bx,word ptr paginas_actuales
					mov	word ptr cabecera_paginas_actuales,bx
					mov	bx,word ptr paginas_actuales+2
					mov	word ptr cabecera_paginas_actuales+2,bx

					jmp	short menu_grabar_48k_2


menu_grabar_48k:

					mov	cabecera_bits_estado,bl

menu_grabar_48k_2:

					mov	bl,control_brillo
					mov	cabecera_control_brillo,bl

					mov	bl,disparador_defecto
					mov	cabecera_disparador_defecto,bl

;					mov	bl,sonido
;					mov	cabecera_sonido,bl
					mov	bl,bits_estado8
					and	bl,00000001b
					xor	bl,1
					mov	cabecera_sonido,bl

					mov	bx,ax

					mov	al,reg_r
					and	al,01111111b
					or		al,reg_r_bit7
					mov	reg_r,al


					mov	menu_grabar_handle,bx
					mov	cx,38+256	 ; Longitud 38+256
					mov	dx,offset signatura
					mov	ah,40h
					int	21h
					jnc	menu_grabar3
					jmp	menu_grabar_error0

menu_grabar3:

					;Ver si es modo 128k
					test	cabecera_bits_estado,00010000b
					jz		menu_grabar3_48k

					;Es de 128k
					;Paginar RAMS 0,1,2,3

					xor	al,al
					mov	ah,4

					call	menu_grabar_3_128k_pagina

					;Ahora hay que grabar 65536 bytes
					mov	es,seg_spectrum
					mov	posicion_buffer_disco,0

					xor	bx,bx
					mov	ch,0
					call	menu_grabar_bucle0

					;Paginar RAMS 4,5,6,7

					xor	al,al
					mov	ah,8

					call	menu_grabar_3_128k_pagina

					;Ahora hay que grabar 65536 bytes
					mov	es,seg_spectrum

					xor	bx,bx
					mov	ch,0
					call	menu_grabar_bucle0

					;Restaurar posicion de las paginas anteriores

					xor	al,al
					mov	bx,offset cabecera_paginas_actuales

menu_grabar_3_128_buc:
					mov	ah,byte ptr [bx]
					push	ax
					push	bx
					call	paginar
					pop	bx
					pop	ax
					inc	bx
					inc	al
					cmp	al,4
					jnz   menu_grabar_3_128_buc

					jmp	menu_grabar_cierra


menu_grabar_3_128k_pagina:
					push	ax
					call	paginar
					pop	ax
					inc	al
					inc	ah
					cmp	al,4
					jnz	menu_grabar_3_128k_pagina
					ret

menu_grabar3_48k:


					call	menu_grabar4
					jmp	menu_grabar_cierra


menu_grabar4:
					mov	es,seg_spectrum

					mov	posicion_buffer_disco,0
					;Aqui se dispone a grabar los datos comprimidos
					;En AL se tiene el dato actual, en AH el dato anterior
					;y en CL el numero de repeticiones
					;En CH se guarda el byte leido sin repetici�n.
					;Si es un 221 y inmediatamente luego sigue una secuencia
					;de compresi�n,se grabar� como:
					;221 byte 221 221 byte veces-1

					;En BX se tiene la direccion
					mov	bx,16384
					mov	ch,0
menu_grabar_bucle0:
					mov	ah,byte ptr es:[bx]
					inc	bx
menu_grabar_bucle:
					cmp	bx,0

					jnz   menu_grabar_bucle1
					jmp   menu_grabar_cierra0

menu_grabar_bucle1:
					mov	al,byte ptr es:[bx]
					cmp	al,ah
					jnz	menu_grabar_no_repe
					mov	cl,2
					inc	bx
menu_grabar_bucle2:
					cmp	bx,0
					jz		menu_grabar_bucle2_sigue
					mov	al,byte ptr es:[bx]
					inc	bx
					cmp	al,ah
					jnz	menu_grabar_bucle2_sigue
					inc	cl
					cmp	cl,0
					jz    menu_grabar_compresion

					jmp	short menu_grabar_bucle2

menu_grabar_bucle2_sigue:
					cmp	ah,221
					jz    menu_grabar_compresion
					cmp	cl,5
					jc		menu_grabar_cl_veces

menu_grabar_compresion:
					cmp	ch,221
					jnz	menu_grabar_compresion2
					;Inmediatamente antes hab�a un 221
					call	menu_grabar_un_byte
					push	ax
					mov	ah,221
					call	menu_grabar_un_byte
					call	menu_grabar_un_byte
					pop	ax

					call	menu_grabar_un_byte
					mov	ah,cl
					dec	ah
					call	menu_grabar_un_byte
					jmp	short menu_grabar_compresion3

menu_grabar_compresion2:
					push	ax
					mov	ah,221
					call	menu_grabar_un_byte
					call	menu_grabar_un_byte
					pop	ax

					call	menu_grabar_un_byte
					mov	ah,cl
					call	menu_grabar_un_byte
					xor   ch,ch
					mov   ah,al
					cmp   bx,0
					jz    menu_grabar_cierra00
					jmp   short menu_grabar_compresion30
menu_grabar_compresion3:
					xor	ch,ch
					mov	ah,al
					cmp	bx,0
					jz		menu_grabar_cierra0
menu_grabar_compresion30:
					cmp	cl,0
					jnz	menu_grabar_bucle
					jmp   menu_grabar_bucle0

menu_grabar_cl_veces: ;Graba el byte ah cl veces
					call	menu_grabar_un_byte
					dec	cl
					jnz	menu_grabar_cl_veces
					mov	ah,al
menu_grabar_cl_veces2:
					jmp   menu_grabar_bucle

menu_grabar_no_repe:
					mov	ch,ah
					call	menu_grabar_un_byte
					mov	ah,al
					inc	bx
					jmp	short menu_grabar_cl_veces2

menu_grabar_cierra0:
					call	menu_grabar_un_byte
menu_grabar_cierra00:


					ret
menu_grabar_cierra:
					;Cerrar fichero

					mov	cx,posicion_buffer_disco
					jcxz	menu_grabar_cierra2
					xor	dx,dx
					mov	bx,menu_grabar_handle
					push	ds
					mov	ds,seg_buffer_disco
					mov	ah,40h
					int	21h
					pop	ds


menu_grabar_cierra2:
					mov	ah,3eh
					int	21h
					jmp	short menu_grabar_error

menu_grabar_error0:
;Se salta aqui en caso de error despues de abrir fichero

					mov	ah,3eh
					mov	bx,menu_grabar_handle
					int	21h
					stc

menu_grabar_error:
					;Se salta aqui en caso de error en abrir fichero o despu�s de
					;grabar con �xito

					pushf
					cli
					call	cambia_vectores
					mov	contador_disco,50*segundos
					mov	es,seg_spectrum
					sti
					popf
					jc		menu_grabar_error2
					jmp	menu_inicio

menu_grabar_error2:
;Avisamos error io 
						or		bits_estado8,00000010b
;Numero de error
						mov error_io_num,7


;					call	print_cadena
;					db		13
;					db		TESP,"Error de disco!"
;					db		TING,"Disk error!"
;					db		255
;					call	espera_tecla
;call	espera_tecla

					jmp 	menu_inicio

menu_grabar_un_byte:  ;Grabar byte AH
					push	cx
					push	bx
					push	ax
					cmp	posicion_buffer_disco,BUFFER_DISCO
					jnz   menu_grabar_un_byte2
					mov	cx,BUFFER_DISCO
					xor	dx,dx
					mov	bx,menu_grabar_handle
					push	ds
					mov	ds,seg_buffer_disco
					push	ax
					mov	ah,40h
					int	21h
					pop	ax
					pop	ds
					mov	posicion_buffer_disco,0


menu_grabar_un_byte2:

					mov	bx,posicion_buffer_disco
					push	ds
					mov	ds,seg_buffer_disco
					mov	byte ptr [bx],ah
					inc	bx
					pop	ds
					mov	posicion_buffer_disco,bx
					pop	ax
					pop	bx
					pop	cx
					clc
					ret


menu_grabar_handle	dw	?

menu_ordenador:
					mov	al,linea_menu_ordenador
					mov	tabla_menu_ordenador,al
					call	actualiza_y_espera
menu_ordenador0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;          "01234567890123456789012345678901
					db    TESP,"Seleccion de Ordenador          "
					db         "----------------------          "
					db    TING,"Machine Selection               "
					db         "-----------------               "
					db    TCOM
					db		     "                                "
					db    "    "
					db    "Spectrum 16k                    "
					db    "Spectrum 48k                    "
					db    "Inves Spectrum +                "
					db    "Spectrum 128k                   "
					db    "Spectrum 128k ("
					db    TESP,"Castellano)"
					db    TING,"Spanish)   "
					db    TCOM,"      "
					db    "Spectrum +2                     "
					db    "Spectrum +2 ("
					db    TESP,"Frances)"
					db    TING,"French) "
					db    TCOM,"           "
					db    "Spectrum +2 ("
					db    TESP,"Castellano)"
					db    TING,"Spanish)   "
					db    TCOM,"        "
					db    "Spectrum +2A (ROM v4.0)         "
					db    "Spectrum +2A (ROM v4.1)         "
					db    "Spectrum +2A ("
					db    TESP,"Castellano)"
					db    TING,"Spanish)   "
					db    TCOM,"       "
					db    "                                "
					db		TESP,"ESC Volver al Menu Principal"
					db		TING,"ESC Back to the Main Menu   "
					db		255

					mov   bx,offset tabla_menu_ordenador
					call	tecla_menu
					jnz   menu_ordenador_tecla_salvapan
					jmp	bx
menu_ordenador_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp   menu_ordenador0

ordenador_16k:
					mov   ordenador_emulado,0
					jmp   short menu_ordenador_cambia
ordenador_48k:
					mov   ordenador_emulado,1
					jmp   short menu_ordenador_cambia
ordenador_inves:
					mov   ordenador_emulado,2
					jmp   short menu_ordenador_cambia
ordenador_128k:
					mov   ordenador_emulado,3
					jmp   short menu_ordenador_cambia
ordenador_128ks:
					mov   ordenador_emulado,10
					jmp   short menu_ordenador_cambia
ordenador_p2:
					mov   ordenador_emulado,4
					jmp   short menu_ordenador_cambia
ordenador_p2f:
					mov   ordenador_emulado,5
					jmp   short menu_ordenador_cambia
ordenador_p2s:
					mov   ordenador_emulado,6
					jmp   short menu_ordenador_cambia
ordenador_p2a40:
					mov   ordenador_emulado,7
					jmp   short menu_ordenador_cambia
ordenador_p2a41:
					mov   ordenador_emulado,8
					jmp   short menu_ordenador_cambia
ordenador_p2as:
					mov   ordenador_emulado,9
					jmp   short menu_ordenador_cambia

menu_ordenador_cambia:
					call	restaurar_vectores
					call  inicializa_ordenador
					cli
					jnc   menu_ordenador_cambia2

					call	cambia_vectores
					sti
					call	actualiza_y_espera
					mov	word ptr cursor_x,0

					call	print_cadena
					db    TESP,"Error al cargar ROM!"
					db    TING,"Error when loading ROM!"
					db    255
					call  espera_tecla
					jmp   menu_inicio


menu_ordenador_cambia2:
					call  cambia_vectores
					mov   reg_pc,0
					and   bits_estado2,11101111b ;Liberar si hay interrupcion pendiente
					mov   interrumpir,0
					sti
					jmp   menu_inicio

menu_pantalla:
					call	actualiza_y_espera
menu_pantalla0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;          "01234567890123456789012345678901
					db		TESP,"Ajustes de Pantalla             "
					db		     "-------------------             "
					db		TING,"Screen Settings                 "
					db		     "---------------                 "
					db		TCOM
					db		"                                "
					db		"    "
					db		TESP,"Cargar Pantalla"
					db		TING,"Load Screen    "
					db		TCOM,"             "
					db		"    "
					db		TESP,"Grabar Pantalla"
					db		TING,"Save Screen    "
					db		TCOM,"             "

					db		"    "
					db		TESP,"Control de Brillo: "
					db    TING,"Brightness Control:"
					db		255
					mov	al,control_brillo
					push	ax
					cmp	al,128
					jnc	pantalla_brillo_men
					mov	al,'+'
					call	print_caracter
					pop	ax
					jmp	short pantalla_brillo_men2
pantalla_brillo_men:
					mov	al,'-'
					call	print_caracter
					pop	ax
					neg   al
pantalla_brillo_men2:
					mov	bl,32
					call	print_num8
					call	print_cadena
					db		"   ",255
pantalla_vga_sigue:

					call	print_cadena
					db		"    "
					db		TESP,"Protector de Pantalla: "
					db		TING,"Screen Saver:          "
					db		255
					test	bits_estado2,2
					call	print_siNZ

pantalla_no_protector_sigue:
					call	print_cadena
					db		"  "

					db  255

					test	bits_estado8,00000100b
					jnz		menu_pantalla_raw_record

					call  print_cadena
					db		"                                "
					db		"    "
					db		TESP,"Iniciar grabacion video"
					db		TING,"Start recording video  "
					db		TCOM,"     ",255
					jmp	short menu_pantalla_raw_record2

menu_pantalla_raw_record:
					call  print_cadena
					db		"                                "
					db		"    "
					db		TESP,"Detener grabacion video"
					db		TING,"Stop  recording video  "
					db		TCOM,"     ",255


menu_pantalla_raw_record2:
					call	print_cadena
					db		"    "
					db		TESP,"Fichero:"
					db		TING,"File:   "

					db		255
					mov	bx,offset write_video_nombre
					mov	ah,20
					call	print_cadena_longitud
					mov	al,32
					call	print_caracter_veces		

					call print_cadena
					db		"    FPS: ",255
			
					;50 / frames_video_file

					mov	al,50
					xor	ah,ah
					div   frames_video_file

					call	print_num8_spc

					call print_cadena
					db		"                  "

					db		"                                "
					db		"    "
					db		TESP,"ESC Volver al Menu Principal"
					db		TING,"ESC Back to the Main Menu   "
					db		255

					mov	bx,offset tabla_menu_pantalla
					call	tecla_menu
					jnz   menu_pantalla_tecla_salvapan
					jmp	bx
menu_pantalla_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_pantalla0

pantalla_brillo_menos:
					mov	al,control_brillo
					or		al,al
					jns 	pantalla_brillo_menos2
					cmp   al,-57-5
					jc		pantalla_brillo_menos_fin
pantalla_brillo_menos2:
					dec	al
					mov	control_brillo,al
					call	vga_actualiza_brillo
pantalla_brillo_menos_fin:

					jmp	menu_pantalla0

pantalla_brillo_mas:

					mov	al,control_brillo
					or		al,al
					js		pantalla_brillo_mas2
					cmp   al,24-5
					jnc	pantalla_brillo_mas_fin
pantalla_brillo_mas2:
					inc	al
					mov	control_brillo,al
					call	vga_actualiza_brillo
pantalla_brillo_mas_fin:
					jmp	menu_pantalla0

pantalla_protector:
					xor	bits_estado2,2
					jmp	menu_pantalla0

;pantalla_actualizacion:
;					xor	bits_estado6,01000000b
;					call	sincronizar_reset
;					jmp	menu_pantalla0

;pantalla_frameskip_cambiar:
;					mov	word ptr cursor_x,0812h
;					mov	bx,offset buffer_numero
;					mov   byte ptr [bx],0
;					mov	cx,0202h
;					push	bx
;					call	input
;					pop	bx
;					jnc   pantalla_frameskip_cambiar2

;					call	convertir_ascii_num
;					or		al,al
;					jz		pantalla_frameskip_cambiar2

;					mov	pantalla_frameskip,al
;pantalla_frameskip_cambiar2:
;					jmp	menu_pantalla0


pantalla_record_ins:

					test	bits_estado8,00000100b
					jnz	pantalla_record_ins_ext


COMMENT !


bits_estado8	db		0+4   ;temporal

  Byte de estado no 7. Codificaci?n:
  bit
  ---
  7-3 No usados
  2   A 1 indica que hay que grabar video en archivo de salida
  1   A 1 indica que ha habido un error al grabar, cargar, output file sound,etc. Ver variable error_io_num. 
Intentare reconvertir la gestion de errores a este formato nuevo. Ver output file sound
  0   A 1 indica que el sonido esta silenciado
!




						
					;Mirar si existe el fichero
					mov	dx,offset write_video_nombre
					call	crea_si_no_existe
					jz		pantalla_record_ins2
					jmp	menu_pantalla

pantalla_record_ins2:
					or		bits_estado8,00000100b
					mov	frames_video_file_actual,0
					jmp	menu_pantalla0

pantalla_record_ins_ext:
					and  	bits_estado8,11111011b
					jmp	menu_pantalla0
					


pantalla_record_fic_filesel:
					and  	bits_estado8,11111011b ;Decir que no hay archivo raw insertado
					mov	bx,offset cad_raw_video
					mov	dx,offset write_video_nombre
					call	filesel
					pushf
					call  actualiza_y_espera
					popf
					mov   word ptr cursor_x,0
					cmc
					jmp	short pantalla_record_fic0

pantalla_record_fic:
					and  	bits_estado8,11111011b ;Decir que no hay archivo raw insertado
					mov	word ptr cursor_x,090Ch
					mov	bx,offset write_video_nombre
					mov	cl,79
					mov	ch,19
					call	input
pantalla_record_fic0:
					jc		pantalla_record_fic_fin
					mov	byte ptr write_video_nombre,0

pantalla_record_fic_fin:
					jmp	menu_pantalla0





;         frames_video_file
;50 FPS = 1
;25 FPS = 2
;10 FPS = 5
; 5 FPS = 10
; 2 FPS = 25
; 1 FPS = 50


pantalla_tabla_frames_record db 1,2,5,10,25,50



pantalla_record_fps_menos:
					mov	al,frames_video_file
					cmp	al,50
					jnc 	pantalla_record_fps_mef

					call pantalla_record_busca_fps

					mov al,byte ptr [bx+1]
					mov   frames_video_file,al


pantalla_record_fps_mef:
					jmp	menu_pantalla0

pantalla_record_fps_mas:
					mov	al,frames_video_file
					cmp	al,1
					jz		pantalla_record_fps_maf

					call pantalla_record_busca_fps

					mov al,byte ptr [bx-1]
					mov   frames_video_file,al

pantalla_record_fps_maf:
					jmp	menu_pantalla0

pantalla_record_busca_fps:
					mov bx,offset pantalla_tabla_frames_record
pantalla_record_busca_fps_buc:					
					cmp	al,byte ptr [bx]
					jz pantalla_record_busca_fps_buc_fin
					inc bx
					jmp short pantalla_record_busca_fps_buc


pantalla_record_busca_fps_buc_fin:	
					ret



pantalla_cargar_filesel:
					mov	bx,offset cad_scr
					mov	dx,offset cad_pantalla
					call	filesel
					pushf
					call	actualiza_y_espera
					popf
					mov	word ptr cursor_x,0
					cmc

					jmp	short pantalla_cargar0


pantalla_cargar:
					call	actualiza_y_espera
					mov	word ptr cursor_x,0
					call	print_cadena
					db		TESP,"Nombre del fichero:"
					db		TING,"File name:"

					db		255

					mov	bx,offset cad_pantalla
					mov	cl,79
					mov	ch,8+4
					mov	byte ptr [bx],0
					call	input
pantalla_cargar0:
					jnc	pantalla_cargar_fin

					cli
					call	restaura_vectores
					sti

					mov	dx,offset cad_pantalla
					mov   ax,3d00h  ; Funci�n 3d, 0: Lectura

					int	21h
					jc		pantalla_cargar_error

					mov	bx,ax  ; BX=File handle

					push	bx
					;		mov	fic_zx_handle,bx
					mov	cx,6912	 ; Longitud total: 38
					mov	dx,16384
					push	ds
					mov	ds,seg_spectrum
					mov	ah,3fh
					int	21h
					pop	ds
					pop	bx

					jc		pantalla_cargar_error

					mov	ah,3eh
					int	21h   ; Cerrar fichero

pantalla_cargar_error:

					pushf
					cli
					call	cambia_vectores
					mov	contador_disco,50*segundos
					mov	es,seg_spectrum
					sti
					popf
					jnc   pantalla_cargar_fin

					call	print_cadena
					db		13
					db		TESP,"Error al cargar pantalla!"
					db		TING,"Error when loading screen!"
					db		255
					call	espera_tecla

pantalla_cargar_fin:

					jmp	menu_pantalla

pantalla_grabar_filesel:
					mov	bx,offset cad_scr
					mov	dx,offset cad_pantalla
					call	filesel
					pushf
					call	actualiza_y_espera
					popf
					mov	word ptr cursor_x,0
					cmc

					jmp	short pantalla_grabar0


pantalla_grabar:
					call	actualiza_y_espera
					mov	word ptr cursor_x,0
					call	print_cadena
					db		TESP,"Nombre del fichero:"
					db		TING,"File name:"
					db		255

					mov	bx,offset cad_pantalla
					mov     byte ptr [bx],0

					mov	cl,79
					mov	ch,8+4
					call	input
pantalla_grabar0:
					jc		pantalla_grabar2
					jmp	pantalla_grabar_fin

pantalla_grabar2:
					cli
					call	restaura_vectores
					sti

					mov	ah,5bh
					xor	cx,cx
					mov	dx,offset cad_pantalla
					int	21h
					mov	bx,ax

					jnc   pantalla_grabar3

					cmp	ax,50h
					jnz	pantalla_grabar_no_pregunta
					call	menu_grabar_pregunta
					jz		pantalla_grabar_reemplaza
					clc
					jmp	menu_grabar_error

pantalla_grabar_no_pregunta:
					stc
					jmp	pantalla_grabar_error

pantalla_grabar_reemplaza:
					cli
					call	restaura_vectores
					sti

					mov	ah,3ch ;Reemplazar fichero
					xor	cx,cx
					mov	dx,offset cad_pantalla
					int	21h
					mov	bx,ax
					jc		pantalla_grabar_error

pantalla_grabar3:

					push	bx
					mov	cx,6912

					mov	dx,16384
					push	ds
					mov	ds,seg_spectrum
					mov	ah,40h
					int	21h

					pop	ds
					pop	bx
					jc		pantalla_grabar_error

					mov	ah,3eh
					int	21h

pantalla_grabar_error:


					pushf
					cli
					call	cambia_vectores
					mov	contador_disco,50*segundos
					mov	es,seg_spectrum
					sti
					popf
					jnc   pantalla_grabar_fin

					call	print_cadena
					db		13
					db		TESP,"Error al grabar pantalla!"
					db		TING,"Error when saving screen!"
					db		255
					call	espera_tecla

pantalla_grabar_fin:

					jmp	menu_pantalla


vga_actualiza_brillo:	;Actualiza valores de los colores segun el brillo actual
;Cada trio de color se extrae de buffer_paleta y se pone el color de
;acorde con el brillo en la paleta de destino. Luego se llama
;a inicializa_paleta y a pon_color_borde

					cli
					mov	si,offset buffer_paleta
					mov	di,brillo0
					mov	cx,16
actualiza_brillo0:
					push	cx
					lodsb
					mov	cl,al
					lodsb
					mov	ah,al
					lodsb
					mov	dl,cl
					call	incrementa_color
					mov	byte ptr [di],dl
					inc 	di
					mov	dl,ah
					call	incrementa_color
					mov	byte ptr [di],dl
					inc 	di
					mov	dl,al
					call	incrementa_color
					mov	byte ptr [di],dl
					inc 	di
					pop	cx
					loop	actualiza_brillo0
					call	vga_inicializa_paleta
					sti
					ret

incrementa_color:
					or		dl,dl
					jz		incrementa_color_fin
					add	dl,control_brillo
					jns	incrementa_color2
					mov	dl,0
					jmp	short incrementa_color_fin
incrementa_color2:
					cmp	dl,64
					jc		incrementa_color_fin
					mov	dl,63
incrementa_color_fin:
					ret

menu_depuracion:
					call	actualiza_y_espera
menu_depuracion0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901
					db		TESP
					db		"Menu Depuracion                 "
					db		"---------------                 "
					db		"                                "
					db		"    Generar RESET               "
					db		"    Generar NMI                 "
					db		"    Ver Registros               "
					db		"    Pokear                      "
					db		"                                "
					db		"    ESC Volver al Menu Principal"

					db		TING
					db		"Debug Menu                      "
					db		"----------                      "
					db		"                                "
					db		"    Generate RESET              "
					db		"    Generate NMI                "
					db		"    Show Registers              "
					db		"    Poke                        "
					db		"                                "
					db		"    ESC Back to the Main Menu   "

					db		255

					mov	bx,offset tabla_menu_depuracion
					call	tecla_menu
					jnz   menu_depuracion_tecla_salvapan
					jmp	bx

menu_depuracion_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_depuracion0

depuracion_RESET:
					;Provocar RESET

					mov	si,0
					mov	reg_pc,si

					test	bits_estado3,00100000b
					jnz	depuracion_RESET_48k

					mov   word ptr puerto_32765,0
					mov   ax,seg_spectrum
					mov   seg_pant,ax

					;Poner ROM 0, RAMS normales
					xor	ax,ax
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

depuracion_RESET_48k:
					and   bits_estado2,11101111b ;Liberar si hay interrupcion pendiente
					mov   interrumpir,0


					jmp	menu_volver

depuracion_NMI:
					;Provocar NMI
					mov	bx,reg_sp
					dec 	bx
					dec	bx
					mov	reg_sp,bx ;Guardar SP del Z80
					mov	si,reg_pc
					mov	word ptr es:[bx],si ;Guardar PC en la pila
					mov	si,102 ;102=066h
					mov	reg_pc,si
					jmp	menu_volver

depuracion_pokear:
					call  actualiza_y_espera

					mov	word ptr cursor_x,0
					call	print_cadena
					db		TESP,"Direccion: "
					db		TING,"Address: "
					db		255
					mov	cx,0505h
					mov	bx,offset buffer_numero
					mov	byte ptr [bx],0
					push	bx
					call	input
					pop	bx
					jnc	depuracion_pokear_fin
					push	bx
					mov	al,8
					call	print_caracter
					mov	al,"="
					call	print_caracter
					pop	bx

					call	convertir_ascii_num

					push	ax
					mov	bx,ax
					mov	al,byte ptr es:[bx]
					mov	bl,128
					call	print_num8

					call	print_cadena
					db		13
					db		TESP,"Valor: "
					db		TING,"Value: "

					db		255

					mov	cx,0303h
					mov	bx,offset buffer_numero
					mov	byte ptr [bx],0

					push	bx
					call	input
					pop	bx
					pop	ax

					jnc   depuracion_pokear_fin
					push	ax
					call	convertir_ascii_num
					pop	bx
					mov	byte ptr es:[bx],al

depuracion_pokear_fin:
					jmp	menu_depuracion


depuracion_registros:
					call  actualiza_y_espera
depuracion_registros00:
					call	espera_no_tecla
depuracion_registros0:

					mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901
					db		TESP
					db		"Ver Registros                   "
					db		"-------------                   "
					db		TING
					db		"Show Registers                  "
					db		"--------------                  "

					db		TCOM
					db		"                                ",255

					mov	cx,13
					mov	bx,offset reg_c
					mov	si,offset mensaje_registros
tecla_regbuc:	push	cx
					push	bx
					mov	al,byte ptr [si]
					push	si
					call	print_caracter
					pop	si
					inc	si
					mov	al,byte ptr [si]
					push	si
					call	print_caracter
					pop	si
					inc	si
					mov	al,byte ptr [si]
					push	si
					call	print_caracter
					pop	si
					inc	si
					pop	bx
					mov	ax,word ptr [bx]
					inc	bx
					inc	bx
					push	bx
					mov	bl,'0'
					push	si
					call	print_num
					mov	al,32
					call	print_caracter
					pop	si
					pop   bx
					pop	cx
					cmp	cx,10 ;Mirar si esta en registro AF o AF'
					jz   reg_buc_af
					cmp cx,4
					jnz reg_buc_no_af
					
reg_buc_af:
					;Imprimir flags
					push	cx
					push	bx
					push	si
					dec	bx
					dec	bx
					mov	al,byte ptr [bx]
					call	print_flags
					mov	al,32
					call	print_caracter
					pop	si
					pop	bx
					pop	cx
reg_buc_no_af:

;reg_buc_sigue:
					mov	cursor_x,0
					inc	cursor_y
					dec	cx
					jz		tecla_reg_espera
					jmp   tecla_regbuc
tecla_reg_espera:
					test	bits_estado3,00100000b
					jz    menu_registros_mem0

					;Solo hay 48k de memoria

					mov   ax,64*256+32
					call	print_caracter_veces
					jmp   menu_registros_mem_sigue

menu_registros_mem0:


					call	print_cadena
					db		TESP,"Memoria:"
					db		TING,"Memory: "
					db		255

					mov	cx,4
					mov	bx,offset paginas_actuales
menu_registros_mem:
					push	cx
					push	bx
					mov	al,' '
					call	print_caracter
					mov	al,'R'
					call	print_caracter
					pop	bx

					mov	al,byte ptr [bx]
					inc	bx
					push	bx
					cmp	al,4
					jc		menu_registros_memrom
					sub	al,4
					mov	ah,'A'
					jmp	short menu_registros_mem2
menu_registros_memrom:
					mov	ah,'O'
					;Ver si es paginacion de 128 o +2
					test  bits_estado6,00010000b
					jz		menu_registros_mem2
					and	al,1 ;Decir solo ROM 0 o 1
menu_registros_mem2:
					xchg	al,ah
					push	ax
					call  print_caracter
					mov	al,'M'
					call	print_caracter
					pop	ax
					mov	al,ah
					call	print_num8_sin_spc

					pop	bx
					pop	cx
					loop	menu_registros_mem

					call	print_cadena
					db		"    ",255
					call  print_cadena
					db    "SCR=",255
					mov   al,puerto_32765
					and   al,8
					shr   al,1
					shr   al,1
					shr   al,1
					add   al,5
					cmp   al,6
					jnz   menu_registros_mem3
					inc   al
menu_registros_mem3:
					call  print_num8_sin_spc
					call  print_cadena
					db    " ",255
					call	print_cadena
					db		TESP,"Paginacion: "
					db		TING,"Paging:     "
					db		255

					test  puerto_32765,32
					call  print_siZ

					call	print_cadena
					db		"           ",255
					
menu_registros_mem_sigue:

					call	print_cadena
					db		"FPS=",255
					

					;hay 0 frames? frame forzado.......
					test bits_estado7,01000000b
					jz menu_registros_mem_sigue2
					call print_cadena
					db "   *1",255
					jmp short menu_registros_mem_sigue3

menu_registros_mem_sigue2:
					mov	al,sync_frames_ult

					call	print_num8_spc
menu_registros_mem_sigue3:
					call	print_cadena
					db    " [254]: C=",255
					mov   al,puerto_254
					push  ax
					and   al,7
					call  print_num8_sin_spc
					call  print_cadena
					db    " S=",255
					pop   ax
					push  ax
					and   al,16
					shr   al,1
					shr   al,1
					shr   al,1
					shr   al,1

					call  print_num8_sin_spc
					pop   ax
					and   al,8
					shr   al,1
					shr   al,1
					shr   al,1

					call  print_num8_sin_spc
					call  print_cadena
					db    "       ",255

;debug
;					mov al,temporal
;					call	print_num8_spc
;					mov al, seg_blaster_write
;					call	print_num8_spc
;debug

					call  print_cadena

					db		"                                "
					db		"    "
					db		TESP,"Interrupciones: "
					db		TING,"Interrupts:     "
					db		255
					test	bits_estado,1
					call	print_siNZ

					call	print_cadena
					db		"         "
					db		"    "
					db		TESP,"Modo Interrupcion:"
					db		TING,"Interrupt Mode:   "

					db		TCOM," IM",255
					mov	al,bits_estado
					and	al,2
					shr	al,1
					add	al,'1'
					call	print_caracter

COMMENT!
					mov al,seg_blaster_play
					call print_num8_sin_spc					
					mov	al,32
					call	print_caracter

					mov al,seg_blaster_write
					call print_num8_sin_spc

          call	print_cadena
          db " ",255
          
					mov al,registro_noise
					inc al
					call print_num8_sin_spc

          call	print_cadena
          db " ",255

					
					mov ax,randomize_noise
					call	print_num
					
					call	print_cadena
!

					call	print_cadena
					;               "01234567890123456789012345678901
					db		"                                          "
					db		TESP,"ESC Volver al Menu Anterior "
					db		TING,"ESC Back to Previous Menu   "
					db		255


					mov	bx,offset tabla_menu_registros
					mov	tecla_menu_volver,1
					call	tecla_menu
					mov	tecla_menu_volver,0
					jnz   menu_registros_tecla
					jmp	bx

menu_registros_tecla:

					;Mirar si se ha de activar el salvapantallas
					test	bits_estado2,4
					jnz	menu_registros_tecla0
					jmp	depuracion_registros0
menu_registros_tecla0:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	depuracion_registros0

registros_interrupcion:
					xor	bits_estado,1
					and   bits_estado2,11101111b ;Decir que no hay interrupcion
														  ;pendiente
					jmp	depuracion_registros00

registros_modo:
					xor	bits_estado,2
					jmp	depuracion_registros00

menu_cinta:
					call	actualiza_y_espera
menu_cinta0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901
					db		TESP
					db		"Emulacion de Cinta              "
					db		"------------------              "
					db		TING
					db		"Tape Emulation                  "
					db		"--------------                  "
					db		TCOM
					db		"                                "
					db		"    ",255
					test	bits_estado3,4
					jnz	menu_cinta_entrada_si
					call	print_cadena
					db		"Insert"
					db		TESP,"ar"
					db		TING,"  "
					db		255
					jmp	short menu_cinta_entrada_si2
menu_cinta_entrada_si:
					call	print_cadena
					db		TESP,"Extraer "
					db		TING,"Eject   "
					db		255
menu_cinta_entrada_si2:
					call	print_cadena
					db		TESP," fichero de Entrada "
					db		"    Fichero:"
					db		TING," Input File         "
					db		"    File:   "

					db		255
					mov	bx,offset fichero_TAP_entrada
					mov	ah,20
					call	print_cadena_longitud
					mov	al,32
					call	print_caracter_veces
					call	print_cadena
					db		"    ",255
					test	bits_estado3,2
					jnz	menu_cinta_salida_si
					call	print_cadena
					db		"Insert"
					db		TESP,"ar"
					db		TING,"  "
					db		255
					jmp	short menu_cinta_salida_si2
menu_cinta_salida_si:
					call	print_cadena
					db		TESP,"Extraer "
					db		TING,"Eject   "
					db		255

menu_cinta_salida_si2:
					call	print_cadena
					db		TESP," fichero de Salida  "
					db		"    Fichero:"
					db		TING," Output File        "
					db		"    File:   "

					db		255
					mov	bx,offset fichero_TAP_salida
					mov	ah,20
					call	print_cadena_longitud
					mov	al,32
					call	print_caracter_veces

					call	print_cadena
					db		"    "
					db		TESP,"Cargar cualquier flag: "
					db		TING,"Any Flag Loading:      "
					db		255
					test	bits_estado4,1
					call	print_siZ
					call	print_cadena
					db		"  "

					db		"                                "
					db		"    "
					db		TESP,"Cargar desde casette"
					db		TING,"Load from tape      "

					db		TCOM,"        ",255


menu_cinta_nosb:

					call	print_cadena
					db		"                                "
					db		"    "
					db		TESP,"ESC Volver al Menu Principal"
					db		TING,"ESC Back to the Main Menu   "

					db		255

					mov	bx,offset tabla_menu_cinta
					call	tecla_menu
					jnz   menu_cinta_tecla_salvapan
					jmp	bx

menu_cinta_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_cinta0

cinta_ins_entrada:

					test	bits_estado3,4
					jnz	cinta_ins_entrada_ext

					;Mirar si existe el fichero
					mov	dx,offset fichero_TAP_entrada
					call	crea_si_no_existe
					jz		cinta_ins_entrada2
					jmp	menu_cinta

cinta_ins_entrada2:
					or		bits_estado3,4
					mov	puntero_cinta_l,0
					mov	puntero_cinta_h,0 ;Poner puntero de lectura a 0
					;mov	si,offset rom_load
					call	cinta_ins_entrada_fin
					jmp	menu_cinta0

cinta_ins_entrada_ext:
					call  cinta_ins_entrada_ext2
					jmp	menu_cinta0
cinta_ins_entrada_ext2:
					;mov	si,offset buffer_rom_load
					and  	bits_estado3,11111011b

cinta_ins_entrada_fin:

					;mov	di,1378
					;mov	cx,offset fin_rom_load-offset rom_load

cinta_ins_entrada_fin1:
					;push	es

					;Mirar si solo hay 48k
					;test	bits_estado3,00100000b
					;jnz	cinta_ins_entrada_fin1_48k

					;mov   al,byte ptr paginas_actuales
					;mov	old_logica0_temporal,al

					;push	si
					;push	di
					;push	cx
					;xor	al,al
					;mov	ah,3

					;call	paginar ;Poner ROM3
					;pop	cx
					;pop	di
					;pop	si
					;push	si
					;push	di
					;push	cx

					;mov   es,seg_spectrum

;Eliminado
;Eliminado					cld
;Eliminado					rep	movsb
;Eliminado

					;mov	ah,old_logica0_temporal
					;xor	al,al

					;mov   si_despaginar_rom,1
					;call	paginar ;Restaurar ROM

					;pop	cx
					;pop	di
					;pop	si


;cinta_ins_entrada_fin1_48k:

					;Mirar si esta la ROM3 paginada
					;call	donde_rom3
					;cmp	ax,seg_spectrum
					;jnz	cinta_ins_entrada_fin2
					;mov	es,ax
					;cld
					;Eliminado rep	movsb


cinta_ins_entrada_fin2:

					;pop	es

					ret

cinta_fic_entrada_filesel:
					and  	bits_estado3,11111011b ;Decir que no hay cinta insertada
					mov	bx,offset cad_tap
					mov	dx,offset fichero_TAP_entrada
					call	filesel
					pushf
					call  actualiza_y_espera
					popf
					mov   word ptr cursor_x,0
					cmc
					jmp	short cinta_fic_entrada0

cinta_fic_entrada:
					call  cinta_ins_entrada_ext2
					mov	word ptr cursor_x,040Ch
					mov	bx,offset fichero_TAP_entrada
					mov	cl,79
					mov	ch,19
					call	input
cinta_fic_entrada0:
					jc		cinta_fic_entrada_fin
					mov	byte ptr fichero_TAP_entrada,0

cinta_fic_entrada_fin:
					jmp	menu_cinta0
cinta_ins_salida:
					test	bits_estado3,2
					jnz	cinta_ins_salida_ext

					;Mirar si existe el fichero
					mov	dx,offset fichero_TAP_salida
					call	crea_si_no_existe
					jz		cinta_ins_salida2
					jmp	menu_cinta

cinta_ins_salida2:

					or		bits_estado3,2
					;mov	si,offset rom_save
					call	cinta_ins_salida_fin
					jmp	menu_cinta0

cinta_ins_salida_ext:
					call	cinta_ins_salida_ext2
					jmp	menu_cinta0
cinta_ins_salida_ext2:
					and  	bits_estado3,11111101b
					;mov	si,offset buffer_rom_save

cinta_ins_salida_fin:
					;mov	di,1222
					;mov	cx,offset rom_load-offset rom_save
					jmp	cinta_ins_entrada_fin1

cinta_fic_salida_filesel:
					and  	bits_estado3,11111101b ;Decir que no hay cinta insertada
					mov	bx,offset cad_tap
					mov	dx,offset fichero_TAP_salida
					call	filesel
					pushf
					call  actualiza_y_espera
					popf
					mov   word ptr cursor_x,0
					cmc
					jmp	short cinta_fic_salida0


cinta_fic_salida:
					call  cinta_ins_salida_ext2
					mov	word ptr cursor_x,060Ch
					mov	bx,offset fichero_TAP_salida
					mov	cl,79
					mov	ch,19
					call	input
cinta_fic_salida0:
					jc		cinta_fic_salida_fin
					mov	byte ptr fichero_TAP_salida,0

cinta_fic_salida_fin:

					jmp	menu_cinta0

cinta_cualquier_flag:
					xor	bits_estado4,1
					jmp	menu_cinta0

menu_casete_cond:
					cmp	soundblaster_presente,1
					jnz   menu_casete_cond_fin

					mov	al,bits_estado3
					not	al
					test	al,00000010b

menu_casete_cond_fin:

					ret



menu_casete:
					call	actualiza_y_espera
menu_casete0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901
					db		TESP,"Cargar desde Casette            "
					db		     "--------------------            "
					db		TING,"Load from Tape                  "
					db		     "--------------                  "

					db		TCOM,"                                "
					db		"    "
					db		TESP,"Baudios: "
					db		TING,"Bauds:   "

					db		255
					mov	ax,baudios
					mov	bl,32
					call  print_num

					call  print_cadena
					db		"              "
					db		"     "
					db		TESP,"Frecuencia: "
					db		TING,"Frequency:  "

					db		255
					call	calcula_frecuencia
					mov	bl,32
					call  print_num

					call	print_cadena
					db		"  Hz      "

					db		"    "
					db		TESP,"Longitud onda guia:    "
					db		TING,"Length leader s. wave: "
					db		255
					mov	al,load_tono
					call	print_num8_spc
					call	print_cadena
					db		"    "
					db		TESP,"Longitud tono guia:    "
					db		TING,"Length leader signal:  "
					db		255
					mov	ax,load_ondas_guia
					call	print_num_spc
					call	print_cadena
					db		"    "
					db		TESP,"Longitud onda ceros:   "
					db		TING,"Length zero wave:      "
					db		255
					mov	al,load_ceros
					call	print_num8_spc
					call	print_cadena
					db		"    "
					db		TESP,"Longitud onda unos:    "
					db		TING,"Length one wave:       "
					db		255
					mov	al,load_unos
					call	print_num8_spc

					call	print_cadena
					;     "01234567890123456789012345678901
					db		"                                "


					db		"    "
					db		TESP,"Filtro de Entrada:"
					db		TING,"Input Filter:     "
					db		255
					mov	al,0ch
					call	read_ct
					shr	al,1
					shr	al,1
					shr	al,1
					mov	ah,al
					test  al,00000100b
					jz    menu_casete_filtro
					mov	al,2
					jmp	short menu_casete_filtro1

menu_casete_filtro:
					and	al,1
menu_casete_filtro1:
					mov	bx,offset filtros
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

					call	print_cadenas

					call	print_cadena
					db		"   "
					db		"    "
					db		TESP,"Volumen canal izq.: "
					db		TING,"Left chann. Volume: "
					db		255
					mov	al,2eh
					call	read_ct
					push	ax
					shr	al,1
					shr	al,1
					shr	al,1
					shr	al,1
					mov	bl,32
					call	print_num8

					call	print_cadena
					db		"   "
					db		"    "
					db		TESP,"Volumen canal der.: "
					db		TING,"Right chan. Volume: "
					db		255
					pop	ax
					and	al,00001111b
					mov	bl,32
					call	print_num8

					call	print_cadena
					db		"   ",255

					call	print_cadena
					db    "    "

					db		TESP,"Ver franjas del Borde: "
					db		TING,"Show Border:           "
					db		255

					test	bits_estado5,00001000b
					call	print_siZ
					call	print_cadena
					db		"  "
					db		"    "
					db		TESP,"Autocorreccion Checksum: "
					db		TING,"Checksum Autocorrection: "
					db		255
					test  load_flags,00000001b
					call	print_siNZ

					call	print_cadena
					db		"                                "
					db		"    "
					db		TESP,"Comenzar a Cargar"
					db		TING,"Start Loading    "
					db		TCOM,"           "
					db		"                                "
					db		"    "
					db		TESP,"ESC Volver al Menu Anterior "
					db		TING,"ESC Back to Previous Menu   "
					db		255

					mov	bx,offset tabla_menu_casete
					call	tecla_menu
					jnz   menu_casete_tecla_salvapan
					jmp	bx

menu_casete_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_casete0

casete_baudios_menos:
					mov	ax,baudios
					cmp	ax,750
					jz    casete_baudios_menos2
					sub	ax,250
casete_baudios_menos1:
					mov	baudios,ax
casete_baudios_menos2:
					jmp	menu_casete0

casete_baudios_mas:
					mov	ax,baudios
					cmp   ax,6000
					jz    casete_baudios_menos2
					add	ax,250
					jmp	short casete_baudios_menos1

casete_filtro:
					mov	al,0ch
					call	read_ct
					mov	ah,al
					and	al,00101000b

					cmp	al,00001000b
					jnz	casete_filtro1
					;Es filtro Ninguno
					mov	al,00100000b
					jmp	short casete_filtro3

casete_filtro1:

					test	al,00100000b
					jz	   casete_filtro2

					;Es filtro bajo
					xor	al,al
					jmp	short casete_filtro3
casete_filtro2:
					;Es filtro alto
					mov	al,00001000b
casete_filtro3:
					and	ah,11000111b
					or		al,ah
					mov	ah,0ch
					call	write_ct
					jmp	menu_casete0

casete_izq_menos:
					mov	al,2eh
					call	read_ct
					mov	ah,al
					and	al,11110000b
					or		al,al
					jz		casete_izq_menos2
					sub	al,00010000b

casete_izq_menos1:
					and	ah,00001111b
casete_izq_menos11:
					or		al,ah
					mov	ah,2eh
					call	write_ct
casete_izq_menos2:

					jmp	menu_casete0

casete_izq_mas:
					mov	al,2eh
					call	read_ct
					mov	ah,al
					and	al,11110000b
					cmp	al,11110000b
					jz		casete_izq_menos2
					add	al,00010000b
					jmp	short casete_izq_menos1

casete_der_menos:
					mov	al,2eh
					call	read_ct
					mov	ah,al
					and	al,00001111b
					or		al,al
					jz		casete_izq_menos2
					dec	al

casete_der_menos1:
					and	ah,11110000b
					jmp	short casete_izq_menos11

casete_der_mas:
					mov	al,2eh
					call	read_ct
					mov	ah,al
					and	al,00001111b
					cmp	al,00001111b
					jz		casete_izq_menos2
					inc	al
					jmp	short casete_der_menos1

casete_autoch:
					xor	load_flags,00000001b
					jmp	menu_casete0

casete_onda_guia_menos:
					cmp	load_tono,1
					jz    casete_onda_guia_menos1
					dec	load_tono

casete_onda_guia_menos1:
					jmp	menu_casete0

casete_onda_guia_mas:
					cmp	load_tono,255
					jz    casete_onda_guia_mas1
					inc	load_tono

casete_onda_guia_mas1:
					jmp	menu_casete0

casete_tono_guia_menos:
					cmp	load_ondas_guia,1
					jz    casete_tono_guia_menos1
					dec	load_ondas_guia

casete_tono_guia_menos1:
					jmp	menu_casete0

casete_tono_guia_mas:
					cmp	load_ondas_guia,65535
					jz    casete_tono_guia_mas1
					inc	load_ondas_guia

casete_tono_guia_mas1:
					jmp	menu_casete0

casete_onda_ceros_menos:
					cmp	load_ceros,1
					jz    casete_onda_ceros_menos1
					dec	load_ceros

casete_onda_ceros_menos1:
					jmp	menu_casete0

casete_onda_ceros_mas:
					cmp	load_ceros,255
					jz    casete_onda_ceros_mas1
					inc	load_ceros

casete_onda_ceros_mas1:
					jmp	menu_casete0

casete_onda_unos_menos:
					cmp	load_unos,1
					jz    casete_onda_unos_menos1
					dec	load_unos

casete_onda_unos_menos1:
					jmp	menu_casete0

casete_onda_unos_mas:
					cmp	load_unos,255
					jz    casete_onda_unos_mas1
					inc	load_unos

casete_onda_unos_mas1:
					jmp	menu_casete0

;casete_border_cond:
;					cmp	vga_presente,1
;					ret

casete_border:
					xor	bits_estado5,00001000b
					jmp	menu_casete0

casete_cargar:

					call	actualiza_y_espera
					mov	word ptr cursor_x,0

casete_cargar1:

					;test or		bits_estado4,10001000b ;No hacer multitarea,
														  ;No refrescar el sonido

					;Desactivar canales 0,1,2,7
;					call	silenciar_canales

					;Guardar valor anterior del border
					mov	al,border
					mov	old_border,al

					or    bits_estado6,00001000b
					;Decir a int 8 que esta cargando

					cli
					call  cambia_vectores_pit
					sti
					


					call  load
					
   				and   bits_estado6,11110111b
					cli
					call  cambia_vectores_pit
					sti


					mov	al,old_border
					mov	border,al

					;test and	bits_estado4,01110111b

          jmp	menu_casete
					
;para buscar en debug........
;db 4,3,2,1,0

menu_hardware:
					call	actualiza_y_espera

menu_hardware0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;          "01234567890123456789012345678901
					db		TESP,"Ajustes de Hardware             "
					db			  "-------------------             "
					db		TING,"Hardware Settings               "
					db			  "-----------------               "
					db		TCOM
					db		     "                                "
					db		"    "
					db		TESP,"Teclado "
					db		TING,"Keyboard"
					db		TCOM," Issue"
					db		255
					mov	al,'1'
					test	puerto_65278,64
					jnz   hardware_issue1
					inc	al
hardware_issue1:
					call	print_caracter
					call	print_cadena
					db		"             "
					db		"    "
					db		TESP,"Disparador Automatico: "
					db		TING,"AutoFire:              "
					db		255
					call	hardware_disparador_cond
					call	print_siZ

					call	print_cadena
					db		"  "
					db		"    Fre"
					db		TESP,"c. disparador:"
					db		TING,"q. autofire:  "
					db		TCOM,"    ",255

					mov	al,50
					xor	ah,ah
					div   disparador_defecto
					call	print_num8_spc
					call	print_cadena
					db		"Hz",255
					
					
					
					call print_cadena
					db		"    "
					db 	TESP,"Cursores emulan: "
					db 	TING,"Cursors emulate: "
					db		255
					
					test  bits_estado8,8
					jz		hardware_cursores_si
					
					call	print_cadena
					db		"Kempston"
					db		255
					
					jmp	short hardware_cursores_sigue
					
;bits_estado8	db		0   
;COMMENT !
;  Byte de estado no 7. Codificaci�n:
;  bit
;  ---
;  7-4 No usados
;  3	A 1 indica que los cursores emulan el joystick kempston. Sino, emulan los cursores de spectrum (SHIFT+5/6/7/8)

hardware_cursores_si:

					call	print_cadena
					db		TESP,"Cursores"
					db		TING,"Cursors "
					db		255


hardware_cursores_sigue:


					
					
					
					

					call	print_cadena

					db		"   "
					db    "    "
					db		TESP,"Valor POKE a ROM"
					db		TING,"ROM POKE Value  "
					db		TCOM,": ",255

					mov	al,valor_poke_rom
					call	print_num8_spc
					call	print_cadena
					db		"     ",255

hardware_no_inves:

					call	print_cadena
					db		"    "
					db		TESP,"Memoria compartida: "
					db		TING,"Contended memory:   "
					db		255
					test	bits_estado7,00001000b
					call	print_siZ

					call	print_cadena
					db		"         "
					db		TESP,"Sincronismo: "
					db		TING,"Syncronism:  "
					db		255
					
					
					mov al,bits_estado6
          rol al,1
          and al,1
          test bits_estado6,01000000b
          jz  hardware_no_blaster_sinc
          inc al
          
hardware_no_blaster_sinc:
					mov	bx,offset modos_sincronismo
					call	print_cadenas

hardware_no_blaster_sinc_2:


					call print_cadena

					db		" "

					;     "01234567890123456789012345678901
					db		"                                "
					db		"    "
					db		TESP,"ESC Volver al Menu Principal"
					db		TING,"ESC Back to the Main Menu   "
					db		255

					mov	bx,offset tabla_menu_hardware
					call	tecla_menu
					jnz   menu_hardware_tecla_salvapan
					jmp	bx

menu_hardware_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_hardware0

hardware_teclado:
					mov	cx,8
					mov	bx,offset puerto_65278
tecla_issue_buc:
					xor	byte ptr [bx],64
					inc	bx
					loop  tecla_issue_buc
					jmp	menu_hardware0

hardware_disparador:
					xor	bits_estado2,64
					jmp	menu_hardware0

hardware_disparador_cond:
					mov	al,bits_estado2
					not	al
					test	al,64
					ret

hardware_disparador_menos:
					mov	al,disparador_defecto
					cmp	al,32
					jnc 	hardware_disparador_mef
					shl   disparador_defecto,1
hardware_disparador_mef:
					jmp	menu_hardware0

hardware_disparador_mas:
					mov	al,disparador_defecto
					cmp	al,1
					jz		hardware_disparador_maf
					shr   disparador_defecto,1
hardware_disparador_maf:
					jmp	menu_hardware0

hardware_poke_rom_cond:
					mov	al,bits_estado5
					not	al
					test	al,00100000b
					ret


hardware_poke_rom:

					mov	word ptr cursor_x,0618h
					mov	bx,offset buffer_numero
					mov     byte ptr [bx],0
					mov	cx,0303h
					push	bx
					call	input
					pop	bx
					jnc   hardware_poke_rom_fin

					call	convertir_ascii_num
					mov	valor_poke_rom,al

hardware_poke_rom_fin:
					jmp	menu_hardware0

hardware_compartido:
					xor   bits_estado7,00001000b
					jmp	menu_hardware0
					
hardware_sinc_blaster:					
;bits_estado6: 00XXXXXXb : timer
;bits_estado6: 10XXXXXXb : sound blaster
;bits_estado6: 11XXXXXXb : sound blaster+timer
          mov al,bits_estado6
          test al,10000000b
          jnz  hardware_sinc_blaster2
          or   bits_estado6,10000000b
          jmp	menu_hardware0
          
hardware_sinc_blaster2:
          test al,01000000b
          jnz hardware_sinc_blaster3
          or  bits_estado6,01000000b
          jmp	menu_hardware0
          
hardware_sinc_blaster3:
          xor bits_estado6,11000000b
          jmp	menu_hardware0

hardware_sinc_blaster_cond:	
					cmp soundblaster_presente,1
					ret
					
hardware_cursores:
			mov 	al,bits_estado8
			xor	al,8
			mov   bits_estado8,al
			
			test  al,8
			jz		hardware_cursores2
			
			call hardware_pon_cursores_kempston
			jmp	menu_hardware0
hardware_cursores2:
			call	hardware_pon_cursores_cursores
			jmp	menu_hardware0
			

;Establecer cursores activan cursores de spectrum (SHIFT+ 5/6/7/8)
hardware_pon_cursores_cursores:

		mov byte ptr tabla_puerto_arr,4+4*16
		mov byte ptr tabla_puerto_arr+1,0+1*16

		mov byte ptr tabla_puerto_izq,3+5*16
		mov byte ptr tabla_puerto_izq+1,0+1*16

		mov byte ptr tabla_puerto_der,4+3*16
		mov byte ptr tabla_puerto_der+1,0+1*16

		mov byte ptr tabla_puerto_aba,4+5*16
		mov byte ptr tabla_puerto_aba+1,0+1*16
		
		ret

hardware_pon_cursores_kempston:

                mov byte ptr tabla_puerto_arr,15+4*16
                mov byte ptr tabla_puerto_arr+1,0

                mov byte ptr tabla_puerto_izq,15+2*16
                mov byte ptr tabla_puerto_izq+1,0

                mov byte ptr tabla_puerto_der,15+1*16
                mov byte ptr tabla_puerto_der+1,0

                mov byte ptr tabla_puerto_aba,15+3*16
                mov byte ptr tabla_puerto_aba+1,0
                

		ret

menu_volver:	;Volver al emulador
					and	bits_estado4,11010111b ;Refrescar el sonido,
														  ;no multitarea
														  
					mov	primera_vez,0									

					call	actualiza_pantalla
					call	liberar_escape
					call  sincronizar_reset
					mov	si,reg_pc
					mov	di,reg_sp
					cargar_flags_con_f
					jmp	interprete

menu_sonido:
					call	actualiza_y_espera
menu_sonido0:
					mov	word ptr cursor_x,0
					call	print_cadena
					;     "01234567890123456789012345678901
					db		TESP,"Ajustes de Sonido               "
					db		     "-----------------               "
					db		TING,"Audio Settings                  "
					db		     "--------------                  "

					db		TCOM,"                                "
					db		"    "
					db		255


					call	print_cadena
					db		TESP,"Sonido activo: "
					db		TING,"Output sound:  "
					db		255

					test	bits_estado8,00000001b
					call	print_siZ

;					db		TESP,"Salida Altavoz:"
;					db		TING,"Speaker Output:"
;					db		255
;					cmp	sonido,0
;					jnz	menu_sonido_altavoz_si
;					mov	al,2
;					jmp	short menu_sonido_altavoz_sigue

;menu_sonido_altavoz_si:

;					mov	al,bits_estado3
;					shr	al,1
;					shr	al,1
;					shr	al,1
;					and	al,1

menu_sonido_silenciado:
;					mov	bx,offset salidas_altavoz
;					call	print_cadenas

					call	print_cadena

					db    "              "
					;db    TESP,"Frec. salida sonido: "
					;db    TING,"Sound output freq.:  "
					;db		255

					;mov	al,frecuencia_sonido
					;sub	al,2
					;mov	bx,offset frecuencias_salidas_altavoz
					;call	print_cadenas

					;call	print_cadena
					;db    "? KHz      "
					;      "01234567890123456789012345678901
					db		TESP,"Chip AY presente: "
					db		TING,"AY Chip present:  "
					db		255

					test	bits_estado4,00000010b
					call	print_siZ

					call	print_cadena
					db    "       "
;					db 	255

;					db		"    "
;					db		TESP,"Salida Chip AY:"
;					db		TING,"AY Chip Output:"
;					db		255
;					mov	al,1
;					test  bits_estado4,00010000b
;					jz    menu_sonido_sb
;					inc	al
;menu_sonido_sb:
;					mov	bx,offset salidas_altavoz
;					call	print_cadenas

					db		"    "
					db		TESP,"Modo de Stereo: "
					db		TING,"Stereo Mode:    "
					db		255
					mov	bx,offset modos_stereo
					mov	al,bits_estado7
					and	al,3
					call	print_cadenas

					call	print_cadena
;Emulacion de ruido de momento no soportada. Se deja el texto para el futuro
					db		"        "
					db		"    "
					db		TESP,"Emulacion de Ruido: "
					db		TING,"Noise Emulation:    "
					db		255
					test	bits_estado7,00000100b
					call	print_siZ

					call	print_cadena
					db		"     "

					db		"    "
					db		TESP,"Ver registros Chip AY"
					db		TING,"Show AY registers    "
					db		TCOM,"       ",255

					test	bits_estado7,10000000b
					jnz		menu_sonido_raw_record

					call  print_cadena
					db		"                                "
					db		"    "
					db		TESP,"Iniciar grabacion audio"
					db		TING,"Start recording audio  "
					db		TCOM,"     ",255
					jmp	short menu_sonido_raw_record2

menu_sonido_raw_record:
					call  print_cadena
					db		"                                "
					db		"    "
					db		TESP,"Detener grabacion audio"
					db		TING,"Stop  recording audio  "
					db		TCOM,"     ",255


menu_sonido_raw_record2:
					call	print_cadena
					db		"    "
					db		TESP,"Fichero:"
					db		TING,"File:   "

					db		255
					mov	bx,offset write_sound_nombre
					mov	ah,20
					call	print_cadena_longitud
					mov	al,32
					call	print_caracter_veces		


					call print_cadena
					db		"                                "
					db		"    "
					db		TESP,"ESC Volver al Menu Principal"
					db		TING,"ESC Back to the Main Menu   "
					db		255

					mov	bx,offset tabla_menu_sonido
					call	tecla_menu
					jnz   menu_sonido_tecla_salvapan
					jmp	bx

menu_sonido_tecla_salvapan:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	menu_sonido0

;sonido_salida:
;					cmp	sonido,0
;					jnz	sonido_salida_si

					;Activar sonido, salida a speaker
;					mov	sonido,1

;					jmp	menu_sonido0

;sonido_salida_si:
;					test	bits_estado3,00001000b
;					jz		sonido_salida_si2
;					;Desactivar sonido,salida a speaker
;					and	bits_estado3,11110111b
;					mov	sonido,0
;					jmp	menu_sonido0
;sonido_salida_si2:
;					or		bits_estado3,00001000b
;					jmp	menu_sonido0

;sonido_altavoz_modo:
;					xor   bits_estado6,00100000b
;					jmp	menu_sonido0

;sonido_altavoz_frec_cond:
;					call  sonido_altavoz_modo_cond
;					jnz   sonido_altavoz_frec_cond2
;					test  bits_estado6,00100000b
;sonido_altavoz_frec_cond2:
;					ret

sonido_si_no:
					xor   bits_estado8,00000001b
					jmp	menu_sonido0

sonido_altavoz_frec:
					;xor	frecuencia_sonido,1
					;Frecuencia 2 (15625/2) o 3 (15625/3)
					jmp	menu_sonido0


;sonido_altavoz_modo_cond:
;					mov	al,bits_estado3
;					not	al
;					test	al,00001000b
;					ret

sonido_presente:

					;call	silenciar_canales
					;Aqui podria pasar que se refrescara
					;el sonido, con lo que si hubiera
					;algun canal de ruido activo,
					;se quedar�a siempre activo

					xor   bits_estado4,00000010b
					jmp	menu_sonido0

;sonido_salida_ay:
;					xor	bits_estado4,00010000b
;					;call	silenciar_canales
;					jmp	menu_sonido0

sonido_modo_stereo:
					;call	silenciar_canales
					mov	al,bits_estado7
					mov	ah,al
					and	al,3
					inc	al
					cmp	al,3
					jnz	sonido_modo_stereo2
					xor	al,al
sonido_modo_stereo2:
					and	ah,11111100b
					or		ah,al
					mov	bits_estado7,ah
;Obsoleto					call	pon_modo_stereo
					jmp	menu_sonido0

sonido_modo_stereo_cond:
					test	bits_estado4,00010000b ;Salida de sonido a Sblaster
					ret

;Emulacion de ruido
sonido_ruido:
					xor	bits_estado7,00000100b
					jmp	menu_sonido0

;sonido_refresco_ay_cond:
;					call	sonido_ver_registros_cond
;					jnz   sonido_refresco_ay_cond_fin
;					cmp	soundblaster_presente,1

sonido_refresco_ay_cond_fin:
					ret

;sonido_refresco_ay:
;					xor	bits_estado4,00000100b
;					call	silenciar_canales

;					jmp	menu_sonido0

sonido_ver_registros_cond:
					test	bits_estado4,00000010b
					ret

sonido_ver_registros:
					call	actualiza_y_espera
					call	espera_no_tecla
sonido_ver_registros0:
					mov	word ptr cursor_x,0

					mov	bx,offset ay_3_8912_registros
					xor	cx,cx
sonido_ver_registros_buc:
					push	bx
					push	cx
					mov	al,"R"
					call	print_caracter
					pop	ax ;Recuperar contador
					push	ax
					mov	bl,128
					call	print_num
					mov	al,":"
					call	print_caracter
					pop	cx
					pop	bx
					mov	al,byte ptr [bx]
					push	bx
					push	cx
					call	print_num8_sin_spc
					mov	al,32
					call	print_caracter
					mov	al,32
					call	print_caracter

					mov	al,13
					call	print_caracter
					pop	cx
					pop	bx
					inc	bx
					inc	cx
					cmp	cx,16
					jnz   sonido_ver_registros_buc

					call	print_cadena
					db    13,"[REG]= ",255
					mov	al,puerto_65533
					call  print_num8_sin_spc
					mov	al,32
					call	print_caracter
					mov	al,32
					call	print_caracter


					call	print_cadena
					db    13,"[VAL]= ",255
					mov	al,puerto_49149
					call  print_num8_sin_spc
					mov	al,32
					call	print_caracter
					mov	al,32
					call	print_caracter



sonido_ver_registros_lee:
					call	lee_teclas_and
					jnz   sonido_ver_registros_fin

					;Mirar si se ha de activar el salvapantallas
					test	bits_estado2,4
					jnz   sonido_ver_registros_lee2

					jmp   sonido_ver_registros0

sonido_ver_registros_lee2:
					call	salvapan_inicio
					call	act_pantalla_border
					jmp	sonido_ver_registros

;					call	espera_no_tecla
;					call	espera_tecla
					
					
sonido_ver_registros_fin:
					jmp	menu_sonido


sonido_record_ins:

					test	bits_estado7,10000000b
					jnz	sonido_record_ins_ext


COMMENT !
  Byte de estado no 7. Codificaci?n:
  bit
  ---
  7   A 1 indica que hay cinta insertada y activada para la grabacion de sonido en archivo raw
!

						
					;Mirar si existe el fichero
					mov	dx,offset write_sound_nombre
					call	crea_si_no_existe
					jz		sonido_record_ins2
					jmp	menu_sonido

sonido_record_ins2:
					or		bits_estado7,10000000b

					jmp	menu_sonido0

sonido_record_ins_ext:
					and  	bits_estado7,01111111b
					jmp	menu_sonido0
					


sonido_record_fic_filesel:
					and  	bits_estado7,01111111b ;Decir que no hay archivo raw insertado
					mov	bx,offset cad_raw_audio
					mov	dx,offset write_sound_nombre
					call	filesel
					pushf
					call  actualiza_y_espera
					popf
					mov   word ptr cursor_x,0
					cmc
					jmp	short sonido_record_fic0

sonido_record_fic:
					and  	bits_estado7,01111111b ;Decir que no hay archivo raw insertado
					mov	word ptr cursor_x,0A0Ch
					mov	bx,offset write_sound_nombre
					mov	cl,79
					mov	ch,19
					call	input
sonido_record_fic0:
					jc		sonido_record_fic_fin
					mov	byte ptr write_sound_nombre,0

sonido_record_fic_fin:
					jmp	menu_sonido0









menu_salir:
					call	actualiza_y_espera
					mov	word ptr cursor_x,0900h

					call	print_cadena
					db		13
					db		"--------------------------------"
					db		TESP
					db		"Seguro que quieres salir ? (S/N)"
					db		TING
					db		"Sure you want to exit now ?(Y/N)"
					db		TCOM
					db		"--------------------------------",255
					call	lee_siono
					jz		menu_salir2

					call	actualiza_y_espera
					jmp	menu_inicio

menu_salir2:
					jmp	fin_spectrum


lee_teclas_and:
					call	lee_todas_teclas

					pushf
					test	bits_estado4,10000000b
					jnz	lee_teclas_and2 ;No se debe hacer multitarea, pues
					;esta (posiblemente) la rutina de carga directa de casette

					call  multitarea_interprete

lee_teclas_and2:

					popf

					and	al,31
					cmp	al,31
					ret


espera_tecla:
					call	lee_teclas_and

					jz		espera_tecla
					ret

espera_no_tecla:
					call	lee_teclas_and

					jnz 	espera_no_tecla
					ret



print_cadena:	;A esta rutina se llama as�:
					;CALL print_cadena
					;db	"Mensaje",255
					;...continuaci�n del programa
					;

					pop	bx
					call	print_cadena_sinlongitud
					jmp	bx

print_codigo13:
					push  ax
					push  bx
					mov	al,13
					call  print_caracter
					pop   bx
					pop   ax
					ret

print_caracter: ;Rutina de imprimir un solo caracter. Cambiando el puntero
					 ;print_add se puede llamar a otras direcciones

					mov	bx,print_add
					jmp	bx

print_caracter0:
					 ;Rutina de imprimir un solo caracter con actualizaci�n
					 ;de coordenadas
					 ;Entrada=AL Caracter
					 ;Si AL=8, retrocede cursor

					cmp	al,13
					jnz	print_caracter1
					mov	cursor_x,0
					inc	cursor_y
					ret
print_caracter1:
					cmp	al,8
					jnz	print_caracter3
					cmp	cursor_x,0
					jz		print_caracter2
					dec	cursor_x
print_caracter2:
					ret

print_caracter3:
					call	vga_print_char
					mov	bl,cursor_x
					inc	bl
					cmp	bl,32
					jnz	car_no_fin_linea
					xor	bl,bl
					inc	cursor_y
car_no_fin_linea:
					mov	cursor_x,bl
					ret


cursor_x			db		?
cursor_y			db		?
color_tinta		db		9
color_papel		db		10
print_add		dw		print_caracter0
add_num			dw		0 ;Direcci�n de escritura de la cifra del n�mero de snap

mensaje_registros: db "BC DE HL AF IX IY BC'DE'HL'AF'IR SP PC "
mensaje_flags:		 db "SZxHxVNC"

print_num8_sin_spc:
					mov	bl,128
					jmp	short print_num8

print_num_spc:
					mov	bl,32
					jmp	short print_num

print_num8_spc:
					mov	bl,32

print_num8:		;Rutina de impresion de un numero de 8 bits
					;Entrada:  AL=Numero
					xor	ah,ah

print_num:		;Rutina de impresi�n de un n�mero de 16 bits
					;Entrada:  AX=N�mero
					;			  BL=Caracter a imprimir en el caso de que
					;				  el numero empiece con cero
					;				  Si BL>127 no imprime nada

					mov	bp,10000
					mov	cx,5
					xor	bh,bh ;A 0 indica que primera cifra es 0

print_num_buc: push	cx
					xor	dx,dx
					div	bp
					push	dx ;Resto
					push  ax ;Cociente
					;Ahora dividir bp entre 10
					xor	dx,dx
					mov	ax,bp
					mov	bp,10
					div	bp
					mov	bp,ax ;BP=Cociente
					pop	ax
					pop	dx
					add	al,'0'
					cmp	al,'0'
					jnz	no_es_cero
					or		bh,bh
					jnz	no_es_cero
					;Si es la �ltima cifra,s� imprimir 0
					or		bp,bp
					jz    print_cifra
					cmp	bl,128
					jnc	no_cifra
					mov	al,bl
					jmp	short print_cifra
no_es_cero:    inc	bh ;bh no va a ser cero
print_cifra:   push	bp
					push	bx
					push  dx
					call	print_caracter
					pop	dx ;AX=DX=Resto
					pop	bx
					pop	bp
no_cifra:      mov	ax,dx
					pop	cx
					loop  print_num_buc
					ret

print_flags:	;Rutina de imprimir el estado de los flags
					;Utilizado por F8=Ver registros
					;Entrada: AL=Flags
					mov	si,offset mensaje_flags
					mov	cx,8 ;8 flags
print_flags_buc:
					rol	al,1
					jnc	flag_a_0
					mov	bl,byte ptr [si]
					jmp	short print_flags_sigue
flag_a_0:    	mov	bl,32
print_flags_sigue:
					inc   si
					push	ax
					push	si
					push	cx
					mov	al,bl
					call	print_caracter
					pop	cx
					pop	si
					pop	ax
					loop	print_flags_buc
					ret

print_siNZ:
;Rutina que imprime SI si NZ o NO si Z
					jnz	print_siNZ_si
print_siNZ_no:
					call	print_cadena
					db		TESP,"No "
               db		TING,"Off"
					db		255
					ret

print_siNZ_si:	call	print_cadena
					db		TESP,"Si "
					db		TING,"On "
					db		255
					ret

print_siZ:
;Rutina que imprime SI si Z o NO si NZ
					jz		print_siNZ_si
					jmp	short print_siNZ_no



input:			;Rutina de entrada de texto
					;Registros de entrada:
					;BX=Puntero de la cadena a guardar
					;Debe haber el c�digo 0 al inicio de BX para hacer un
					;input nuevo, o a final del texto si se quiere editar
					;CL=Longitud maxima que se acepta (sin incluir c�digo 0)
					;CH=Longitud visible en pantalla (sin incluir el _)
					;
					;Registros de salida:
					;En direcci�n BX esta la cadena acabada en codigo 0
					;Devuelve NC si se pulsa ESCape

;					push	bx
;					push	cx

					or		bits_estado4,00001000b ;No refrescar el sonido

					;Desactivar canales 0,1,2,7
					;call	silenciar_canales
;					pop	cx
;					pop	bx

					call    pon_estados_a0

					mov	ax,word ptr cursor_x
					mov	input_cursor,ax ;Guardar posici�n original del cursor

					push	bx
					push	cx
					push	es
					cli
					;;????
					mov    inicializa_int_irq_event,0
					;;????????
					call	restaura_vectores
					sti
					pop	es

;Antes hay que liberar la �ltima tecla pulsada del puerto
					call	liberar_teclas

					pop	cx
					pop	bx
					xor	dh,dh ;DH contiene el n�mero de caracteres escritos
					push	bx

input00:			;Ver si hay caracteres ya escritos
					cmp	byte ptr [bx],0
					jz		input000
					inc	bx
					inc	dh
					jmp	short input00
input000:		pop	bx

					xor	ah,ah
					mov	al,dh
					add	bx,ax
					;BX est� en el final del texto

input1:        ;Escribir cadena
					mov	ax,input_cursor
					mov	word ptr cursor_x,ax
					push	bx

					mov	al,dh
					cmp	al,ch
					jnc	input10
					mov	al,dh
					jmp	short input11
input10:			mov	al,ch
input11: 		xor	ah,ah
					sub	bx,ax
					;AL contiene los caracteres que se escribiran en pantalla
					;Lo que queda para lo visible en pantalla se completa
					;con espacios(despu�s del _)
					mov	ah,ch
					sub	ah,al ;AH contiene lo que falta para completar la
									;pantalla
					or		al,al ;Si no hay caracteres a escribir, saltar
					jz		input112
input111:		push	ax
					push	bx
					push	cx
					push	dx
					mov	al,byte ptr [bx]
					call	print_caracter
					pop	dx
					pop	cx
					pop	bx
					pop	ax
					inc	bx
					dec	al
					jnz	input111

input112:		push	ax
					push	bx
					push	cx
					push	dx
					mov   al,'_'
					call	print_caracter
					pop	dx
					pop	cx
					pop	bx
					pop	ax
					or		ah,ah
					jz    input20

					push	ax
					push	cx
					push	dx
					mov	al,32
					call	print_caracter_veces
					pop	dx
					pop	cx
					pop	ax

input20:			pop	bx 	;BX esta al final de cadena

input2:			mov	ax,0c07h
					int	21h
					cmp	al,27
					jz		input_fin2
					cmp	al,13
					jz		input_fin
					cmp	al,8
					jnz	input3
					or		dh,dh
					jz		input2
					dec	dh
					dec  	bx
					jmp	short input1
input3:			cmp	cl,dh
					jz		input2
					cmp	al,32
					jc		input2
					cmp	al,128
					jnc	input2
					mov	byte ptr [bx],al
					inc	bx
					inc	dh
					jmp	input1

input_fin:		mov	byte ptr [bx],0
					stc
input_fin2:

               ;hlt    ;;?????????
               
					pushf
					cli
					push	es
					call	cambia_vectores
					pop	es
					;mov   es,seg_spectrum
					sti

					and	bits_estado4,11110111b ;Refrescar el sonido
					
					mov	primera_vez,0

					popf

					ret

input_cursor	dw		0

convertir_ascii_num:
;Rutina que convierte una cadena de texto en un numero
;Entrada: BX=Offset Cadena, acabada en 0
;Salida:  AX=Numero
					xor	ax,ax
convertir_ascii_num_buc:
					mov	cl,byte ptr [bx]
					or		cl,cl
					jz		convertir_ascii_num_fin
					;Multiplicar AX por 10
					;mov	ch,10
					;mul	ch
					shl	ax,1 ;*2
					mov	dx,ax
					shl	ax,1 ;*4
					shl	ax,1 ;*8
					add   ax,dx ;*10
					sub 	cl,'0'
					xor	ch,ch
					add	ax,cx
					inc	bx
					jmp	short convertir_ascii_num_buc
convertir_ascii_num_fin:
					ret


liberar_teclas:
;Liberar todas las teclas pulsadas, incluso el joystick
					mov	cx,15
					mov	bx,offset puerto_65278
					mov	al,byte ptr [bx]
					;and	al,64
					or		al,10111111b ;Conservar bit de teclado ISSUE 1 o 2
input0:
					mov	byte ptr [bx],al
					inc	bx
					loop	input0
					mov	byte ptr [bx],31 ;Joystick a 0
					ret

actualiza_y_espera:

					call  act_pantalla
					ret

act_pantalla_border:  ;Actualizar toda la pantalla. Aqui deberia actualizar
;primero el border!

act_pantalla:  ;No actualizar el border
					call	actualiza_pantalla
					ret

print_cadena_sinlongitud:
;Imprime una cadena hasta encontrar el codigo 255
;Si se encuentra el codigo 254 (TESP) indica que el texto esta en espa�ol,
;en cuyo caso se imprime siempre que no se ejecute el emulador
;con la opcion /ENG. Si se encuentra el codigo 253 (TING) indica que el texto
;esta en ingles, en cuyo caso se imprime si se ejecuta el emulador con
;la opcion /ENG. Si se encuentra el codigo TCOM, indica que el texto es comun
;para todos los idiomas (si es a principio de texto, no hace falta ponerlo)
;Entrada: BX=Offset de la cadena
;TCAT				equ	251
TESP				equ	254
TING				equ   253
TCOM				equ	252
					and	bits_estado5,11111011b
print_cadena_sinlongitud_buc:
					mov	al,byte ptr [bx]
					inc	bx
					cmp	al,255
					jz		print_cadena_sinlongitud_fin

					cmp	al,TCOM
					jnz	print_cadena_sinlongitud_buc1
					and	bits_estado5,11111011b
					mov	al,byte ptr [bx]
					inc	bx
					jmp	short print_cadena_sinlongitud_buc3

print_cadena_sinlongitud_buc1:

					cmp	al,TESP
					jnz   print_cadena_sinlongitud_buc2
					and	bits_estado5,11111011b
					mov	al,byte ptr [bx]
					inc	bx

					test	bits_estado5,00000010b ;Ver si esta la opcion espa�ol
					jz    print_cadena_sinlongitud_buc3
					or		bits_estado5,00000100b ;No escribir el texto
					jmp	short	print_cadena_sinlongitud_buc

print_cadena_sinlongitud_buc2:
					cmp	al,TING
					jnz   print_cadena_sinlongitud_buc3
					and	bits_estado5,11111011b
					mov	al,byte ptr [bx]
					inc	bx

					test	bits_estado5,00000010b ;Ver si esta la opcion ingles
					jnz   print_cadena_sinlongitud_buc3
					or		bits_estado5,00000100b ;No escribir el texto
					jmp	short	print_cadena_sinlongitud_buc

print_cadena_sinlongitud_buc3:

					test	bits_estado5,00000100b
					jnz   print_cadena_sinlongitud_buc

					push	bx
					call	print_caracter
					pop	bx
					jmp	short	print_cadena_sinlongitud_buc
print_cadena_sinlongitud_fin:
					ret

print_cadenas:
;Imprime una cadena numero AL (primera: AL=0)
;Entrada: BX=Offset , AL=numero de la cadena
;Deben estar las cadenas asi: "Hola",255,"Adios",255,etc...
					or		al,al
					jz		print_cadenas_sigue
print_cadenas_buc:
					mov	ah,byte ptr [bx]
					inc	bx
					cmp	ah,255
					jnz 	print_cadenas_buc
					dec	al
					jmp	short print_cadenas
print_cadenas_sigue:
					jmp	print_cadena_sinlongitud
					


print_cadena_longitud:
;Imprime una cadena con longitud determinada o hasta encontrar el codigo 0
;Entrada: BX=Offset de la cadena
;			 AH=Longitud maxima de la cadena
;Salida:	 AH=Longitud que queda por escribir
print_cadena_longitud_buc:
					mov	al,byte ptr [bx]
					or		al,al
					jz		print_cadena_longitud_fin
					
					push	ax
					push	bx
					call	print_caracter
					pop	bx
					pop	ax
					inc	bx
					dec	ah
					jnz	print_cadena_longitud_buc
print_cadena_longitud_fin:
					ret

print_caracter_veces:
;Rutina que imprime el caracter AL las veces indicadas por AH
;Si AH=0, se vuelve sin hacer nada
					or		ah,ah
					jnz	print_caracter_veces2
					ret
print_caracter_veces2:
					mov	cl,ah
					xor	ch,ch

print_caracter_xveces:
;Rutina que imprime el caracter AL las veces indicadas por CX

					push	ax
					push	cx
					call	print_caracter
					pop	cx
					pop	ax
					loop	print_caracter_xveces
					ret

lee_siono:
;Rutina que lee la tecla s/y o la n
;devuelve Z si se pulsa s/y, devuelve NZ si se pulsa n
					call	espera_no_tecla
lee_siono_buc:

					call  multitarea_interprete

					test	byte ptr puerto_65022,2
					jz		lee_siono_fin

					test	byte ptr puerto_57342,16
					jz		lee_siono_fin
;puerto_57342	db		255  ; Y    U    I    O    P     ;5

					test	byte ptr puerto_32766,8
					jnz   lee_siono_buc
					or		al,1
lee_siono_fin:
					ret

crea_si_no_existe:
;Rutina que pregunta para crear un fichero si este no existe
;Entrada: DX=Offset de la cadena del fichero
;Salida:	 Z si existe el fichero o se ha creado
;			 NZ si no se ha creado y por lo tanto no existe
					push	dx
					call	liberar_teclas
					pop	dx
					xor	cx,cx
					mov	ah,4eh
					int	21h
					
					;jnc	crea_si_no_existe2
					jc		crea_si_no_existe00

					jmp	crea_si_no_existe2

crea_si_no_existe00:					

					push	dx
					push	cx
					call	actualiza_y_espera
					mov	word ptr cursor_x,0

					call	print_cadena
					db		13
					db		TESP,"El fichero no existe,",13
					db		"desea crearlo ? (S/N)"
					db		TING,"File doesn't exist,",13
					db		"do you want to create it ? (Y/N)"

					db		255
					call	lee_siono
					pop	cx
					pop	dx
					jz	   crea_si_no_existe1

					;Hay que apagar el motor
					mov	contador_disco,50*segundos

					ret
crea_si_no_existe1:
					mov	ah,3ch
					int	21h
					mov	bx,ax
					mov	ah,3eh
					int	21h

crea_si_no_existe2:
					;Hay que apagar el motor
					mov	contador_disco,50*segundos

					xor	al,al
					ret

multitarea_mirar:

					;Volver al menu 50 veces por segundo

					test	bits_estado4,01000000b
					jz		multitarea_no_volver

					mov	ah,dh
					sahf

					jmp	[p_menu_activo]

multitarea_no_volver:

					mov	interrumpir,1
					mov	al,bits_estado2
					jmp   no_salvapan ;Saltarse la comprobacion de ESC y salvapant.


multitarea_interprete:
;Rutina que es llamada por la rutina de lee_tecla_tabla, input,... para
;emular el Z80 mientras el menu esta activo


					pushf

					test	bits_estado5,1
					jz    multitarea_interprete1

					popf
					ret

multitarea_interprete1:
					push	ax
					push	bx
					push	cx
					push	dx
					push	si
					push	di


					mov	[p_menu_activo],offset multitarea_buc
					mov	interrumpir,1
					and	bits_estado4,10111111b ;Decir que no ha habido int. 8

multitarea_interprete11:
					mov	si,reg_pc
					mov	di,reg_sp
					cargar_flags_con_f

					lahf

					mov	dh,ah
					jmp	no_interrumpir

multitarea_buc:

					cargar_reg_f
					mov	reg_pc,si
					mov	reg_sp,di


					pop	di
					pop	si
					pop	dx
					pop	cx
					pop	bx
					pop	ax
					popf
					ret

tecla_menu_print_spc:
;Imprime 2 espacios (guardando BX)
					push	bx
					call	print_cadena
					db		"   ",255
					pop	bx
					ret

tecla_menu_ver_si_perm:
;Dice si la opcion se puede seleccionar, en cuyo caso se activa Z
;Entrada: BX=Direccion opcion menu
;Salida: Z si se puede seleccionar. BX no modificado
					;Mirar si la opcion es de condicion

					mov	ah,byte ptr [bx]
					test	ah,MENU_CONDICION
					jz    tecla_menu_ver_si_perm_fin
					;Llamar a la condicion
					and	ah,MENU_CONDICION-1 ;Con esto se hace que
					;se aplica una mascara de los bits anteriores a MENU_CONDICION
					;y por lo tanto, se saltaran esos parametros
					call	tabla_menu_longitud

					xor	ah,ah
					push	bx
					inc	bx
					add	bx,ax
					call	[bx]
					pop	bx

tecla_menu_ver_si_perm_fin:
					ret
					
					
tecla_menu_ver_si_archivo:
					mov	ah,byte ptr [bx]
					test	ah,MENU_ARCHIVO
					ret
					 

tecla_menu_volver_Z:
					;Esperar a liberar teclas y poner Z
					push	bx
					call	espera_no_tecla
					pop	bx
					xor	al,al
					ret

tecla_menu_print_cursor:
					call	print_cadena
					db		" ->",255
					ret
					
tecla_menu_print_cursor_archivo:
					call	print_cadena
					db		" F>",255
					ret					


tecla_menu:
;Rutina de gestion de las opciones del menu
;Entrada: BX=Direccion tabla
;Salida:  NZ si no se elige ninguna opcion y ha de salir el salvapantallas
;			 Z  si se elige una opcion, BX=Direccion de salto
;
;Dentro de la rutina: BX=Direccion de la posicion actual de opcion
;Error: Si la primera opción tiene tecla asociada, por ejemplo, ESC, no funcionara la tecla. Pero si por seleccion (enter)


					mov   tecla_menu_dir,bx
tecla_menu0:   mov   bx,tecla_menu_dir
					mov	word ptr cursor_x,0
					;Posicionarse en la opcion indicada
					mov   cl,byte ptr [bx]
					xor	ch,ch
					inc	bx
					call	tabla_menu_saltar

					call	tecla_menu_ver_si_perm

					;Opcion no permitida.
					jnz	tecla_menu_buc00

tecla_menu_buc0:
					call tecla_menu_ver_si_archivo
					jz   tecla_menu_buc0_no_arch

					;Escribir cursor archivo
					push	bx
					call	tecla_menu_print_cursor_archivo
					pop	bx
					jmp	short tecla_menu_buc000
					

tecla_menu_buc0_no_arch:
					;Escribir cursor
					push	bx
					call	tecla_menu_print_cursor
					pop	bx
					jmp	short tecla_menu_buc000

tecla_menu_buc00:
					;Escribir cursor no seleccionable
					push	bx
					call	print_cadena
					db		" -x",255
					pop	bx


tecla_menu_buc000:
					mov	cursor_x,0

tecla_menu_buc:
					cmp	tecla_menu_volver,1
					jz    tecla_menu_buc2

					push	bx
					call	espera_no_tecla
					pop	bx

tecla_menu_buc2:
					push	bx
					call	lee_teclas_and
					pop	bx

					jnz 	tecla_menu_pulsada

					;Mirar si se debe volver (solo usado en menu registros
					;y en menu registros_sonido)
					cmp	tecla_menu_volver,1
					jnz   tecla_menu_buc21

					;Poner NZ
					or		al,1
					ret

tecla_menu_buc21:

					;Mirar si se ha de activar el salvapantallas
					test	bits_estado2,4
					jz		tecla_menu_buc2
					ret   ;NZ

tecla_menu_pulsada:
					;Mirar si se pulsa ENTER, + , -, arriba, abajo.
					;Luego, mirar si se pulsa alguna de las teclas de la tabla

					test	puerto_49150,1  ; H		J	  K	 L    Enter
					jnz	tecla_menu_noenter

tecla_menu_enter:

					;Se pulsa el ENTER. Ver si se puede seleccionar
					call	tecla_menu_ver_si_perm
					jnz	tecla_menu_enter_no

					;Devolver direccion de salto. Si la opcion es + -, devolver
					;la de +

					;Devolver la primera direccion (que corresponde a la del
					;enter o a la de +)
					inc	bx
					mov	bx,word ptr [bx]
					jmp	tecla_menu_volver_Z

tecla_menu_enter_no:
					jmp	tecla_menu_buc

tecla_menu_noenter:
					;Mirar si se pulsa arriba
					test	puerto_221,8	  ; Ins  Arr  Aba  Izq  Der
					jz	tecla_menu_arriba

					test	puerto_61438,8   ; 6    7    8    9    0     ;4
					jnz 	tecla_menu_noarriba


tecla_menu_arriba:
					;Mirar si la opcion seleccionada no es la 0
					push	bx
					mov	bx,tecla_menu_dir
					cmp	byte ptr [bx],0
					pop	bx
					jz    tecla_menu_buc

					push	bx
					call	espera_no_tecla
					pop	bx

					call  tecla_menu_print_spc
					mov	bx,tecla_menu_dir
					dec	byte ptr [bx]
					jmp	tecla_menu0

tecla_menu_noarriba:		;Mirar si se pulsa abajo
					test	puerto_221,4	  ; Ins  Arr  Aba  Izq  Der
					jz	tecla_menu_abajo

                                        test    puerto_61438,16   ; 6    7    8    9    0     ;4
                                        jnz     tecla_menu_noabajo


tecla_menu_abajo:

					push	bx
					call	espera_no_tecla
					pop	bx

					call  tecla_menu_print_spc

					;Mirar si hay mas opciones
					mov	cx,1
					call	tabla_menu_saltar
					cmp	byte ptr [bx],MENU_FIN
					mov	bx,tecla_menu_dir
					jz		tecla_menu_abajo_tope

					inc	byte ptr [bx]

					jmp	tecla_menu0

tecla_menu_abajo_tope:
					jmp	tecla_menu0

tecla_menu_noabajo:
					;Mirar si se pulsa +

					test	puerto_especial7,4  ; -    5    +    End  PgDn
					jnz	tecla_menu_nomas
					jmp   tecla_menu_enter

tecla_menu_nomas:

					;Mirar si se pulsa -  o  ESPACIO

					test	puerto_especial7,16  ; -    5    +    End  PgDn
					jnz	tecla_menu_nomenos
					
tecla_menu_menos0:					
										
					;Ver si se permite pulsar -
					mov	ah,byte ptr [bx]
					test	ah,MENU_MASMENOS
					jz		tecla_menu_menos_no
					;Ver si se puede seleccionar
					call	tecla_menu_ver_si_perm
					jz		tecla_menu_menos

tecla_menu_menos_no:
					jmp	tecla_menu_buc

tecla_menu_menos:
					mov	bx,word ptr [bx+3] ;El - esta despues del +
					jmp	tecla_menu_volver_Z

tecla_menu_nomenos:
					;Mirar si se ESPACIO

					test	puerto_32766,1  ; B    N    M    Simb Space
					jz	    tecla_menu_menos0

;tecla_menu_nomenos2:
					;Ver si se pulsa cualquier otra tecla que este en las
					;teclas del menu

					mov	bx,tecla_menu_dir
					inc	bx
tecla_menu_teclas_buc:
					mov	ah,byte ptr [bx]
					test	ah,MENU_TECLADO
					jz    tecla_menu_teclas_sigue
					and	ah,MENU_TECLADO-1
					call	tabla_menu_longitud
					xor	ah,ah
					push	bx
					add	bx,ax
					inc	bx
					mov	dl,byte ptr [bx+2]
					mov	bx,word ptr [bx]
					test	byte ptr [bx],dl
					pop	bx
					jnz   tecla_menu_teclas_sigue

					;Ver si se puede seleccionar
					call	tecla_menu_ver_si_perm
					jnz	tecla_menu_teclas_sigue

					;Devolver direccion de salto. Si la opcion es + -, devolver
					;la de +

					;Devolver la primera direcion (que corresponde a la del
					;enter o a la de +)
					inc	bx
					mov	bx,word ptr [bx]
					jmp	tecla_menu_volver_Z

tecla_menu_teclas_sigue:
					mov	cx,1
					call	tabla_menu_saltar
					cmp   byte ptr [bx],MENU_FIN
					jnz	tecla_menu_teclas_buc

					jmp	tecla_menu0

tabla_menu_saltar:
;Rutina que, dado una posicion en la tabla, se salta N opciones y se situa
;en la opcion siguiente
;Se usa cuando se pulsa flecha abajo (CX=1) y al pulsar flecha arriba
;(CX=posicion opcion-1 y BX=Inicio de la tabla+1 ->el +1 es para saltar el
;byte de posicion de opcion)
;Entrada: BX=Direccion tabla, CX=Opciones a saltar
;Salida:  BX=Direccion tabla actual, cursor_y=posicion actual del cursor

					call	tabla_menu_saltar_saltos
					jcxz	tabla_menu_saltar_fin

tabla_menu_saltar_buc:

					mov	ah,byte ptr [bx]
					call	tabla_menu_longitud
					;Si es 0, no era una opcion (era salto de linea)
					or		al,al
					jnz   tabla_menu_no0

					call	tabla_menu_saltar_saltos

					mov	ah,byte ptr [bx]
					call	tabla_menu_longitud

tabla_menu_no0:
					inc	bx

					xor	ah,ah
					add	bx,ax

					inc   cursor_y
					dec	cx
					jz    tabla_menu_saltar_fin

					jmp	short tabla_menu_saltar_buc


tabla_menu_saltar_fin:
					call	tabla_menu_saltar_saltos
					ret

tabla_menu_saltar_saltos:
;Rutina que situa el BX a partir de donde no hay saltos de linea
;Entrada: BX
;Salida: BX, cursor_y actualizado
					mov	ah,byte ptr [bx]
					cmp	ah,MENU_NADA
					jnz   tabla_menu_saltar_saltos2
					inc	cursor_y
					inc	bx
					jmp	short tabla_menu_saltar_saltos
tabla_menu_saltar_saltos2:
					ret

tabla_menu_longitud:
;Subrutina que devuelve el numero de bytes que ocupa una opcion
;(sin contar el byte de tipo_opcion)
;Entrada: AH=Tipo opcion
;Salida:  AL=Longitud

					xor	al,al
					test	ah,MENU_OPCION
					jz		tabla_menu_longitud_noopc
					add   al,2
tabla_menu_longitud_noopc:
					test	ah,MENU_MASMENOS
					jz		tabla_menu_longitud_nomasmenos
					add   al,2 ;Si es + -, hay 2 direcciones de memoria, pero
					;ya habra incrementado 1 al detectar que es opcion
tabla_menu_longitud_nomasmenos:
					test	ah,MENU_CONDICION
					jz		tabla_menu_longitud_nocond
					add   al,2
tabla_menu_longitud_nocond:
					test	ah,MENU_TECLADO
					jz		tabla_menu_longitud_notecla
					add   al,3

tabla_menu_longitud_notecla:
					ret

tecla_menu_dir	dw	? ;Direccion de inicio de la tabla
tecla_menu_volver db 0 ;Si esta a 1, en el bucle de espera_tecla,se vuelve
						;(solo usado en ver registros, ver reg. sonido)

COMMENT !
La tabla del menu tiene la siguiente estructura:

					db		?
Opcion sobre la que esta posicionado el cursor,comenzando desde 0

					db		tipo_opcion
Tipo de opcion de menu indicada; es un campo de bits:
MENU_NADA : Indica que aqui no hay ninguna opcion, es un salto de linea;
si las opciones del menu empiezan en la linea 2, al principio de la tabla
ha de haber 2 MENU_NADA
MENU_OPCION : Indica que a continuacion hay una opcion valida
MENU_MASMENOS : Indica que la opcion admite las teclas + y -; si se pulsa
enter, funciona como el +. Si no se pone, la opcion solo admite ENTER
MENU_CONDICION : Indica que la opcion puede seleccionarse o no; se llama
a una funcion indicada, y si devuelve Z se puede seleccionar.
MENU_TECLADO : Indica que la opcion tambien puede seleccionarse pulsando una
tecla determinada
MENU_ARCHIVO : Indica que el cursor mostrara "F>" al pasar sobre la opcion
MENU_FIN : Fin de opciones del menu

					db o dw parametros
Aqui van las direcciones a las que salta la opcion, condicion, etc.Se
escriben siguiendo este orden:
					dw		direccion_salto ;Direccion a la que salta la opcion,
					en caso que no admita + -

					dw		direccion_salto_mas,direccion_salto_menos ;Direcciones
					a las que salta en caso de pulsar + o -, si se pulsa enter,
					vale como +

					dw		direccion_condicion ;Direccion de la rutina que comprueba
					una condicion determinada para ver si la opcion puede
					seleccionarse (debe devolver Z si se puede seleccionar)

					dw		puerto_tecla, db mascara_tecla ;Valores de la tecla en
					caso que la opcion pueda seleccionarse o no

Si hay una caracteristica de la opcion que no existe, entonces no hace
falta poner el db o dw en la tabla, pero SI hay que mantener el orden de
los parametros

!

MENU_NADA 		equ	0
MENU_OPCION 	equ	1
MENU_MASMENOS 	equ 	2
MENU_CONDICION equ	4
MENU_TECLADO	equ	8
MENU_ARCHIVO	equ	16
MENU_FIN 		equ	128

MENU_SALTO		equ	MENU_NADA

tabla_menu_menu db      0
					db		MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA

					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    menu_cargar_filesel,menu_cargar
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw		menu_grabar_filesel,menu_grabar
					db    MENU_OPCION
					dw    menu_ordenador
					db		MENU_OPCION
					dw		menu_pantalla
					db		MENU_OPCION
					dw		menu_depuracion
					db		MENU_OPCION
					dw		menu_cinta

					db		MENU_OPCION
					dw		menu_hardware


					db		MENU_OPCION
					dw		menu_sonido
					db		MENU_OPCION
					dw		menu_idioma
					db		MENU_OPCION
					dw		menu_pausa
					db		MENU_OPCION
					dw		menu_velocidad

					db		MENU_SALTO


					db		MENU_OPCION+MENU_TECLADO
					dw		menu_ayuda
					dw		puerto_especial1
					db		16					;Tecla F1

					db		MENU_OPCION+MENU_TECLADO
					dw		menu_volver
					dw		puerto_especial1
					db		8					;Tecla F2

					
					db		MENU_OPCION+MENU_TECLADO
					dw		menu_salir
					dw		puerto_especial2
					db		1					;Tecla F10


					db		MENU_FIN



tabla_menu_ordenador db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA
					db    MENU_OPCION
					dw    ordenador_16k
					db    MENU_OPCION
					dw    ordenador_48k
					db    MENU_OPCION
					dw    ordenador_inves
					db    MENU_OPCION
					dw    ordenador_128k
					db    MENU_OPCION
					dw    ordenador_128ks
					db    MENU_OPCION
					dw    ordenador_p2
					db    MENU_OPCION
					dw    ordenador_p2f
					db    MENU_OPCION
					dw    ordenador_p2s
					db    MENU_OPCION
					dw    ordenador_p2a40
					db    MENU_OPCION
					dw    ordenador_p2a41
					db    MENU_OPCION
					dw    ordenador_p2as

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_inicio
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN



tabla_menu_depuracion db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_OPCION
					dw		depuracion_RESET
					db		MENU_OPCION
					dw    depuracion_NMI
					db		MENU_OPCION
					dw    depuracion_registros
					db		MENU_OPCION
					dw    depuracion_pokear

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_inicio
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN

tabla_menu_registros db	0
					db		MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA
					db    MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA,MENU_NADA

					db		MENU_OPCION
					dw    registros_interrupcion
					db		MENU_OPCION
					dw    registros_modo

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_depuracion
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN

tabla_menu_hardware db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_OPCION
					dw    hardware_teclado
					db		MENU_OPCION
					dw    hardware_disparador
					db		MENU_OPCION+MENU_MASMENOS+MENU_CONDICION
					dw    hardware_disparador_mas,hardware_disparador_menos
					dw		hardware_disparador_cond

					db		MENU_OPCION
					dw		hardware_cursores

					
					db		MENU_OPCION+MENU_CONDICION
					dw    hardware_poke_rom,hardware_poke_rom_cond
					db		MENU_OPCION
					dw		hardware_compartido
					db		MENU_OPCION+MENU_CONDICION
					dw    hardware_sinc_blaster,hardware_sinc_blaster_cond

					

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_inicio
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN

tabla_menu_pantalla db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    pantalla_cargar_filesel,pantalla_cargar
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    pantalla_grabar_filesel,pantalla_grabar
					db		MENU_OPCION+MENU_MASMENOS
					dw		pantalla_brillo_mas,pantalla_brillo_menos
					db		MENU_OPCION
					dw		pantalla_protector
;					db		MENU_OPCION
;					dw		pantalla_actualizacion
;					db		MENU_OPCION
;					dw		pantalla_frameskip_cambiar

					db		MENU_NADA
					db		MENU_OPCION
					dw    pantalla_record_ins
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    pantalla_record_fic_filesel,pantalla_record_fic

					db		MENU_OPCION+MENU_MASMENOS
					dw    pantalla_record_fps_mas,pantalla_record_fps_menos

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_inicio
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN

tabla_menu_cinta db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_OPCION
					dw    cinta_ins_entrada
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    cinta_fic_entrada_filesel,cinta_fic_entrada
					db		MENU_OPCION
					dw    cinta_ins_salida
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    cinta_fic_salida_filesel,cinta_fic_salida
					db		MENU_OPCION
					dw    cinta_cualquier_flag

					db		MENU_SALTO

					db		MENU_OPCION+MENU_CONDICION
					dw		menu_casete
					dw    menu_casete_cond

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_inicio
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN

tabla_menu_casete db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA
					db		MENU_OPCION+MENU_MASMENOS
					dw    casete_baudios_mas,casete_baudios_menos

					db		MENU_SALTO

					db		MENU_OPCION+MENU_MASMENOS
					dw		casete_onda_guia_mas,casete_onda_guia_menos
					db		MENU_OPCION+MENU_MASMENOS
					dw    casete_tono_guia_mas,casete_tono_guia_menos
					db		MENU_OPCION+MENU_MASMENOS
					dw    casete_onda_ceros_mas,casete_onda_ceros_menos
					db		MENU_OPCION+MENU_MASMENOS
					dw    casete_onda_unos_mas,casete_onda_unos_menos

					db		MENU_SALTO

					db		MENU_OPCION
					dw    casete_filtro
					db		MENU_OPCION+MENU_MASMENOS
					dw    casete_izq_mas,casete_izq_menos
					db		MENU_OPCION+MENU_MASMENOS
					dw    casete_der_mas,casete_der_menos
					db		MENU_OPCION
          ;+MENU_CONDICION
					dw    casete_border
					;dw    casete_border_cond
					db		MENU_OPCION
					dw    casete_autoch

					db		MENU_SALTO

					db		MENU_OPCION
					dw    casete_cargar

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_cinta
					dw		puerto_especial3
					db		16					;Tecla ESC


					db		MENU_FIN


tabla_menu_sonido db 0
					db		MENU_NADA,MENU_NADA,MENU_NADA

					db		MENU_OPCION
					dw    sonido_si_no


;					db		MENU_OPCION
;					dw    sonido_salida

;					db		MENU_OPCION+MENU_CONDICION
;					dw		sonido_altavoz_modo,sonido_altavoz_modo_cond
;					db    MENU_OPCION+MENU_CONDICION
;					dw    sonido_altavoz_frec,sonido_altavoz_frec_cond
;					db    MENU_NADA

					db		MENU_OPCION
					dw    sonido_presente

;					db		MENU_OPCION+MENU_CONDICION
;					dw    sonido_salida_ay
;					dw		sonido_ver_registros_cond

					db		MENU_OPCION+MENU_CONDICION
					dw		sonido_modo_stereo
					dw		sonido_modo_stereo_cond
					db		MENU_OPCION+MENU_CONDICION
					dw		sonido_ruido
					dw		sonido_ver_registros_cond

;					db		MENU_OPCION+MENU_CONDICION
;					dw		sonido_refresco_ay
;					dw		sonido_refresco_ay_cond
					db		MENU_OPCION+MENU_CONDICION
					dw		sonido_ver_registros
					dw		sonido_ver_registros_cond

					db		MENU_NADA
					db		MENU_OPCION
					dw    sonido_record_ins
					db		MENU_OPCION+MENU_MASMENOS+MENU_ARCHIVO
					dw    sonido_record_fic_filesel,sonido_record_fic

					db		MENU_SALTO

					db		MENU_OPCION+MENU_TECLADO
					dw    menu_inicio
					dw		puerto_especial3
					db		16					;Tecla ESC

					db		MENU_FIN




