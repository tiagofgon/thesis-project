# This makefile builds the library "pthva" as well as the test
# cases
# ============================================================

VPATH =  src

EXECS_PTH=  barr1_p barr2_p  iotask_p 
EXECS_STD=  barr1_s barr2_s  iotask_s exfuture  \
            io1_s io2_s pio1_s pio2_s twait_s
EXECS_OMP=  iotomp

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

# ------------------
# Pthreads examples
# -----------------

barr1_p:  Barrier1_P.o
	${CXX} -obarr1_p Barrier1_P.o ${PATHV} ${LBASE} -lvath
	rm *.o

barr2_p:  Barrier2_P.o
	${CXX} -obarr2_p Barrier2_P.o ${PATHV} ${LBASE} -lvath
	rm *.o

iotask_p :IoTask_P.o
	${CXX} -oiotask_p IoTask_P.o ${PATHV} ${LBASE} -lvath
	rm *.o

# ------------------
# C++11 examples
# -----------------
barr1_s:  Barrier1_S.o
	${CXX} -obarr1_s Barrier1_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

barr2_s:  Barrier2_S.o
	${CXX} -obarr2_s Barrier2_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

twait_s : TimedWait_S.o
	${CXX} -otwait_s TimedWait_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

iotask_s :IoTask_S.o
	${CXX} -oiotask_s IoTask_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

exfuture: ExFuture_S.o
	${CXX} -oexfuture ExFuture_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

io1_s:   Io1_S.o
	${CXX} -oio1_s Io1_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

io2_s:   Io2_S.o
	${CXX} -oio2_s Io2_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

pio1_s:  Pio1_S.o
	${CXX} -opio1_s Pio1_S.o ${PATHV} ${LBASE} -lvath
	rm *.o

pio2_s:  Pio2_S.o
	${CXX} -opio2_s Pio2_S.o ${PATHV} ${LBASE} -lvath
	rm *.o


# -------------------
# OpenMP example
# -------------------

iotomp: IoTaskOmp.o
	${CXX} -oiotomp ${OPENMP_FLAG} IoTaskOmp.o $ ${PATHV} ${LBASE} -lvath
	rm *.o

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS_PTH} ${EXECS_STD} ${EXECS_OMP}

allpth:   ${EXECS_PTH}
allstd:   ${EXECS_STD}
allomp:   ${EXECS_OMP}

.PHONY: runpth
runpth:
	./barr1_p 
	./barr2_p  
	./iotask_p  

.PHONY: runstd
runstd:
	./barr1_s 
	./barr2_s  
	./iotask_s
	./exfuture
	./io1_s
	./io2_s
	./pio1_s
	./pio2_s

.PHONY: runomp
runomp:
	./iotomp

