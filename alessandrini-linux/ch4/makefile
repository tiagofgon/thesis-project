# This makefile builds the chapter 3 examples
# ===========================================

VPATH =  include src

EXECS_STD =  mcsafe_s grstd_s 
EXECS_SPOOL = mcsafe mcobj mcsafetbb grsafe grunsafe grobj grtbb   
EXECS_OMP =  mcsafeomp gromp

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


# -------------< Pthreads codes > ------------

mcsafe_p:  McSafe_P.o
	${CXX} -omcsafe_p McSafe_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o
  
# -------------< Pure C++11 codes > ------------

mcsafe_s:  McSafe_S.o
	${CXX} -omcsafe_s McSafe_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

grstd_s:  GrStd_S.o
	${CXX} -ogrstd_s GrStd_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

# ----------------< SPool codes > --------------

mcsafe: McSafe.o
	${CXX} -omcsafe McSafe.o ${PATHV} ${LBASE} -lvath 
	rm *.o

mcobj:  McObj.o
	${CXX} -omcobj McObj.o ${PATHV} ${LBASE} -lvath 
	rm *.o

mcsafetbb: McSafeTbb.o
	${CXX} -omcsafetbb McSafeTbb.o ${PATHV} ${LBASE}  -lvath -ltbb
	rm *.o

grsafe: GrSafe.o
	${CXX} -ogrsafe GrSafe.o ${PATHV} ${LBASE} -lvath 
	rm *.o

grunsafe: GrUnsafe.o
	${CXX} -ogrunsafe GrUnsafe.o ${PATHV} ${LBASE}  -lvath 
	rm *.o

grobj:  GrObj.o
	${CXX} -ogrobj GrObj.o ${PATHV} ${LBASE} -lvath 
	rm *.o

grtbb : GrTbb.o
	${CXX} -ogrtbb GrTbb.o ${PATHV} ${LBASE} -lvath -ltbb
	rm *.o


# -------------< OpenMP codes > --------------

mcsafeomp:  McSafeOmp.o
	${CXX} -omcsafeomp ${OPENMP_FLAG} McSafeOmp.o ${PATHV} -lvath
	rm *.o

gromp :  GrOmp.o
	${CXX} -ogromp ${OPENMP_FLAG} GrOmp.o ${PATHV} -lvath
	rm *.o

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS_STD} ${EXECS_SPOOL} ${EXECS_OMP} 

allstd:  ${EXECS_STD}
allomp:  ${EXECS_OMP}
allsp:   ${EXECS_SPOOL}

.PHONY: runspool
runsp:
	./mcobj
	./mcsafe
	./mcsafetbb
	./grsafe
	./grunsafe
	./grobj
	./grtbb

.PHONY: runomp
runomp:
	./gromp
	./mcsafeomp


.PHONY:  runstd
runstd:
	./mcsafe_s
	./grstd_s


