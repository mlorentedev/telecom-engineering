/*
**	Fichero: tcp.h	
**	Autor:Manuel Lorente Almán	
**	Fecha: 25-12-2013
**	
**	
**	Descripcion: Fichero de librería donde se exponen las estructuras
**      utilizadas a lo largo del programa con sus respectivos campos,
**      los mensajes de error, constantes utilizadas y el prototipo 
**      y una breve explicacion de las funciones que desarrollan 
**      el programa. 
*/

#ifndef TCP_H
#define TCP_H
/*Mensajes de error y respuesta*/

#define ERROR1 "respuesta incorrecta del servidor %s\n"
#define ERROR2 "respuesta del servidor %s: registro no encontrado\n"
#define ERROR3 "timeout; sin respuesta del servidor %s\n"
#define ERROR4 "numero incorrecto de argumentos en linea de comando\n"
#define ERROR5 "opcion no reconocida\n"
#define ERROR6 "no es posible establecer la conexion\n"
#define ERROR7 "no se encuentra el fichero %s\n"
#define ERROR8 "La maquina con la que contacta no existe\n"
#define ERROR9 "no es posible completar la transferencia\n" 
#define RES1 "Fichero %s subido\n"
#define RES2 "respuesta del servidor %s: %s %s %c\n"

/*Constantes del fichero*/

#define TAM 40                  /* Tamaño de buffer de envio y lineas leidas*/
#define SOCKET_UDP 0            /* Macros de control transferencia de archivos*/
#define SOCKET_TCP 1
#define SOCKET_TCP_PASIVO 2
#define TRUE 1
#define FALSE 0
#define EXIT_ERROR 1
#define PUERTOUDP "55555"       /* Puerto UDP del sistema*/
#define PUERTO "cpr"
#define PLAZO         30        /* Plazo de espera lectura/escritura */
#define TAM_MAX_LINEA 1024      /* Tamanio maximo de linea a leer    */

struct registro{
  char name[TAM];     /* Nombre de la direccion */
  char value[TAM];    /* Direccion ip asociada a la direccion*/
  char type;          /* Tipo del registro a buscar*/
};

typedef struct pdu_rr{
  char req;           /* Bit a rellenar si peticion(0) o respuesta (1)*/
  unsigned int sn;    /* Numero de secuencia del envio*/
  struct registro rr; /* Cuerpo del mensaje a enviar*/
}PDU_RR;

/*Funciones temporizadores encargadas de los tiempos de espera
 en los procesos*/

int recibido_en_plazo(int descriptor, int segundos);
int espera_recepcion(int descriptor, int segundos);
int espera_envio(int descriptor, int segundos);

/*Bateria de funciones para transferencia
 de mensajes*/

int lee(int s, char *buffer, int longitud, int segundos);
int escribe(int s, const char *buffer, int longitud, int segundos);
int envia_mensaje(int s_cliente, char *datos, int len, int segundos);
char *lee_mensaje(int s_cliente, int segundos, int *out_longitud);

/*Conjunto de funciones para el tratamiento de 
  las direcciones de la maquinas*/

int traduce_a_direccion(const char *maquina, const char *puerto,
                        int tipo, struct addrinfo *info);
void imprime_extremo_conexion(const struct sockaddr *direccion,
                              socklen_t len, int tipo);
int inicia_socket_cliente(const char *maquina, const char *puerto);


/*Equipo de funciones que se encargan del 
  mantenimiento del programa,  
  gestiona los errores
  en el caso de que los hubiese*/

void Gestiona_Errores(int error, char *servidor );
void error_fatal(char *mens);
int Elige_opcion(char *opcion);

#endif





