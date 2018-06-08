COMMENT !
Seccion 39
lnformacion de refereneia
Temas tratados:
Hardware
El +2A est dise~ado en torno al microprocesador Z80, el cual funciona a una velocid~l
de 3.5469 MHz (unos tres millones y medio de ciclos por segundo).
La memoria del +2A est dividida en 64K de ROM y 12&K de RAM y distribuida en p~gl~
nas de 16K. A las cuatro p~ginas de ROM (0-3) se accede a trav s de los 16K inferior
(direcciones 0000h a 3FFFh) del mapa de la memoria. Las ocho p~ginas de RAM (9-7~
son accesibles a traves de los 16K superiores del mapa (direcciones C000h a FFFFh). L~,
p~gina 5 de la RAM tambien puede ser encajada en el margen de 4000h a 7FFFh, y Il
pAgina 2 en el comprendido entre 8000h ~ BFFFh. Ademas, hay varias formas de ocupa'
los 64K con p ginas de RAM; las combinaciones posibles han sido descritas en la Secci~~
24, apartado 'Gesti~n de la memoria'.
Fisicamente hablando, las ROM se encuentran n dos dispositivos de 32K cada uno ( imi~
lares al 27256) ~ue el sistema trata como si fuesen dos circuitos de 16K, La RAM cst~
compuesta por dieciseis circuitos de 64K ~~ 4 bits (41464), algunos de l s ~uales (bano~ i
RAM4 RAM7) est~n compartidos entre los circuitos que producen la i agen de pantal~
y el Z80A. Los otros circuitos (bancos RAMO a RAM3), asi como la~ROM, son de ~~
exclusivo del Z80A.
El eistema de compartici~n de RAM (cntrc los circuitos de video y el p oce ador) funcio~,'
del siguiente modo:
- Cada cuadro de TV consta de 311 lineas.
- De ellas, solo hay conflicto en 192.
- Cada linea de TV dura 228 estados T.
- De estos, s~1o hay conflicto en 128 estados T.
- Durante estos 128 estados T, al procesador solo se le permite usar 1 de cada ; l~
otros 7 est~n dedicados al controlador de video.
El e cto global es una reducci~n de la frecuencia de 4 a 2.66 MHz.
La matriz 16gica no comprometida (ULA, Uncommi~ted Logic Array) controla la mayor
parte de las operaciones de E/S: teclado, interfaz del magnet~fono, pantalla y partc ~
interfaz de la impresora. Convierte los bvtes que se encuentran en la memoria en form~:
y colores para la pantalla ~ permite a~ ZSOA explorar el teclado y leer y escribir,los dat~
en la cinta.
308
Capitulo 8. Guia de programaci~n en +3 BASIC
I
El sonido en tres canales es producido por el AY-3-8912, un cir~uito de sonido muy popu-
lar; este dispositivo controla tambi n las puertas RS232/MIDI y AUX.
Las dos Puertas serie so1o pueden ser controladas por programa. El +2A no incluye soft-
ware para el ~ontrol de la puerta AUX, el cual deberi ser ~estionado por e~ pr~~rama de
usuario. En cambio, la puerta RS23~/MIDI es controlada Plenamentc por +3 BASIC.
La manera en ue ~rabaja el AY-3-8912 es bastante ~ompleja; se recomienda a quienes
~ sientan tentados a experimentar que consulten la ho~a de datos del AY-3-8912. No obs-
tante, la siguiente informaci~n deberia ser suficiente para empezar.
El circuito de sonido contiene dieciseis registros; para seleccionarlos, primero se escribe
el numero de registro en la puerta de escritura de direcciones, FFFDh (65533), y despues
lee el valor del registro (en la misma direccion) o se escribe en la direccion de escritura
de registros de datos, BFFDh (49149). Una vez seleccionado un registro, se puede realizar
cualquier numero de operaciones de lectura o escritura de datos. S~1o habr~ que volver
escribir en la puerta de escritura de direcciones cuando se necesite seleccio~ar otro registro.
La frecuencia de reloj basica de este circuito es 1.7734 MHz (con precisi~n del 0.01~~o).
Los registros hacen lo siguiente:
RO � Ajuste fino del tono, canal A
R1 � Ajuste aproximado del tono, canal A-
R2 � Ajuste fino del tono, canal B
R3 � Ajuste aproximado del tono, canal B
R4 � Ajuste fino del tono, canal C
R5 � Ajuste aproximado del tono, canal C

El tono de cada canal es un valor de 12 bits que se forma combinando los bits D3-DO
del registro de ajuste aproximado y los bits D7-DO del registro de ajuste fino. La uni-
dad b~sica del tono es la frecuencia de reloj ~ividida por 16 (es decir, 110.83 KHz).
Como el contador es de 12 bits, se puede g ~erar frecuencias de 27 Hz a 110 KHz.
R6 � Control del generador de ruido, D4-DO
El periodo del generador de ruido se toma contando los cinco bits inferiores del regis-
tro de ruido cada periodo del reloj de sonido dividido por 16.
R7 � Control del mezclador y de E/S
D7 No utilizado
D6 1=puerta de entrada, 0=puerta de salida
D5 Ruido en el canal C
D4 Ruido en el canal B
D3 Ruido en el canal A
D2 Tono en el canal C
D1 Tono en el canal B
DO Tono en el canal A
Seccion 30. Informaci~n de referencia
309
Este registro controla la mezcla de ruido y tono para cada canal y la direc i~n
puerta de E/S de o~ho bits. Un cero en un bit de mezcla indica que la funci~n
a~tivada.
R8 � Control de amplitud del ca~al A
R9 � Control de amplitud del canal B
RA � Control de amplitud del canal C
D4
l=utilizar generador de envolvente
O=utilizar el valor de D3-DO como amplitud
D3-DO Amplitud
Estos tres registros controlan la amplitud de cada canal y si esta debe ser modul~h
o no por los registros de envolvente.
RB � Ajuste aproximado del periodo de envolvente
RC � Ajuste fino del periodo de envolvente
Los valores de ocho bits de RB y RC se combinan para producir un numero de 1~
bits que se cuenta en unidades de 256 por el periodo del relo~ de sonido. La~ frecuea-
cias de envolvente Pueden estar entre 0.1 Hz 6 KHz.
RD � Control de envolventes
D3 Continua
D2 Ataque
Dl Alternada
DO Sostenida
El diagrama de las formas de envolvente (Seccion 19 de este capitulo) da una ilustraci~n
gr
fica de los posibles estados de este registro.
Si est~ conectada la unidad de disquete externa, su control lo realiza el circuito controla-
dor ~PD765A del interfaz externo. Tal como explicamos en la Secci~n 23, el registro dc
datos de este di~positivn est en la direcci~n 3FFDh (16381); el re~istro de estado se en-
cuentra en 2FFDh (12285). Est~ dispositivo es muy complejo, por lo ue no se debe expe-
. rimenta~ con l si no se conoce perfectamente su funcionamiento (consultese la ho a de
datos del fabricante).
La puerta de impresora paralelo (Centronics) es basicamente un 'latch' de 8 bits (74273)
cuya direcci~n es OFFDh (4093). La se~al STROBE para la impresora es generada Por la
ULA, Y est~ accesible en el bit 4 dc la direc~.i~n 1FFDh (8189). El estado de la senal BUSY
procedente de la impresora puede ser leido en el bit 0 de la direccion OFFDh (4093).
310
Capitulo 8. Guia de programacion en +3 BASIC
!

PUBLIC in_p65533,out_p65533,out_p49149


;valor_2345678	dd		2345678

ay_Reg 			db		?  ;
ay_Val  			db		?  ;
ay_Tono        dw		?  ;
ay_temp			db		?  ;Variables temporales




FRECUENCIA_AY	equ 1773400 ;1'7734*1000000 (Hz)
;FRECUENCIA_AY_L equ FRECUENCIA_AY MOD 65536
;FRECUENCIA_AY_H equ FRECUENCIA_AY / 65536
FRECUENCIA_AY_L equ 3928
FRECUENCIA_AY_H equ 27

FRECUENCIA_NOISE_AY  equ   886700
FRECUENCIA_NOISE_AY_L  equ 34732
FRECUENCIA_NOISE_AY_H equ 13

registro_ampl		db		+1 ;AMPLITUD_AY
registro_contador	dw 0; cuando pasa de FRECUENCIA_BLASTER, invertir registro_ampl
registro_volumen db ?

registro_ampl1	db +1 ;AMPLITUD_AY
registro_ampl2	db +1 ;AMPLITUD_AY
registro_ampl3	db +1 ;AMPLITUD_AY
registro_contador1	dw 0
registro_contador2	dw 0
registro_contador3	dw 0
;registro_canalABC db ? ;Indica que canal se est� calculando
;0=A, 1=B, 2=C
registro_mascara db 0 ; mascara 0000X00Y -> X noise canal Y tone canal
registro_noise db +1
registro_contador_noise dw 0
randomize_noise dw ?
registro_beeper db ?
registro_canal_a db ?
registro_canal_b db ?
registro_canal_c db ?
;Valor de los canales a,b,c. Mezclar posteriormente con beeper y modo stereo


volumes        db 0,0,1,1
               db 1,2,2,3
               db 4,6,8,10
               db 14,16,20,24


in_p65533:

					test	bits_estado4,00000010b
					jz		in_p65533_si

					mov	al,255
					ret

in_p65533_si:

					mov	bx,offset ay_3_8912_registros
					mov	al,puerto_65533
					and	al,00001111b
					xlat
					ret

out_p65533:

					test	bits_estado4,00000010b
					jnz	out_p65533_fin

					mov	puerto_65533,bl
out_p65533_fin:

					ret

out_p49149:
					;Mirar si existe el chip AY
					test	bits_estado4,00000010b
					jnz   out_p49149_no

					;Existe el chip AY

					mov	puerto_49149,bl

					mov	cl,bl
					mov	bx,offset ay_3_8912_registros
					mov	al,puerto_65533
					and	al,00001111b
					xor	ah,ah
					add	bx,ax
					mov   byte ptr [bx],cl
					ret


out_p49149_no:

					ret





da_amplitud_ay:
					;Entrada: AL,AH=Canales (se tiene el valor del beeper)
					;Salida: AL,AH=Canales (valores del beeper mezclados con chip AY)

					;Comprovar si hay chip AY
					;Calcular valores (onda cuadrada?) para los 3 canales
					;Y ubicarlos en izquierda y/o derecha, segun modo de stereo
					
					;Si solo hay beeper, o la salida del AY va a nada, los dos canales seran iguales
					mov ah,al
					
					test  bits_estado4,00000010b
					jz    da_amplitud_ay0
					;Solo hay beeper.
          ret

da_amplitud_ay0:

					test	bits_estado4,00010000b  ;Mirar salida a Sblaster
					jz    da_amplitud_ay00
					;La salida del chip AY va a nada
					
					ret
da_amplitud_ay00:

          ;Guardar beeper
          mov registro_beeper,al
          
					push	cx
					push	bx
					push dx
					
					call genera_noise

          ;Canal A
					mov cl,byte ptr [ay_3_8912_registros+7]
					and cl,00001001b
					mov registro_mascara,cl

					mov	bx,offset ay_3_8912_registros
					mov	cl,registro_ampl1
					mov	registro_ampl,cl
					mov	cx,registro_contador1
					mov	registro_contador,cx
					
					mov   cl,byte ptr [ay_3_8912_registros+8]

			    mov   registro_volumen,cl

					call	da_amplitud_ay1
					mov registro_canal_a,al
					mov	cl,registro_ampl
					mov	registro_ampl1,cl
					mov	cx,registro_contador
					mov	registro_contador1,cx
da_amplitud_ay_A:					

          ;Canal B
					mov cl,byte ptr [ay_3_8912_registros+7]
					shr cl,1
					and cl,00001001b
					mov registro_mascara,cl


					mov	bx,offset ay_3_8912_registros+2
					mov	cl,registro_ampl2
					mov	registro_ampl,cl
					mov	cx,registro_contador2
					mov	registro_contador,cx

					mov   cl,byte ptr [ay_3_8912_registros+9]

			    mov   registro_volumen,cl
					
					call	da_amplitud_ay1
					mov registro_canal_b,al
					mov	cl,registro_ampl
					mov	registro_ampl2,cl
					mov	cx,registro_contador
					mov	registro_contador2,cx

da_amplitud_ay_B:					

          ;Canal C
 					mov cl,byte ptr [ay_3_8912_registros+7]
					shr cl,1
					shr cl,1
					and cl,00001001b
					mov registro_mascara,cl

					mov	bx,offset ay_3_8912_registros+4
					mov	cl,registro_ampl3
					mov	registro_ampl,cl
					mov	cx,registro_contador3
					mov	registro_contador,cx

					mov   cl,byte ptr [ay_3_8912_registros+10]

			    mov   registro_volumen,cl

					call	da_amplitud_ay1
					mov registro_canal_c,al
					mov	cl,registro_ampl
					mov	registro_ampl3,cl
					mov	cx,registro_contador
					mov	registro_contador3,cx
					
da_amplitud_ay_C:					

          pop dx
					pop 	bx
					pop	cx

          ;Tener en cuenta modo estereo
;bits_estado7	db		0+8
COMMENT !
  Byte de estado no 7. Codificaci�n:
  bit
  ---
  1-0 Indica el modo de stereo del chip AY:
	  0=Mono
	  1=ACB Stereo (Canal A=Izq,Canal C=Centro,Canal B=Der)
	  2=ABC Stereo (Canal A=Izq,Canal B=Centro,Canal C=Der)
!
          mov al,bits_estado7
          and al,00000011b
          cmp al,0
          jnz da_amplitud_ay_nomono
          ;Modo Mono. Todos canales iguales
          mov al,registro_beeper
          add al,registro_canal_a
          add al,registro_canal_b
          add al,registro_canal_c
          mov ah,al
          ret
          
da_amplitud_ay_nomono:
          cmp al,1
          jnz da_amplitud_ay_noacb
          mov al,registro_canal_a
          add al,registro_beeper
          add al,registro_canal_c
          mov ah,registro_canal_b
          add ah,registro_beeper
          add ah,registro_canal_c
          ret

          
          
da_amplitud_ay_noacb:
          mov al,registro_canal_a
          add al,registro_beeper
          add al,registro_canal_b
          mov ah,registro_canal_c
          add ah,registro_beeper
          add ah,registro_canal_b

					ret
					
					
					
da_amplitud_ay1:
;Entrada: BX offset al valor de tono   , registro_contador, registro_volumen,
;         registro_ampl, registro_canalABC
;Retorna en AL valores canales.
					push	bx	
					push	dx
					;push	ax
					
					mov	bx,[bx]
					and	bh,00001111b
					
					
					shl	bx,1
					shl	bx,1
					shl	bx,1
					shl	bx,1
					;BX=BX*16
					
					
					
					cmp	bx,28
					jnc	da_amplitud_ay2
					mov	ax,65535
					jmp	short da_amplitud_ay3
					
da_amplitud_ay2:					
					mov	ax,FRECUENCIA_AY_L
					mov	dx,FRECUENCIA_AY_H
					div	bx

da_amplitud_ay3:
					;en AX=Frecuencia (en Hz)
					mov	bx,ax
					cmp	bx,FRECUENCIA_BLASTER
					jc		da_amplitud_ay4

					mov	bx,FRECUENCIA_BLASTER ;bx=incremento
					jmp	short da_amplitud_ay5
da_amplitud_ay4:
					mov	ax,registro_contador
da_amplitud_ay5:					
					add	ax,bx
					cmp	ax,FRECUENCIA_BLASTER
					jc		da_amplitud_ay6
					neg registro_ampl
					sub	ax,FRECUENCIA_BLASTER
da_amplitud_ay6:
					mov	registro_contador,ax
				
          call  da_volumen
          
					;pop	bx

          ;;;;;;;;Recuperamos AX - Valores del beeper, acumulado AY

					;add	bh,al; registro_ampl
					;add	bl,al; registro_ampl
					;mov	ax,bx
					pop	dx
					pop	bx
					
          ret
          
genera_noise:
;Actualiza el valor de registro_noise adecuadamente (+1 o -1)
;Debe mirar si se ha llegado a la frecuencia indicada. En ese caso,
;generar un numero aleatorio +1 o -1. Si no se ha llegado, no alterar registro_noise

          test bits_estado7,00000100b
          jnz genera_noise1 ;no generar ruido
          
					push ax
					push bx
					push dx

					call incrementa_contador_noise

					pop dx
					pop bx
					pop ax
genera_noise1:
					ret

          
incrementa_contador_noise:

					mov	bl,byte ptr [ay_3_8912_registros+6]
					and	bl,00011111b
					mov bh,0

					shl	bx,1
					shl	bx,1
					shl	bx,1
      		;BX=BX*8

					cmp	bx,14
					jnc incrementa_contador_noise2
					mov	ax,65535
					jmp	short incrementa_contador_noise3

incrementa_contador_noise2:
					mov	ax,FRECUENCIA_NOISE_AY_L
					mov	dx,FRECUENCIA_NOISE_AY_H
					div	bx

incrementa_contador_noise3:
					;en AX=Frecuencia (en Hz)
					mov	bx,ax
					cmp	bx,FRECUENCIA_BLASTER
					jc		incrementa_contador_noise4

					mov	bx,FRECUENCIA_BLASTER ;bx=incremento
					jmp	short incrementa_contador_noise5
incrementa_contador_noise4:
					mov	ax,registro_contador_noise
incrementa_contador_noise5:
					add	ax,bx
					cmp	ax,FRECUENCIA_BLASTER
					jc		incrementa_contador_noise6
					sub	ax,FRECUENCIA_BLASTER
					;Generar 0 o 1 aleatorio neg registro_noise
					mov registro_contador_noise,ax
					call generar_random_noise

incrementa_contador_noise6:
					mov	registro_contador_noise,ax

          ret
					
;Seguimos la misma formula RND del spectrum:
;0..1 -> n=(75*(n+1)-1)/65536
;0..65535 -> n=65536/(75*(n+1)-1)
generar_random_noise:

          mov dx,0
          mov ax,randomize_noise
          inc ax
          jnz generar_random_noise2
          ;Overflow
          inc dx
generar_random_noise2:

          mov bx,75
          mul bx
          
          dec ax
          jnz generar_random_noise3
          
          dec dx
          
generar_random_noise3:


          ;;DX:AX /=65536. Agafar resto de la division
          mov randomize_noise,ax
          
          ;Generar +1 o -1
          ;0..32767 -> +1
          ;32768..65535 -> -1
          
          test ah,10000000b
          jnz generar_random_noise4
          mov registro_noise,+1
          ret
          
generar_random_noise4:

          mov registro_noise,-1
          
          ret

da_volumen:
;Entrada: registro_ampl es el signo +1 o -1, para tono
;registro_mascara es la mascara 0000X00Y : X-noise Y-Tone
;registro_noise es el signo +1 o -1, para ruido
;registro_volumen es el registro de volumen 0..15
;Salida: En AL,amplitud a aplicar (con signo) = -AMPLITUD_AY,...,+AMPLITUD_AY
COMMENT !
    The noise and tone output of a channel is combined in the mixer in the
    following way:

        Output_A = (Tone_A OR Ta) AND (Noise OR Na)

    Here Tone_A is the binary output of tone generator A, and Noise is the
    binary output of the noise generator.  Note that setting both Ta and Na
    to 1 produces a constant 1 as output.  Also note that setting both Ta
    and Na to 0 produces bursts of noise and half-periods of constant
    output 0.
( Tone_a OR 0 ) AND ( Noise_a OR 0 )= Tone_a AND Noise_a
( Tone_a OR 0 ) AND ( Noise_a OR 1 )= Tone_a
( Tone_a OR 1 ) AND ( Noise_a OR 0 )= Noise_a
( Tone_a OR 1 ) AND ( Noise_a OR 1 )= 1
!


               ;Se puede usar bx,dx,ax

               ;En DL Meteremos Tone_a or Ta
               mov dl,registro_mascara
               cmp dl,00001001b
               jnz da_volumen01
               ;Ta=1 Na=1
               mov dh,1
               jmp short da_volumen10
da_volumen01:
               cmp dl,00000001b
               jnz da_volumen02
               ;Ta=1 Na=0
               mov dh,registro_noise
               jmp short da_volumen10

da_volumen02:
               cmp dl,00001000b
               jnz da_volumen03
               ;Ta=0 Na=1
               mov dh,registro_ampl
               jmp short da_volumen10

da_volumen03:
               ;Ta=0 Na=0
               mov dl,registro_noise
               mov dh,registro_ampl
               and dh,dl

da_volumen10:

               ;En DH Tenemos Output_A
               mov   al,registro_volumen
               test  al,16
               jz    da_volumen1
               mov   al,15      ;de momento,pasamos de los envolventes
               ;Si esta activo el generador de envolventes,
               ;el volumen no se tiene en cuenta
               ;de momento, lo fijamos a volumen 15
da_volumen1:
               mov   bx,offset volumes
               xlat

               ;Si es registro_ampl -1, negar

               test  dh,10000000b
               jnz   da_volumen2
               ret
da_volumen2:   neg   al
               ret
					
