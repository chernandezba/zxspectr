;Este fichero contiene las rutinas para las instrucciones
;con prefijo 237=0EDH

PUBLIC t_codigos_prefed,t_codigos_prefed_estados

t_codigos_prefed:
					etiqueta_nop 0,63
					etiqueta_coded 64,68
					dw		codigo201 ;RETN=RET
					etiqueta_coded 70,75
					dw		coded68  ;*NEG
					dw		codigo201 ;RETI=RET
					dw		coded70   ;*IM 0/1
					etiqueta_coded 79,83
					dw		coded68  ;*NEG
					dw		codigo201 ;RET2* = RET
					dw		coded70 ;IM1 = IM0
					etiqueta_coded 87,91
					dw		coded68  ;*NEG
					dw		codigo201 ;RET3* = RET
					etiqueta_coded 94,98
					dw		codigo34 ;LD (NN),HL
					dw		coded68  ;*NEG
					dw		codigo201 ;RET4* = RET
					dw		coded70   ;*IM 0
					etiqueta_coded 103,106
					dw		codigo42 ;LD HL,(NN)
					dw		coded68  ;*NEG
					dw		codigo201 ;RET5* = RET
					dw		coded70   ;*IM 0/1
					etiqueta_coded 111,112
					dw		codigo0 ;113=OUT(C),F = Nada
					etiqueta_coded 114,115
					dw		coded68  ;*NEG
					dw		codigo201 ;RET6* = RET
					dw		coded70   ;*IM 1
					dw		codigo0
					etiqueta_coded 120,123
					dw		coded68  ;*NEG
					dw		codigo201 ;RET7* = RET
					dw    coded94   ;*IM 2
					etiqueta_nop 127,159
					etiqueta_coded 160,163
					etiqueta_nop 164,167
					etiqueta_coded 168,171
					etiqueta_nop 172,175
					etiqueta_coded 176,179
					etiqueta_nop 180,183
					etiqueta_coded 184,187
					etiqueta_nop 188,255

t_codigos_prefed_estados db    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 ;31
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 ;47
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 ;63
	db 12,12,15,20, 8,14, 8, 9,12,12,15,20, 8,14, 8, 9 ;79
	db 12,12,15,20, 8,14, 8, 9,12,12,15,20, 8,14, 8, 9 ;95
	db 12,12,15,20, 8,14, 8,18,12,12,15,20, 8,14, 8,18 ;111
	db 12,12,15,20, 8,14, 8, 8,12,12,15,20, 8,14, 8, 8 ;128
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 16,16,16,16, 8, 8, 8, 8,16,16,16,16, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8

;Nota: Cada instruccion que no tiene funcion, dura 8 estados


coded64:			;IN B,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_b,al
					ret

coded65:			;OUT (C),B
					mov	ax,word ptr reg_c
					mov	bl,reg_b
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded66:			;SBC HL,BC
					sbc_hl_reg c
					ret

coded67:			;LD (NN),BC
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	ax,word ptr reg_c
					poke_word_sin_pf
					popf
					ret

coded68:			;NEG
					;Flags:
					;S si negativo
					;Z si cero
					;H si acarreo bit 3
					;P/V a 1 si a era 80h
					;N a 1
					;C a 1 si a no era 0
					mov	al,reg_a
					mov	cl,al
					neg	al
					lahf
					;			SZxHxVNC
					and	ah,11111010b
					or		cl,cl
					jz		coded68_2
					or		ah,00000001b ;Poner C a 1
coded68_2:     cmp	cl,80h
					jnz	coded68_3
					or		ah,00000100b ;Poner P/V a 1
coded68_3:		sahf
					mov	reg_a,al
					mov	flag_n,2 ;Decir que es resta
					ret
					
coded70:			;IM 0
					lahf
					and	bits_estado,11111101b
					sahf
					ret

coded71:			;LD I,A
					mov	al,reg_a
					mov	reg_i,al
					ret

coded72:			;IN C,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_c,al
					ret

coded73:			;OUT (C),C
					mov	ax,word ptr reg_c
					mov	bl,al
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded74:			;ADC HL,BC
					adc_hl_reg c
					ret

coded75:			;LD BC,(NN)
					pushf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					mov	al,byte ptr es:[bx] ;Leer word de NN
					inc	bx
					mov	ah,byte ptr es:[bx]
					mov	word ptr reg_c,ax
					popf
					ret

coded79:			;LD R,A
					mov	al,reg_a
					mov	reg_r,al
					lahf
					and	al,10000000b
					mov	reg_r_bit7,al
					sahf
					ret

coded80:       ;IN D,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_d,al
					ret

coded81:			;OUT (C),D
					mov	ax,word ptr reg_c
					mov	bl,reg_d
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded82:			;SBC HL,DE
					sbc_hl_reg e
					ret

coded83:			;LD (NN),DE
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	ax,word ptr reg_e
					poke_word_sin_pf
					popf
					ret

coded87:			;LD A,I
					;Si EI->Flag P/V = 1
					;Si DI->Flag P/V = 0
					;SZxHxVxC
					;S a 1 si I es negativo
					;Z a 1 si I es 0
					;H a 0
					;N a 0
					mov	al,reg_i
					mov	reg_a,al
					mov	flag_n,0
					or		al,al


					lahf
					test	bits_estado,1
					jz    coded87_di
					or		ah,00000100b
					sahf
					ret
coded87_di:		and	ah,11111011b
					sahf
					ret

coded88:			;IN E,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_e,al
					ret

coded89:			;OUT (C),E
					mov	ax,word ptr reg_c
					mov	bl,reg_e
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded90:			;ADC HL,DE
					adc_hl_reg e
					ret

coded91:			;LD DE,(NN)
					pushf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					mov	al,byte ptr es:[bx] ;Leer word de NN
					inc	bx
					mov	ah,byte ptr es:[bx]
					mov	word ptr reg_e,ax
					popf
					ret

coded94:			;IM 2
					lahf
					or		bits_estado,00000010b
					sahf
					ret

coded95:			;LD A,R
					;Si EI->Flag P/V = 1
					;Si DI->Flag P/V = 0
					;SZxHxVxC
					;S a 1 si R es negativo
					;Z a 1 si R es 0
					;H a 0
					;N a 0
					mov	al,reg_r
					and	al,01111111b
					or		al,reg_r_bit7
					mov	reg_a,al
					mov	flag_n,0					
					or		al,al

					lahf
					test	bits_estado,1
					jz    coded95_di
					or		ah,00000100b
					sahf
					ret
coded95_di:		and	ah,11111011b
					sahf
					ret

coded96:       ;IN H,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_h,al
					ret

coded97:			;OUT (C),H
					mov	ax,word ptr reg_c
					mov	bl,reg_h
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded98:			;SBC HL,HL
					sbc_hl_reg l
					ret

coded103:		;RRD
					;    A       (HL)     Pb:Parte baja
					;      ------<----    Pa:Parte alta
					;      |         |
					; -------   -------
					; |Pa|Pb|   |Pa|Pb|
					; -------   -------
					;             |->|
					;
					;      |--->--|
					;Se conserva el flag C
					;los otros se alteran seg£n el resultado de A
					pushf
					mov	bx,word ptr reg_l
					mov	dx,bx
					peek_byte
					;En AL tenemos (HL)
					mov	ah,al
					mov	bl,reg_a
					mov	bh,bl
					and	bl,11110000b
					and	al,00001111b
					popf
					jc    coded103_c
					or		bl,al
					jmp	short coded103_sigue
coded103_c:		or		bl,al
					stc
coded103_sigue:
					mov	reg_a,bl
					pushf ;Guardar flags

					and	bh,00001111b
					and	ah,11110000b
					or		ah,bh
					ror	ah,1
					ror	ah,1
					ror	ah,1
					ror	ah,1
					mov	bx,dx ;Recuperar valor de HL
					mov	al,ah
					call	vga_poke_byte
					popf
					ret

coded104:		;IN L,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_l,al
					ret

coded105:		;OUT (C),L
					mov	ax,word ptr reg_c
					mov	bl,reg_l
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded106:		;ADC HL,HL
					adc_hl_reg l
					ret

coded111:		;RLD
					;    A       (HL)     Pb:Parte baja
					;      ------<-       Pa:Parte alta
					;      |      |
					; -------   -------
					; |Pa|Pb|   |Pa|Pb|
					; -------   -------
					;             |<-|
					;
					;      |------>--|
					;Se conserva el flag C
					;los otros se alteran seg£n el resultado de A
					pushf
					mov	bx,word ptr reg_l
					mov	dx,bx
					peek_byte
					;En AL tenemos (HL)
					ror	al,1
					ror	al,1
					ror	al,1
					ror	al,1
					mov	ah,al
					mov	bl,reg_a
					mov	bh,bl
					and	al,00001111b
					and	bl,11110000b
					popf
					jc    coded111_c
					or		bl,al
					jmp	short coded111_sigue
coded111_c:		or		bl,al
					stc
coded111_sigue:
					mov	reg_a,bl
					pushf ;Guardar flags

					and	bh,00001111b
					and	ah,11110000b
					or		ah,bh
					mov	bx,dx ;Recuperar valor de HL
					mov	al,ah
					call	vga_poke_byte
					popf
					ret

coded112:      ;IN F,(C) ;S¢lo afecta a los flags
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					ret


coded114:		;SBC HL,SP
					mov	bx,word ptr reg_l
					mov	cx,bx
					sbb   bx,di
					lahf
					and	ah,11111011b
					or		cx,cx ;Si cambia de 1 a 0, overflow
					jns   nopv_ed114
					or		bx,bx ;Antes era bit 15=1
					js		nopv_ed114 
					;Hay cambio. Pasa el bit 15 a 0
					or		ah,00000100b 	
nopv_ed114:
					sahf
					mov	word ptr reg_l,bx
					ret

coded115:		;LD (NN),SP
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	ax,di
					poke_word_sin_pf
					popf
					ret

coded120:		;IN A,(C)
					mov	bx,word ptr reg_c
					call	codigo219_lee
					;El IN r,(c) actualiza flags seg£n el valor del puerto
					or		al,al
					mov	reg_a,al
					ret

coded121:   	;OUT (C),A
					mov	ax,word ptr reg_c
					mov	bl,reg_a
					pushf ;OUT (c),r no modifica flags
					call	codigo211_saca
					popf
					ret

coded122:		;ADC HL,SP
					mov	bx,word ptr reg_l
					mov	cx,bx
					adc   bx,di
					lahf
					and	ah,11111011b
					or		cx,cx ;Si cambia de 0 a 1, overflow
					js    nopv_ed122
					or		bx,bx ;Antes era bit 15=0
					jns   nopv_ed122 
					;Hay cambio. Pasa el bit 15 a 1
					or		ah,00000100b 	
nopv_ed122:
					sahf
					mov	word ptr reg_l,bx
					ret

coded123:		;LD SP,(NN)
					pushf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					mov	al,byte ptr es:[bx] ;Leer word de NN
					inc	bx
					mov	ah,byte ptr es:[bx]
					mov	di,ax
					popf
					ret

coded160:		;LDI
					pushf
					mov	bx,word ptr reg_l
					peek_byte
					mov	bx,word ptr reg_e
					call	vga_poke_byte
					inc	word ptr reg_l
					inc	word ptr reg_e
					dec	word ptr reg_c
					;Si BC=0 P/V a 0 si BC<>0 P/V a 1
					jz		coded160_cero
					popf
					lahf
					;SZxHxVxC
					or		ah,00000100b
					sahf
					ret
coded160_cero: popf
					lahf
					and	ah,11111011b
					sahf
					ret

coded161:		;CPI
					mov	bx,word ptr reg_l
					peek_byte
					mov	ah,reg_a
					cmp	ah,al
					mov	flag_n,2 ;Avisar que hay resta (CP)
					lahf
					inc	word ptr reg_l
					dec	word ptr reg_c
					;Si BC=0 P/V a 0 si BC<>0 P/V a 1
					jz		coded161_cero
					;SZxHxVxC
					or		ah,00000100b  ;P/V a 1
					sahf
					ret
coded161_cero: ;lahf
					and	ah,11111011b
					sahf
					ret

coded162:		;INI
					mov	bx,word ptr reg_c
					call	codigo219_lee
					mov	bx,word ptr reg_l
					call	vga_poke_byte
					inc	word ptr reg_l
					dec	reg_b
					ret

coded163:		;OUTI
					mov	bx,word ptr reg_l
					mov	bl,byte ptr es:[bx]
					mov	al,reg_c
					call  codigo211_saca
					inc	word ptr reg_l
					dec	reg_b
					ret

coded168:		;LDD
					pushf
					mov	bx,word ptr reg_l
					peek_byte
					mov	bx,word ptr reg_e
					call	vga_poke_byte
					dec	word ptr reg_l
					dec	word ptr reg_e
					dec	word ptr reg_c
					;Si BC=0 P/V a 0 si BC<>0 P/V a 1
					jz		coded168_cero
					popf
					lahf
					;SZxHxVxC
					or		ah,00000100b
					sahf
					ret
coded168_cero: popf
					lahf
					and	ah,11111011b
					sahf
					ret

coded169:		;CPD
					mov	bx,word ptr reg_l
					peek_byte
					mov	ah,reg_a
					cmp	ah,al
					mov	flag_n,2 ;Avisar que hay resta (CP)
					lahf
					dec   word ptr reg_l
					dec	word ptr reg_c
					;Si BC=0 P/V a 0 si BC<>0 P/V a 1
					jz		coded169_cero
					;SZxHxVxC
					or		ah,00000100b
					sahf
					ret
coded169_cero: and	ah,11111011b
					sahf
					ret

coded170:		;IND
					mov	bx,word ptr reg_c
					call	codigo219_lee
					mov	bx,word ptr reg_l
					call	vga_poke_byte
					dec	word ptr reg_l
					dec	reg_b
					ret

coded171:		;OUTD
					mov	bx,word ptr reg_l
					mov	bl,byte ptr es:[bx]
					mov	ax,word ptr reg_c
					dec	ah
					call  codigo211_saca
					dec	word ptr reg_l
					dec	reg_b
					ret

coded176:		;LDIR
					call	coded160
					jpe	coded176_nocero ;P/V alzado si BC<>0
					ret

coded176_nocero:
					;Apuntar a la misma instrucci¢n y repetir
					lahf
					dec	si
					dec	si
					sahf
					ret

coded177:		;CPIR
					call	coded161
					jz		coded177_fin ;Se cumple que A=(HL)
					jpo	coded177_fin ;BC=0 (P/V a 0 = JPO)
					;Repetir
					lahf
					dec	si
					dec	si
					sahf
					ret
coded177_fin:	ret

coded178:		;INIR
					call	coded162
					jz		coded178_fin
					lahf
					dec	si
					dec	si
					sahf
					ret
coded178_fin:	ret

coded179:		;OTIR
					call	coded163
					jz		coded179_fin
					lahf
					dec	si
					dec	si
					sahf
					ret
coded179_fin:	ret

coded184:		;LDDR
					call	coded168
					jpe	coded184_nocero ;P/V alzado si BC<>0
					ret

coded184_nocero:
					;Apuntar a la misma instrucci¢n y repetir
					lahf
					dec	si
					dec	si
					sahf
					ret

coded185:		;CPDR
					call	coded169
					jz		coded185_fin ;Se cumple que A=(HL)
					jpo	coded185_fin ;BC=0 (P/V a 0 = JPO)

					;Repetir
					lahf
					dec	si
					dec	si
					sahf
					ret
coded185_fin:	ret

coded186:		;INDR
					call	coded170
					jz		coded186_fin
					lahf
					dec	si
					dec	si
					sahf
					ret
coded186_fin:	ret

coded187:		;OTDR
					call	coded171
					jz		coded187_fin
					lahf
					dec	si
					dec	si
					sahf
					ret
coded187_fin:	ret
