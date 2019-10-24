# This makefile builds the Section 6 examples
# ===========================================

VPATH= src  

EXECS = vaccs_bis dbs_bis

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""

LBASE  = -lpthread

ifdef CPP11_ENV
PATHV = -L../../vath_std
else
PATHV = -L../../vath_pth 
endif

include makefile-compiler

# --------------------------------------- 
#  This directory contains C++11 examples 
# ---------------------------------------

vaccs_bis: VAccessBis_S.o
	${CXX} -ovaccs_bis VAccessBis_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o

dbs_bis:  DbSearchBis_S.o
	${CXX} -odbs_bis DbSearchBis_S.o ${PATHV} ${LBASE} -lvath 
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} 
	rm *.o

all :    ${EXECS}

.PHONY: runstd
runstd:
	./vacc_bis
	./dbs_bis

