;Este fichero contiene las rutinas para los c¢digos
;con prefijo 221

PUBLIC t_codigos_221,t_codigos_221253_estados


t_codigos_221:	etiqueta_nop 0,8
					dw		coddd9
					etiqueta_nop 10,24
					dw		coddd25
					etiqueta_nop 26,32
					etiqueta_coddd 33,38
					dw		codigo0
					dw		codigo0
					etiqueta_coddd 41,46
					etiqueta_nop 47,51
					etiqueta_coddd 52,54
					dw		codigo0
					dw		codigo0
					dw		coddd57
					etiqueta_nop 58,67
					etiqueta_coddd 68,70
					etiqueta_nop 71,75
					etiqueta_coddd 76,78
					etiqueta_nop 79,83
					etiqueta_coddd 84,86
					etiqueta_nop 87,91
					etiqueta_coddd 92,94
					dw 	codigo0
					etiqueta_coddd 96,99
					dw		codigo0
					dw		codigo0
					etiqueta_coddd 102,107
					dw		codigo0
					dw		codigo0
					etiqueta_coddd 110,117
					dw		codigo0
					dw		coddd119
					etiqueta_nop 120,123
					etiqueta_coddd 124,126
					etiqueta_nop 127,131
					etiqueta_coddd 132,134
					etiqueta_nop 135,139
					etiqueta_coddd 140,142
					etiqueta_nop 143,147
					etiqueta_coddd 148,150
					etiqueta_nop 151,155
					etiqueta_coddd 156,158
					etiqueta_nop 159,163
					etiqueta_coddd 164,166
					etiqueta_nop 167,171
					etiqueta_coddd 172,174
					etiqueta_nop 175,179
					etiqueta_coddd 180,182
					etiqueta_nop 183,187
					etiqueta_coddd 188,190
					etiqueta_nop 191,202
					dw		coddd203
					etiqueta_nop 204,224
					dw		coddd225
					dw		codigo0
					dw		coddd227
					dw		codigo0
					dw		coddd229
					etiqueta_nop 230,232
					dw		coddd233
					dw		codigo0
					dw		coddd235
					etiqueta_nop 236,248
					dw		coddd249
					etiqueta_nop 250,255

t_codigos_221253_estados db    4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4
	db 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4, 4, 4, 4, 4 ;31
	db 4,14,20,10, 8, 8,11, 4, 4,15,20,10, 8, 8,11, 4 ;47
	db 4, 4, 4, 4,23,23,19, 4, 4,15, 4, 4, 4, 4, 4, 4 ;63
	db 4, 4, 4, 4, 8, 8,19, 4, 4, 4, 4, 4, 8, 8,19, 4 ;79
	db 4, 4, 4, 4, 8, 8,19, 4, 4, 4, 4, 4, 8, 8,19, 4
	db 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	db 19,19,19,19,19,19,19,19, 4, 4, 4, 4, 8, 8,19, 4
	db 4, 4, 4, 4, 8, 8,19, 4, 4, 4, 4, 4, 8, 8,19, 4
	db 4, 4, 4, 4, 8, 8,19, 4, 4, 4, 4, 4, 8, 8,19, 4
	db 4, 4, 4, 4, 8, 8,19, 4, 4, 4, 4, 4, 8, 8,19, 4
	db 4, 4, 4, 4, 8, 8,19, 4, 4, 4, 4, 4, 8, 8,19, 4
	db 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
	db 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
	db 4,14, 4,23, 4,15, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4
	db 4, 4, 4, 4, 4, 4, 4, 4, 4,10, 4, 4, 4, 4, 4, 4

;Cada instruccion no usada gasta 4 estados


coddd9:			;ADD IX,BC
					add_xy_reg c,ixl
					ret

coddd25:			;ADD IX,DE
					add_xy_reg e,ixl
					ret

coddd33:			;LD IX,NN
					lee_word_pc
					mov	word ptr reg_ixl,ax
					ret

coddd34:			;LD (NN),IX
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	ax,word ptr reg_ixl
					poke_word_sin_pf
					popf
					ret

coddd35:			;INC IX
					lahf ;no modifica flags
					inc	word ptr reg_ixl
					sahf
					ret

coddd36:			;INC IXh
					inc_reg ixh
					ret

coddd37:			;DEC IXh
					dec_reg ixh
					ret

coddd38:			;LD IXh,N
					lee_byte_pc
					mov	reg_ixh,al
					ret

coddd41:			;ADD IX,IX
					add_xy_reg ixl,ixl
					ret

coddd42:			;LD IX,(NN)
					pushf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					mov	al,byte ptr es:[bx] ;Leer word de NN
					inc	bx
					mov	ah,byte ptr es:[bx]
					mov	word ptr reg_ixl,ax
					popf
					ret

coddd43:			;DEC IX
					lahf ;No modifica flags
					dec	word ptr reg_ixl
					sahf
					ret

coddd44:			;INC IXl
					inc_reg ixl
					ret

coddd45:			;DEC IXl
					dec_reg ixl
					ret

coddd46:			;LD IXl,N
					lee_byte_pc
					mov	reg_ixl,al
					ret

coddd52:			;INC (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					inc_al
					poke_byte
					ret

coddd53:			;DEC (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					dec_al
					poke_byte
					ret

coddd54:			;LD (IX+d),N
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					lods  es:fuente
					popf
					poke_byte
					ret

coddd57:			;ADD IX,SP
					;FLAGS: Carry si hay acarreo, H si acarreo bit 11
					;los otros no modificados
					lahf
					mov   bl,ah
					mov   cx,di
					mov   ax,word ptr reg_ixl
					add   al,cl
					adc   ah,ch
					mov   word ptr reg_ixl,ax
					lahf
					and   ah,00010001b
					and   bl,11101110b
					or    ah,bl
					sahf
					mov   flag_n,0
					ret

coddd68:			;LD B,IXh
					mov	al,reg_ixh
					mov	reg_b,al
					ret

coddd69:			;LD B,IXl
					mov	al,reg_ixl
					mov	reg_b,al
					ret

coddd70:			;LD B,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_b,al
					ret

coddd76:			;LD C,IXh
					mov	al,reg_ixh
					mov	reg_c,al
					ret

coddd77:			;LD C,IXl
					mov	al,reg_ixl
					mov	reg_c,al
					ret

coddd78:			;LD C,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_c,al
					ret

coddd84:			;LD D,IXh
					mov	al,reg_ixh
					mov	reg_d,al
					ret

coddd85:			;LD D,IXl
					mov	al,reg_ixl
					mov	reg_d,al
					ret

coddd86:			;LD D,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_d,al
					ret

coddd92:			;LD E,IXh
					mov	al,reg_ixh
					mov	reg_e,al
					ret

coddd93:			;LD E,IXl
					mov	al,reg_ixl
					mov	reg_e,al
					ret

coddd94:			;LD E,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_e,al
					ret

coddd96:			;LD IXh,B
					mov	al,reg_b
					mov	reg_ixh,al
					ret

coddd97:			;LD IXh,C
					mov	al,reg_c
					mov	reg_ixh,al
					ret

coddd98:			;LD IXh,D
					mov	al,reg_d
					mov	reg_ixh,al
					ret

coddd99:			;LD IXh,E
					mov	al,reg_e
					mov	reg_ixh,al
					ret

coddd102:		;LD H,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_h,al
					ret

coddd103:		;LD IXh,A
					mov	al,reg_a
					mov	reg_ixh,al
					ret

coddd104:		;LD IXl,B
					mov	al,reg_b
					mov	reg_ixl,al
					ret

coddd105:		;LD IXl,C
					mov	al,reg_c
					mov	reg_ixl,al
					ret

coddd106:		;LD IXl,D
					mov	al,reg_d
					mov	reg_ixl,al
					ret

coddd107:		;LD IXl,E
					mov	al,reg_e
					mov	reg_ixl,al
					ret

coddd110:		;LD L,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_l,al
					ret

coddd111:		;LD IXl,A
					mov	al,reg_a
					mov	reg_ixl,al
					ret

coddd112:		;LD (IX+d),B
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_b
					popf
					poke_byte
					ret

coddd113:		;LD (IX+d),C
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_c
					popf
					poke_byte
					ret

coddd114:		;LD (IX+d),D
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_d
					popf
					poke_byte
					ret

coddd115:		;LD (IX+d),E
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_e
					popf
					poke_byte
					ret

coddd116:		;LD (IX+d),H
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_h
					popf
					poke_byte
					ret

coddd117:   	;LD (IX+d),L
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_l
					popf
					poke_byte
					ret

coddd119:   	;LD (IX+d),A
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,reg_a
					popf
					poke_byte
					ret

coddd124:		;LD A,IXh
					mov	al,reg_ixh
					mov	reg_a,al
					ret

coddd125:		;LD A,IXl
					mov	al,reg_ixl
					mov	reg_a,al
					ret

coddd126:		;LD A,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_a,al
					ret

coddd132:		;ADD A,IXh
					add_al_reg ixh
					ret

coddd133:		;ADD A,IXl
					add_al_reg ixl
					ret

coddd134:		;ADD A,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					add_al_dl
					ret

coddd140:		;ADC A,IXh
					adc_al_reg ixh
					ret

coddd141:		;ADC A,IXl
					adc_al_reg ixl
					ret

coddd142:		;ADC A,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					adc_al_dl
					ret

coddd148:		;SUB IXh
					sub_al_reg ixh
					ret

coddd149:		;SUB IXl
					sub_al_dl ixl
					ret

coddd150:		;SUB (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					sub_al_dl
					ret

coddd156:		;SBC A,IXh
					sbc_al_reg ixh
					ret

coddd157:		;SBC A,IXl
					sbc_al_reg ixl
					ret

coddd158:		;SBC A,(IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					sbc_al_dl
					ret

coddd164:		;AND IXh
					mov	al,reg_ixh
					and   reg_a,al
					ret

coddd165:		;AND IXl
					mov	al,reg_ixl
					and	reg_a,al
					ret

coddd166:		;AND (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   bl,byte ptr es:[bx]
					popf
					and	reg_a,bl
					ret

coddd172:		;XOR IXh
					mov	al,reg_ixh
					xor   reg_a,al
					ret

coddd173:		;XOR IXl
					mov	al,reg_ixl
					xor	reg_a,al
					ret

coddd174:		;XOR (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   bl,byte ptr es:[bx]
					popf
					xor	reg_a,bl
					ret

coddd180:		;OR IXh
					mov	al,reg_ixh
					or    reg_a,al
					ret

coddd181:		;OR IXl
					mov	al,reg_ixl
					or	   reg_a,al
					ret

coddd182:		;OR (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   bl,byte ptr es:[bx]
					popf
					or	   reg_a,bl
					ret

coddd188:		;CP IXh
					cp_al_reg ixh
					ret

coddd189:		;CP IXl
					cp_al_reg ixl
					ret

coddd190:		;CP (IX+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_ixl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					cp_al_dl
					ret

coddd203:		
					ret

coddd225:		;POP IX
					pop_ax
					mov	word ptr reg_ixl,ax
					ret

coddd227:		;EX (SP),IX
					pushf

					mov   al,byte ptr es:[di]
					mov   ah,byte ptr es:[di+1]

					xchg  word ptr reg_ixl,ax   ;HL=(SP)
					mov   byte ptr es:[di],al
					mov   byte ptr es:[di+1],ah

					mov   bx,di  ;BX=SP

					push	bx
					push	ax
					call	vga_poke_byte
					pop	ax
					pop	bx
					mov	al,ah
					inc	bx
					call	vga_poke_byte

					popf
					ret


coddd229:		;PUSH IX
					mov	ax,word ptr reg_ixl
					push_ax
					ret

coddd233:		;JP (IX)
					mov	si,word ptr reg_ixl
					ret

coddd235:		;EX DE,IX
					mov	bx,word ptr reg_ixl
					xchg	word ptr reg_e,bx
					mov	word ptr reg_ixl,bx

coddd249:		;LD SP,IX
					mov	di,word ptr reg_ixl
					ret
