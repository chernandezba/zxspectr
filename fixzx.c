//Este programa arregla los ficheros .ZX antiguos. Debido a un error
//en el reservado3, que estaba definido el dup al reves, o sea:
//db	  255 dup (246) ;Reservado para uso futuro
/*
signatura			db 	"SP" ;Se cambia por "ZX" al grabarlo
long_prog			dw		49152
pos_inicial			dw		16384
reg_c				   db		?
reg_b					db    ?
reg_e					db		?
reg_d					db    ?
reg_l					db		?
reg_h					db		?
reg_f					db		?
reg_a					db		?
reg_ixl				db		?
reg_ixh				db		?
reg_iyl				db		?
reg_iyh                         db              ?

;c_ identifica a c'

reg_c_				db		?
reg_b_				db		?
reg_e_				db		?
reg_d_				db		?
reg_l_				db		?
reg_h_				db		?
reg_f_				db		?
reg_a_				db    ?

reg_r					db		?
reg_i					db		?
reg_sp				dw		?
reg_pc				dw		0 ;Para que si no se cargue ningun programa, se salte
								  ;a la direcci¢n 0

reservado1			dw		0
border				db		0
reservado2			db		0 ;Antes de la version 2 de la cabecera,esto
;era un word y reservado20 no existia.

bits_estado			db		0
  COMMENT !
							7-6     Reservados para uso interno, siempre 0
							5       Estado del Flash: 0 - tinta INK, papel PAPER
															  1 - tinta PAPER, papel INK
							4       Interrupci¢n pendiente de ejecutarse
							3       Reservado para uso futuro
							2       Biestable IFF2 (uso interno)
							1       Modo de interrupci¢n: 0=IM1, 1=IM2
							0       Biestable IFF1 (estado de interrupci¢n):
											0 - Interrupciones desactivadas (DI)
											1 - Interrupciones activadas (EI)
  En este emulador s¢lo se usan el bit 5(estado del flash), el bit 1
  (si se est  en modo IM0 o IM1=0, modo IM2=1), y el bit 0 (DI o EI)
  !
reservado20	db 0

;Lo que sigue a continuacion es la cabecera extendida para los ficheros ZX
offset 38
VERS									equ	2 ;Version de la cabecera ZX

version								db		VERS ;Version de la cabecera ZX
cabecera_velocidad				dw		0
cabecera_dirs_pant				dw		64
;Ya no se usa. Se usaba antiguamente para la version CGA
;y la actualizacion de pantalla. No usar nunca en versiones futuras

cabecera_frecuencia				db		1
;Ya no se usa. Se usaba antiguamente para la version CGA
;y la actualizacion de pantalla. No usar nunca en versiones futuras

cabecera_control_brillo			db		0
cabecera_disparador_defecto	db		25
cabecera_sonido					db		1
cabecera_bits_estado				db		0+2 ;Protector de pantallas activo
;Se guardan varios bits en el fichero ZX.El significado es el siguiente:
COMMENT !
Bit	Significado
---	-----------
7     Contiene el bit 6 del puerto del teclado (Issue 1 o 2)
6		Indica si est  activado el disparador autom tico
5		A 0 indica que el refresco de Flash est  activo, sino a 1
4     A 1 indica que el programa a cargar es de 128k (version 2+)
3     A 1 indica que no es posible cambiar el color del borde mediante
		OUT 211,valor (version 2+)
2     No usado
1	 	A 1 indica que est  habilitado el protector de pantallas
0		Paleta CGA (version 2+)
		;0=Negro,Verde,Rojo,Amarillo
		;1=Cyan,Magenta,Blanco

!

cabecera_puerto_32765	db		0  ;Valor del ultimo OUT al puerto 32765
											;(version 2+)
cabecera_puerto_8189		db		0  ;Valor del ultimo OUT al puerto 8189
											;(version 2+)

cabecera_paginas_actuales db 0,5+4,2+4,0+4
;Contiene para cada parte de memoria la pagina(ROM o RAM) asignada
;(version 2+)

puerto_65533				db		255 ;Valor del ultimo OUT al puerto 65533
;Se implemento a partir de una version avanzada de la version 2. Si se
;lee una version anterior, se leera 255, pues estaba reservado

ay_3_8912_registros		db		16 dup (255) ;Contenido de los registros del
;chip de sonido
;Se implemento a partir de una version avanzada de la version 2. Si se
;lee una version anterior, se leera 255, pues estaba reservado.

reservado3							db	  223 dup (255) ;Reservado para uso futuro
;La longitud añadida a la cabecera extendida es de 256 bytes

COMMENT !

Cuando se graba un fichero ZX los datos estan comprimidos, de la manera
siguiente:
Si un byte se encuentra repetido mas de 4 veces, se graba como:
(221) (221) (byte) (numero de veces)
Si justo antes de la repetici¢n hay un c¢digo 221, se graba como:
(221) (byte a repetir) (221) (221) (bytes a repetir) (numero de veces-1)

*/

#include <stdio.h>
#include <dos.h>

int main(void)
{

	FILE *in;
	int valor,version;
	char cadena[80];
	int n;

	if ((in = fopen(_argv[1], "r+"))
		 == NULL)
	{
		fprintf(stderr, "Cannot open input file.\n");
		return 1;
	}

	fseek(in,38,0); //Ponerse en la version
	version=fgetc(in);
	printf ("Version,%u\n",version);
	fseek(in,48,0); //Ponerse en los datos extendidos (version 2+)
	valor=fgetc(in);
	printf ("Valor: %x\n",valor);
	if (valor!=246) {
	  printf ("\a");
	  exit(0);
	}
	printf ("Poner a 255? ");
	scanf("%s",cadena);
	if (cadena[0]=='s' || cadena[0]=='S') {
	  fseek(in,48,0); //Ponerse en los datos extendidos (version 2+)
	  printf ("Actualizando");
	  for (n=1;n<=246;n++) {
		 fputc(255,in);
	  }
	}
	fseek(in,0,2);
	fclose(in);


}
