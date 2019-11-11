# This makefile builds the library "pthva" as well as the test
# cases
# ============================================================

VPATH = src include test 

EXECS = sum  sumbb  tblock  tsynch  tbarrier  tsbarrier \
        tbb1  tbb2  tbb3  sor  sorpb  sorpq  dbsearch   \
        qsort  tblock tblock1 tblock2 tsusp trbuff trank  \
        tnrank spawn  spawn1  spawn2  tspool  tspool1  tnp1  \
        tnp2  tscout  rwtest  rwtest2  rwtbtest tqueue \
        tqueue2  fcontrol 

EXECSMIC = msum msumb mblock msynch mrwtest mthp2 \
           mbarrier msbarrier mbb1 mbb2 mbb3 msor   \
           msorpb msorpq  mrank mspool msp1 msp2 msus1 

OBJECTS = Barrier.o BLock.o Common.o Timer.o \
          SpBarrier.o BkBarrier.o InputList.o NPool.o \
          SPool.o CpuTimer.o ThreadMgr.o Task.o 


include makefile-compiler

#""""""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the static library libvath
# """""""""""""""""""""""""""""""""""""""""""""""""""

libva:  libvath.a
libvath.a: libvath.a(Barrier.o) libvath.a(BLock.o) libvath.a(Common.o) 
libvath.a: libvath.a(Timer.o) libvath.a(SpBarrier.o)
libvath.a: libvath.a(BkBarrier.o) libvath.a(NPool.o)
libvath.a: libvath.a(SPool.o) libvath.a(CpuTimer.o) libvath.a(Task.o)
libvath.a: libvath.a(ThreadMgr.o) 

libvath.a(Barrier.o): Barrier.o Barrier.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(SpBarrier.o): SpBarrier.o SpBarrier.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(BkBarrier.o): BkBarrier.o BkBarrier.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(BkSPool.o): BkSPool.o BkSPool.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(SpBLock.o): SpBLock.o SpBLock.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(BLock.o): BLock.o BLock.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(Task.o): Task.o Task.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(NPool.o): NPool.o NPool.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(SPool.o): SPool.o SPool.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(CpuTimer.o): CpuTimer.o CpuTimer.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(Timer.o): Timer.o Timer.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(ThreadMgr.o): ThreadMgr.o ThreadMgr.h
	$(AR) $(ARFLAGS) $@ $<

libvath.a(Common.o): Common.o Common.h
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
# This part deals with the tests of the static
# library libvath.a and the dynamic library
# libvath.so
# """""""""""""""""""""""""""""""""""""""""""""""

sum  :  Sum.o
	${CXX} -osum Sum.o -L. -lpthread -lvath
	rm *.o

sumbb : SumBB.o
	${CXX} -osumbb SumBB.o -L. -lpthread -lvath
	rm *.o

tblock:  TBlock.o
	${CXX} -otblock TBlock.o -L. -lpthread -lvath
	rm *.o

tsynch: SynchTest.o
	${CXX} -otsynch SynchTest.o -L. -lpthread -lvath
	rm *.o

tbarrier:  BarrierTest.o
	${CXX} -otbarrier BarrierTest.o -L. -lpthread -lvath
	rm *.o

tsbarrier:  SBarrierTest.o
	${CXX} -otsbarrier SBarrierTest.o -L. -lpthread -lvath
	rm *.o

tbb1:   TestBB1.o
	${CXX} -otbb1 TestBB1.o -L. -lpthread -lvath
	rm *.o

tbb2:   TestBB2.o
	${CXX} -otbb2 TestBB2.o -L. -lpthread -lvath
	rm *.o

tbb3:   TestBB3.o
	${CXX} -otbb3 TestBB3.o -L. -lpthread -lvath
	rm *.o

sor:    SorTh.o
	${CXX} -osor SorTh.o -L. -lpthread -lvath
	rm *.o

sorpb:  SorThB.o
	${CXX} -osorpb SorThB.o -L. -lpthread -lvath
	rm *.o

sorpq:  SorThQ.o
	${CXX} -osorpq SorThQ.o -L. -lpthread -lvath
	rm *.o

dbsearch:  DbSearch.o
	${CXX} -odbsearch DbSearch.o -L. -lvath -lpthread 
	rm *.o

qsort:  QSort.o
	${CXX} -oqsort QSort.o -L. -lpthread -lvath 
	rm *.o

tblock1:  TBlock1.o
	${CXX} -otblock1 TBlock1.o -L. -lpthread -lvath
	rm *.o

tblock2:  TBlock2.o
	${CXX} -otblock2 TBlock2.o -L. -lvath  -lpthread
	rm *.o

tsusp:  TestSuspend1.o
	${CXX} -otsusp TestSuspend1.o -L. -lpthread -lvath
	rm *.o

trbuff: TRingBuff.o
	${CXX} -otrbuff TRingBuff.o -L. -lpthread -lvath
	rm *.o

trank:  TestRank.o
	${CXX} -otrank TestRank.o -L. -lpthread -lvath
	rm *.o

tnrank:  TestNPRank.o
	${CXX} -otnrank TestNPRank.o -L. -lpthread -lvath 
	rm *.o

spawn:  Spawn.o
	${CXX} -ospawn Spawn.o -L. -lpthread -lvath
	rm *.o

spawn1:  Spawn1.o
	${CXX} -ospawn1 Spawn1.o -L. -lpthread -lvath
	rm *.o

spawn2:  Spawn2.o
	${CXX} -ospawn2 Spawn2.o -L. -lpthread -lvath
	rm *.o

tspool:  TestSPool.o
	${CXX} -otspool TestSPool.o -L. -lpthread -lvath
	rm *.o

tspool1:  TestSPool1.o
	${CXX} -otspool1 TestSPool1.o -L. -lpthread -lvath
	rm *.o

tnp1:  TNPool1.o
	${CXX} -otnp1 TNPool1.o -L. -lpthread -lvath
	rm *.o

tnp2:  TNPool2.o
	${CXX} -otnp2 TNPool2.o -L. -lpthread -lvath
	rm *.o

tscout:  TestSCout.o
	${CXX} -otscout TestSCout.o -L. -lpthread -lvath 
	rm *.o

rwtest: RWtest.o
	${CXX} -orwtest RWtest.o -L. -lpthread -lvath
	rm *.o

rwtest2: RWtest2.o
	${CXX} -orwtest2 RWtest2.o -L. -lpthread -lvath
	rm *.o

rwtbtest: RWTbbtest.o
	${CXX} -orwtbtest RWTbbtest.o -L. -lpthread -lvath  -ltbb
	rm *.o

tqueue: TQueue.o
	${CXX} -otqueue TQueue.o -L. -lpthread -lvath
	rm *.o

tqueue2:  TQueue2.o
	${CXX} -otqueue2 TQueue2.o -L. -lpthread -lvath
	rm *.o

fcontrol:  FlowControl.o
	${CXX} -ofcontrol FlowControl.o -L. -lpthread -lvath
	rm *.o


#""""""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the tests of the dynamic
# library fir Intel Xeon Phi: libvamic.so
# """""""""""""""""""""""""""""""""""""""""""""""

msum  :  Sum.o
	${CXX} -mmic -omsum Sum.o -L. -lpthread -lvamic
	rm *.o

msumb : SumBB.o
	${CXX} -mmic -omsumb SumBB.o -L. -lpthread -lvamic
	rm *.o

mblock:  TBlock.o
	${CXX} -mmic -omblock TBlock.o -L. -lpthread -lvamic
	rm *.o

msynch: SynchTest.o
	${CXX} -mmic -omsynch SynchTest.o -L. -lpthread -lvamic
	rm *.o

mrwtest: RWtest.o
	${CXX} -mmic -omrwtest RWtest.o -L. -lpthread -lvamic
	rm *.o

mbarrier:  BarrierTest.o
	${CXX} -mmic -ombarrier BarrierTest.o -L. -lpthread -lvamic
	rm *.o

msbarrier:  SBarrierTest.o
	${CXX} -mmic -omsbarrier SBarrierTest.o -L. -lpthread -lvamic
	rm *.o

mbb1    :  TestBB1.o
	${CXX} -mmic -ombb1 TestBB1.o -L. -lpthread -lvamic
	rm *.o

mbb2  :  TestBB2.o
	${CXX} -mmic -ombb2 TestBB2.o -L. -lpthread -lvamic
	rm *.o

mbb3  :  TestBB3.o
	${CXX} -mmic -ombb3 TestBB3.o -L. -lpthread -lvamic
	rm *.o

msor  :  SorTh.o
	${CXX} -mmic -omsor SorTh.o -L. -lpthread -lvamic
	rm *.o

msorpb   :  SorThB.o
	${CXX} -mmic -omsorpb SorThB.o -L. -lpthread -lvamic
	rm *.o

msorpq   :  SorThQ.o
	${CXX} -mmic -omsorpq SorThQ.o -L. -lpthread -lvamic
	rm *.o

msus1:  TestSuspend1.o
	${CXX} -mmic -omsus1 TestSuspend1.o -L. -lpthread -lvamic
	rm *.o

mrank  :  TestRank.o
	${CXX} -mmic -omrank TestRank.o -L. -lpthread -lvamic
	rm *.o

mthp1   :  ThPool1.o
	${CXX} -mmic -omthp1 ThPool1.o -L. -lpthread -lvamic
	rm *.o

mthp2  :  ThPool2.o
	${CXX} -mmic -omthp2 ThPool2.o -L. -lpthread -lvamic
	rm *.o

msp1  :  Spawn1.o
	${CXX} -msp1 -omsp1 Spawn1.o -L. -lpthread -lvamic
	rm *.o

msp2  :  Spawn2.o
	${CXX} -mmic -omsp2 Spawn2.o -L. -lpthread -lvamic
	rm *.o

mspool  :  TestSPool.o
	${CXX} -mmic -omspool TestSPool.o -L. -lpthread -lvamic
	rm *.o

mallocator :  TestAllocator.o
	${CXX} -mmic -omallocator TestAllocator.o -L. -lpthread -lvamic
	rm *.o

mqueue  :  TQueue.o
	${CXX} -mmic -omqueue TQueue.o -L. -lpthread -lvamic
	rm *.o

mqueue2  :  TQueue2.o
	${CXX} -mmic -omqueue2 TQueue2.o -L. -lpthread -lvamic
	rm *.o

mcontrol  :  FlowControl.o
	${CXX} -mmic -omcontrol FlowControl.o -L. -lpthread -lvamic
	rm *.o

mnp1  :  TNPool1.o
	${CXX} -mmic -omnp1 TNPool1.o -L. -lpthread -lvamic
	rm *.o


#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS}
	rm *.o

all:  ${EXECS}

allmic: ${EXECSMIC}

.PHONY: execall
execall:
	./sum
	./sumbb
	./tblock 
	./tblock1
	./tblock2
	./tsynch 
	./tbarrier 
	./tsbarrier 
	./tbb1
	./tbb2
	./tbb3 
	./sor 
	./sorpb 
	./sorpq
	./dbsearch
	./qsort
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
	./rwtest
	./rwtest2
	./rwtbtest
	./tqueue
	./tqueue2
	./fcontrol

