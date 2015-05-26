complier=arm-linux-gcc
#complier=gcc

AutoMachined:AutoMachined.o Init.o Control.o GetSensorData.o
	${complier} -o AutoMachined Init.o AutoMachined.o Control.o GetSensorData.o
Init.o:
	${complier} -o Init.o -c Init.c
AutoMachined.o:
	${complier} -o AutoMachined.o -c AutoMachined.c

Control.o:
	${complier} -o Control.o -c Control.c
GetSensorData.o:
	${complier} -o GetSensorData.o	-c GetSensorData.c

clean:
	rm AutoMachined
	rm *.o


