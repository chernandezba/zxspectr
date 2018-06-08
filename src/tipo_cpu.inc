mensi386			db		"Procesador 386 o superior disponible",10,13,"$"
menno386			db		"Procesador 386 o superior no disponible",10,13,"$"

old_int6			dd		?

new_int6:
					mov	bp,sp ;No importa modificar el BP
					and	bits_estado3,10111111b ;No es 386 o superior
					;Incrementar IP de retorno en 3
					add	word ptr ss:[bp],3
					iret


detectar_386:
;Aqui se detecta si el procesador es un 386 o superior.
;Los procesadores 186 y superior producen una interrupcion 6 si se ejecuta
;una instruccion no permitida. Por lo tanto, se ejecutara una instruccion
;disponible a partir del 386 y si no hay interrupcion es que hay un 386
;Antes hay que ver que el procesador no sea un 8086/8088; para esto se
;sabe que un 8086/8, cuando hace un PUSH, primero decrementa el SP y luego
;guarda el valor. En cambio, un 186 o superior, primero guarda el valor y luego
;decrementa el SP.
					test	parametros_flags,00010000b
					jnz	detectar_386_fin

					or		bits_estado3,01000000b ;Asumir un 386
					mov   ax,sp
					push  sp
					pop   bx
					cmp	ax,bx
					jz		detectar_386_2 ;Como minimo es un 186
					and	bits_estado3,10111111b
					jmp	detectar_386_fin

detectar_386_2:
					;Guardar vector 6
					cli
					push	es
					xor	ax,ax
					mov	es,ax
					mov	bx,6*4
					mov	ax,word ptr es:[bx]
					mov	word ptr old_int6,ax
					mov	ax,word ptr es:[bx+2]
					mov	word ptr old_int6+2,ax
					;Cambiar vector 6
					mov	ax,offset new_int6
					mov	word ptr es:[bx],ax
					mov	ax,cs
					mov	word ptr es:[bx+2],ax
					sti
					;Ejecutar codigo para 386 o superior
					db		0Fh,20h,0C2h   ;mov edx, cr0

					nop   ;Por si acaso
					nop
					;Restaurar vector 6
					cli
					mov	ax,word ptr old_int6
					mov	word ptr es:[bx],ax
					mov	ax,word ptr old_int6+2
					mov	word ptr es:[bx+2],ax
					pop	es
					sti
detectar_386_fin:
					test	bits_estado3,01000000b
					jnz	detectar_386_si
					mov	dx,offset menno386
					jmp	short detectar_386_si2
detectar_386_si:
					mov	dx,offset mensi386
detectar_386_si2:
					mov	ah,9
					int   21h
					ret
