;Este fichero contiene las rutinas para las instrucciones
;con prefijo 203=0CBH

PUBLIC t_codigos_prefcb,t_codigos_prefcb_estados


t_codigos_prefcb:
					etiqueta_codcb 0,255

t_codigos_prefcb_estados db    8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8 ;31
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8 ;47
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8 ;63
	db 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8 ;79
	db 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8
	db 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8
	db 8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8
	db 8, 8, 8, 8, 8, 8,15, 8, 8, 8, 8, 8, 8, 8,15, 8


codcb0:			;RLC B
					rotacion rol,b
					ret

codcb1:			;RLC C
					rotacion rol,c
					ret

codcb2:			;RLC D
					rotacion rol,d
					ret

codcb3:			;RLC E
					rotacion rol,e
					ret

codcb4:			;RLC H
					rotacion rol,h
					ret

codcb5:			;RLC L
					rotacion rol,l
					ret

codcb6:			;RLC (HL)
					rotacion_hl rol
					ret

codcb7:			;RLC A
					rotacion rol,a
					ret

codcb8:			;RRC B
					rotacion ror,b
					ret

codcb9:			;RRC C
					rotacion ror,c
					ret

codcb10:			;RRC D
					rotacion ror,d
					ret

codcb11:			;RRC E
					rotacion ror,e
					ret

codcb12:			;RRC H
					rotacion ror,h
					ret

codcb13:			;RRC L
					rotacion ror,l
					ret

codcb14:			;RRC (HL)
					rotacion_hl ror
					ret

codcb15:			;RRC A
					rotacion ror,a
					ret

codcb16:			;RL B
					rotacion rcl,b
					ret

codcb17:			;RL C
					rotacion rcl,c
					ret

codcb18:			;RL D
					rotacion rcl,d
					ret

codcb19:			;RL E
					rotacion rcl,e
					ret

codcb20:			;RL H
					rotacion rcl,h
					ret

codcb21:			;RL L
					rotacion rcl,l
					ret

codcb22:			;RL (HL)
					rotacion_hl rcl
					ret

codcb23:			;RL A
					rotacion rcl,a
					ret

codcb24:       ;RR B
					rotacion rcr,b
					ret

codcb25:       ;RR C
					rotacion rcr,c
					ret

codcb26:       ;RR D
					rotacion rcr,d
					ret

codcb27:       ;RR E
					rotacion rcr,e
					ret

codcb28:       ;RR H
					rotacion rcr,h
					ret

codcb29:       ;RR L
					rotacion rcr,l
					ret

codcb30:       ;RR (HL)
					rotacion_hl rcr
					ret

codcb31:       ;RR A
					rotacion rcr,a
					ret


codcb32:			;SLA B
					rotacion sal,b
					ret

codcb33:			;SLA C
					rotacion sal,c
					ret

codcb34:			;SLA D
					rotacion sal,d
					ret

codcb35:			;SLA E
					rotacion sal,e
					ret

codcb36:			;SLA H
					rotacion sal,h
					ret

codcb37:			;SLA L
					rotacion sal,l
					ret

codcb38:			;SLA (HL)
					rotacion_hl sal
					ret

codcb39:			;SLA A
					rotacion sal,a
					ret

codcb40:			;SRA B
					rotacion sar,b
					ret

codcb41:			;SRA C
					rotacion sar,c
					ret

codcb42:			;SRA D
					rotacion sar,d
					ret

codcb43:			;SRA E
					rotacion sar,e
					ret

codcb44:			;SRA H
					rotacion sar,h
					ret

codcb45:			;SRA L
					rotacion sar,l
					ret

codcb46:			;SRA (HL)
					rotacion_hl sar
					ret

codcb47:			;SRA A
					rotacion sar,a
					ret


codcb48:			;SLL B
					rotacion_sll b
					ret

codcb49:			;SLL C
					rotacion_sll c
					ret

codcb50:			;SLL D
					rotacion_sll d
					ret

codcb51:			;SLL E
					rotacion_sll e
					ret

codcb52:			;SLL H
					rotacion_sll h
					ret

codcb53:			;SLL L
					rotacion_sll l
					ret

codcb54:			;SLL (HL)
					rotacion_sll_hl
					ret

codcb55:			;SLL A
					rotacion_sll a
					ret

codcb56:			;SRL B
					rotacion shr,b
					ret

codcb57:			;SRL C
					rotacion shr,c
					ret

codcb58:			;SRL D
					rotacion shr,d
					ret

codcb59:			;SRL E
					rotacion shr,e
					ret

codcb60:			;SRL H
					rotacion shr,h
					ret

codcb61:			;SRL L
					rotacion shr,l
					ret

codcb62:			;SRL (HL)
					rotacion_hl shr
					ret

codcb63:			;SRL A
					rotacion shr,a
					ret



;BIT b,reg ->Si bit b de reg es 0, flag Z=0 sino flag Z=1

bits

;RES b,reg ->Poner a 0 bit b del registro b
ress
;SET b,reg ->Poner a 1 bir b del registro b
_sets     ;SETS es palabra reservada
