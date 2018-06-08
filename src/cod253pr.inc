;Este fichero contiene las rutinas para los codigos
;con prefijo 253

PUBLIC t_codigos_253


t_codigos_253:	etiqueta_nop 0,8
					dw		codfd9
					etiqueta_nop 10,24
					dw		codfd25
					etiqueta_nop 26,32
					etiqueta_codfd 33,38
					dw		codigo0
					dw		codigo0
					etiqueta_codfd 41,46
					etiqueta_nop 47,51
					etiqueta_codfd 52,54
					dw		codigo0
					dw		codigo0
					dw		codfd57
					etiqueta_nop 58,67
					etiqueta_codfd 68,70
					etiqueta_nop 71,75
					etiqueta_codfd 76,78
					etiqueta_nop 79,83
					etiqueta_codfd 84,86
					etiqueta_nop 87,91
					etiqueta_codfd 92,94
					dw 	codigo0
					etiqueta_codfd 96,99
					dw		codigo0
					dw		codigo0
					etiqueta_codfd 102,107
					dw		codigo0
					dw		codigo0
					etiqueta_codfd 110,117
					dw		codigo0
					dw		codfd119
					etiqueta_nop 120,123
					etiqueta_codfd 124,126
					etiqueta_nop 127,131
					etiqueta_codfd 132,134
					etiqueta_nop 135,139
					etiqueta_codfd 140,142
					etiqueta_nop 143,147
					etiqueta_codfd 148,150
					etiqueta_nop 151,155
					etiqueta_codfd 156,158
					etiqueta_nop 159,163
					etiqueta_codfd 164,166
					etiqueta_nop 167,171
					etiqueta_codfd 172,174
					etiqueta_nop 175,179
					etiqueta_codfd 180,182
					etiqueta_nop 183,187
					etiqueta_codfd 188,190
					etiqueta_nop 191,202
					dw		codfd203
					etiqueta_nop 204,224
					dw		codfd225
					dw		codigo0
					dw		codfd227
					dw		codigo0
					dw		codfd229
					etiqueta_nop 230,232
					dw		codfd233
					dw		codigo0
					dw		codfd235
					etiqueta_nop 236,248
					dw		codfd249
					etiqueta_nop 250,255

codfd9:			;ADD IY,BC
					add_xy_reg c,iyl
					ret

codfd25:			;ADD IY,DE
					add_xy_reg e,iyl
					ret

codfd33:			;LD IY,NN
					lee_word_pc
					mov	word ptr reg_iyl,ax
					ret

codfd34:			;LD (NN),IY
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	ax,word ptr reg_iyl
					poke_word_sin_pf
					popf
					ret

codfd35:			;INC IY
					lahf ;no modifica flags
					inc	word ptr reg_iyl
					sahf
					ret

codfd36:			;INC IYh
					inc_reg iyh
					ret

codfd37:			;DEC IYh
					dec_reg iyh
					ret

codfd38:			;LD IYh,N
					lee_byte_pc
					mov	reg_iyh,al
					ret

codfd41:			;ADD IY,IY
					add_xy_reg iyl,iyl
					ret

codfd42:			;LD IY,(NN)
					pushf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					mov	al,byte ptr es:[bx] ;Leer word de NN
					inc	bx
					mov	ah,byte ptr es:[bx]
					mov	word ptr reg_iyl,ax
					popf
					ret

codfd43:			;DEC IY
					lahf ;No modifica flags
					dec	word ptr reg_iyl
					sahf
					ret

codfd44:			;INC IYl
					inc	reg_iyl
					ret

codfd45:			;DEC IYl
					dec	reg_iyl
					ret

codfd46:			;LD IYl,N
					lee_byte_pc
					mov	reg_iyl,al
					ret

codfd52:			;INC (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					inc_al
					poke_byte
					ret

codfd53:			;DEC (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					dec_al
					poke_byte
					ret

codfd54:			;LD (IY+d),N
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					lods  es:fuente
					popf
					poke_byte
					ret

codfd57:			;ADD IY,SP
					;FLAGS: Carry si hay acarreo, H si acarreo bit 11
					;los otros no modificados
					lahf
					mov   bl,ah
					mov   cx,di
					mov   ax,word ptr reg_iyl
					add   al,cl
					adc   ah,ch
					mov   word ptr reg_iyl,ax
					lahf
					and   ah,00010001b
					and   bl,11101110b
					or    ah,bl
					sahf
					mov   flag_n,0
					ret


codfd68:			;LD B,IYh
					mov	al,reg_iyh
					mov	reg_b,al
					ret

codfd69:			;LD B,IYl
					mov	al,reg_iyl
					mov	reg_b,al
					ret

codfd70:			;LD B,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_b,al
					ret

codfd76:			;LD C,IYh
					mov	al,reg_iyh
					mov	reg_c,al
					ret

codfd77:			;LD C,IYl
					mov	al,reg_iyl
					mov	reg_c,al
					ret

codfd78:			;LD C,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_c,al
					ret

codfd84:			;LD D,IYh
					mov	al,reg_iyh
					mov	reg_d,al
					ret

codfd85:			;LD D,IYl
					mov	al,reg_iyl
					mov	reg_d,al
					ret

codfd86:			;LD D,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_d,al
					ret

codfd92:			;LD E,IYh
					mov	al,reg_iyh
					mov	reg_e,al
					ret

codfd93:			;LD E,IYl
					mov	al,reg_iyl
					mov	reg_e,al
					ret

codfd94:			;LD E,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_e,al
					ret

codfd96:			;LD IYh,B
					mov	al,reg_b
					mov	reg_iyh,al
					ret

codfd97:			;LD IYh,C
					mov	al,reg_c
					mov	reg_iyh,al
					ret

codfd98:			;LD IYh,D
					mov	al,reg_d
					mov	reg_iyh,al
					ret

codfd99:			;LD IYh,E
					mov	al,reg_e
					mov	reg_iyh,al
					ret

codfd102:		;LD H,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_h,al
					ret

codfd103:		;LD IYh,A
					mov	al,reg_a
					mov	reg_iyh,al
					ret

codfd104:		;LD IYl,B
					mov	al,reg_b
					mov	reg_iyl,al
					ret

codfd105:		;LD IYl,C
					mov	al,reg_c
					mov	reg_iyl,al
					ret

codfd106:		;LD IYl,D
					mov	al,reg_d
					mov	reg_iyl,al
					ret

codfd107:		;LD IYl,E
					mov	al,reg_e
					mov	reg_iyl,al
					ret

codfd110:		;LD L,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_l,al
					ret

codfd111:		;LD IYl,A
					mov	al,reg_a
					mov	reg_iyl,al
					ret

codfd112:		;LD (IY+d),B
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_b
					popf
					poke_byte
					ret

codfd113:		;LD (IY+d),C
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_c
					popf
					poke_byte
					ret

codfd114:		;LD (IY+d),D
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_d
					popf
					poke_byte
					ret

codfd115:		;LD (IY+d),E
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_e
					popf
					poke_byte
					ret

codfd116:		;LD (IY+d),H
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_h
					popf
					poke_byte
					ret

codfd117:   	;LD (IY+d),L
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_l
					popf
					poke_byte
					ret

codfd119:   	;LD (IY+d),A
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,reg_a
					popf
					poke_byte
					ret

codfd124:		;LD A,IYh
					mov	al,reg_iyh
					mov	reg_a,al
					ret

codfd125:		;LD A,IYl
					mov	al,reg_iyl
					mov	reg_a,al
					ret

codfd126:		;LD A,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov	al,byte ptr es:[bx]
					popf
					mov	reg_a,al
					ret

codfd132:		;ADD A,IYh
					add_al_reg iyh
					ret

codfd133:		;ADD A,IYl
					add_al_reg iyl
					ret

codfd134:		;ADD A,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					add_al_dl
					ret

codfd140:		;ADC A,IYh
					adc_al_reg iyh
					ret

codfd141:		;ADC A,IYl
					adc_al_reg iyl
					ret

codfd142:		;ADC A,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					adc_al_dl
					ret

codfd148:		;SUB IYh
					sub_al_reg iyh
					ret

codfd149:		;SUB IYl
					sub_al_reg iyl
					ret

codfd150:		;SUB (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					sub_al_dl
					ret

codfd156:		;SBC A,IYh
					sbc_al_reg iyh
					ret

codfd157:		;SBC A,IYl
					sbc_al_reg iyl
					ret

codfd158:		;SBC A,(IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					sbc_al_dl
					ret

codfd164:		;AND IYh
					mov	al,reg_iyh
					and   reg_a,al
					ret

codfd165:		;AND IYl
					mov	al,reg_iyl
					and	reg_a,al
					ret

codfd166:		;AND (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   bl,byte ptr es:[bx]
					popf
					and	reg_a,bl
					ret

codfd172:		;XOR IYh
					mov	al,reg_iyh
					xor   reg_a,al
					ret

codfd173:		;XOR IYl
					mov	al,reg_iyl
					xor	reg_a,al
					ret

codfd174:		;XOR (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   bl,byte ptr es:[bx]
					popf
					xor	reg_a,bl
					ret

codfd180:		;OR IYh
					mov	al,reg_iyh
					or    reg_a,al
					ret

codfd181:		;OR IYl
					mov	al,reg_iyl
					or	   reg_a,al
					ret

codfd182:		;OR (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   bl,byte ptr es:[bx]
					popf
					or	   reg_a,bl
					ret

codfd188:		;CP IYh
					cp_al_reg iyh
					ret

codfd189:		;CP IYl
					cp_al_reg iyl
					ret

codfd190:		;CP (IY+d)
					pushf
					lods  es:fuente
					cbw
					mov	bx,word ptr reg_iyl
					add	bx,ax
					mov   dl,byte ptr es:[bx]
					popf
					cp_al_dl
					ret

codfd203:
					ret

codfd225:		;POP IY
					pop_ax
					mov	word ptr reg_iyl,ax
					ret

codfd227:		;EX (SP),IY
					pushf
					mov   al,byte ptr es:[di]
					mov   ah,byte ptr es:[di+1]


					xchg  word ptr reg_iyl,ax   ;HL=(SP)
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


codfd229:		;PUSH IY
					mov	ax,word ptr reg_iyl
					push_ax
					ret

codfd233:		;JP (IY)
					mov	si,word ptr reg_iyl
					ret

codfd235:		;EX DE,IY
					mov	bx,word ptr reg_iyl
					xchg	word ptr reg_e,bx
					mov	word ptr reg_iyl,bx

codfd249:		;LD SP,IY
					mov	di,word ptr reg_iyl
					ret
