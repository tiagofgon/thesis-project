VPATH = src

EXECS =  heat  heatnp heatnpa heatbb \
         sor sornp sornpa sortbb \
		 tnp1 tnp2 \
		 fibonacci fibonacci2 fibonacci3 \
		 heatnptfg

EXECS_X =  heath  heath_t  dsor dsortbb

EXECSOMP = heatf heatf2 heatomp  sorf sorf2 soromp

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
LBASE  = -lpthread

PATHV = -L../vath_tfg


include makefile-compiler

# """"""""""""""""""
# HEAT codes in book
# """""""""""""""""" 
heat:   Heat.o HeatAux.o
	${CXX} -oheat Heat.o HeatAux.o ${PATHV} ${LBASE} -lvath 
	rm Heat.o

heatnp:  HeatNP.o HeatAux.o
	${CXX} -oheatnp HeatNP.o HeatAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm HeatNP.o

heatnpa:  HeatNPa.o HeatAux.o
	${CXX} -oheatnpa HeatNPa.o HeatAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm HeatNPa.o

heatf:  HeatF.o HeatAux.o
	${CXX} -oheatf ${OPENMP_FLAG} HeatF.o HeatAux.o ${PATHV} ${LBASE} -lvath
	rm HeatF.o

heatomp:  HeatOmp.o HeatAux.o
	${CXX} -oheatomp ${OPENMP_FLAG} HeatOmp.o HeatAux.o ${PATHV} ${LBASE} -lvath
	rm HeatOmp.o

heatbb:  HeatTBB.o HeatAux.o
	${CXX} -oheatbb HeatTBB.o HeatAux.o -lm -ltbb 
	rm HeatTBB.o

# """"""""""""""""""
# My codes
# """""""""""""""""" 
tnp1:   TNPool1.o
	${CXX} -otnp1 TNPool1.o ${PATHV} ${LBASE} -lvath 
	rm TNPool1.o

tnp2:   TNPool2.o
	${CXX} -otnp2 TNPool2.o ${PATHV} ${LBASE} -lvath
	rm TNPool2.o

fibonacci: fibonacci.o
	${CXX} -ofibonacci fibonacci.o ${PATHV} ${LBASE} -lvath
	rm fibonacci.o

fibonacci2: fibonacci2.o
	${CXX} -ofibonacci2 fibonacci2.o ${PATHV} ${LBASE} -lvath
	rm fibonacci2.o

fibonacci3: fibonacci3.o
	${CXX} -ofibonacci3 fibonacci3.o -pthread ${PATHV} ${LBASE} -lvath
	rm fibonacci3.o

heatnptfg:  HeatNPtfg.o HeatAux.o
	${CXX} -oheatnptfg HeatNPtfg.o HeatAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm HeatNPtfg.o

addvec:  AddVectors.o
	${CXX} -oaddvec AddVectors.o ${PATHV} ${LBASE} -lvath 
	rm AddVectors.o

# """""""""""""""""""""""
# Additional heat codes
# """"""""""""""""""""""
heatf2:  HeatF2.o HeatAux.o
	${CXX} -oheatf2 ${OPENMP_FLAG} HeatF2.o HeatAux.o ${PATHV} ${LBASE} -lvath
	rm HeatF2.o

heath:  HeatTh.o HeatAux.o
	${CXX} -oheath HeatTh.o HeatAux.o ${PATHV} ${LBASE} -lvath  -ltbb
	rm HeatTh.o

heath_t: HeatTh_T.o HeatAux.o
	${CXX} -oheath_t HeatTh_T.o HeatAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm HeatTh_T.o


# """""""""""""""""
# SOR codes
# """""""""""""""""
sor:    Sor.o SorAux.o
	${CXX} -osor Sor.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm Sor.o

sors:    SorSeq.o SorAux.o
	${CXX} -osors SorSeq.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm SorSeq.o

dsor:    DSor.o SorAux.o 
	${CXX} -odsor DSor.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm DSor.o

dsorb:   DSorb.o SorAux.o 
	${CXX} -odsorb DSorb.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm DSorb.o

dsortbb: DSorTbb.o SorAux.o 
	${CXX} -odsortbb DSorTbb.o SorAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm DSorTbb.o

sortbb:  SorTBB.o SorAux.o
	${CXX} -osortbb SorTBB.o SorAux.o ${PATHV} ${LBASE} -ltbb 
	rm SorTBB.o

sornp:  SorNP.o SorAux.o
	${CXX} -osornp SorNP.o SorAux.o ${PATHV} ${LBASE} -lvath 
	rm SorNP.o

sornpa:  SorNPa.o SorAux.o
	${CXX} -osornpa SorNPa.o SorAux.o ${PATHV} ${LBASE} -lvath -ltbb 
	rm SorNPa.o

sorf:   SorF.o SorAux.o
	${CXX} -osorf ${OPENMP_FLAG} SorF.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm SorF.o

soromp: SorOmp.o SorAux.o
	${CXX} -osoromp ${OPENMP_FLAG} SorOmp.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm SorOmp.o

# """"""""""""""""""""
# Additional SOR codes
# """"""""""""""""""""
sorf2:   SorF2.o SorAux.o
	${CXX} -osorf2 ${OPENMP_FLAG} SorF2.o SorAux.o ${PATHV} ${LBASE} -lvath
	rm SorF2.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""
clean:
	rm -f ${EXECS} ${EXECS_X} ${EXECSOMP}
	rm *.o

all:    ${EXECS}
allx:    ${EXECS_X}
allomp: ${EXECSOMP}


.PHONY: runall
runall:
	./heat
	./heatnp
	./heatnpa
	./heatbb 
	./sor 
	./sornp
	./sornpa
	./sortbb
	./tnp1
	./tnp2
	./fibonacci
	./fibonacci2
	./fibonacci3
	./heattfg

.PHONY: runallx
runallx:
	./heath
	./heath_t
	./sorth

.PHONY: runomp
runomp:
	./heatf
	./heatf2
	./heatomp
	./sorf
	./sorf2 
	./soromp 
