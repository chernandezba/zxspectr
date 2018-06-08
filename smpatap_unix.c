#include <stdio.h>
#include <stdlib.h>

#define NO_RUIDO 		2
#define ONDAS_GUIA 	10


char *tipos_fichero[]={
  "Programa",
  "Matriz Num.",
  "Matriz Car.",
  "Bytes",
  "Flag"
};

int ondas_leidas;

unsigned char carry;

unsigned char *memoria;
unsigned char *memoria_original;
unsigned long bytes_leidos;

char fichero[255],fichero_tap[255];

char tono_guia=14,ceros=6,unos=12;

FILE *fic,*fic_tap;
char byte_cambio,cambio=0;
char final_fichero=0;

int da_ascii(int codigo)
{
  return (codigo<127 && codigo>31 ? codigo : '.');
}

int da_abs(int valor)
{
  if (valor>=0) return valor;
  else return -valor;
}

int lee_byte(void)
//Funcion que lee byte del fichero
//Mira si hay un byte de cambio de onda, en cuyo caso lo devuelve
//Pone final_fichero a 1 si se llega al final del fichero
{
  if (cambio) {
	 cambio=0;
	 return byte_cambio;
  }
	 
  else {
	 byte_cambio=fgetc(fic);
  }
	                                
  if (feof(fic)) {
	 final_fichero=1;
	 return;
  }
	 
  return byte_cambio;
}

char da_signo(char valor)
//Devuelve el signo de valor: -1,+1 o 0
{

  if (valor>=0) return 1;
  if (valor<0) return -1;
  //else return 0;  	
}

int lee_onda(unsigned char *longitud,unsigned char *amplitud)
//Funcion que lee una onda completa de sonido
//Da la maxima amplitud (en positivo) y la longitud
//de esa onda
//Devuelve -1 si se llega al final del fichero
{
  char byte,byte_anterior,veces=0;

  *longitud=1;
  *amplitud=0;

  byte_anterior=lee_byte();
  *amplitud=da_abs(byte_anterior);

  
  if (final_fichero) return -1;
  //signo=da_signo(byte_anterior);
  do {
	 byte=lee_byte();
	 if (final_fichero) return -1;
	 
	 if (da_abs(byte)>*amplitud) *amplitud=da_abs(byte);
	 if (da_signo(byte)!=da_signo(byte_anterior)
		  //&& da_abs(byte)>=NO_RUIDO
		  ) {
		if (veces==1) {
		  cambio=1;

		  return 0;
		}
		veces++;
	 }
	 (*longitud)++;
	 byte_anterior=byte;
  } while (1);
}

int dice_bit(char longitud)
//Dice si el bit es 0 o 1 segun su amplitud
//Devuelve -1 si no es un bit aceptado
{
  if (longitud>=ceros-2 && longitud<=ceros+2) return 0;
  if (longitud>=unos-2 && longitud<=unos+2) return 1;
  return -1;
}

int lee_8_bits(void/*char si_longitud_anterior*/)
//Devuelve 8 bits leidos 


//No usado:
//Se puede entrar la longitud anterior leida, si no entrarlo con -1

//Devuelve -1 si se llega al final del fichero 
//Devuelve -2 si se encuentra ruido
//Devuelve -3 si se encuentran datos sin sentido
{
  unsigned char longitud,amplitud;
  char bit;
  int n,byte=0;

  for (n=0;n<8;n++) {
	 /*if (si_longitud_anterior!=-1) {
		longitud=si_longitud_anterior;
		si_longitud_anterior=-1;
		amplitud=10; //Hacer que se entre amplitud_anterior
	 }*/
	 /*else*/
	 if (lee_onda(&longitud,&amplitud)==-1) return -1;
	 
	 if (amplitud<NO_RUIDO) return -2;
	 bit=dice_bit(longitud);
	 if (bit==-1) return -3;

	 byte=byte*2+bit;
  }
  return byte;
}

int pon_tipo(void)
//Escribe tipo de fichero
{
  int n;
  unsigned short *p;
  unsigned char tipo;

  if (bytes_leidos!=19) {
	 printf ("%s:%u\nLongitud: ",
		tipos_fichero[4],*memoria_original);
	 if (bytes_leidos>2) printf ("%u+2\n",bytes_leidos-2);
	 else printf ("%u\n",bytes_leidos);
	 return 0;
  }
  tipo=memoria_original[1];
  printf ("%s:",tipos_fichero[tipo]);
  for (n=0;n<10;n++) putchar(da_ascii(memoria_original[2+n]));
  p=(unsigned short *) &memoria_original[12];
  printf ("\nLongitud:%u\n",*p);
  if (tipo==3) printf ("Inicio:%u\n",p[1]);
  if (!tipo) {
	 printf ("Variables:%u\nAutorun:",*p-p[2]);
	 if (p[1]<32767) printf ("%u\n",p[1]);
	 else printf("Ninguno\n");
  }
}
  

void main(void)
{

  unsigned char amplitud,longitud;
  int byte,byte2,tecla;
  unsigned int n;
  char buffer_pregunta[80];

  printf ("\nSMPATAP V1.0\n"
			 "(c) Cesar Hernandez Bano (10/09/1998),(17/09/2013)\n\n"
			 "Extension del programa ZXSPECTR para convertir ficheros de sonido .SMP\n"
			 "de cinta de ZX Spectrum y convertirlos en ficheros .TAP\n\n");

  printf("\nIntroduce el nombre del fichero SMP:");
  scanf("%s",fichero);
  if ((fic=fopen(fichero,"rb"))==NULL) {
	 printf ("\nError al abrir fichero\a\n");
	 exit(0);
  }

  printf("\nIntroduce el nombre del fichero TAP:");
  scanf("%s",fichero_tap);
  if ((fic_tap=fopen(fichero_tap,"a+b"))==NULL) {
	 printf ("\nError al abrir fichero\a\n");
	 exit(0);
  }

  //Asignar memoria
  if ((memoria_original=(unsigned char *)malloc(65536L))==NULL) {
	 printf ("No hay memoria\a\n");
	 exit(0);
  }
  
  do {
	  carry=0;

	  cambio=0;
	  final_fichero=0;

	  bytes_leidos=0;


	 memoria=memoria_original;
  
	 //Leer unas ondas de tono guia
	 n=0;
	 do {
		if (lee_onda(&longitud,&amplitud)==-1) goto fin;
		if (amplitud<NO_RUIDO || (!(longitud>=tono_guia-2 && longitud<=tono_guia+2))
			 ) {
		  n=0;
		  continue;
		}
		n++;
	 } while (n<ONDAS_GUIA);	 
	 printf ("\n\nLeyendo tono guia...\n");
	 do {
	  if (lee_onda(&longitud,&amplitud)==-1) goto fin;
	 } while (amplitud>=NO_RUIDO && (longitud>=tono_guia-2 && 
				 longitud<=tono_guia+2));
	 //Hay que saber si se esta en mitad o al final de la onda falsa
	 if (longitud>6) { //en mitad de la onda falsa
		cambio=0;
		byte=byte_cambio;
		do {
		  byte2=lee_byte();
		  if (final_fichero) goto fin;
		} while (da_signo(byte)==da_signo(byte2));
	 }
		
	 printf ("Leyendo datos...\n\n");

	 
	 //Despues del tono guia viene una onda falsa, no utilizable,
	 //parecida a un bit 0
  
	 do {
		byte=lee_8_bits(/*-1*/);
		if (byte==-1) goto fin;
		if (byte<0) break;
		*memoria++=byte;
		carry^=byte;
		bytes_leidos++;
	 } while (1);
	 if (carry) {
		printf ("Error de carga!\a\n");
		getchar();
	 }
  
	 if (bytes_leidos) {
		if (!pon_tipo()) {
		  n=bytes_leidos;
		  if (n>80*19) n=80*19;
		  memoria=memoria_original;
		  printf ("\n");
		  for (;n;n--) {
			 putchar(da_ascii(*memoria++));
		  }
		  printf ("\n");
		}
	 }
	 if (bytes_leidos) {
		printf ("\nEscriba N si no desea grabar el bloque ");
		scanf("%s",buffer_pregunta);
		tecla=buffer_pregunta[0];
		if (!(tecla=='n' || tecla=='N')) {
		  fwrite(&bytes_leidos,1,2,fic_tap);
		  fwrite(memoria_original,1,bytes_leidos,fic_tap);
		  if (ferror(fic_tap)) perror("Error:");
		}
	 }
	 else if (bytes_leidos) getchar();

  } while (1);
  
fin:
  fcloseall();
  free(memoria);
  
}  
