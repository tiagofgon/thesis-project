# This makefile builds the chapter 3 examples
# ===========================================

VPATH =  include src

EXECS_PTH = hello1p hello2p  hello3p hello4p retstring
EXECS_CPP11 = cpp1s cpp2s cpp3s cpp4s cpp5s runt1s runt2s thmoves
EXECS_SPOOL = calcpi  addvec areapi dbsearch   
EXECS_OMP =  calcpiomp areapiomp 

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# to find libvath
# """""""""""""""""""""""""""""""""""""""

ifdef CPP11_ENV
PATHV = -L../vath_std
LBASE = -lpthread 
else
PATHV = -L../vath_pth 
LBASE  = -lpthread
endif

ifdef USING_TBB
LTBB = -ltbb
else
LTBB = 
endif

include makefile-compiler

# -------------< Pure Pthreads codes > --------------

hello1p: Hello1_P.o
	${CXX} -ohello1p Hello1_P.o -lpthread
	rm *.o

hello2p: Hello2_P.o
	${CXX} -ohello2p Hello2_P.o -lpthread
	rm *.o

hello3p: Hello3_P.o
	${CXX} -ohello3p Hello3_P.o -lpthread
	rm *.o

hello4p: Hello4_P.o
	${CXX} -ohello4p Hello4_P.o -lpthread
	rm *.o

retstring:  RetString_P.o
	${CXX} -oretstring RetString_P.o -lpthread
	rm *.o

# -------------< Pure C++11 codes > ------------

cpp1s:  Cpp1_S.o
	${CXX} -ocpp1s Cpp1_S.o ${LBASE}
	rm *.o

cpp2s:  Cpp2_S.o
	${CXX} -ocpp2s Cpp2_S.o ${LBASE}
	rm *.o

cpp3s:  Cpp3_S.o
	${CXX} -ocpp3s Cpp3_S.o ${LBASE}
	rm *.o

cpp4s:  Cpp4_S.o
	${CXX} -ocpp4s Cpp4_S.o ${LBASE}
	rm *.o

cpp5s:  Cpp5_S.o
	${CXX} -ocpp5s Cpp5_S.o ${LBASE}
	rm *.o


runt1s: RunTeam1_S.o
	${CXX} -orunt1s RunTeam1_S.o ${LBASE}
	rm *.o

runt2s: RunTeam2_S.o
	${CXX} -orunt2s RunTeam2_S.o ${LBASE}
	rm *.o

thmoves: ThMove_S.o
	${CXX} -othmoves ThMove_S.o ${LBASE}
	rm *.o

# ----------------< SPool codes > --------------

calcpi: CalcPi.o
	${CXX} -ocalcpi CalcPi.o ${PATHV} ${LBASE} ${LTBB} -lvath 
	rm *.o

addvec:  AddVectors.o
	${CXX} -oaddvec AddVectors.o ${PATHV} ${LBASE} ${LTBB} -lvath 
	rm *.o

areapi : AreaPi.o
	${CXX} -oareapi AreaPi.o ${PATHV} ${LBASE} ${LTBB} -lvath 
	rm *.o

dbsearch:   DbSearch.o
	${CXX} -odbsearch DbSearch.o ${PATHV} ${LBASE} ${LTBB} -lvath 
	rm *.o

# -------------< OpenMP codes > --------------

calcpiomp:  CalcPiOmp.o
	${CXX} -ocalcpiomp ${OPENMP_FLAG} CalcPiOmp.o ${PATHV} -lvath
	rm *.o

areapiomp:  AreaPiOmp.o
	${CXX} -oareapiomp ${OPENMP_FLAG} AreaPiOmp.o
	rm *.o



#"""""""""""""""""""""""""""""""""""
# Subsidiary targets
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS_PTH} ${EXECS_CPP11} ${EXECS_SPOOL} ${EXECS_OMP} 

allpth:  ${EXECS_PTH}
allstd:  ${EXECS_CPP11}
allomp:  ${EXECS_OMP}
allsp:   ${EXECS_SPOOL}

.PHONY: runpth
runpth:
	./hello1p 
	./hello2p
	./hello3p
	./hello4p
	./retstring

.PHONY: runomp
runomp:
	./areapiomp
	./calcpiomp

.PHONY: runstd
runstd:
	./cpp1s
	./cpp2s
	./cpp3s
	./cpp4s
	./cpp5s
	./runt1s
	./runt2s
	./thmoves

.PHONY:  runspool
runspool:
	./addvec
	./calcpi
	./areapi
	./dbsearch

