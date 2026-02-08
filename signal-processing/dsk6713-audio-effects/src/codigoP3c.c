/*Código base para la práctica 3*/

#define RIGHT 0
#define LEFT 1
#define TAMBUFFER 7000
#define TAMBUFFERCORO1 180
#define TAMBUFFERCORO2 340
#include "lab3ccfg.h" /*Colocar aquí el nombre de la cabecera derivada del archivo .cdb*/
#include <csl.h>
#include <csl_irq.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h" 
#include "dsk6713_dip.h"

 float sine_table[32]={0,1.95,3.83,5.56,7.07,8.31,9.24,9.81,10,9.81,9.24,8.31,7.07,5.56,3.83,1.95,0,-1.95,-3.83,-5.56,-7.07,-8.31,-9.24,-9.81,-10,-9.81,-9.24,-8.31,-7.07,-5.56,-3.83,-1.95
		};
Uint32 fs=DSK6713_AIC23_FREQ_8KHZ; //frecuencia de muestreo
int canal=RIGHT;
DSK6713_AIC23_CodecHandle hAIC23_handle;
//DSK6713_AIC23_CodecHandle hCodec; // generación del manejador
int bufferinL[TAMBUFFER],bufferinR[TAMBUFFER], coro1L[TAMBUFFERCORO1], coro2L[TAMBUFFERCORO2], coro1R[TAMBUFFERCORO1], coro2R[TAMBUFFERCORO2];
int cont_buffer=0;
int cont_buffer_coro1=0;
int cont_buffer_coro2=0;
Uint32  muestrainL,muestrainR,muestrain,muestraoutL,muestraoutR;
int amplitude=2;
	Int16  OUT_R, OUT_L;
	Uint32 IN_L, IN_R;
	int gain=0;
	int ciclo=0;
	int n=0;

/* Ajustes de configuración del códec */
DSK6713_AIC23_Config config = { 
0x0017, /* 0 DSK6713_AIC23_LEFTINVOL Left line input channel volume */ \
0x0017, /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume */\
0x01f9, /* 2 DSK6713_AIC23_LEFTHPVOL Left channel headphone volume */ \
0x01f9, /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume */ \
0x0011, /* 4 DSK6713_AIC23_ANAPATH Analog audio path control */ \
0x0000, /* 5 DSK6713_AIC23_DIGPATH Digital audio path control */ \
0x0000, /* 6 DSK6713_AIC23_POWERDOWN Power down control */ \
0x0043, /* 7 DSK6713_AIC23_DIGIF Digital audio interface format */ \
0x0001, /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control */ \
0x0001 /* 9 DSK6713_AIC23_DIGACT Digital interface activation */ \
};

void idle(); //asociar a una tarea IDL
void tremolo(); //rutina de interrupción
void procesa(); //rutina de interrupción

void main()
{
	Uint32 aux1;
	DSK6713_DIP_init();
	DSK6713_init();
	hAIC23_handle=DSK6713_AIC23_openCodec(0, &config);
	DSK6713_AIC23_setFreq(hAIC23_handle, fs); 

	IRQ_globalEnable();
	IRQ_enable(IRQ_EVT_RINT1);

	DSK6713_AIC23_read(hAIC23_handle, &aux1); //Hacemos una lectura previa. El valor leído no se utiliza.
}

void idle()
{

	LOG_printf(&LOG0,"Aplicación Ejecutándose");
}

void procesa() /*asociar a HWI_INT12*/
{

	// leemos del codec

	if(canal==RIGHT){

		DSK6713_AIC23_read(hAIC23_handle, &muestrainR);

		muestraoutR=amplitude*coro1R[cont_buffer_coro1] + amplitude*bufferinR[cont_buffer] + amplitude*coro2R[cont_buffer_coro2];		
		coro1R[cont_buffer_coro1]=bufferinR[cont_buffer];
		coro2R[cont_buffer_coro2]=bufferinR[cont_buffer];
		bufferinR[cont_buffer]=muestrainR;

		if (DSK6713_DIP_get(0) == 0){
			DSK6713_AIC23_write(hAIC23_handle, muestrainR);
		}

		if (DSK6713_DIP_get(1) == 0){
			DSK6713_AIC23_write(hAIC23_handle, muestraoutR);
		}
		canal=LEFT;
	}
	else{

		DSK6713_AIC23_read(hAIC23_handle, &muestrainL);

		muestraoutL=amplitude*coro1L[cont_buffer_coro1] + amplitude*bufferinL[cont_buffer] + amplitude*coro2L[cont_buffer_coro2];
		coro1L[cont_buffer_coro1]=bufferinL[cont_buffer];
		coro2L[cont_buffer_coro2]=bufferinL[cont_buffer];
		bufferinL[cont_buffer]=muestrainL;

		if (DSK6713_DIP_get(0) == 0){
			DSK6713_AIC23_write(hAIC23_handle, muestrainL);
		}

		if (DSK6713_DIP_get(1) == 0){
			DSK6713_AIC23_write(hAIC23_handle, muestraoutL);
		}

		if(++cont_buffer_coro2>=TAMBUFFERCORO2) //aumentamos indice de la posicion del buffer 
			cont_buffer_coro2=0;
		if(++cont_buffer_coro1>=TAMBUFFERCORO1)
			cont_buffer_coro1=0;
		if(++cont_buffer>=TAMBUFFER)
			cont_buffer=0;

		canal=RIGHT;

	}
}


void tremolo() /*asociar a HWI_INT13*/
{

	if (DSK6713_DIP_get(2) == 0){
		n=0;
		ciclo=0;
		for (n=0;n<=31;n++){
			for (ciclo=0;ciclo<=5;ciclo++){
				if(canal==RIGHT){
					DSK6713_AIC23_read(hAIC23_handle, &IN_R);

					OUT_R=IN_R;

					gain=sine_table[n]+10;
					//enviamos por los canales con cierta ganancia
					DSK6713_AIC23_write(hAIC23_handle, (OUT_R * gain));
					canal=LEFT;
				}
				else {
					DSK6713_AIC23_read(hAIC23_handle, &IN_L);

					OUT_L=IN_L;

					gain=sine_table[n]+10;
					//enviamos por los canales con cierta ganancia
					DSK6713_AIC23_write(hAIC23_handle, (OUT_L * gain));
					canal=RIGHT;
				}
			}
		}
	}
}

	

