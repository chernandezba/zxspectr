extrn fuente:BYTE

;VARIABLES

extrn reg_c:BYTE,reg_b:BYTE,reg_e:BYTE,reg_d:BYTE
extrn reg_l:BYTE,reg_h:BYTE,reg_f:BYTE,reg_a:BYTE
extrn reg_ixl:BYTE,reg_ixh:BYTE,reg_iyl:BYTE,reg_iyh:BYTE

extrn reg_c_:BYTE,reg_b_:BYTE,reg_e_:BYTE,reg_d_:BYTE
extrn reg_l_:BYTE,reg_h_:BYTE,reg_f_:BYTE,reg_a_:BYTE

extrn reg_r:BYTE,reg_r_bit7:BYTE,reg_i:BYTE
extrn reg_sp:WORD

extrn flag_n:BYTE

extrn bits_estado:BYTE,bits_estado2:BYTE,bits_estado3:BYTE,bits_estado4:BYTE
extrn bits_estado5:BYTE,bits_estado6:BYTE
extrn interrumpir:BYTE

extrn border:BYTE,sonido:BYTE,soundblaster_presente:BYTE
extrn puerto_221:BYTE
extrn puerto_65278:BYTE
extrn puerto_65022:BYTE
extrn puerto_64510:BYTE
extrn puerto_63486:BYTE
extrn puerto_61438:BYTE
extrn puerto_57342:BYTE
extrn puerto_49150:BYTE
extrn puerto_32766:BYTE
extrn previo_puerto254:BYTE,previo_salida_sonido:BYTE
extrn valor_poke_rom:BYTE,puerto_254:BYTE
extrn poke_word_ax:WORD,poke_word_bx:WORD,call_ax_ax:WORD
extrn tabla_daa:WORD
extrn sync_estados_actual:WORD,sync_lineas_actual:WORD,sync_borde_sup:WORD
extrn pantalla_tabla:WORD,seg_pant:WORD,sync_lineas:WORD
extrn ordenador_emulado:BYTE,puerto_32765:BYTE


;RUTINAS
extrn out211_blaster:near
extrn menu:near
extrn out_p32765:near,out_p8189:near
extrn puerto204:near,puerto205:near,puerto206:near
extrn puerto204_in:near,puerto_in_254:near
extrn in_p65533:near,out_p65533:near,out_p49149:near
extrn si_interrumpir:near
extrn vga_poke_byte:near
extrn codigo0:near

;PUNTEROS A RUTINAS
extrn p_out_32765:WORD,p_out_8189:WORD,p_in_255:WORD


if1
  include macros.mac
endif

codigo  SEGMENT BYTE PUBLIC

ASSUME CS:codigo,DS:codigo

;include codsinpr.inc
;include cod237pr.inc
;include cod203pr.inc
include cod221pr.inc
include cod253pr.inc
include codxycb.inc

codigo   ENDS
			END
