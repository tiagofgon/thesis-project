VPATH = src include 

EXECS_SPOOL = scap_tbb 
EXECS_PTH = scp1_p scp2_p scp3_p scp4_p
EXECS_STD  = scp1_s scp3_s
EXECS_OMP = atestomp scap_omp

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

# -------------< Portable, using SPool+TBB > ---------------

scap_tbb: ScaProd_Tbb.o
	${CXX} -oscap_tbb ScaProd_Tbb.o ${PATHV} ${LBASE} -lvath -ltbb
	rm *.o

# -------------< Pthreads specific codes > ---------------

scp1_p: ScaProd1_P.o
	${CXX} -oscp1_p ScaProd1_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scp2_p: ScaProd2_P.o
	${CXX} -oscp2_p ScaProd2_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scp3_p: ScaProd3_P.o
	${CXX} -oscp3_p ScaProd3_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scp4_p: ScaProd4_P.o
	${CXX} -oscp4_p ScaProd4_P.o ${PATHV} ${LBASE} -lvath 
	rm *.o

# -------------< C++11 specific codes > ---------------

scp1_s: ScaProd1_S.o
	${CXX} -oscp1_s ScaProd1_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scp3_s: ScaProd3_S.o
	${CXX} -oscp3_s ScaProd3_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

# -------------< OpenMp specific codes > --------------

atestomp: AtomicTestOmp.o
	${CXX} -oatestomp ${OPENMP_FLAG} AtomicTestOmp.o ${PATHV} ${LBASE} -lvath 
	rm *.o

scap_omp: ScaProd_Omp.o
	${CXX} -oscap_omp ${OPENMP_FLAG} ScaProd_Omp.o ${PATHV} ${LBASE} -lvath 
	rm *.o


clean:
	rm -f ${EXECS_STD} ${EXECS_PTH} ${EXECS_OMP} ${EXECS_SPOOL}

allpth:  ${EXECS_PTH}
allstd:  ${EXECS_STD}
allsp:   ${EXECS_SPOOL}
allomp:  ${EXECS_OMP}

.PHONY: runpth
runpth:
	./scp1_p 
	./scp2_p 
	./scp3_p
	./scp4_p 

.PHONY: runstd
runstd:
	./scp1_s
	./scp3_s 

.PHONY: runomp
runomp:
	./atestomp 

.PHONY: runspool
runspool:
	./scap_tbb 

