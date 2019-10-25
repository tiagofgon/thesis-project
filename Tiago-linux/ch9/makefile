# This makefile builds the Section 6 examples
# ===========================================

VPATH= src include 

EXECS=  tblock tsynch tbkb tqueue1 tqueue2 
EXECS_PTH = dbs_p spool_p vaccs_p
EXECS_STD = vaccstbb tabarrier_s spool_s
EXECS_OMP = tompblock tsynchomp tompsynch tbkbomp tblockomp

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

# ------------------------------- 
#  Fully portable "vath" examples 
# -------------------------------

tblock: TBlock.o
	${CXX} -otblock TBlock.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tsynch: TSynch.o
	${CXX} -otsynch TSynch.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tbkb:   TBkBarrier.o
	${CXX} -otbkb TBkBarrier.o ${PATHV} ${LBASE} -lvath 
	rm *.o


tqueue1: TQueue1.o
	${CXX} -otqueue1 TQueue1.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tqueue2: TQueue2.o
	${CXX} -otqueue2 TQueue2.o ${PATHV} ${LBASE} -lvath 
	rm *.o

# ------------------- 
#  Pthreads examples
# -------------------
vaccs_p: VAccess_P.o
	${CXX} -ovaccs_p VAccess_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

spool_p: SimplePool_P.o
	${CXX} -ospool_p SimplePool_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

dbs_p:  DbSearch_P.o
	${CXX} -odbs_p DbSearch_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

# ---------------- 
#  C++11 examples 
# ----------------
vaccstbb: VAccessTbb.o
	${CXX} -ovaccstbb VAccessTbb.o ${PATHV} ${LBASE} -lvath -ltbb
	rm *.o

spool_s: SimplePool_S.o
	${CXX} -ospool_s SimplePool_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tabarrier_s: TABarrier_S.o
	${CXX} -otabarrier_s TABarrier_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o


#------------------
# OpenMP examples
#------------------
tompsynch: TOmpSynch.o
	${CXX} -otompsynch ${OPENMP_FLAG} TOmpSynch.o ${PATHV} ${LBASE} -lvath
	rm *.o

tompblock: TOmpBlock.o
	${CXX} -otompblock ${OPENMP_FLAG} TOmpBlock.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tblockomp: TBlockOmp.o
	${CXX} -otblockomp ${OPENMP_FLAG} TBlockOmp.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tsynchomp: TSynchOmp.o
	${CXX} -otsynchomp ${OPENMP_FLAG} TSynchOmp.o ${PATHV} ${LBASE} -lvath 
	rm *.o

tbkbomp: TBkBarrierOmp.o
	${CXX} -otbkbomp ${OPENMP_FLAG} TBkBarrierOmp.o ${PATHV} ${LBASE} -lvath 
	rm *.o

# --------
#  OTHERS
# --------
dbst :  DbSearch_T.o
	${CXX} -odbst DbSearch_T.o ${PATHV} ${LBASE} -ltbb -lvath 
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECS_PTH} ${EXECS_STD} ${EXECS_OMP}
	rm *.o

allsp :    ${EXECS}
allpth:   ${EXECS_PTH}
allomp:   ${EXECS_OMP}
allstd:   ${EXECS_STD}

.PHONY: runspool
runspool:
	./tblock 
	./tsynch 
	./tbkb 
	./tqueue1
	./tqueue2

.PHONY: runpth
runpth:
	./dbs_p
	./spool_p
	./vaccs_p 

.PHONY: runstd
runstd:
	./vaccstbb
	./tabarrier_s
	./spool_s

.PHONY: runomp
runomp:
	./tompblock 
	./tsynchomp 
	./tompsynch 
	./tbkbomp
