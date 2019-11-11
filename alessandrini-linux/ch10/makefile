
VPATH = src src/dag

EXECS = icv1  icv2 \
        task2  task3  task4 \
        sect1 sect2 sect3 \
        pqsort areaomp        \
        calcpi1  calcpi2 calcpi3

EXECS4 = feach  depend  ompreorder \
         dbomp4_a  dbomp4_b areabis

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""

LBASE  = -lpthread

ifdef CPP11_ENV
PATHV = -L../vath_std
else
PATHV = -L../vath_pth 
endif

include makefile-compiler

#""""""""""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the applications in subdirectories
# of this directory.
# """""""""""""""""""""""""""""""""""""""""""""""""""""""

# ---------< Basic OpenMP > ------------

icv1:   Icv1.o
	${CXX} -oicv1 ${OPENMP_FLAG} Icv1.o
	rm *.o

icv2:   Icv2.o
	${CXX} -oicv2 ${OPENMP_FLAG} Icv2.o -lpthread
	rm *.o

task2:  Task2.o
	${CXX} -otask2 ${OPENMP_FLAG} Task2.o ${PATHV} ${LBASE} -lvath
	rm *.o

task3:  Task3.o
	${CXX} -otask3 ${OPENMP_FLAG} Task3.o ${PATHV} ${LBASE} -lvath
	rm *.o

task4:  Task4.o
	${CXX} -otask4 ${OPENMP_FLAG} Task4.o ${PATHV} ${LBASE} -lvath
	rm *.o

sect1:  Sections1.o
	${CXX} -osect1 ${OPENMP_FLAG} Sections1.o ${PATHV} ${LBASE} -lvath
	rm *.o

sect2:  Sections2.o
	${CXX} -osect2 ${OPENMP_FLAG} Sections2.o ${PATHV} ${LBASE} -lvath
	rm *.o

sect3:  Sections3.o
	${CXX} -osect3 ${OPENMP_FLAG} Sections3.o ${PATHV} ${LBASE} -lvath
	rm *.o

pqsort: QsortOmp.o
	${CXX} -opqsort ${OPENMP_FLAG} QsortOmp.o 
	rm *.o

areaomp: AreaOmp.o
	${CXX} -oareaomp ${OPENMP_FLAG} AreaOmp.o 
	rm *.o

calcpi1:   CalcPi1.o
	${CXX} -ocalcpi1 ${OPENMP_FLAG} CalcPi1.o 
	rm *.o

calcpi2:   CalcPi2.o
	${CXX} -ocalcpi2 ${OPENMP_FLAG} CalcPi2.o 
	rm *.o

calcpi3:   CalcPi3.o
	${CXX} -ocalcpi3 ${OPENMP_FLAG} CalcPi3.o 
	rm *.o


# ----------< OpenMP 4.0 >-------------------------------------

feach:  Foreach.o
	${CXX} -ofeach ${OPENMP_FLAG} Foreach.o ${PATHV} ${LBASE} -lvath
	rm *.o

depend:  Depend.o
	${CXX} -odepend ${OPENMP_FLAG} Depend.o ${PATHV} ${LBASE} -lvath
	rm *.o

dbomp4_a:  DbOmp4-A.o
	${CXX} -odbomp4_a ${OPENMP_FLAG} DbOmp4-A.o ${PATHV} ${LBASE} -lvath
	rm *.o

dbomp4_b:  DbOmp4-B.o
	${CXX} -odbomp4_b ${OPENMP_FLAG} DbOmp4-B.o ${PATHV} ${BASE} -lvath
	rm *.o

areabis: AreaOmpBis.o
	${CXX} -oareabis ${OPENMP_FLAG} AreaOmpBis.o 
	rm *.o


# This this the graph traversal code:
# ----------------------------------
ompreorder:  PreorderOmp.o VGraph.o
	${CXX} -oompreorder PreorderOmp.o VGraph.o $(OPENMP_FLAG) ${PATHB} ${PATHV} ${LBASE} -lvath 
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECS4}

allomp: ${EXECS}
allomp4: ${EXECS4}

.PHONY: runomp
runomp:
	./calcpi1
	./calcpi2
	./calcpi3
	./icv1
	./icv2 
	./task2 
	./task3
	./task4 
	./sect1
	./sect2
	./sect3
	./pqsort
	./areaomp

.PHONY: runomp4
runomp4:
	./feach
	./depend
	./areabis
	./ompreorder
	./dbomp4_a
	./dbomp4_b

