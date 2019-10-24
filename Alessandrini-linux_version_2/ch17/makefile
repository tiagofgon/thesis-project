# This makefile builds the Chapter 9 examples
# ===========================================

VPATH = src src/basic  

EXECSX = qsort unbal recursive nest1 mcpirec \
        arec2a arec3 areath 

EXECS = tnp1 tnp2 feach nest sp1 sp2 sp3 tnpnew \
        preorder arec1 arec2 pqsort onegen twogen fibonacci fibonacci2 fibonacci3 addvec stencil

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning ibrary paths
# to find libvath
# """""""""""""""""""""""""""""""""""""""

LBASE  = -lpthread

ifdef STD_CPP11_ENV
PATHV = -L../vath_tiago
else
PATHV = -L../vath_tiago
endif

include makefile-compiler

# ---------- < Book examples > ----------------------
 
tnp1:   TNPool1.o
	${CXX} -std=c++14 -pthread -otnp1 TNPool1.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tnp2:   TNPool2.o
	${CXX} -otnp2 TNPool2.o ${PATHV} ${LBASE} -lvath
	rm *.o

sp1:    Spawn1.o
	${CXX} -osp1 Spawn1.o ${PATHV} ${LBASE} -lvath
	rm *.o

sp2:    Spawn2.o
	${CXX} -osp2 Spawn2.o ${PATHV} ${LBASE} -lvath
	rm *.o

sp3:    Spawn3.o
	${CXX} -osp3 Spawn3.o  ${PATHV} ${LBASE} -lvath
	rm *.o

arec1:  AreaRec1.o
	${CXX} -oarec1 AreaRec1.o  ${PATHV} ${LBASE} -lvath
	rm *.o

arec2:  AreaRec2.o
	${CXX} -oarec2 AreaRec2.o ${PATHV} ${LBASE} -lvath
	rm *.o

feach:  Foreach.o
	${CXX} -ofeach Foreach.o ${PATHV} ${LBASE} -lvath
	rm *.o

nest:   Nested.o
	${CXX} -onest Nested.o  ${PATHV} ${LBASE} -lvath
	rm *.o

pqsort: PQsort.o
	${CXX} -opqsort PQsort.o ${PATHV} ${LBASE} -lvath
	rm *.o

preorder:  Preorder.o TGraph.o
	${CXX} -opreorder Preorder.o TGraph.o ${PATHV} ${LBASE} -lvath -ltbb
	rm *.o

twogen: TwoGauss.o GaussVec.o
	${CXX} -otwogen TwoGauss.o GaussVec.o ${PATHV} ${LBASE} -lvath

onegen: OneGauss.o GaussVec.o
	${CXX} -oonegen OneGauss.o GaussVec.o ${PATHV} ${LBASE} -lvath

tnpnew:   TNPoolnew.o
	${CXX} -otnp2 TNPoolnew.o ${PATHV} ${LBASE} -lvath
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

stencil:  Stencil.o
	${CXX} -o stencil Stencil.o ${PATHV} ${LBASE} ${LTBB} -lvath 
	rm *.o



# ---------- < Additional examples > ----------------------


arec2a: AreaRec2a.o
	${CXX} -oarec2a AreaRec2a.o  ${PATHV} ${LBASE} -lvath
	rm *.o

arec3:  AreaRec3.o
	${CXX} -oarec3 AreaRec3.o  ${PATHV} ${LBASE} -lvath
	rm *.o

areath: AreaThp.o
	${CXX} -oareath AreaThp.o  ${PATHV} ${LBASE} -lvath
	rm *.o

recursive: Recursive.o
	${CXX} -orecursive Recursive.o  ${PATHV} ${LBASE} -lvath
	rm *.o

unbal:  Unbalanced.o
	${CXX} -ounbal Unbalanced.o  ${PATHV} ${LBASE} -lvath
	rm *.o

nest1:  Nested1.o
	${CXX} -onest1 Nested1.o ${PATHV} ${LBASE} -lvath
	rm *.o

qsort:  Qsort.o
	${CXX} -oqsort Qsort.o 
	rm *.o

mcpirec:  McpiRec.o
	${CXX} -omcpirec McpiRec.o  ${PATHV} ${LBASE} -lvath
	rm *.o

# """"""""""""""""""""""""""""""""""""""""""""""""""""""""""
# Compilation of the hybrid MPI-Threads example, using mpich
# """"""""""""""""""""""""""""""""""""""""""""""""""""""""""
mpig:   src/MpiGauss.C  src/GaussVec.C
	mpicxx -ompig src/MpiGauss.C src/GaussVec.C ${CPPFLAGS} ${PATHV} -lvath -lpthread

# """"""""""""""""""""""""""""""""""
# Some basic tests
# """"""""""""""""""""""""""""""""""
tmtk1:  Multitasks1.o 
	${CXX} -otmtk1 Multitasks1.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tmtk2:  Multitasks2.o 
	${CXX} -otmtk2 Multitasks2.o  ${PATHV} ${LBASE} -lvath 
	rm *.o

tmtx:  TestMutex.o 
	${CXX} -otmtx TestMutex.o ${PATHV} ${LBASE} -lvath 
	rm *.o

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECSX}
	rm *.o

allbook:  ${EXECS}
allsuppl: ${EXECSX}

.PHONY: runbook
runbook:
	./tnp1 
	./tnp2
	./feach 
	./sp1
	./sp2
	./sp3
	./arec1
	./arec2
	./nest
	./pqsort 
	./preorder
	./onegen
	./twogen
	./tnpnew
	./fibonacci
	./fibonacci2
	./fibonacci3
	./stencil

.PHONY: runsuppl
runsuppl:
	./qsort
	./unbal
	./recursive
	./nest1
	./mcpirec
	./arec2a
	./arec3
	./areath

