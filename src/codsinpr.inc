;Este fichero contiene las rutinas para los 256 c�digos del Spectrum
;sin prefijo

PUBLIC t_codigos_sin_prefijo,t_codigos_sin_prefijo_estados
PUBLIC in_255_48,in_255_p2a,puerto_no_usado_255,codigo0


t_codigos_sin_prefijo:
					etiqueta_codigo 0,255
t_codigos_sin_prefijo_estados db 4,10, 7, 6, 4, 4, 7, 4,4,11, 7, 6, 4, 4, 7, 4
	db 8,10, 7, 6, 4, 4, 7, 4,12,11, 7, 6, 4, 4, 7, 4
	db 7,10,16, 6, 4, 4, 7, 4, 7,11,16, 6, 4, 4, 7, 4
	db 7,10,13, 6,11,11,10, 4, 7,11,13, 6, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4
	db 5,10,10,10,10,11, 7,11, 5,10,10, 0,10,17, 7,11
	db 5,10,10,11,10,11, 7,11, 5, 4,10,11,10, 0, 7,11
	db 5,10,10,19,10,11, 7,11, 5, 4,10, 4,10, 0, 7,11
	db 5,10,10, 4,10,11, 7,11, 5, 6,10, 4,10, 0, 7,11


codigo0:			;NOP
					ret

codigo1: 		;LD BC,NN
					lee_word_pc
					mov	word ptr reg_c,ax
					ret

codigo2:			;LD (BC),A
					mov	bx,word ptr reg_c
					mov	al,reg_a
					poke_byte
					ret

codigo3:			;INC BC
					lahf ;no modifica flags
					inc	word ptr reg_c
					sahf
					ret

codigo4:			;INC B
					inc_reg b
					ret

codigo5:			;DEC B
					dec_reg b
					
					ret

codigo6:			;LD B,N
					lee_byte_pc
					mov	reg_b,al
					ret

codigo7:			;RLCA
					rotacion0_a rol
					ret

codigo8:			;EX AF,AF'
					cargar_reg_f
					mov	ax,word ptr reg_f
					mov	bx,word ptr reg_f_
					xchg  ax,bx
					mov	word ptr reg_f,ax
					mov	word ptr reg_f_,bx
					
					mov	ah,al
					and	al,00000010b
					mov	flag_n,al
					and	ah,11010101b
					sahf
					
					ret

codigo9:			;ADD HL,BC
					add_hl_reg c
					ret

codigo10:		;LD A,(BC)
					mov	bx,word ptr reg_c
					peek_byte
					mov	reg_a,al
					ret

codigo11:		;DEC BC
					lahf ;no modifica flags
					dec 	word ptr reg_c
					sahf
					ret

codigo12:		;INC C
					inc_reg c
					ret

codigo13:		;DEC C
					dec_reg c
					ret

codigo14:		;LD C,N
					lee_byte_pc
					mov	reg_c,al
					ret

codigo15:		;RRCA
               rotacion0_a ror
					ret

codigo16:		;DJNZ DIS
					lods  es:fuente
					lahf
					mov	ch,ah
					cbw
					dec   reg_b
					jz 	codigo16_cero
					add   si,ax
codigo16_cero:	mov	ah,ch
					sahf
					ret

codigo17:		;LD DE,NN
					lee_word_pc
					mov	word ptr reg_e,ax
					ret

codigo18:		;LD (DE),A
					mov	bx,word ptr reg_e
					mov	al,reg_a
					poke_byte
					ret

codigo19:		;INC DE
					lahf
					inc	word ptr reg_e
					sahf
					ret

codigo20:		;INC D
					inc_reg d
					ret

codigo21:		;DEC D
					dec_reg d
					ret

codigo22:		;LD D,N
					lee_byte_pc
					mov	reg_d,al
					ret

codigo23:		;RLA
					rotacion0_a rcl
					ret

codigo24:		;JR DIS
					pushf
					lods  es:fuente
					cbw
					add	si,ax
					popf
					ret

codigo25:      ;ADD HL,DE
					add_hl_reg e
					ret

codigo26:		;LD A,(DE)
					mov	bx,word ptr reg_e
					peek_byte
					mov	reg_a,al
					ret

codigo27:		;DEC DE
					lahf
					dec	word ptr reg_e
					sahf
					ret

codigo28:		;INC E
					inc_reg e
					ret

codigo29:		;DEC E
					dec_reg e
					ret

codigo30:		;LD E,N
					lee_byte_pc
					mov	reg_e,al
					ret

codigo31:		;RRA
					rotacion0_a rcr
					ret

codigo32:		;JR NZ,DIS
					lee_desp_pc
					jnz	codigo32_nz
					ret
codigo32_nz:	pushf
					add	si,ax
					popf
					ret

codigo33:		;LD HL,NN
					lee_word_pc
					mov	word ptr reg_l,ax
					ret

codigo34:		;LD (NN),HL
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	ax,word ptr reg_l
					poke_word_sin_pf
					popf
					ret

codigo35:		;INC HL
					lahf
					inc	word ptr reg_l
					sahf
					ret

codigo36:		;INC H
					inc_reg h
					ret

codigo37:		;DEC H
					dec_reg h
					ret

codigo38:		;LD H,N
					lee_byte_pc
					mov	reg_h,al
					ret

codigo39:		;DAA
					lahf
					mov	ch,ah
					xor	bh,bh
					mov	bl,reg_a
					test	ah,00000001b ;C
					jz    codigo39_noc
					add	bh,1
codigo39_noc:
					test	ah,00010000b ;H
					jz    codigo39_noh
					add	bh,2
codigo39_noh:
					test	flag_n,00000010b ;N
					jz    codigo39_non
					add	bh,4
codigo39_non:
					add	bx,bx ;*2
					add	bx,offset tabla_daa
					mov	ax,word ptr [bx]
					mov	word ptr reg_f,ax
					xchg  ah,al ;Tener en AH=Flags, AL=A
					mov	flag_n,ah
					and	flag_n,00000010b
					and	ah,11010101b ;SZxHxVNC
					and	ch,00101010b
					or		ah,ch
					sahf
					ret

					ret

codigo40:		;JR Z,DIS
					lee_desp_pc
					jz	 codigo40_z
					ret
codigo40_z:	   pushf
					add	si,ax
					popf
					ret


codigo41:      ;ADD HL,HL
					add_hl_reg l
					ret

codigo42:		;LD HL,(NN)
					pushf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					mov	al,byte ptr es:[bx] ;Leer word de NN
					inc	bx
					mov	ah,byte ptr es:[bx]
					mov	word ptr reg_l,ax
					popf
					ret

codigo43:		;DEC HL
					lahf
					dec   word ptr reg_l
					sahf
					ret

codigo44:		;INC L
					inc_reg l
					ret

codigo45:		;DEC L
					dec_reg l
					ret

codigo46:		;LD L,N
					lee_byte_pc
					mov	reg_l,al
					ret

codigo47:		;CPL
					;Flags: H,N a 1. Resto no afectados
					lahf
					not	reg_a
					;        SZxHxVNC
					or		ah,00010000b
					mov	flag_n,2
					sahf
					ret

codigo48:		;JR NC,DIS
					lee_desp_pc
					jnc   codigo48_nc
					ret
codigo48_nc:   pushf
					add	si,ax
					popf
					ret

codigo49:		;LD SP,NN
					lee_word_pc
					mov	di,ax
					ret

codigo50:		;LD (NN),A
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si
					mov	al,reg_a
					call	vga_poke_byte
					popf
					ret

codigo51:		;INC SP
					lahf
					inc	di
					sahf
					ret

codigo52:		;INC (HL)
					mov	bx,word ptr reg_l
					mov	al,byte ptr es:[bx]
					inc_al 
					poke_byte
					ret

codigo53:		;DEC (HL)
					mov	bx,word ptr reg_l
					mov	al,byte ptr es:[bx]
					dec_al 
					poke_byte
					ret

codigo54:		;LD (HL),N
					lods  es:fuente
					pushf
					mov	bx,word ptr reg_l
					call	vga_poke_byte
					popf
					ret

codigo55:		;SCF
					stc
					lahf
					;		   SZxHxVNC
					and	ah,1110111b
					mov	flag_n,0
					sahf
					ret

codigo56:		;JR C,DIS
					lee_desp_pc
					jc	 codigo56_c
					ret
codigo56_c:	   pushf
					add	si,ax
					popf
					ret

codigo57:      ;ADD HL,SP
					;FLAGS: Carry si hay acarreo, H si acarreo bit 11
					;los otros no modificados


					lahf
					mov   bl,ah
					mov   cx,di
					mov   ax,word ptr reg_l
					add   al,cl
					adc   ah,ch
					mov   word ptr reg_l,ax
					lahf
					and   ah,00010001b
					and   bl,11101110b
					or    ah,bl
					sahf
					mov   flag_n,0
					ret

codigo58:		;LD A,(NN)
					lahf
					mov	bl,byte ptr es:[si] ;Leer NN
					inc	si
					mov	bh,byte ptr es:[si]
					inc	si

					peek_byte
					mov	reg_a,al
					sahf
					ret

codigo59:		;DEC SP
					lahf
					dec	di
					sahf
					ret

codigo60:		;INC A
					mov	al,reg_a ;Se carga para actualizar flag AF
					inc_al
					mov	reg_a,al
					ret

codigo61:		;DEC A
					mov	al,reg_a ;Se carga para actualizar flag AF
					dec_al
					mov	reg_a,al
					ret

codigo62:      ;LD A,N
					lee_byte_pc
					mov	reg_a,al
					ret

codigo63:		;CCF
					;Flag H=Carry anterior
					mov	flag_n,2
					cmc
					lahf
					jnc 	codigo63_2
					;Era carry 0
					;        SZxHxVNC
					and	ah,11101111b
					sahf
					ret
codigo63_2:
					or		ah,00010000b
					sahf
					ret

codigo64:		;LD B,B
					ret

codigo65:		;LD B,C
					mov	al,reg_c
					mov	reg_b,al
					ret

codigo66:		;LD B,D
					mov	al,reg_d
					mov	reg_b,al
					ret

codigo67:		;LD B,E
					mov	al,reg_e
					mov	reg_b,al
					ret

codigo68:		;LD B,H
					mov	al,reg_h
					mov	reg_b,al
					ret


codigo69:		;LD B,L
					mov	al,reg_l
					mov	reg_b,al
					ret

codigo70:		;LD B,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_b,al
					ret

codigo71:		;LD B,A
					mov	al,reg_a
					mov	reg_b,al
					ret

codigo72:		;LD C,B
					mov	al,reg_b
					mov	reg_c,al
					ret

codigo73:		;LD C,C
					ret

codigo74:		;LD C,D
					mov	al,reg_d
					mov	reg_c,al
					ret

codigo75:		;LD C,E
					mov	al,reg_e
					mov	reg_c,al
					ret

codigo76:		;LD C,H
					mov	al,reg_h
					mov	reg_c,al
					ret

codigo77:		;LD C,L
					mov	al,reg_l
					mov	reg_c,al
					ret

codigo78:		;LD C,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_c,al
					ret

codigo79:		;LD C,A
					mov	al,reg_a
					mov	reg_c,al
					ret

codigo80:		;LD D,B
					mov	al,reg_b
					mov	reg_d,al
					ret

codigo81:		;LD D,C
					mov	al,reg_c
					mov	reg_d,al
					ret

codigo82:		;LD D,D
					ret

codigo83:		;LD D,E
					mov	al,reg_e
					mov	reg_d,al
					ret

codigo84:		;LD D,H
					mov	al,reg_h
					mov	reg_d,al
					ret

codigo85:		;LD D,L
					mov	al,reg_l
					mov	reg_d,al
					ret

codigo86:		;LD D,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_d,al
					ret

codigo87:		;LD D,A
					mov	al,reg_a
					mov	reg_d,al
					ret

codigo88:		;LD E,B
					mov	al,reg_b
					mov	reg_e,al
					ret

codigo89:		;LD E,C
					mov	al,reg_c
					mov	reg_e,al
					ret

codigo90:		;LD E,D
					mov	al,reg_d
					mov	reg_e,al
					ret

codigo91:		;LD E,E
					ret

codigo92:		;LD E,H
					mov	al,reg_h
					mov	reg_e,al
					ret

codigo93:		;LD E,L
					mov	al,reg_l
					mov	reg_e,al
					ret

codigo94:		;LD E,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_e,al
					ret

codigo95:		;LD E,A
					mov	al,reg_a
					mov	reg_e,al
					ret

codigo96:		;LD H,B
					mov	al,reg_b
					mov	reg_h,al
					ret

codigo97:		;LD H,C
					mov	al,reg_c
					mov	reg_h,al
					ret

codigo98:		;LD H,D
					mov	al,reg_d
					mov	reg_h,al
					ret

codigo99:		;LD H,E
					mov	al,reg_e
					mov	reg_h,al
					ret

codigo100:		;LD H,H
					ret

codigo101:		;LD H,L
					mov	al,reg_l
					mov	reg_h,al
					ret

codigo102:		;LD H,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_h,al
					ret

codigo103:		;LD H,A
					mov	al,reg_a
					mov	reg_h,al
					ret

codigo104:		;LD L,B
					mov	al,reg_b
					mov	reg_l,al
					ret

codigo105:		;LD L,C
					mov	al,reg_c
					mov	reg_l,al
					ret

codigo106:		;LD L,D
					mov	al,reg_d
					mov	reg_l,al
					ret

codigo107:		;LD L,E
					mov	al,reg_e
					mov	reg_l,al
					ret

codigo108:		;LD L,H
					mov	al,reg_h
					mov	reg_l,al
					ret

codigo109:		;LD L,L
					ret

codigo110:		;LD L,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_l,al
					ret

codigo111:		;LD L,A
					mov	al,reg_a
					mov	reg_l,al
					ret

codigo112:		;LD (HL),B
					mov	bx,word ptr reg_l
					mov	al,reg_b
					poke_byte
					ret

codigo113:		;LD (HL),C
					mov	bx,word ptr reg_l
					mov	al,reg_c
					poke_byte
					ret

codigo114:		;LD (HL),D
					mov	bx,word ptr reg_l
					mov	al,reg_d
					poke_byte
					ret

codigo115:		;LD (HL),E
					mov	bx,word ptr reg_l
					mov	al,reg_e
					poke_byte
					ret

codigo116:		;LD (HL),H
					mov	bx,word ptr reg_l
					mov	al,reg_h
					poke_byte
					ret

codigo117:		;LD (HL),L
					mov	bx,word ptr reg_l
					mov	al,reg_l
					poke_byte
					ret


codigo118:		;HALT

					lahf
					or		bits_estado5,00010000b ;Decir que se ejecuta HALT
					dec	si
					sahf

					ret

codigo119:		;LD (HL),A
					mov	bx,word ptr reg_l
					mov	al,reg_a
					poke_byte
					ret

codigo120:		;LD A,B
					mov	al,reg_b
					mov	reg_a,al
					ret

codigo121:		;LD A,C
					mov	al,reg_c
					mov	reg_a,al
					ret

codigo122:		;LD A,D
					mov	al,reg_d
					mov	reg_a,al
					ret

codigo123:		;LD A,E
					mov	al,reg_e
					mov	reg_a,al
					ret

codigo124:		;LD A,H
					mov	al,reg_h
					mov	reg_a,al
					ret

codigo125:		;LD A,L
					mov	al,reg_l
					mov	reg_a,al
					ret
codigo126:		;LD A,(HL)
					mov	bx,word ptr reg_l
					peek_byte
					mov	reg_a,al
					ret

codigo127:		;LD A,A
					ret

codigo128:		;ADD A,B
					add_al_reg b
					ret

codigo129:		;ADD A,C
					add_al_reg c
					ret

codigo130:		;ADD A,D
					add_al_reg d
					ret

codigo131:		;ADD A,E
					add_al_reg e
					ret

codigo132:		;ADD A,H
					add_al_reg h
					ret

codigo133:		;ADD A,L
					add_al_reg l
					ret

codigo134:		;ADD A,(HL)
					mov	bx,word ptr reg_l
					mov	dl,byte ptr es:[bx]
					add_al_dl
					ret

codigo135:		;ADD A,A
					add_al_reg a
					ret

codigo136:		;ADC A,B
					adc_al_reg b
					ret

codigo137:		;ADC A,C
					adc_al_reg c
					ret

codigo138:		;ADC A,D
					adc_al_reg d
					ret

codigo139:		;ADC A,E
					adc_al_reg e
					ret

codigo140:		;ADC A,H
					adc_al_reg h
					ret

codigo141:		;ADC A,L
					adc_al_reg l
					ret

codigo142:		;ADC A,(HL)
					mov	bx,word ptr reg_l
					mov	dl,byte ptr es:[bx]
					adc_al_dl
					ret

codigo143:		;ADC A,A
					adc_al_reg a
					ret

codigo144:		;SUB B
					sub_al_reg b
					ret

codigo145:		;SUB C
					sub_al_reg c
					ret

codigo146:		;SUB D
					sub_al_reg d
					ret

codigo147:		;SUB E
					sub_al_reg e
					ret

codigo148:		;SUB H
					sub_al_reg h
					ret

codigo149:		;SUB L
					sub_al_reg l
					ret

codigo150:		;SUB (HL)
					mov	bx,word ptr reg_l
					mov	dl,byte ptr es:[bx]
					sub_al_dl
					ret

codigo151:		;SUB A
					sub_al_reg a
					ret

codigo152:		;SBC A,B
					sbc_al_reg b
					ret

codigo153:		;SBC A,C
					sbc_al_reg c
					ret

codigo154:		;SBC A,D
					sbc_al_reg d
					ret

codigo155:		;SBC A,E
					sbc_al_reg e
					ret

codigo156:		;SBC A,H
					sbc_al_reg h
					ret

codigo157:		;SBC A,L
					sbc_al_reg l
					ret

codigo158:		;SBC A,(HL)
					mov	bx,word ptr reg_l
					mov	dl,byte ptr es:[bx]
					sbc_al_dl
					ret

codigo159:		;SBC A,A
					sbc_al_reg a
					ret

codigo160:		;AND B  El AND deja al flag AF desconocido
					mov	al,reg_b
					and	reg_a,al
					ret

codigo161:		;AND C
					mov	al,reg_c
					and	reg_a,al
					ret

codigo162:		;AND D
					mov	al,reg_d
					and	reg_a,al
					ret

codigo163:		;AND E
					mov	al,reg_e
					and	reg_a,al
					ret

codigo164:		;AND H
					mov	al,reg_h
					and	reg_a,al
					ret

codigo165:		;AND L
					mov	al,reg_l
					and	reg_a,al
					ret

codigo166:		;AND (HL)
					mov	bx,word ptr reg_l
					peek_byte
					and	reg_a,al
					ret

codigo167:		;AND A
					mov	al,reg_a
					and	reg_a,al
					ret

codigo168:		;XOR B
					mov	al,reg_b
					xor	reg_a,al
					ret

codigo169:		;XOR C
					mov	al,reg_c
					xor	reg_a,al
					ret

codigo170:		;XOR D
					mov	al,reg_d
					xor	reg_a,al
					ret

codigo171:		;XOR E
					mov	al,reg_e
					xor	reg_a,al
					ret

codigo172:		;XOR H
					mov	al,reg_h
					xor	reg_a,al
					ret

codigo173:		;XOR L
					mov	al,reg_l
					xor	reg_a,al
					ret

codigo174:		;XOR (HL)
					mov	bx,word ptr reg_l
					peek_byte
					xor	reg_a,al
					ret

codigo175:		;XOR A
					mov	al,reg_a
					xor	reg_a,al
					ret

codigo176:		;OR B
					mov	al,reg_b
					or	   reg_a,al
					ret

codigo177:		;OR C
					mov	al,reg_c
					or 	reg_a,al
					ret

codigo178:		;OR D
					mov	al,reg_d
					or 	reg_a,al
					ret

codigo179:		;OR E
					mov	al,reg_e
					or 	reg_a,al
					ret

codigo180:		;OR H
					mov	al,reg_h
					or 	reg_a,al
					ret

codigo181:		;OR L
					mov	al,reg_l
					or 	reg_a,al
					ret

codigo182:		;OR (HL)
					mov	bx,word ptr reg_l
					peek_byte
					or	   reg_a,al
					ret

codigo183:		;OR A
					mov	al,reg_a
					or 	reg_a,al
					ret

codigo184:		;CP B
					cp_al_reg b
					ret

codigo185:		;CP C
					cp_al_reg c
					ret

codigo186:		;CP D
					cp_al_reg d
					ret

codigo187:		;CP E
					cp_al_reg e
					ret

codigo188:		;CP H
					cp_al_reg h
					ret

codigo189:		;CP L
					cp_al_reg l
					ret

codigo190:		;CP (HL)
					mov	bx,word ptr reg_l
					mov	dl,byte ptr es:[bx]
					cp_al_dl
					ret

codigo191:		;CP A
					cp_al_reg a
					ret

codigo192:		;RET NZ
					jnz	codigo192_nz
					ret
codigo192_nz:	;Realizar un ret del Z80
					ret_pc
					ret

codigo193:		;POP BC
					pop_ax
					mov	word ptr reg_c,ax
					ret

codigo194:		;JP NZ,NN
					lee_word_pc
					jnz	codigo194_nz
					ret
codigo194_nz:	;Realizar JP NN
					mov	si,ax
					ret

codigo195:		;JP NN
					lee_word_pc
					mov	si,ax
					ret

codigo196:		;CALL NZ,NN
					lee_word_pc
					jnz	codigo196_nz
					ret
codigo196_nz:	;Realizar CALL
					call_ax
					ret

codigo197:		;PUSH BC
					mov	ax,word ptr reg_c
					push_ax
					ret

codigo198:		;ADD A,N
					mov	dl,byte ptr es:[si]
					lahf
					inc	si
					sahf
					add_al_dl

					ret

codigo199:		;RST 0
					rst_n 0
					ret

codigo200:		;RET Z
					jz	   codigo200_z
					ret
codigo200_z:	;Realizar un ret del Z80
					ret_pc
					ret

codigo201:		;RET
					ret_pc
					ret

codigo202:		;JP Z,NN
					lee_word_pc
					jz	   codigo202_z
					ret
codigo202_z:	;Realizar JP NN
					mov	si,ax
					ret

codigo203:
					ret

codigo204:		;CALL Z,NN
					lee_word_pc
					jz	   codigo204_z
					ret
codigo204_z:	;Realizar CALL
					call_ax
					ret

codigo205:     ;CALL NN
					lee_word_pc
					call_ax
					ret

codigo206:		;ADC A,N
					mov	dl,byte ptr es:[si]
					lahf
					inc	si
					sahf
					adc_al_dl
					ret

codigo207:		;RST 8
					rst_n 8
					ret

codigo208:		;RET NC
					jnc   codigo208_nc
					ret
codigo208_nc:	;Realizar un ret del Z80
					ret_pc
					ret

codigo209:		;POP DE
					pop_ax
					mov	word ptr reg_e,ax
					ret

codigo210:		;JP NC,NN
					lee_word_pc
					jnc	codigo210_nc
					ret
codigo210_nc:	;Realizar JP NN
					mov	si,ax
					ret

codigo211:		;OUT (N),A
					lods  es:fuente
					pushf

					mov	bl,reg_a
					mov	ah,bl
					call	codigo211_saca
					popf
					ret

codigo211_saca: ;Se puede llamar desde aqu� con AX el puerto, y en BL el valor
					;Si el puerto es par, hace como si estuviera en
					;memoria compartida
					test	al,1
					jnz	codigo211_saca_impar
					or    bits_estado5,01000000b
codigo211_saca_impar:

					cmp	al,254 ;Puerto del borde/sonido
					jz		codigo211_p254

					cmp	ax,8189
					jnz	no_codigo_8189
					call  [p_out_8189]
					ret
no_codigo_8189:
COMMENT !
					cmp   al,204 ;Puerto cadena inicializaci�n de cinta
					jnz	no_codigo_204
					jmp	puerto204
no_codigo_204: cmp	al,205 ;Puerto de datos cinta de carga
					jnz	no_codigo_205

					pop   dx  ;Quitar direccion de retorno
					popf
					push	dx
					cargar_reg_f
					call	puerto205
					cargar_flags_con_f
					pop	dx
					pushf
					push 	dx

					ret


no_codigo_205:
					cmp	al,206 ;Puerto de datos cinta de grabar
					jnz	no_codigo_206
					jmp	puerto206
!
no_codigo_206:
					mov   cx,ax
					and   cx,1100000000000010b
					cmp   cx,1100000000000000b
					jnz	no_codigo_65533
					call	out_p65533

					ret

no_codigo_65533:
					mov   cx,ax
					and   cx,1100000000000010b
					cmp   cx,1000000000000000b
					jnz	no_codigo_49149
					call	out_p49149

					ret

no_codigo_49149:
					;Ver si es +2A o 128k
					test  bits_estado6,00010000b
					jz    no_codigo_128
					;Es un 128

					test  ax,1000000000000010b
					;Para 128 (no para +2A)
					;A1=0,A15=0
codigo_128:

					jnz   no_codigo_32765

					jmp   [p_out_32765]
no_codigo_128:
					;No es 128. Puede ser cualquier otro
					cmp   ax,32765
					jmp   short codigo_128

no_codigo_32765:

					ret


codigo211_p254:
					;Guardarlo en variable puerto_254
					mov   puerto_254,bl
					;Mirar si modo Inves
					test	bits_estado5,00100000b
					jz    codigo211_p254_noinves
					and	bl,valor_poke_rom
codigo211_p254_noinves:
					;Mirar si modo inves
					test	bits_estado5,00100000b
					jz    codigo211_p254_noinves2

					;Hacer XOR con bits 3 y 4
					mov	ah,bl
					ror	ah,1
					mov	al,bl
					and	ax,0000100000001000b
					xor   ah,al

					mov	al,previo_puerto254
					mov   previo_puerto254,ah
					and   al,00001000b
					jmp	short codigo211_p254_noinves3

codigo211_p254_noinves2:

					mov	ah,bl ;Valor en puerto 254 XXXSSCCC S:sonido C:color
					mov	al,previo_puerto254
					mov   previo_puerto254,ah
					and	ax,0001100000011000b
codigo211_p254_noinves3:
					cmp   al,ah

					;Si son iguales, no conmutar altavoz
					jz    codigo211_p254_noconm
					;Conmutar altavoz
					xor   previo_salida_sonido,2
codigo211_p254_noconm:



;enviar sonido a buffer, soundblaster o archivo .raw
mov	ah,previo_salida_sonido

				test	bits_estado7,10000000b
				jnz	codigo211_p254_blaster
				
				cmp	soundblaster_presente,0
				jz    codigo211_p254_no_sonido ;Sound Blaster no presente

				test	bits_estado8,00000001b
				jnz		codigo211_p254_no_sonido  ;sonido silenciado


codigo211_p254_blaster:

				push ax
				push bx
				call	out211_blaster
				pop bx 
				pop ax




					;;cmp	sonido,0
					;;jz		codigo211_p254_no_sonido ;El sonido est� desactivado					

					;;test	bits_estado3,00001000b
					;;jz		speaker  ;Salida por el altavoz de PC

;Salida por la Sound Blaster
					;cmp	soundblaster_presente,0
					;jz    codigo211_p254_no_sonido ;Sound Blaster no presente

					;call	out211_blaster
					;;jmp	short codigo211_p254_no_sonido

;;speaker:
;;					in		al,97 ;Puerto sonido
;;					and	al,11111101b
;;					or		al,ah
;;					out	97,al ;Enviar activaci�n/desactivaci�n de altavoz

codigo211_p254_no_sonido:
					and	bl,00000111b
					mov	border,bl

					ret

codigo212:		;CALL NC,NN
					lee_word_pc
					jnc	codigo212_nc
					ret
codigo212_nc:	;Realizar CALL
					call_ax
					ret

codigo213:		;PUSH DE
					mov	ax,word ptr reg_e
					push_ax
					ret

codigo214:		;SUB N
					mov	dl,byte ptr es:[si]
					lahf
					inc	si
					sahf
					sub_al_dl
					ret

codigo215:		;RST 16
					rst_n 16
					ret

codigo216:		;RET C
					jc    codigo216_c
					ret
codigo216_c:	;Realizar un ret del Z80
					ret_pc
					ret

codigo217:		;EXX Intercanviar BC,BC' HL,HL' DE,DE'
					mov	ax,word ptr reg_c
					mov	bx,word ptr reg_l
					mov	cx,word ptr reg_e

					xchg	ax,word ptr reg_c_
					xchg  bx,word ptr reg_l_
					xchg  cx,word ptr reg_e_

					mov	word ptr reg_c,ax
					mov	word ptr reg_l,bx
					mov	word ptr reg_e,cx

					ret

codigo218:		;JP C,NN
					lee_word_pc
					jc	   codigo218_c
					ret
codigo218_c:	;Realizar JP NN
					mov	si,ax
					ret

codigo219:		;IN A,(N)
					pushf
					mov	bl,byte ptr es:[si]
					inc	si
					mov	bh,reg_a
					call	codigo219_lee
					popf
					mov	reg_a,al
					ret
codigo219_lee:	;EN BX puerto a leer Salida=AL=Valor del puerto
					;se puede llamar aqu� desde rutinas externas

					;Si el puerto es par, hace como si estuviera en
					;memoria compartida
					test	bx,1
					jnz	codigo219_lee_impar
					or    bits_estado5,01000000b
codigo219_lee_impar:
					cmp	bl,254 ;Puerto teclado
					jnz	codigo219_no_teclado
					;jz		codigo219_254
					jmp	codigo219_254
codigo219_no_teclado:
;					cmp	bx,204 ;Error en cinta
;					jnz	codigo219_2
;					jmp	puerto204_in
codigo219_2:
					mov   ax,bx
					and   ax,1100000000000010b
					cmp   ax,1100000000000000b
					jnz	codigo219_3
					jmp	in_p65533

codigo219_3:
					;Si A5=0, kempston joystick
					test	bl,00100000b
					jnz	codigo219_no211
					jmp	codigo219_211
					;jz	   codigo219_211
codigo219_no211:
					;Ver si es +2A o 128k
					test  bits_estado6,00010000b
					jz    puerto_no128k
					;Es un 128 o +2

					test  bx,1000000000000010b
					;Para 128 (no para +2A)
					;A1=0,A15=0

					jnz   puerto_no128k

					;Ver si paginacion bloqueada
					test  puerto_32765,00100000b
					jnz   puerto_no128k

					;Si se lee puerto de paginacion en modo 128k, se
					;hace un OUT al puerto con valor 255

					mov	ax,bx
					mov	bl,255
					jmp   [p_out_32765]
					ret

puerto_no128k:

					;Puerto no usado
					jmp	[p_in_255]

in_255_p2a:
					;Se es +2A, entonces solo retornar byte de la ULA
					;si el puerto tiene la mascara 0000XXXXXXXXXX01
					;o sea, puertos 1,5,9,13,17,..., hasta 4093
					;y ademas no esta la paginacion bloqueada
					;El byte retornado de la ULA se le hace OR 1
					mov	ax,bx
					and   ax,1111000000000011b
					cmp	ax,1
					jnz   in_255_p2a_255

					;Ver si paginacion activa
					test  puerto_32765,00100000b
					jnz   in_255_p2a_255
					call	in_255_48
					or		al,1
					ret

in_255_p2a_255:
					jmp   puerto_no_usado_255

in_255_48:
					;Retornar byte que lee la ULA
					;128 ciclos, dibujar pantalla
					;96 ciclos, dibujar border
					;Retornar 255 cuando no se lee byte

					;Ver en que estado esta
					mov   ax,sync_estados_actual
					cmp	ax,128
					jnc	puerto_no_usado_255 ;Se lee el border


					;Devolver byte que lee la ULA

					;Obtener linea
					mov	cx,sync_lineas_actual
					sub 	cx,sync_borde_sup
					jc		puerto_no_usado_255 ;se lee el border superior
					cmp	cx,sync_lineas
					jnc	puerto_no_usado_255 ;se lee el border inferior

					;Pixel o atributo?
					;Metodo aproximado: Segun bit 1=0 leer pixel
					;Sino leer color
					test  ax,2
					jz		puerto_no_usado_pixel

					;Leer color
					;En 128, se leen 32 bytes
					;Dividir estados entre 4 (128/32=4)
					shr	ax,1
					shr	ax,1
					;Leer byte de pantalla
					;Dividir linea por 8

					shr	cx,1
					shr	cx,1
					shr	cx,1
					add	ax,cx
					add   ax,22528

					push	es
					mov	es,seg_pant
					mov	bx,ax
					mov	al,byte ptr es:[bx]
					pop	es
					ret

puerto_no_usado_pixel:
					;Leer pixel
					;En 128, se leen 32 bytes
					;Dividir estados entre 4 (128/32=4)

					;Leer direccion de pantalla
					mov	bx,cx
					add	bx,bx ;*2
					add   bx,bx ;*4
					add	bx,offset pantalla_tabla
					mov	cx,word ptr [bx]
					;En 128, se leen 32 bytes
					;Dividir estados entre 4 (128/32=4)

					shr	ax,1
					shr	ax,1

					mov   bx,cx
					add   bx,ax

					push	es
					mov	es,seg_pant
					mov	al,byte ptr es:[bx]
					pop	es
					ret


puerto_no_usado_255:
					mov	al,255
					ret

codigo219_211: test  bits_estado4,00100000b
					jz    codigo219_211_nomultitarea
					xor   al,al
					ret

codigo219_211_nomultitarea:
					mov   al,puerto_221
					not	al
					and	al,31
					ret

codigo219_254:
					;Ver si se esta en modo multitarea menu-emulacion z80
					test	bits_estado4,00100000b

					jz		codigo219_254_1

					mov	al,byte ptr puerto_65278
					or		al,0011111b ;Ninguna tecla pulsada
					ret

codigo219_254_1:

					or		bh,bh ;Ver si es puerto 254=0x00FE
					jnz	codigo219_254_2
					;Leer puerto 254 (todas teclas simultaneamente)
					call	puerto_in_254
					ret

codigo219_254_2:
					cmp	bh,254
					jnz	puerto_tec_no254
					mov	al,puerto_65278
					ret

puerto_tec_no254:
					cmp	bh,253
					jnz	puerto_tec_no253
					mov	al,puerto_65022
					ret

puerto_tec_no253:
					cmp	bh,251
					jnz	puerto_tec_no251
					mov	al,puerto_64510
					ret

puerto_tec_no251:
					cmp	bh,247
					jnz	puerto_tec_no247
					mov	al,puerto_63486
					ret

puerto_tec_no247:
					cmp	bh,239
					jnz	puerto_tec_no239
					mov	al,puerto_61438
					ret

puerto_tec_no239:
					cmp	bh,223
					jnz	puerto_tec_no223
					mov	al,puerto_57342
					ret

puerto_tec_no223:
					cmp	bh,191
					jnz	puerto_tec_no191
					mov	al,puerto_49150
					ret

puerto_tec_no191:
					cmp	bh,127
					jnz	puerto_tec_no127
					mov	al,puerto_32766
					ret

puerto_tec_no127:
					mov	al,255 ;Puerto no determinado
					ret

codigo220:		;CALL C,NN
					lee_word_pc
					jc	   codigo220_c
					ret
codigo220_c:	;Realizar CALL
					call_ax
					ret

codigo221:
					ret

codigo222:		;SBC A,N
					mov	dl,byte ptr es:[si]
					pushf
					inc	si
					popf
					sbc_al_dl
					ret

codigo223:		;RST 24
					rst_n 24
					ret

codigo224:		;RET PO
					jpo   codigo224_po
					ret
codigo224_po:	;Realizar un ret del Z80
					ret_pc
					ret

codigo225:		;POP HL
					pop_ax
					mov	word ptr reg_l,ax
					ret

codigo226:		;JP PO,NN
					lee_word_pc
					jpo   codigo226_po
					ret
codigo226_po:	;Realizar JP NN
					mov	si,ax
					ret

codigo227:		;EX (SP),HL
					pushf
					mov   al,byte ptr es:[di]
					mov   ah,byte ptr es:[di+1]

					xchg  word ptr reg_l,ax   ;HL=(SP)
					mov	byte ptr es:[di],al
					mov	byte ptr es:[di+1],ah
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

codigo228:		;CALL PO,NN
					lee_word_pc
					jpo   codigo228_po
					ret
codigo228_po:	;Realizar CALL
					call_ax
					ret

codigo229:		;PUSH HL
					mov	ax,word ptr reg_l
					push_ax
					ret

codigo230:		;AND N
					lee_byte_pc
					and	reg_a,al
					ret

codigo231:		;RST 32
					rst_n 32
					ret

codigo232:		;RET PE
					jpe   codigo232_pe
					ret
codigo232_pe:	;Realizar un ret del Z80
					ret_pc
					ret

codigo233:		;JP (HL)
					mov	si,word ptr reg_l
					ret

codigo234:     ;JP PE,NN
					lee_word_pc
					jpe   codigo234_pe
					ret
codigo234_pe:	;Realizar JP NN
					mov	si,ax
					ret

codigo235:		;EX DE,HL
					mov	bx,word ptr reg_l
					xchg	word ptr reg_e,bx
					mov	word ptr reg_l,bx
					ret

codigo236:		;CALL PE,NN
					lee_word_pc
					jpe   codigo236_pe
					ret
codigo236_pe:	;Realizar CALL
					call_ax
					ret

codigo237:
					ret

codigo238:		;XOR N
					lee_byte_pc
					xor	reg_a,al
					ret

codigo239:		;RST 40
					rst_n 40
					ret

codigo240:		;RET P ;P=No negativo
					jns   codigo240_ns
					ret
codigo240_ns:	;Realizar un ret del Z80
					ret_pc
					ret

codigo241:		;POP AF
					pop_ax
					mov	word ptr reg_f,ax

					mov   ah,al
					mov   ch,al
					and   ch,00000010b
					mov   flag_n,ch  ;Poner flag N

					and	ah,11010101b
					sahf

					ret

codigo242:		;JP P,NN P=No negativo
					lee_word_pc
					jns   codigo242_ns
					ret
codigo242_ns:	;Realizar JP NN
					mov	si,ax
					ret

codigo243:		;DI
					lahf
					and   bits_estado,11111110b
					sahf
					ret

codigo244:		;CALL P,NN P=No Signo
					lee_word_pc
					jns   codigo244_ns
					ret
codigo244_ns:	;Realizar CALL
					call_ax
					ret

codigo245:		;PUSH AF

					lahf
					mov	ch,ah
					and	ah,11010101b
					or		ah,flag_n

					mov	cl,reg_f
					and	cl,00101000b
					or		ah,cl

					mov	al,reg_a
					xchg	ah,al

					push_ax
					mov	ah,ch
					sahf
					ret

codigo246:		;OR N
					lee_byte_pc
					or	   reg_a,al
					ret

codigo247:		;RST 48
					rst_n 48
					ret

codigo248:		;RET M M=Signo negativo
					js    codigo248_s
					ret
codigo248_s:	;Realizar un ret del Z80
					ret_pc
					ret

codigo249:		;LD SP,HL
					mov	di,word ptr reg_l
					ret

codigo250:		;JP M,NN M=Signo negativo
					lee_word_pc
					js    codigo250_s
					ret
codigo250_s:	;Realizar JP NN
					mov	si,ax
					ret

codigo251:		;EI
					lahf
					or    bits_estado,1
					sahf
					ret

codigo252:		;CALL M,NN M=Signo negativo
					lee_word_pc
					js    codigo252_s
					ret
codigo252_s:	;Realizar CALL
					call_ax
					ret

codigo253:		ret

codigo254:		;CP N
					mov	dl,byte ptr es:[si]
					pushf
					inc	si
					popf
					cp_al_dl
					ret

codigo255:		;RST 56
               rst_n 56
					ret
