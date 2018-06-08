;Programa controlador de la interrupci�n 9 (TECLADO)
;Importante:
;Cuando Bloq. Num. esta activo, a cada inicio de pulsacion de tecla
;extendida le preceden los codigos 224, 42 (como si fuera la tecla
;extendida del Mayusculas izquierdo. En la repeticion de esa tecla se
;muestra su codigo normal. Al final acaba con 224, codigo de tecla+128,
;224, 170 (170=42+128)
;Ejemplo: Pulsar Flecha arriba: Codigos generados:
;224,42,224,72, 224,72,224,72,224,72, 224,200,224,170
;(Pulsar).......(Pulsada).............(Liberar)

int9_ax			dw		?
int9_bx			dw		?
int9_cx			dw		?
int9_ds			dw		?



int9:
					mov	cs:int9_ax,ax
					mov	cs:int9_ds,ds

					mov	ax,cs
					mov	ds,ax

					or		bits_estado6,00000100b

					in		al,60h
					cmp	al,1 ;Comprobar si es tecla ESC
					jnz	no_es_esc
					;Avisar que se ha pulsado ESC
					or		bits_estado2,8      ;8  4  2  1
					;Avisar que hay que interrumpir al interprete
					mov	interrumpir,1
					jmp	short no_es_ext

no_es_esc:
					cmp	al,224
					jnz   no_es_ext
					or		bits_estado2,128 ;Se pulsa (o libera) tecla extendida
					jmp	short int9fin

no_es_ext:

					mov	ah,al
					and	al,127
					and	ah,128 ;AH contiene 0:Si se pulsa tecla,128:Si se libera

					test	bits_estado2,128
					jz		no_previa_ext
					and	bits_estado2,01111111b
					;Si pulsa (o libera) una tecla extendida

					;Mirar si se genera codigo de Bloq. Num, en cuyo caso se
					;ignora, si no pareceria que se pulsa (o libera) la
					;tecla de mayusculas izquierda
					cmp	al,42
					jz    int9fin

no_previa_ext:

					or		al,al
					jz		int9fin
					cmp	al,84
					jnc	int9fin
					dec	al

					mov	int9_bx,bx

					mov	int9_cx,cx

					mov	bx,offset tabla_puertos_spectrum

					;AL=tecla
					;AH=bit pulsado/no pulsado

					push  bx
					push  ax

					;mov	cl,ah ;CL=bit 128 pulsado/no pulsado
					
					shl   al,1

					xlat  ;al=(BX+AL)
					
					
					;mov	ah,cl ;AH=bit 128 pulsado/no pulsado

					call int9_pon_tecla

					pop  ax
					;AL=tecla
					;AH=bit pulsado/no pulsado


					pop  bx

					;Obtener si genera pulsacion de dos teclas
					inc  bx

					;mov	cl,ah ;CL=bit 128 pulsado/no pulsado
					
					shl   al,1

					xlat  ;al=(BX+AL)

					or   al,al
					jz   int9_no_segunda_tecla
					;mov	ah,cl

					call int9_pon_tecla
int9_no_segunda_tecla:
					mov	cx,int9_cx

					mov	bx,int9_bx




int9fin:

					mov	segundos_salvapan,0

					mov	ds,int9_ds


					in		al,061h
					or		al,128
					out	061h,al  ;Liberar teclado
					and	al,127
					out	061h,al
					mov	al,32
					out	32,al

					mov	ax,cs:int9_ax
					
					iret


;Pone el bit de la tecla de puerto indicada a pulsada/no pulsada
;Entrada :
;					AL=tecla segun tabla_puertos_spectrum (Formato: ;bbbpppp :   
; b: n£mero del bit de la tecla(1,2,3,4 o 5)
;				 p: desplazamiento desde puerto_65278 del puerto de la tecla
;					AH contiene 0:Si se pulsa tecla,128:Si se libera



int9_pon_tecla:
					mov	cl,al
					shr	cl,1
					shr	cl,1
					shr	cl,1
					shr	cl,1 ;en cl n�mero de bit
					mov	ch,128
int9_pon_tecla_1:			rol	ch,1
					rol	ah,1 ;Rotar bit de pulsado/liberado
					dec	cl
					jnz	int9_pon_tecla_1
					not	ch
					;En	Ch m�scara de bit

					and	al,15
					mov	cl,al ;En CL desplazamiento de puerto
					mov	bx,offset puerto_65278
					xlat
					and	al,ch
					or		al,ah
					mov	ch,al ;En CH valor del puerto
					xor	ah,ah
					mov	al,cl
					add	bx,ax
					mov	byte ptr [bx],ch

					ret
