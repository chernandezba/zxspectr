;Rutinas del salvapantallas

segundos_salvapan db	0 ;Lleva la cuenta de los segundos/2 que no se ha 
							  ;pulsado tecla, usado en el salvapantallas

salvapan_longitud	equ 15 ;Longitud de la cola de la "serpiente"
salvapan_cola	dd		 salvapan_longitud dup (?) ;Coordenadas de cada punto
																;de la cola (x,y)
serpie_x			dw		 0
serpie_y       dw		 0 ;Coordenadas de la cabeza de la serpiente

serpie_incx		dw		 1 ;Incremento de la x de la cabeza de la serpiente
serpie_incy		dw		 1 ;Incremento de la y de la cabeza de la serpiente

inc_seg_salvapan:	
					test	bits_estado2,4
					jnz   inc_reg_salvapan2 ;El salvapantallas est  activo

					mov	al,segundos_salvapan
					inc	al
					mov	segundos_salvapan,al
					cmp	al,2*60*2 ;2 minutos*60 segundos*2
					jnz	inc_reg_salvapan2
					;Mirar si est  la opci¢n habilitada
					test	bits_estado2,2
					jz		inc_reg_salvapan2
					mov	interrumpir,1
					or		bits_estado2,4 ;Indicar que se ha de activar el
												;salvapantallas
inc_reg_salvapan2:
					ret

salvapan_inicio: ;Rutina del salvapantallas
					;El salvapantallas consiste en una serpiente que
					;va rebotando por la pantalla y limpiando su contenido

					mov	interrumpir,0

					or		bits_estado4,00001000b ;No refrescar el sonido

					;Desactivar canales 0,1,2,7
					;call	silenciar_canales


					;Inicializar coordenadas de la serpiente
					mov	cx,salvapan_longitud+1
					mov	bx,offset salvapan_cola
					mov	ax,160  ;
					mov	dx,100  ;Inicio cola en el centro de la pantalla
salvapan_inicio2:
					mov	word ptr [bx],ax
					inc	bx
					inc	bx
					mov	word ptr [bx],dx
					inc	bx
					inc	bx
					inc	ax
					inc	dx
					loop	salvapan_inicio2

salvapan_bucle:
					cmp	segundos_salvapan,0
					jnz	salvapan_bucle2
					and	bits_estado2,11111011b
					and	bits_estado4,11110111b ;Refrescar el sonido
					
					mov    primera_vez,0

					ret ;Volver si se pulsa tecla
salvapan_bucle2:
					call	pon_serpie
					;hlt

					;Esperar una interrupcion 8 (y cada 50 Hz)
					and	bits_estado4,10111111b ;Eliminar int. pendiente
salvapan_bucle2_esp:
					test	bits_estado4,01000000b
					jz		salvapan_bucle2_esp

					call	quita_serpie
					call	mueve_cola
					mov	ax,serpie_x
					cmp	ax,0
					jz		span_topex
					cmp	ax,319
					jz		span_topex
					call	mueve_x
					call	point
					jz		span_miray
					call	quita_serpie ;La serpiente ha chocado
span_topex:		;Hacer que rebote en x
					neg	serpie_incx
					mov	ax,serpie_x
					call	mueve_x
span_miray:
					mov	ax,serpie_y
					cmp	ax,0
					jz		span_topey
					cmp	ax,199
					jz		span_topey
					call	mueve_y
					call	point
					jz		span_sigue
					call	quita_serpie ;La serpiente ha chocado
span_topey:		;Hacer que rebote en y
					neg	serpie_incy
					mov	ax,serpie_y
					call	mueve_y
span_sigue:		jmp	salvapan_bucle

mueve_x:
					mov	bx,serpie_incx
					add	ax,bx
					mov	serpie_x,ax
					ret
mueve_y:
					mov	bx,serpie_incy
					add	ax,bx
					mov	serpie_y,ax
					ret

quita_serpie:	;Borra la serpiente de pantalla
					mov	ch,1
					jmp	short pon_serpie_buc

pon_serpie:		;Dibuja la serpiente en pantalla
					xor	ch,ch

pon_serpie_buc: ;Bucle com£n para poner y quitar serpiente
					;Si se entra con CH=0, se pone serpiente
					;Si CH no es 0, se quita serpiente
					mov	cl,salvapan_longitud+1
					mov	bx,offset salvapan_cola
pon_serpie_buc2:
					mov	ax,word ptr [bx]
					inc	bx
					inc	bx
					mov	dx,word ptr [bx]
					inc	bx
					inc	bx
					push	bx
					mov	bx,dx
					push	cx
					or		ch,ch
					jnz	pon_serpie2
					call	pon_punto
					jmp	short pon_serpie_sigue
pon_serpie2:	call	quita_punto

pon_serpie_sigue:
					pop	cx
					pop	bx
					dec	cl
					jnz	pon_serpie_buc2
					ret

pon_punto:		;Dibuja el punto en pantalla
					call	salvapan_calcula
					push	es

					mov   es,seg_vga
					mov	ah,7

pon_punto_fin:
					mov	byte ptr es:[bx],ah
					pop	es
					ret

quita_punto:  ;Borra el punto de pantalla
					call	salvapan_calcula
					push	es

					mov   es,seg_vga
					xor	ah,ah

					jmp 	short pon_punto_fin

mueve_cola:		;Aqu¡ se copian las posiciones de la cola con la posici¢n
					;siguiente

					mov	bx,offset salvapan_cola
					mov	cx,salvapan_longitud*2 ;*2 por la x y la y
mueve_cola_buc:
					mov	ax,word ptr [bx+4] ;
					mov	word ptr [bx],ax   ;Copiar posici¢n
					inc 	bx
					inc	bx
					loop	mueve_cola_buc
					ret

point:			;Devuelve Z si hay color negro en la posicion de la cabeza
					;de la serpiente
					mov	ax,serpie_x
					mov	bx,serpie_y
					call	salvapan_calcula
					push	es

					mov   es,seg_vga
					cmp	byte ptr es:[bx],0
point_fin:
					pop	es
					ret


salvapan_calcula: ;Calcular la direcci¢n de memoria donde est  el punto
						;Se devuelve el bit dentro del byte en modo CGA
						;Entrada: AX=Coordenada X BX=Coordenada Y
						;Salida: BX=Offset dentro de la pantalla
						;		   AL=M scara de punto dentro del byte en modo CGA
						;(no usado)
						;			(Ej: 00001100b para la coordenada 2,Y  )
						

   				mov	cx,ax
					mov	ax,320
					mul	bx
					mov   bx,ax
					add	bx,cx
					ret

