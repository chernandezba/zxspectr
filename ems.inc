;Rutinas de control de la paginacion mediante EMS


emmnombre		db		"EMMXXXX0"
handle_ems		dw		?

inicializar_ems:
;Rutina que detecta si esta instalado controlador de EMS y que la version
;sea >=3.0

inicializar_ems00:


					;Ver si se ha forzado noems

					test	parametros_flags,00001000b
					jz    inicializar_ems0
					ret

inicializar_ems0:

					push	es

					xor	ax,ax
					mov	es,ax
					mov	es,word ptr es:[412+2]  ;Direccion del segmento
					;de la interrupcion 67h

					mov	di,10  ;Si hay memoria expandida
					;en su segmento y offset 10 debe haber la cabecera EMMXXXX0


					mov	si,offset emmnombre
					mov	cx,8
					cld
					repz	cmpsb
					pop	es
					jnz	inicializar_ems_fin

					;Mirar si el gestor de EMS existe

					mov	ah,40h
					int	67h
					or		ah,ah
					jnz	inicializar_ems_fin


					;Mirar si es version 3.0 o superior
					mov	ah,46h
					int	67h
					or		ah,ah
					jnz   inicializar_ems_fin
					and	al,11110000b
					cmp	al,3
					jc		inicializar_ems_fin

					;Ver si hay suficientes paginas
					mov	ah,42h
					int	67h

					or		ah,ah
					jnz	inicializar_ems_fin

					;Como minimo, 12 paginas (4 de ROM,8 de RAM)
					cmp	bx,12
					jc    inicializar_ems_fin

					;Ajustar segmento seg_spectrum con el segmento de la EMS
					mov	ah,41h
					int	67h

					or		ah,ah
					jnz	inicializar_ems_fin

					mov	seg_spectrum,bx

					;Asignar 14 paginas
					mov	ah,43h
					mov	bx,14
					int	67h

					or		ah,ah
					jnz	inicializar_ems_fin

					mov	handle_ems,dx

					;Decir que si hay EMS
					or		bits_estado3,00010000b

inicializar_ems_fin:

					ret

mapear_ems:
					mov	dx,handle_ems
					mov	ah,44h
					int	67h
					ret



liberar_ems:
;Libera los bloques de EMS asignados, si es que se usa la EMS

					test	bits_estado3,00010000b
					jz      liberar_ems_fin
					mov	ah,45h
					mov	dx,handle_ems
					int	67h
liberar_ems_fin:
					ret

