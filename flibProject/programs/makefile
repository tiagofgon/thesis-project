VPATH = src

EXECS =  tnp1 \
		 fibonacci_async fibonacci_1 fibonacci_2 \
		 heat feach spawn1 spawn2 spawn3

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
LBASE  = -lpthread

PATHV = -L../flib


include makefile-compiler

# """"""""""""""""""
# My codes
# """""""""""""""""" 
tnp1:   TNPool1.o
	${CXX} -otnp1 TNPool1.o ${PATHV} ${LBASE} -lflib 
	rm TNPool1.o

fibonacci_async: fibonacci_async.o
	${CXX} -ofibonacci_async fibonacci_async.o ${PATHV} ${LBASE} -lflib
	rm fibonacci_async.o

fibonacci_1: fibonacci_1.o
	${CXX} -ofibonacci_1 fibonacci_1.o -pthread ${PATHV} ${LBASE} -lflib
	rm fibonacci_1.o

fibonacci_2: fibonacci_2.o
	${CXX} -ofibonacci_2 fibonacci_2.o -pthread ${PATHV} ${LBASE} -lflib
	rm fibonacci_2.o

heat:  heat.o HeatAux.o
	${CXX} -oheat heat.o HeatAux.o ${PATHV} ${LBASE} -lflib -ltbb 
	rm heat.o

addvec:  AddVectors.o
	${CXX} -oaddvec AddVectors.o ${PATHV} ${LBASE} -lflib 
	rm AddVectors.o

feach:  Foreach.o
	${CXX} -ofeach Foreach.o ${PATHV} ${LBASE} -lflib
	rm Foreach.o

spawn1:  Spawn1.o
	${CXX} -ospawn1 Spawn1.o ${PATHV} ${LBASE} -lflib
	rm Spawn1.o

spawn2:  Spawn2.o
	${CXX} -ospawn2 Spawn2.o ${PATHV} ${LBASE} -lflib
	rm Spawn2.o

spawn3:  Spawn3.o
	${CXX} -ospawn3 Spawn3.o ${PATHV} ${LBASE} -lflib
	rm Spawn3.o

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""
clean:
	rm -f ${EXECS}
	rm *.o

all:    ${EXECS}

