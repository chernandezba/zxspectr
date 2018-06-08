//Informacion referente a DMA extraida de sblast2.pas

#include <dos.h>
#include <stdio.h>  
#include <conio.h>
#include <stdlib.h>

#define IRQ_INT 	8+irq

unsigned int segmento;
//unsigned char listo=0,byte_leido;
unsigned char fin=0;                  
unsigned long bytes_escritos=0;
unsigned int bytes=32000-1;  //Lee +1 byte

unsigned long total=0;

char bloque=0;

char fichero[256];
FILE *fic;
char *variable_blaster;  

void interrupt ( *oldhandler)(__CPPARGS);

int puerto_sb=0x220;
int dma_ch=1,irq=5;
int dma_page,dma_address,dma_lenght;

struct DMA_DATA {
  int page;
  int address;
  int lenght;
} dma_data[4]=
{
{0x87,0,1},
{0x83,2,3},
{0x81,4,5},
{0x82,6,7}

};

unsigned int frecuencia=11111;
unsigned int baudios=1500;

int dsp_reset,dsp_read_data,
	 dsp_write,dsp_data_available,
	 ct_index,ct_data;

unsigned char read_dsp(void)
{
  while (!(inportb(dsp_data_available) & 0x80));
  return inportb(dsp_read_data);
}

void write_dsp(unsigned char valor)
{
  while (inportb(dsp_data_available) & 0x80);
  outportb(dsp_write,valor);
}


void inicializa_dma(void)
{                                    
  unsigned long direccion;


  direccion=(unsigned long)segmento*16;         
                                           
  outportb(0x0a,dma_ch+4);
  outportb(0x0c,0);                      
  outportb(0x0b,0x44 + dma_ch); //0x45=grabacion
  outportb(dma_data[dma_ch].address,direccion & 0xFF);
  outportb(dma_data[dma_ch].address,(direccion & 0xFF00L)>> 8);
  outportb(dma_data[dma_ch].page,(direccion & 0xF0000L) >> 16);
  outportb(dma_data[dma_ch].lenght,bytes & 0xFF);
  outportb(dma_data[dma_ch].lenght,(bytes & 0xFF00L)>> 8);
  outportb(0x0a,dma_ch); //Activar canal DMA

}

void inicializa_frecuencia(void)
{
  unsigned char valor;

  valor=256-1000000L/frecuencia;
  //printf ("Frecuencia real:%u\n",(unsigned int)(1000000L/(256-valor)));
  write_dsp(0x40); 
  write_dsp(valor & 0xFF);
}

void inicializa_longitud_datos(void)
{
  write_dsp(0x24);  //0x24      0x14???
  
  write_dsp(bytes & 0xFF);
  write_dsp((bytes & 0xFF00L)>> 8);
  /*
							  ³ D0h    DMA Stop         ³
							³ D1h    Turn speaker on  ³
							³ D3h    Turn speaker off ³
							³ D4h    DMA Continue     ³
  */
  //write_dsp(0xd4);
}



unsigned char read_dac(void)
{
  write_dsp(0x20);
  return read_dsp();
}


void interrupt interrupcion(void)
{

  inportb(dsp_data_available);

		if (!bloque) {
		  segmento+=32768L/16;
		  bloque=1;
		}
		else {
		  segmento-=32768L/16;
		  bloque=0;
		}


		inicializa_dma();
				//printf ("Inicializando DMA\n");
	 
	 
		//printf ("Estableciendo longitud de datos\n");

		inicializa_longitud_datos();

  
  fin=1;
  
  outportb(0x20,0x20);
  
}

void inicializa_irq(void)
{

  oldhandler = getvect(IRQ_INT);
  setvect(IRQ_INT, interrupcion);
  
}

void restaura_irq(void)
{
  setvect(IRQ_INT, oldhandler);
}



void ct_write(unsigned char index,unsigned char valor)
{
  outportb(ct_index,index);
  outportb(ct_data,valor);
}

void inicializa_puertos(void)
//Inicializa valores de puertos
{
  dsp_reset=0x6+puerto_sb;
  dsp_read_data=0xA+puerto_sb;
  dsp_write=0xC+puerto_sb;
  dsp_data_available=0xE+puerto_sb;
  ct_index=0x4+puerto_sb;
  ct_data=0x5+puerto_sb;
  
}

int inicializa_sonido(void)
{

  //Inicializar DSP
  outportb(dsp_reset,1);
  delay(10); //Esperar mas de lo exigido
  outportb(dsp_reset,0);
  delay(10);
  
  if (!((inportb(dsp_data_available)&0x80)==0x80 && 
		  inportb(dsp_read_data)==0xAA)) {
	 return -1;
  }

  //Inicializar Mezclador CT 1345
  ct_write(0,0);                               
  
  //Seleccionar entrada de linea y filtro alto
  ct_write(0xc,2+4+8);
  
  //Seleccionar volumen de entrada maximo
  ct_write(0x2e,255);

  return 0;
}

void fin_no_blaster(void) 
{

  printf ("\nSound Blaster no detectada!\a\n");
  exit(0);

}


void main(void)
{

  unsigned char far *p0;
  int tecla;
  long n;
  unsigned char freq;


  char grabar=0;
  char tecla_pulsada=0;


  printf ("LINEASMP V1.0\n"       
			 "(c) Cesar Hernandez Ba\xa4o (17/11/1998)\n\n"
			 "Extension del programa ZXSPECTR para leer datos desde casette\n"
			 "y convertirlos en ficheros .SMP\n\n");

  if ( (variable_blaster=getenv("BLASTER"))==NULL   )
	 fin_no_blaster();

  //Formato variable :
  //A220 I5 D1
  //0123456789

  puerto_sb=0x200+0x10*(variable_blaster[2]-'0');
  irq=variable_blaster[6]-'0';
  dma_ch=variable_blaster[9]-'0';

  inicializa_puertos();
  //printf ("\nInicializando Sound Blaster....\n");
  if (inicializa_sonido()==-1) fin_no_blaster();

  printf ("Sound Blaster detectada: A%x I%u D%u\n\n",puerto_sb,irq,dma_ch);
	 
  printf ("La velocidad de lectura es de %u Baudios\n"
			 "Desea cambiarla ? (S/N) ?",baudios);
  tecla=getche();                  
  printf ("\n");
  if (tecla=='S' || tecla=='s') {
	 do {
		printf("\nIntroduce velocidad: (550..8000)");
		scanf("%u",&baudios);
	 } while (baudios<550 || baudios>8000);

	 frecuencia=(unsigned int)((baudios*11111.0)/1500.0);
	 freq=256-1000000L/frecuencia;
	 printf ("Velocidad real:%u Baudios\n",(unsigned int) ( ( (1000000.0/(256.0-freq)) /11111.0)*1500.0));

  }

  printf ("Introduce el nombre del fichero a grabar:");
  scanf("%s",fichero);

  if ((fic=fopen(fichero,"wb")) == NULL) {
	 printf ("Error al abrir fichero!\a\n");
	 exit(0);
  }


  //printf ("\nAsignando 64k de memoria\n");

  if (_dos_allocmem(65536L/16, &segmento)) {
	 printf ("No hay memoria!\a\n");
	 exit(0);
  } 
											 
  
  printf ("Pulsa una tecla para empezar a leer datos...\n");
  getch();

  disable();
  //printf ("Cambiando vector de interrupcion %0x\n",IRQ_INT);
  inicializa_irq();

			//Set_Frequence(Frate);
	//GetIntVec($8+DSP_Irq,SBIntSave);
	//SetIntVec($8+DSP_Irq,@SBInt);
	//Port[$21]:=Port[$21] and not (1 shl DSP_Irq);
	//wr_dsp($D0);
	//Start_DMA_TRANSFER(a);

				//printf ("Estableciendo frecuencia a %uHz\n",frecuencia);
		inicializa_frecuencia();


		outportb(0x21,inportb(0x21) & (!(1 << irq)));
		write_dsp(0xd0);

		inicializa_dma();
				//printf ("Inicializando DMA\n");
	 
	 
		//printf ("Estableciendo longitud de datos\n");

		inicializa_longitud_datos();

			


  
  enable();
  printf ("Leyendo datos...\n");
  do {

  
	 if (!tecla_pulsada) {


	 }

	 if (grabar) {
		for (n=0;n<=bytes;n++) p0[n] -=128;
		total +=bytes+1;
		printf ("Leidos %lu bytes \r",total);
		fwrite(p0,bytes+1,1,fic);
	 }

	 p0=segmento*65536L;
	 
	 if (!tecla_pulsada) {
		while (!fin);

		fin=0;


		grabar=1;

		if (kbhit()) tecla_pulsada=1;	 
	 }
	 else break;
	 
  } while (1);

  printf ("\nLectura completada\n");

  disable();

  write_dsp(0xd0);
  outportb(0x0a,dma_ch+4);
  outportb(0x21,inportb(0x21) | (1 << irq));
  outportb(0x20,0x20);
  
/*
Procedure Stop_Playing;
begin
 if psound then begin
	Stop_DMA;
	Port[DMA_MAsk_Reg]:=DMA_CH+4;
	Port[$21]:=Port[$21] or (1 shl DSP_Irq);
	Port[$20]:=$20;
	SetIntVec($8+ DSP_Irq,SBIntSave);
 end;
end;*/


//  restaura_frecuencia();
  //printf ("Restaurando vector de interrupcion %0x\n",IRQ_INT);
  restaura_irq();
  enable();
//  fwrite((unsigned char far *)(segmento*16L+32768L),32768L,1,fic);
  
  fclose(fic);
  //printf ("Liberando memoria\n");
  _dos_freemem(segmento);

}
