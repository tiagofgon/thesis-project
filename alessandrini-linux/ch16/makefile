# This makefile builds the Chapter 8 examples
# ===========================================

VPATH = src include

EXECS =  block cont arearec1 arearec2 submit \
         synchio recbad mdrec mdsch

EXECSX =  arearec3 areag areaf areagbis \
         recycle recycle1 recycle2 synch2

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

# --------------< Book examples >-------------------------------

block:  Blocking.o
	${CXX} -oblock Blocking.o ${PATHV} -lpthread -lvath -ltbb
	rm *.o

cont:   Continuation.o
	${CXX} -ocont Continuation.o ${PATHV} -lpthread -lvath -ltbb
	rm *.o

arearec1: AreaRec1.o
	${CXX} -oarearec1 AreaRec1.o ${PATHV} -lpthread -lvath -ltbb
	rm *.o

arearec2: AreaRec2.o
	${CXX} -oarearec2 AreaRec2.o -ltbb
	rm *.o

synchio:  SynchIO.o
	${CXX} -osynchio SynchIO.o ${PATHV} -lpthread -lvath -ltbb
	rm *.o

recbad:  RecycleBad.o
	${CXX} -orecbad RecycleBad.o -ltbb
	rm *.o

mdrec:  MdRec.o mdaux.o
	${CXX} -omdrec MdRec.o mdaux.o -ltbb

mdsch:  MdSch.o mdaux.o
	${CXX} -omdsch MdSch.o mdaux.o -ltbb
	rm *.o

submit: Submit.o
	${CXX} -osubmit Submit.o ${PATHV} -lpthread -lvath -ltbb
	rm *.o

# --------------< Additional examples >-------------------------------

arearec3: AreaRec3.o
	${CXX} -oarearec3 AreaRec3.o -ltbb
	rm *.o

areag: AreaGroup.o
	${CXX} -oareag AreaGroup.o -ltbb
	rm *.o

areaf: AreaFixed.o
	${CXX} -oareaf AreaFixed.o -ltbb
	rm *.o

areagbis: AreaGroupBis.o
	${CXX} -oareagbis AreaGroupBis.o -ltbb
	rm *.o

synch2:  SynchIO2.o
	${CXX} -osynch2 SynchIO2.o ${PATHV} -lpthread -lvath -ltbb
	rm *.o

recycle: Recycle.o
	${CXX} -orecycle Recycle.o -ltbb
	rm *.o

recycle1: Recycle1.o
	${CXX} -orecycle1 Recycle1.o -ltbb
	rm *.o

recycle2: Recycle2.o
	${CXX} -orecycle2 Recycle2.o -ltbb
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS} ${EXECSX}

all:  ${EXECS}
allx: ${EXECSX}

.PHONY: runall 
runall:
	./block 
	./cont 
	./arearec1 
	./arearec2 
	./synchio
	./submit 
	./recbad  
	./mdrec
	./mdsch

.PHONY: runallx
runallx:
	./arearec3
	./areag
	./areaf
	./areagbis
	./recycle
	./recycle1
	./recycle2
	./synch2
