VPATH = src

EXECS =  tnp1 \
		 fibonacci_async fibonacci_1 fibonacci_2 \
		 heatnptfg feach

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
	${CXX} -otnp1 TNPool1.o ${PATHV} ${LBASE} -lvath 
	rm TNPool1.o

fibonacci_async: fibonacci_async.o
	${CXX} -ofibonacci_async fibonacci_async.o ${PATHV} ${LBASE} -lvath
	rm fibonacci_async.o

fibonacci_1: fibonacci_1.o
	${CXX} -ofibonacci_1 fibonacci_1.o -pthread ${PATHV} ${LBASE} -lvath
	rm fibonacci_1.o

fibonacci_2: fibonacci_2.o
	${CXX} -ofibonacci_2 fibonacci_2.o -pthread ${PATHV} ${LBASE} -lvath
	rm fibonacci_2.o

heatnptfg:  HeatNPtfg.o HeatAux.o
	${CXX} -oheatnptfg HeatNPtfg.o HeatAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm HeatNPtfg.o

addvec:  AddVectors.o
	${CXX} -oaddvec AddVectors.o ${PATHV} ${LBASE} -lvath 
	rm AddVectors.o

feach:  Foreach.o
	${CXX} -ofeach Foreach.o ${PATHV} ${LBASE} -lvath
	rm Foreach.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""
clean:
	rm -f ${EXECS}
	rm *.o

all:    ${EXECS}

