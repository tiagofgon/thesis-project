# This makefile builds the C++11, implementation of the "vath"
# library as well as the test cases
# ============================================================

VPATH = src include test 

EXECS = sum dbsearch qsort sumbb tblock tblock1 \
        tblock2 tsynch tbarrier tsbarrier tbb1 tbb2 tbb3  \
        sor sorpb sorpq tsusp trbuff trank tnrank spawn   \
        spawn1 spawn2 tspool tspool1 tnp1 tnp2 tscout \
        tqueue  tqueue2  fcontrol 


OBJECTS = Barrier.o  BLock.o  SpBarrier.o  BkBarrier.o  InputList.o \
          NPool.o  SPool.o  ThreadMgr.o  Task.o 

#"""""""""""""""""""""""""""""""""""""""""""""""
# Variables concerning include and library paths
# """"""""""""""""""""""""""""""""""""""""""""""
ifdef CPP11_ENV
PATHVA = ../vath_tfg/ 
CPPFLAGS = -Iinclude -I../vath_tfg/include 
else
PATHVA = ../vath_tfg/
CPPFLAGS = -Iinclude -I../vath_tfg/include
endif


ifdef INTEL_ENV
CXX = icpc
  # ---------------------------------
  ifdef SHAREDLIB
  CXXFLAGS = -O3 -fPIC -vec-report=3
  else
  CXXFLAGS = -O3 -vec-report=3
  endif
  # ----------------------------------
else
CXX = g++
  # ---------------------------------
  ifdef SHAREDLIB
  CXXFLAGS = -std=c++17 -O3 -fPIC 
  else
  CXXFLAGS = -std=c++17 -O3 -g
  endif
  # --------------------------------
endif

#""""""""""""""""""""""""""""""""""""""""
# Some variables concerning library paths
# """""""""""""""""""""""""""""""""""""""
LBASE = -lpthread

#""""""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the static library libvath
# """""""""""""""""""""""""""""""""""""""""""""""""""

libva:  libvath.a
libvath.a: libvath.a(Barrier.o) libvath.a(BkBarrier.o) 
libvath.a: libvath.a(BLock.o) libvath.a(SPool.o)
libvath.a: libvath.a(NPool.o) libvath.a(SpBarrier.o)
libvath.a: libvath.a(Task.o) libvath.a(ThreadMgr.o)
libvath.a: libvath.a(InputList.o)

libvath.a(Barrier.o): Barrier.o Barrier.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(SpBarrier.o): SpBarrier.o SpBarrier.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(BkBarrier.o): BkBarrier.o BkBarrier.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(BLock.o): BLock.o BLock.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(NPool.o): NPool.o NPool.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(SPool.o): SPool.o SPool.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(InputList.o): InputList.o InputList.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(Task.o): Task.o Task.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(ThreadMgr.o): ThreadMgr.o ThreadMgr.h
	$(AR) $(ARFLAGS) $@ $<

#"""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the dynamic libraries 
# libvath.so and libvamic.so
# """"""""""""""""""""""""""""""""""""""""""""

libvath.so:  ${OBJECTS}
	${CXX} -o libvath.so -shared -fPIC ${OBJECTS}

libvamic.so:  ${OBJECTS}
	${CXX} -mmic -o libvamic.so -shared -fPIC ${OBJECTS}


#""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the tests of the library
# """""""""""""""""""""""""""""""""""""""""""""""

sum  :  Sum.o
	${CXX} -osum Sum.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

sumbb: SumBB.o
	${CXX} -osumbb SumBB.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tblock:  TBlock.o
	${CXX} -otblock TBlock.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tsynch: SynchTest.o
	${CXX} -otsynch SynchTest.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tbarrier:  BarrierTest.o
	${CXX} -otbarrier BarrierTest.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tsbarrier:  SBarriertest.o
	${CXX} -otsbarrier SBarriertest.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tbb1    :  TestBB1.o
	${CXX} -otbb1 TestBB1.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tbb2  :  TestBB2.o
	${CXX} -otbb2 TestBB2.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tbb3  :  TestBB3.o
	${CXX} -otbb3 TestBB3.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

sor  :  SorTh.o
	${CXX} -osor SorTh.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

sorpb  :  SorThB.o
	${CXX} -osorpb SorThB.o -L. -L${PATHVA} -lvath -lpthread
	rm *.o

sorpq  :  SorThQ.o
	${CXX} -osorpq SorThQ.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o


dbsearch:  DbSearch.o
	${CXX} -odbsearch DbSearch.o -L. -L${PATHVA} -lvath  ${LBASE}
	rm *.o

qsort:  QSort.o
	${CXX} -oqsort QSort.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tblock1:  TBlock1.o
	${CXX} -otblock1 TBlock1.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tblock2:  TBlock2.o
	${CXX} -otblock2 TBlock2.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tsusp:  TestSuspend1.o
	${CXX} -otsusp TestSuspend1.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

trbuff:  TRingBuff.o
	${CXX} -otrbuff TRingBuff.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

trank  :  TestRank.o
	${CXX} -otrank TestRank.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tnrank :  TestNPRank.o
	${CXX} -otnrank TestNPRank.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

spawn:  Spawn.o
	${CXX} -ospawn Spawn.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

spawn1:  Spawn1.o
	${CXX} -ospawn1 Spawn1.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

spawn2:  Spawn2.o
	${CXX} -ospawn2 Spawn2.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tspool  :  TestSPool.o
	${CXX} -otspool TestSPool.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tspool1  :  TestSPool1.o
	${CXX} -otspool1 TestSPool1.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tnp1:  TNPool1.o
	${CXX} -otnp1 TNPool1.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tnp2:  TNPool2.o
	${CXX} -otnp2 TNPool2.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tscout  :  TestSCout.o
	${CXX} -otscout TestSCout.o -L. -L${PATHVA} -lvath ${LBASE}
	rm *.o

tqueue  :  TQueue.o
	${CXX} -otqueue TQueue.o -L. -lpthread -lvath
	rm *.o

tqueue2  :  TQueue2.o
	${CXX} -otqueue2 TQueue2.o -L. -lpthread -lvath
	rm *.o

fcontrol  :  FlowControl.o
	${CXX} -ofcontrol FlowControl.o -L. -lpthread -lvath
	rm *.o

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS}
	rm *.o

all:  ${EXECS}

.PHONY: execall
execall:
	./sum
	./sumbb
	./tblock 
	./tsynch 
	./tbarrier 
	./tsbarrier 
	./tbb1
	./tbb2
	./tbb3 
	./sor 
	./sorpb 
	./sorpq
	./qsort
	./tblock1
	./tblock2
	./tsusp
	./trbuff
	./trank 
	./tnrank
	./spawn
	./spawn1
	./spawn2
	./tspool
	./tspool1
	./tnp1 
	./tnp2 
	./tscout
	./tqueue
	./tqueue2
	./fcontrol

