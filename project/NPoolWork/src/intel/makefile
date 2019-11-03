EXECS =  
EXECSOMP = heat1 

PATHV = -L../vath 

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
CPPFLAGS = -I../../vath/include -I../include
CXX = icpc
CXXFLAGS = -O3 -openmp -vec-report=3

#""""""""""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the applications in subdirectories
# of this directory.
# """""""""""""""""""""""""""""""""""""""""""""""""""""""

md:     md.o mdaux.o
	${CXX} -omd md.o mdaux.o ${PATHV} -lvath 

heat1:  HeatOmp1.o
	${CXX} -openmp -oheat1 HeatOmp1.o  

heat2:  HeatOmp2.o
	${CXX} -openmp -oheat2 HeatOmp2.o  

heat3:  HeatOmp3.o
	${CXX} -openmp -oheat3 HeatOmp3.o  

heat4:  HeatOmp4.o
	${CXX} -openmp -oheat4 HeatOmp4.o  

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECSOMP}
	rm *.o

all:    ${EXECS}
allomp: ${EXECSOMP}

.PHONY: test
test:
	./md

.PHONY: tomp
tomp:
	./mdomp
