;Programa controlador de la interrupci�n 8 (TIMER) y pantalla
;Es para la pantalla VGA.

;deprecated FRAMES_MAX                      equ 4
;Numero de frames perdidos para indicar que se ha perdido sincronismo

int8_ax dw ?
int8_bx dw ?
int8_cx dw ?
int8_dx dw ?
int8_bp dw ?
int8_si dw ?
int8_di dw ?
int8_ds dw ?

int8:

					mov   word ptr cs:[int8_ds],ds
					mov   word ptr cs:[int8_ax],ax
					mov   ax,cs
					mov   ds,ax

					or		bits_estado7,00010000b



int8_noactualiza:

					test  bits_estado6,00001000b
					jz    int8_nocargando

					jmp   int8_cargando
int8_nocargando:

					dec	segundo_fps
					jnz	int8_no_actfps
					mov	segundo_fps,50
					mov	al,sync_frames
					mov	sync_frames_ult,al
					mov	sync_frames,0
int8_no_actfps:


sincronizar_cpu_no_sonido:




int8_fin:

int8_fin_:


int8_cargando:
					call	int8_0
					jmp	short int8_fin3

int8_fin2:

int8_fin3:

					mov   ds,int8_ds
					mov	al,32
					out	32,al
					mov   ax,word ptr cs:[int8_ax]
					nop
					nop
					iret



int8_0:


					or		bits_estado4,01000000b ;Decir que ha habido int. 8

					;Incrementar reloj MS-DOS
					dec   reloj_msdos
					jnz   int8_2

					mov   reloj_msdos,250
					push  es
					mov   ax,40h
					mov   es,ax
					add   word ptr es:[6ch],91
					jnc   int8_1
					inc   word ptr es:[6eh]
int8_1:
					pop   es


int8_2:
					dec	contador_refresco_ay

					jnz	no_refresco

					mov	contador_refresco_ay,total_refresco_ay

no_refresco:

					dec	contador_flash
					jnz	no_cambio_flash
					xor	bits_estado,00100000b
					mov     contador_flash,16 

					call	inc_seg_salvapan

no_cambio_flash:

					test  bits_estado2,01000000b
					jz		no_disparador ;No actua el disparador autom�tico  jz

					dec	byte ptr disparador
					jnz	no_disparador
					xor   byte ptr puerto_221,16
					mov	al,disparador_defecto
					mov	disparador,al

no_disparador:
					cmp   contador_disco,0 ;si est� a 0, disco parado
					jz 	disco_parado ;jz
					dec	contador_disco
					jnz	disco_parado
					;Apagar motor de disco

					mov   int8_dx,dx
					mov	al,0ch
					mov	dx,3f2h
					out	dx,al
					mov   dx,int8_dx

disco_parado:

interrupciones_di:

					ret


contador_flash db               16
segundos			equ	3 ;3 segundos antes de parar motor de disco
contador_disco db		50*segundos ;Cuando se acabe de llamar a las funciones
reloj_msdos    db    250 ;Cuando llega a 0, se incrementa el reloj del MS-DOS
segundo_fps		db		50  ;Cuando llega a 0,se lee de sync_frames a sync_frames_actual
;(indica que ha pasado un segundo)
;en 91=18.2*5
											;se apague el motor

total_refresco_ay		equ	25
contador_refresco_ay	db	total_refresco_ay

