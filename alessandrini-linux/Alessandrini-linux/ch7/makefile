# This makefile builds the Section 6 examples
# ===========================================

VPATH = src  

EXECSPTH =  test1 test2  test3  test4 
EXECSTD=  test1 test2  test3  test4 

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# to find libvath
# """""""""""""""""""""""""""""""""""""""

LBASE  = -lpthread

ifdef CPP11_ENV
PATHV = -L../vath_std
else
PATHV = -L../vath_pth 
endif

include makefile-compiler

test1 : Test1.o
	${CXX} -otest1 Test1.o ${PATHV} -lpthread -lvath
	rm *.o

test2:  Test2.o
	${CXX} -otest2 Test2.o ${PATHV} -lpthread -lvath
	rm *.o

test3:  Test3.o
	${CXX} -otest3 Test3.o ${PATHV} -lpthread -lvath
	rm *.o

test4:  Test4.o
	${CXX} -otest4 Test4.o ${PATHV} -lpthread -lvath
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECSPTH}

all:  ${EXECSPTH}

.PHONY: test
test:
	./test1 
	./test2  
	./test3  
	./test4 

