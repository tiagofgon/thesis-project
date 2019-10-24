VPATH = src  include 

EXECS =  tsplock trbuff trbuff_t scaprod_t
EXECS_STD  = atest scaprod_s

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


# -------------< Portable codes > ---------------

trbuff: TRBuff.o
	${CXX} -otrbuff TRBuff.o ${PATHV} ${LBASE} -lvath 
	rm *.o

trbuff_t: TRBuff_T.o
	${CXX} -otrbuff_t TRBuff_T.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tsplock: TSpinLock.o
	${CXX} -otsplock TSpinLock.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scaprod_t: ScaProd_T.o
	${CXX} -oscaprod_t ScaProd_t.o ${PATHV} ${LBASE} -lvath -ltbb
	rm *.o


# -------------< C++11 specific codes > ---------------

atest:  McAtomic_S.o
	${CXX} -oatest McAtomic_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scaprod_s: ScaProd_S.o
	${CXX} -oscaprod_s ScaProd_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECS_STD}

allsp:   ${EXECS}
allstd:  ${EXECS_STD}

.PHONY: runspool
runspool:
	./tsplock
	./trbuff
	./trbuff_t 

.PHONY: runstd
runstd:
	./atest
	./scaprod_s

