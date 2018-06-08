;Rutinas para la emulaci�n de cassette a trav�s de un fichero
;La estructura del fichero es la misma que la usada en el emulador Z80:

COMMENT !

	 .TAP FILES:
	 -----------

	 The .TAP files contain blocks of tape-saved data.  All blocks start with
	 two bytes specifying how many bytes will follow (not counting the two
	 length bytes).  Then raw tape data follows, including the flag and
	 checksum bytes.  The checksum is the bitwise XOR of all bytes including
	 the flag byte.  For example, when you execute the line SAVE "ROM" CODE
	 0,2 this will result:


				 |------ Spectrum-generated data -------|       |---------|
       13 00 00 03 52 4f 4d 7x20 02 00 00 00 00 80 f1 04 00 ff f3 af a3

		 ^^^^^       first block is 19 bytes (17 bytes+flag+checksum)
             ^^    flag byte (A reg, 00 for headers, ff for datablocks)
                ^^ first byte of header, indicating a code block

		 filename    ^^^^^^^^^^^^^
       header info               ^^^^^^^^^^^^^^^^^
		 checksum of header                          ^^
		 length of second block                         ^^^^^
       flag byte                                             ^^
		 first two bytes of rom                                  ^^^^^
		 checksum (checkbittoggle would be better)                     ^^


    The emulator will always start reading bytes at the beginning of a
    block.  If less bytes are loaded than are available, the other bytes are
	 skipped, and the last byte loaded is used as checksum.  If more bytes
    are asked for than exist in the block, the loading routine will
    terminate with the usual tape-loading-error flags set, leaving the error
    handling to the calling Z80 program.

	 Note that it is possible to join .TAP files by simply stringing them
    together, for example COPY /B FILE1.TAP + FILE2.TAP ALL.TAP

    For completeness, I'll include the structure of a tape header.  A header
    always consists of 17 bytes:

        Byte    Length  Description
		  0       1       Type (0,1,2 or 3)
		  1       10      Filename (padded with blanks)
		  11      2       Length of data block
        13      2       Parameter 1
        15      2       Parameter 2

	 The type is 0,1,2 or 3 for a Program, Number array, Character array or
    Code file.  A screen$ file is regarded as a Code file with start address
    16384 and length 6912 decimal.  If the file is a Program file, parameter
    1 holds the autostart line number (or a number >=32768 if no LINE
    parameter was given) and parameter 2 holds the start of the variable
    area relative to the start of the program.  If it's a Code file,
    parameter 1 holds the start of the code block when saved, and parameter
    2 holds 32768.  For data files finally, the byte at position 14 decimal
	 holds the variable name.

	 Al grabar un bloque cualquiera, este se coloca al final del fichero .TAP
	 Al cargar un bloque, se dispone de una "cinta infinita", o sea, se va
	 leyendo uno por uno cada bloque, y si se llega al final, se posiciona
	 nuevamente al principio. Si se carga un bloque, y este tiene un flag 
	 diferente, se devuelve error. Si el bloque a cargar es m�s grande
	 que el solicitado, se carga la longitud solicitada y se devuelve error.
	 Si es m�s peque�o que el solicitado, se carga y se devuelve error.
	 En todos los casos de vuelta de error, el puntero se posiciona
	 en el siguiente bloque.
	 Si se intenta cargar o grabar y no hay cinta(fichero .TAP) insertado,
	 se devuelve error. L�gicamente, al grabar desde Basic no se vera el error.
	 Para saber el tipo de error (cinta no insertada), error de disco,  etc.,
	 se debe acudir al men�. Si se intenta cargar o grabar con longitud
	 cero, se devuelve error.

	 Antes de cargar o grabar un bloque, se debe enviar una serie de
	 caracteres ASCII al puerto CCH=204. El primero de estos caracteres
	 debe ser el c�digo 255, que indica el inicio de una operaci�n.
	 Los siguientes deben ser : "C"=67,"I"=73,"N"=78,"T"=84,"A"=65.
	 A partir de aqu� se puede iniciar las siguientes operaciones.
	 En el caso de alguno de los caracteres siguientes no coincida,
	 el puerto queda "bloqueado", y se debe inicializar mandando el 255
	 del inicio.

	 /*Para el caso de cargar o grabar, se debe enviar a trav�s del puerto
	 CDH=205, primero el flag, luego el inicio de carga del fichero (el primer
	 byte el menos significativo, el segundo el m�s significativo), y luego la
	 longitud del fichero (bytes menos y m�s significativo).*/
	 Despu�s de esto, se debe enviar un valor cualquiera al puerto 205 para
	 cargar, o al puerto 206 para grabar
	 Si antes de enviar alguno de estos bytes ha habido alg�n error por parte
	 del usuario, se puede iniciar nuevamente la operaci�n enviando la cadena
	 de inicializaci�n (255,"CINTA").
	 En ese momento, el emulador comienza la operaci�n de disco. Si hay error
	 de cualquier tipo (de flag, de disco, cinta no insertada) se devuelve
	 en el puerto 204 en entrada el bit 0 alzado.
	 Al cargar, si hay error o si no, se posiciona el puntero en el siguiente
	 bloque.

	 El puerto 204 en lectura, indica en el bit 0 si esta alzado
	 que ha habido cualquier tipo de error con la cinta.


!

;PUBLIC puerto204,puerto204_in,puerto205,puerto206

fichero_TAP_entrada 		db		80 dup (0)
fichero_TAP_salida 		db		80 dup (0)
;cadena_inicio		db		"CINTA"
;posicion_cadena	db		0 ;A 0 indica que el puerto CCH=204 est� bloqueado
;o no inicializado hasta mandar el 255. Despu�s de esto, contiene 1..5 el 
;caracter que se debe introducir, si es 6 quiere decir que la cadena
;se ha introducido completamente.

;posicion_puerto205 	db		0 ;Indica el estado de introducci�n de datos
;en el puerto 205. Si es 0, espera flag; si es 1 o 2 espera inicio; si es 3
;o 4 espera longitud. Y si es 5 espera operaci�n: cargar o grabar.
;A 6 indica que se han introducido todos.
cinta_flag			db		?
cinta_inicio		dw		?
cinta_longitud    dw		?
cinta_checksum		db		?

checksum_inicial	db		?

buffer_cinta		db		3 dup (?)
;Buffer para escribir checksum, leer longitud,etc

puntero_cinta_l	dw		0 ;
puntero_cinta_h	dw		0 ;Puntero del fichero para lectura (32 bits)

flag_err				db		? ;para puerto205_cargar_fin, si es 1 pone error
;Se utiliza por la necesidad de devolver error y mirar si eof

fin_cinta_bien:	and	bits_estado2,11111110b
						ret

fin_cinta_mal_grabar:
;Numero de error
						mov error_io_num,2
;Extraemos cinta de salida
						;and	bits_estado3,11111101b

;Avisamos error io 
						or		bits_estado8,00000010b

;Y hacemos que salga el menu
						mov	interrumpir,1
						or		bits_estado2,00001000b
;Version 3.4
						

;;fin_cinta_mal_grabar2:

						call fin_cinta_mal_cargar2
						call expulsar_cinta_salida
						ret_pc
						ret


;Notificar del error al BASIC. Se notifica mediante la modificacion del reg_f
fin_cinta_mal_cargar:	
;Version 3.4
;Numero de error
						mov error_io_num,1
;Extraemos cinta de entrada
;disabled. solo se extrae cinta cuando se llega al final de ella

;Avisamos error io 
						or		bits_estado8,00000010b

;Y hacemos que salga el menu
						mov	interrumpir,1
						or		bits_estado2,00001000b
;Version 3.4

;Ha habido error de tipo inicializacion de OUT. no notificar al BASIC ni por menu
fin_cinta_mal_cargar2:		
						or		bits_estado2,1
;						mov	posicion_cadena,0
						ret

COMMENT !
puerto204_in:		mov	al,bits_estado2
						and	al,1
						ret

puerto204:			;En BL viene el valor
						cmp	bl,255
						jnz	puerto204_2
						mov	posicion_cadena,1
						jmp	short fin_cinta_bien
puerto204_2:		mov	al,posicion_cadena
						or		al,al
						jz		fin_cinta_mal_cargar2 ;El inicio de la cadena no es 255
						cmp	al,6
						jnz	puerto204_3
;Se ha introducido toda la cadena; se vuelve sin hacer nada.
						jmp 	short fin_cinta_bien
						ret
puerto204_3:      mov	ah,bl
						mov	bx,offset cadena_inicio
						dec	al
						xlat
						cmp	al,ah
						jz		puerto204_4
;No coincide la cadena, error y se bloquea el puerto
						jmp	short fin_cinta_mal_cargar2
						ret
puerto204_4:		;Coincide la letra
						inc	posicion_cadena
						jmp	fin_cinta_bien
						;ret
!

pon_valores_cinta:

						;Poner valores de flag,inicio,longitud en
						;posicion correspondiente
						mov	al,reg_a_
pon_valores_cinta2:
						mov	cinta_flag,al
						mov	ax,word ptr reg_ixl
						mov	cinta_inicio,ax
						mov	ax,word ptr reg_e
						mov	cinta_longitud,ax
						ret

COMMENT !
puerto206: ;Grabar
						cmp	posicion_cadena,6
						jz		puerto206_2
						;No est� inicializado el puerto
						jmp	fin_cinta_mal_cargar2
!

;Nos disponemos a grabar
save_1222:
puerto206_2:
						mov	al,reg_a
						call  pon_valores_cinta2
						jmp	puerto205_grabar
COMMENT !
puerto205: ;Cargar
						cmp	posicion_cadena,6
						jz		puerto205_2
						;No est� inicializado el puerto
						jmp	fin_cinta_mal_cargar2
!

;Nos disponemos a cargar
load_1378:
puerto205_2:
						call	pon_valores_cinta

puerto205_cargar:
						mov	checksum_inicial,0
						test	bits_estado3,4
						jnz	puerto205_cargar_
						;No hay cinta insertada
						jmp	fin_cinta_mal_cargar2
puerto205_cargar_:
						cmp	cinta_longitud,0
						jnz	puerto205_cargar_nolon0
						jmp	fin_cinta_mal_cargar2

puerto205_cargar_nolon0:
						call	inicio_cinta

						mov	flag_err,0

						mov	ax,3d00h ;Abrir para lectura
						mov	dx,offset fichero_TAP_entrada
						int	21h
						jnc	puerto205_cargar00
						jmp	puerto205_cargar_fin0

puerto205_cargar00:

						mov	bx,ax

						mov	ax,4200h ;Mover puntero desde inicio
						mov	dx,puntero_cinta_l
						mov	cx,puntero_cinta_h
						int	21h
						jnc	puerto205_cargar1
puerto205_cargar0:
						jmp	puerto205_cargar_fin

puerto205_cargar1:
						test	bits_estado4,1
						jz    puerto205_cargar1_

						and	reg_f_,10111111b ;Poner Flag NZ (no cargar cualquier flag)

puerto205_cargar1_:

						mov	cx,2 ;3
						mov	dx,offset buffer_cinta ;Leer longitud real
						mov	ah,3fh
						int	21h ;Leer longitud
						jc		puerto205_cargar0

						;Ver si esta en modo cargar cualquier flag
						test	reg_f_,01000000b   ;SZxHxVNC
						jz		puerto205_cargar10

						;Modo cualquier flag
						;inc	cinta_longitud
						jmp	short puerto205_cargar2

puerto205_cargar10:
						mov	cx,1
						mov	dx,offset buffer_cinta+2
						mov	ah,3fh
						int	21h ;Leer flag
						jc		puerto205_cargar0

						;Comparar flag
						mov	al,byte ptr buffer_cinta+2
						mov	checksum_inicial,al
						cmp	al,cinta_flag
						jz		puerto205_cargar2

						;No coincide el flag. Avanzar bloque,error y fin de carga
						mov	dx,word ptr buffer_cinta
						dec	dx ;Avanzar bytes-1 (1 del flag ya le�do)
						xor	cx,cx
						mov	ax,4201h
						int	21h
						jc	   puerto205_cargar0


						and	reg_f,11111110b ;SZxHxVNC Decir error de carga
						;mov	flag_err,1

						jmp	puerto205_cargar_mirar

puerto205_cargar2:

						mov	cx,word ptr buffer_cinta
						;CX=Longitud real
						dec	cx
						dec	cx ;2 menos(flag y checksum)
						test	reg_f_,01000000b   ;SZxHxVNC
						jz		puerto205_cargar20
						;Modo cualquier flag
						inc	cx
puerto205_cargar20:

						;Comparar longitud real con la solicitada. cinta_longitud=pedida. cx=la indicada en el .tap
						cmp	cx,cinta_longitud
						jz		puerto205_cargar_si_longitud

						;temporal disabled. aqui fallan el rocman, el lemmings, el converso
						;en el caso del converso, se pone a cargar un bloque de 17 bytes - cabecera
						;cuando el bloque siguiente es mayor (cabecera de tipo sped)
						;temporal mov	flag_err,1
						;al final flag_err no se pone a 1 en ningun sitio! revisar en un futuro la utilidad del mismo



						jc		puerto205_cargar_si_longitud

						;Aqui se piden menos bytes a cargar
						;esto muestra error del emulador I/O. realmente es necesario?

						;Tambien genera error para la rutina de carga

						;Si luego si compara el byte de checksum, realmente vale la pena poner el flag_err?
						;Realmente el byte de checksum en este caso deberia ser el siguiente byte leido
						;no el del final del bloque. Esto ya lo hace en puerto205_cargar21

						;p.ej:
						;cabecera sped: 34 bytes
						;los primeros 17 + 1 bytes son la cabecera estandard + el checksum
						

						mov	cx,cinta_longitud
						mov	dx,cinta_inicio
						mov	ds,seg_spectrum
						mov	ah,3fh
						int	21h
						push	cs
						pop	ds

						jnc	puerto205_cargar21

						jmp   puerto205_cargar_fin
						;jc		puerto205_cargar_fin
puerto205_cargar21:
						call	puerto205_actualiza

						mov	dx,offset cinta_checksum
						mov	cx,1
						mov	ah,3fh
						int	21h
						jnc	puerto205_cargar22

						jmp   puerto205_cargar_fin
						;jc		puerto205_cargar_fin
puerto205_cargar22:

						;mov	al,cinta_checksum
						;xor	reg_a,al

						mov	ax,word ptr buffer_cinta
						dec	ax
						dec	ax

						test	reg_f_,01000000b   ;SZxHxVNC
						jz		puerto205_cargar23
						inc	ax

puerto205_cargar23:

						sub	ax,cinta_longitud
						mov	dx,ax
						xor	cx,cx
						mov	ax,4201h ;Saltar los bytes de m�s
						int	21h
						jnc	puerto205_cargar3

						jmp	puerto205_cargar_fin
						;jc		puerto205_cargar_fin
puerto205_cargar3:

						push	cs
						pop	ds
						jmp   short puerto205_cargar_si_longitud3


puerto205_cargar_si_longitud:
						;Aqu� coincide la longitud
						or		reg_f,00000001b ;SZxHxVNC
						;De momento, decir que no hay error de carga
						mov	dx,cinta_inicio
						mov	ds,seg_spectrum
						mov	ah,3fh
						int	21h
						jnc   puerto205_cargar_si_longitud1
						jmp	puerto205_cargar_fin
						;jc		puerto205_cargar_fin

puerto205_cargar_si_longitud1:
						push	cs
						pop	ds
						call	puerto205_actualiza
puerto205_cargar_si_longitud2:

						;Leer el byte de checksum
						mov	cx,1
						mov	dx,offset cinta_checksum
						mov	ah,3fh
						int	21h
						jnc   puerto205_cargar_si_longitud3
						jmp	puerto205_cargar_fin

puerto205_cargar_si_longitud3:

						mov	al,cinta_checksum
						xor	reg_a,al

						jz    puerto205_cargar_si_longitud31
						and	reg_f,11111110b ;SZxHxVNC Error de carga!

puerto205_cargar_si_longitud31:

						;Si se intenta cargar mas bytes de los que hay,
						;escribir byte de checksum como cargado

						mov	cx,word ptr buffer_cinta
						dec	cx
						dec	cx ;2 menos(flag y checksum)

						test	reg_f_,01000000b   ;SZxHxVNC
						jz		puerto205_cargar_si_longitud4
						inc	cx


puerto205_cargar_si_longitud4:


						;Comparar longitud real con la solicitada
						cmp	cx,cinta_longitud
						jnc	puerto205_cargar_mirar
						and	reg_f,11111110b ;SZxHxVNC Decir error de carga


						push	bx
						mov   bx,cinta_inicio
						add	bx,word ptr buffer_cinta
						dec	bx
						push	ds
						mov	ds,seg_spectrum
						mov	byte ptr [bx],al
						pop	ds
						pop	bx
						inc	word ptr reg_ixl


puerto205_cargar_mirar:


						;Mirar si es final del fichero
						;Se intenta leer 1 byte
						mov	cx,1
						mov	ah,3fh
						int	21h
						or		ax,ax ;Si AX=0, fin fichero
						jnz	puerto205_cargar_no_eof

						;Mejor, expulsamos cinta
						clc
						call puerto205_cargar_fin
						jmp short expulsar_cinta_entrada


expulsar_cinta_entrada:

;						push si
;						push di
;						push ax
;						pushf
						call  cinta_ins_entrada_ext2
;						popf
;						pop ax
;						pop di
;						pop si

						;Hay que hacer el RET del Z80 que nos hemos "cargado" pues hemos expulsado la cinta y modificado la ROM
						;ret_pc

						ret

						;"Rebobinar cinta".Ir al principio del fichero
						;xor	dx,dx
						;xor	cx,cx
						;mov	ax,4200h
						;int	21h
						;jc		puerto205_cargar_fin
						;jmp	short puerto205_cargar_leer_pun

expulsar_cinta_salida:
						
;						push si
;						push di
;						push ax
;						pushf
						call	cinta_ins_salida_ext2
;						popf
;						pop ax
;						pop di
;						pop si

						;Hay que hacer el RET del Z80 que nos hemos "cargado" pues hemos expulsado la cinta y modificado la ROM
						;ret_pc

						ret

puerto205_cargar_no_eof:
						;Retroceder 1 byte
						mov	dx,-1
						mov	cx,-1
						mov	ax,4201h
						int	21h
						jc		puerto205_cargar_fin
puerto205_cargar_leer_pun:
						xor	cx,cx
						xor	dx,dx
						mov	ax,4201h
						int	21h
						jc		puerto205_cargar_fin
						mov	puntero_cinta_l,ax
						mov	puntero_cinta_h,dx

						jmp	puerto205_cargar_fin

puerto205_cargar_fin0:
						push	ax
						pushf
						jmp	puerto205_cargar_fin2


;Aqui se llama, despues de validar la secuencia de codigos OUT, al leer bien (o con error) un bloque de datos
puerto205_cargar_fin:
						push	cs
						pop	ds
						push	ax ;Guardar codigo de error (si hay)
						pushf
						mov	ah,3eh
						int	21h	;Cerrar fichero

puerto205_cargar_fin2:
						call	fin_cinta
						popf
						pop	ax
						jc		puerto205_cargar_fin_mal
						;mov	posicion_cadena,0
						cmp	flag_err,0
						jnz	puerto205_cargar_fin_mal
						jmp	fin_cinta_bien

puerto205_cargar_fin_mal:
						jmp	fin_cinta_mal_cargar

puerto205_actualiza: ;Se actualiza la zona de la pantalla pertinente
;y se crea byte de checksum
						push	cx
						push	bx

						mov	bx,dx
						mov	ah,checksum_inicial
						;xor	ah,ah
puerto205_actualiza_bucle:
						mov	al,byte ptr es:[bx]
						xor	ah,al
						push	cx
						push	bx

						cmp	bx,16384
						jc    puerto205_actualiza_bucle2
						push	ax
						;call	[_poke_byte00_p]
						pop	ax

puerto205_actualiza_bucle2:
						pop	bx
						pop	cx
						inc	bx
						loop 	puerto205_actualiza_bucle

puerto205_actualiza_fin:
						mov	word ptr reg_ixl,bx
						pop	bx
						pop	cx
						mov	reg_a,ah
						ret

puerto205_grabar:
						test	bits_estado3,2
						jnz	puerto205_grabar_
						;No hay cinta insertada
						jmp	fin_cinta_mal_grabar
puerto205_grabar_:
						cmp	cinta_longitud,0
						jnz	puerto205_grabar_nolon0
						jmp	fin_cinta_mal_grabar

puerto205_grabar_nolon0:

						call	inicio_cinta

						mov	dx,offset fichero_TAP_salida
						mov	ax,3d01h ;Abrir fichero escritura
						int	21h
						jc		puerto205_grabar_fin0

						mov	bx,ax
						xor	cx,cx
						xor	dx,dx
						mov	ax,4202h ;Mover puntero al final
						int	21h
						jc		puerto205_grabar_fin

						inc	cinta_longitud
						inc	cinta_longitud ;2 bytes de m�s (flag+checksum)
						mov	cx,2 ;Escribir longitud
						mov	dx,offset cinta_longitud
						mov	ah,40h
						int	21h
						jc		puerto205_grabar_fin

						mov	cx,1 ;Escribir flag
						mov	dx,offset cinta_flag
						mov	ah,40h
						int	21h
						jc		puerto205_grabar_fin

						mov	cx,cinta_longitud
						dec	cx
						dec 	cx
						mov	dx,cinta_inicio
						mov	ds,seg_spectrum
						mov	ah,40h
						int	21h
						jc	  	puerto205_grabar_fin

						;Aqu� se crea el checksum (en la carga no se lee de momento,
						;pero se pone para compatibilidad con emulador Z80)
						mov	si,dx
						mov	ah,cs:cinta_flag ;Primer byte es el del flag
puerto205_checksum:
						lodsb
						xor	ah,al
						loop	puerto205_checksum

						push	cs
						pop	ds
						mov	byte ptr buffer_cinta,ah

						mov	cx,1 ;Escribir checksum
						mov	dx,offset buffer_cinta
						mov	ah,40h
						int	21h
						jmp	puerto205_grabar_fin

puerto205_grabar_fin0:
						push	ax
						pushf
						jmp	puerto205_grabar_fin2

puerto205_grabar_fin:
						push	cs
						pop	ds
						push	ax ;Guardar c�digo de error (si hay)
						pushf
						mov	ah,3eh
						int	21h	;Cerrar fichero

puerto205_grabar_fin2:
						call	fin_cinta
						popf
						pop	ax
						jc		puerto205_grabar_fin_mal
						;mov	posicion_cadena,0
						jmp	fin_cinta_bien
puerto205_grabar_fin_mal:
						jmp	fin_cinta_mal_grabar 

inicio_cinta:		cli
						mov	reg_pc,si
						mov	reg_sp,di
						call  restaura_vectores
						mov	es,seg_spectrum

						;call	liberar_teclas

						sti
						ret

;Se llama aqui en streaming de audio y video para no restaurar int9-teclado.
;En streaming suele suceder que la pulsacion/liberacion de teclas "se va" a la int9 del msdos
;y no al emulador
inicio_cinta_no_int9:		cli
						mov	reg_pc,si
						mov	reg_sp,di
						call  restaura_vectores_no_int9
						mov	es,seg_spectrum


						sti
						ret



fin_cinta:			cli
						call	cambia_vectores
						mov	contador_disco,50*segundos
						mov	si,reg_pc         ;SI=PC
						mov	di,reg_sp			;DI=SP
						mov	es,seg_spectrum

						;Liberar teclas pulsadas, excepto la tecla SPACE,
						;asi se podra interrumpir la carga
						;(si es que no ha aparecido el men�)

						test  bits_estado4,00100000b ;Si esta el menu activo
						jnz   fin_cinta2

						mov	al,puerto_32766  ; B    N    M    Simb Space ;7
						or		al,10111110b
						push	ax

						call	liberar_teclas

						pop	ax
						mov	puerto_32766,al

fin_cinta2:

						sti
						ret

inicializa_rom_cinta: ;Aqui se guardan los contenidos de la ROM de la
;zona de LOAD y SAVE para poderlos restaurar cuando convenga

						ret


COMMENT !
						push	es
						mov     ah,byte ptr paginas_actuales
						push    ax

						test    bits_estado3,00100000b
						jnz     inicializa_rom_cinta0


						xor     al,al
						mov     ah,3

						call    paginar ;Poner ROM3
inicializa_rom_cinta0:
						mov   es,seg_spectrum
						mov	di,1222
						mov	si,offset buffer_rom_save
						mov	cx,offset rom_load-offset rom_save
inicializa_rom_cinta1:
						mov	al,byte ptr es:[di]
						mov	byte ptr [si],al
						inc	si
						inc	di
						loop	inicializa_rom_cinta1

						mov	di,1378
						mov	cx,offset fin_rom_load-offset rom_load
inicializa_rom_cinta2:
						mov	al,byte ptr es:[di]
						mov	byte ptr [si],al
						inc	si
						inc	di
						loop	inicializa_rom_cinta2

						rep	movsb
						pop   ax
						test  bits_estado3,00100000b
						jnz   inicializa_rom_cinta_fin

						xor   al,al

						mov   si_despaginar_rom,1
						call  paginar ;Poner ROM antigua

inicializa_rom_cinta_fin:

						pop	es
						ret

!

COMMENT !
rom_save          db		245				;PUSH AF (Guardar registro A)

						db		62,255			;LD A,255
						db		211,204			;OUT (204),A
						db		62,"C"			;LD A,"C"
						db		211,204			;OUT (204),A
						db		62,"I"			;LD A,"I"
						db		211,204			;OUT (204),A
						db		62,"N"			;LD A,"N"
						db		211,204			;OUT (204),A
						db		62,"T"			;LD A,"T"
						db		211,204			;OUT (204),A
						db		62,"A"			;LD A,"A"
						db		211,204			;OUT (204),A

						db		241				;POP AF

						db		211,206			;OUT (206),A

						db		201				;RET


rom_load          db		62,255			;LD A,255
						db		211,204			;OUT (204),A
						db		62,"C"			;LD A,"C"
						db		211,204			;OUT (204),A
						db		62,"I"			;LD A,"I"
						db		211,204			;OUT (204),A
						db		62,"N"			;LD A,"N"
						db		211,204			;OUT (204),A
						db		62,"T"			;LD A,"T"
						db		211,204			;OUT (204),A
						db		62,"A"			;LD A,"A"
						db		211,204			;OUT (204),A

						db		211,205			;OUT (205),A

						;db		175				;XOR A
						;db		219,204			;IN A,(204)
						;db		230,1				;AND 1 (el Carry se va a 0)
						;db		192				;RET NZ (como error, Carry=0)

						;db		55					;SCF (no hay error)


						db		201				;RET


fin_rom_load		equ 	$

buffer_rom_save   db		offset rom_load-offset rom_save dup (?)
buffer_rom_load   db		offset fin_rom_load-offset rom_load dup (?)
!