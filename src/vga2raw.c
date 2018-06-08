#include <stdio.h>
#include <stdlib.h>

unsigned char buffer_rgb[3];


/* Paletas VGA en 6 bit, Paleta archivo raw 8 bit, multiplicar por 4*/
#define BRI0      (42+5)*4
#define BRI1      (16)*4

/* Tabla para los colores reales */

unsigned char tabla_colores[]={
/*                 RED       GREEN     BLUE                 G R B */
    		0,	       0,        0,			/* 0 En SP: 0 0 0 Black */
				    0,        0,        BRI0,      /* 1        0 0 1 Blue */
				    BRI0,     0,		  0,         /* 2        0 1 0 Red */
						BRI0,	    0,		  BRI0,      /* 3        0 1 1 Magenta */
				    0,		    BRI0,	  0,			/* 4        1 0 0 Green */
						0,		    BRI0,	  BRI0,		/* 5        1 0 1 Cyan */
						BRI0,	    BRI0,	  0,			/* 6        1 1 0 Yellow */
						BRI0,	    BRI0,	  BRI0,		/* 7        1 1 1 White */




/*With brightness */

			0,	       0,        0,			/* 0        0 0 0 Black */
				    0,        0,        BRI0+BRI1, /* 1        0 0 1 Blue */
				    BRI0+BRI1,0,		  0,         /* 2        0 1 0 Red */
						BRI0+BRI1,0,		  BRI0+BRI1, /* 3        0 1 1 Magenta */
				    0,		    BRI0+BRI1,0,			/* 4        1 0 0 Green */
						0,		    BRI0+BRI1,BRI0+BRI1,	/* 5        1 0 1 Cyan */
						BRI0+BRI1,BRI0+BRI1,0,			/* 6        1 1 0 Yellow */
						BRI0+BRI1,BRI0+BRI1,BRI0+BRI1,	/* 7        1 1 1 White */

/*Menu
tinta_menu	db		bri0+bri1,bri0+bri1,0    		;Color de la tinta
				db		0,			 0,		  bri0  		;Color del papel  */
BRI0+BRI1,BRI0+BRI1,0,
0,        0,        BRI0

};
/*mplayer -vo x11 -demuxer rawvideo -rawvideo fps=2:w=320:h=200:format=bgr24 video_conv.raw      

    mplayer  -demuxer rawvideo -rawvideo fps=2:w=320:h=200:format=bgr24 video_conv.raw -audiofile toi.snd -audio-demuxer 20 -rawaudio channels=2:rate=15550:samplesize=1

mencoder  -demuxer rawvideo -rawvideo fps=10:w=320:h=200:format=bgr24 video_conv.raw -audiofile toi.snd -audio-demuxer 20 -rawaudio channels=2:rate=15550:samplesize=1        -ovc lavc -lavcopts vcodec=msmpeg4:vbitrate=8000 -oac copy -o final.avi

					;;mov	ax,50*2 ;Frecuencia siempre sync_lineas*50*2
					;el *2 viene pq es estereo
					
					;15550 o 15600 Hz reales

sync_lineas_128  			equ	311 ;Lineas totales -> 15550
sync_lineas_48				equ	312 ;Lineas totales -> 15600 
*/

convertir_paleta(int valor)
{

unsigned char valor_r,valor_g,valor_b;

/*Menu colours*/
if (valor>=9 && valor <=10) valor+=7;

/*Input pallette: 0-7  64-71 */
if (valor>=64) valor=valor-64+8;

valor_r=tabla_colores[valor*3];
valor_g=tabla_colores[valor*3+1];
valor_b=tabla_colores[valor*3+2];


buffer_rgb[0]=valor_b;
buffer_rgb[1]=valor_g;
buffer_rgb[2]=valor_r;

}

int main (int argc,char *argv[])
{

  int valor;
  FILE *fichero_in,*fichero_out;

	if (argc!=3) {
		printf ("VGA2RAW : Utility to convert .VGA video files to RAWBGR 24 bit files\n"
					"Syntax: %s input output\n",argv[0]);
		return 1;
	}




  if ((fichero_in=fopen(argv[1],"rb"))==NULL) {
    printf ("Error opening input file\n");
    return 1;
  }

  if ((fichero_out=fopen(argv[2],"w"))==NULL) {
    printf ("Error opening output file\n");
    return 1;
  }


  while (!feof(fichero_in)) {
    valor=fgetc(fichero_in);

	convertir_paleta(valor);
   fwrite( &buffer_rgb, 1, 3, fichero_out);
	 

  }

fclose(fichero_in);
fclose(fichero_out);

}
