# This makefile builds the Chapter 9 examples
# ===========================================

EXECS = tnp1 tnp2 fibonacci fibonacci2 fibonacci3 addvec

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning ibrary paths
# to find libvath
# """""""""""""""""""""""""""""""""""""""

LBASE  = -lpthread
PATHV = -L../vath_tiago

include makefile-compiler

# ---------- < Book examples > ----------------------
 
tnp1:   TNPool1.o
	${CXX} -otnp1 TNPool1.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tnp2:   TNPool2.o
	${CXX} -otnp2 TNPool2.o ${PATHV} ${LBASE} -lvath
	rm *.o

fibonacci: fibonacci.o
	${CXX} -ofibonacci fibonacci.o ${PATHV} ${LBASE} -lvath
	rm *.o

fibonacci2: fibonacci2.o
	${CXX} -ofibonacci2 fibonacci2.o ${PATHV} ${LBASE} -lvath
	rm *.o

fibonacci3: fibonacci3.o
	${CXX} -ofibonacci3 fibonacci3.o -pthread ${PATHV} ${LBASE} -lvath
	rm *.o

addvec:  AddVectors.o
	${CXX} -oaddvec AddVectors.o ${PATHV} ${LBASE} ${LTBB} -lvath 
	rm *.o



#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS}
	rm *.o

allbook:  ${EXECS}

.PHONY: runbook
runbook:
	./tnp1 
	./tnp2
	./addvec
	./fibonacci
	./fibonacci2
	./fibonacci3

