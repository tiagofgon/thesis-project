VPATH = src

EXECS =  heat  heatnp heatnpa heatbb \
         sor sornp sornpa sortbb

EXECS_X =  heath  heath_t  dsor dsortbb

EXECSOMP = heatf heatf2 heatomp  sorf sorf2 soromp

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
