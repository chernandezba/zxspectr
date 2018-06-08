;Este fichero contiene las rutinas con prefijo 221 o 253 + 203

PUBLIC t_codigos_xycb,t_codigos_xycb_estados

t_codigos_xycb:
					etiqueta_codxycb 0,63
					etiqueta_nop 64,69
					dw		codxycb70
					etiqueta_nop 71,77
					dw		codxycb78
					etiqueta_nop 79,85
					dw		codxycb86
					etiqueta_nop 87,93
					dw		codxycb94
					etiqueta_nop 95,101
					dw		codxycb102
					etiqueta_nop 103,109
					dw		codxycb110
					etiqueta_nop 111,117
					dw		codxycb118
					etiqueta_nop 119,125
					dw		codxycb126
					dw		codigo0
					etiqueta_codxycb 128,255

t_codigos_xycb_estados db   23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23 ;31
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23 ;47
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23 ;63
	db 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
	db 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
	db 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
	db 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23
	db 23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23


codxycb0:      ;RLC (IX+d),B
					rotacion_xy rol,b
					ret
codxycb1:      ;RLC (IX+d),C
					rotacion_xy rol,c
					ret
codxycb2:      ;RLC (IX+d),D
					rotacion_xy rol,d
					ret
codxycb3:      ;RLC (IX+d),E
					rotacion_xy rol,e
					ret
codxycb4:      ;RLC (IX+d),H
					rotacion_xy rol,h
					ret
codxycb5:      ;RLC (IX+d),L
					rotacion_xy rol,l
					ret
codxycb6:      ;RLC (IX+d)
					rotacion_xy rol
					ret
codxycb7:      ;RLC (IX+d),A
					rotacion_xy rol,a
					ret

codxycb8:      ;RRC (IX+d),B
					rotacion_xy ror,b
					ret
codxycb9:      ;RRC (IX+d),C
					rotacion_xy ror,c
					ret
codxycb10:     ;RRC (IX+d),D
					rotacion_xy ror,d
					ret
codxycb11:     ;RRC (IX+d),E
					rotacion_xy ror,e
					ret
codxycb12:     ;RRC (IX+d),H
					rotacion_xy ror,h
					ret
codxycb13:     ;RRC (IX+d),L
					rotacion_xy ror,l
					ret
codxycb14:     ;RRC (IX+d)
					rotacion_xy ror
					ret
codxycb15:     ;RRC (IX+d),A
					rotacion_xy ror,a
					ret

codxycb16:     ;RL (IX+d),B
					rotacion_xy rcl,b
					ret
codxycb17:     ;RL (IX+d),C
					rotacion_xy rcl,c
					ret
codxycb18:     ;RL (IX+d),D
					rotacion_xy rcl,d
					ret
codxycb19:     ;RL (IX+d),E
					rotacion_xy rcl,e
					ret
codxycb20:     ;RL (IX+d),H
					rotacion_xy rcl,h
					ret
codxycb21:     ;RL (IX+d),L
					rotacion_xy rcl,l
					ret
codxycb22:     ;RL (IX+d)
					rotacion_xy rcl
					ret
codxycb23:     ;RL (IX+d),A
					rotacion_xy rcl,a
					ret

codxycb24:     ;RR (IX+d),B
					rotacion_xy rcr,b
					ret
codxycb25:     ;RR (IX+d),C
					rotacion_xy rcr,c
					ret
codxycb26:     ;RR (IX+d),D
					rotacion_xy rcr,d
					ret
codxycb27:     ;RR (IX+d),E
					rotacion_xy rcr,e
					ret
codxycb28:     ;RR (IX+d),H
					rotacion_xy rcr,h
					ret
codxycb29:     ;RR (IX+d),L
					rotacion_xy rcr,l
					ret
codxycb30:     ;RR (IX+d)
					rotacion_xy rcr
					ret
codxycb31:     ;RR (IX+d),A
					rotacion_xy rcr,a
					ret

codxycb32:     ;SLA (IX+d),B
					rotacion_xy sal,b
					ret
codxycb33:     ;SLA (IX+d),C
					rotacion_xy sal,c
					ret
codxycb34:     ;SLA (IX+d),D
					rotacion_xy sal,d
					ret
codxycb35:     ;SLA (IX+d),E
					rotacion_xy sal,e
					ret

codxycb36:     ;SLA (IX+d),H
					rotacion_xy sal,h
					ret
codxycb37:     ;SLA (IX+d),L
					rotacion_xy sal,l
					ret
codxycb38:     ;SLA (IX+d)
					rotacion_xy sal
					ret
codxycb39:     ;SLA (IX+d),A
					rotacion_xy sal,a
					ret

codxycb40:     ;SRA (IX+d),B
					rotacion_xy sar,b
					ret
codxycb41:     ;SRA (IX+d),C
					rotacion_xy sar,c
					ret
codxycb42:     ;SRA (IX+d),D
					rotacion_xy sar,d
					ret
codxycb43:     ;SRA (IX+d),E
					rotacion_xy sar,e
					ret
codxycb44:     ;SRA (IX+d),H
					rotacion_xy sar,h
					ret
codxycb45:     ;SRA (IX+d),L
					rotacion_xy sar,l
					ret
codxycb46:     ;SRA (IX+d)
					rotacion_xy sar
					ret
codxycb47:     ;SRA (IX+d),A
					rotacion_xy sar,a
					ret

codxycb48:     ;SLL (IX+d),B
					sll_xy b
					ret
codxycb49:     ;SLL (IX+d),C
					sll_xy c
					ret
codxycb50:     ;SLL (IX+d),D
					sll_xy d
					ret
codxycb51:     ;SLL (IX+d),E
					sll_xy e
					ret
codxycb52:     ;SLL (IX+d),H
					sll_xy h
					ret
codxycb53:     ;SLL (IX+d),L
					sll_xy l
					ret
codxycb54:     ;SLL (IX+d)
					sll_xy
					ret
codxycb55:     ;SLL (IX+d),A
					sll_xy a
					ret

codxycb56:     ;SRL (IX+d),B
					rotacion_xy shr,b
					ret
codxycb57:     ;SRL (IX+d),C
					rotacion_xy shr,c
					ret
codxycb58:     ;SRL (IX+d),D
					rotacion_xy shr,d
					ret
codxycb59:     ;SRL (IX+d),E
					rotacion_xy shr,e
					ret
codxycb60:     ;SRL (IX+d),H
					rotacion_xy shr,h
					ret
codxycb61:     ;SRL (IX+d),L
					rotacion_xy shr,l
					ret
codxycb62:     ;SRL (IX+d)
					rotacion_xy shr
					ret
codxycb63:     ;SRL (IX+d),A
					rotacion_xy shr,a
					ret


codxycb70:		;BIT 0,(XY+d)
					bitxycb 0

codxycb78:		;BIT 1,(XY+d)
					bitxycb 1

codxycb86:		;BIT 2,(XY+d)
					bitxycb 2

codxycb94:		;BIT 3,(XY+d)
					bitxycb 3

codxycb102:		;BIT 4,(XY+d)
					bitxycb 4

codxycb110:		;BIT 5,(XY+d)
					bitxycb 5

codxycb118:		;BIT 6,(XY+d)
					bitxycb 6

codxycb126:		;BIT 7,(XY+d)
					bitxycb 7

resxycb
setxycb



