#Makefile tabajo final Fundamentos de Internet
#Manuel Lorente Almán (manloralm)
EJECUTABLE=clientedns3
OBJETOS=clientedns3.o funciones.o
OPCIONES= -g -W -Wall 
COMPILADOR=gcc

todo:	$(EJECUTABLE) clean

$(EJECUTABLE):$(OBJETOS)
	$(COMPILADOR) $(OPCIONES) -o $(EJECUTABLE) $(OBJETOS)

clientedns3.o: clientedns3.c funciones.c tcp.h
	$(COMPILADOR) $(OPCIONES) -c *.c

clean: 
	rm *.o
