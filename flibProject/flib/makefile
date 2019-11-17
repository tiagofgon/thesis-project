# This makefile builds the C++14, implementation of the "flib" library
#
# ============================================================

VPATH = src include 

OBJECTS = BLock.o BkBarrier.o InputList.o NPool.o  SPool.o  ThreadMgr.o  Task.o 

#"""""""""""""""""""""""""""""""""""""""""""""""
# Variables concerning include and library paths
# """"""""""""""""""""""""""""""""""""""""""""""
PATHVA = ../flib/ 
CPPFLAGS = -I include -I ../flib/include 


ifdef INTEL_ENV
CXX = icpc
  # ---------------------------------
  ifdef SHAREDLIB
  CXXFLAGS = -std=c++14 -O3 -fPIC -vec-report=3
  else
  CXXFLAGS = -std=c++14 -O3 -vec-report=3
  endif
  # ----------------------------------
else
CXX = g++
  # ---------------------------------
  ifdef SHAREDLIB
  CXXFLAGS = -std=c++14 -O3 -fPIC 
  else
  CXXFLAGS = -std=c++14 -O3 -g
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
libvath.a: libvath.a(BLock.o) libvath.a(SPool.o)
libvath.a: libvath.a(NPool.o) libvath.a(BkBarrier.o) 
libvath.a: libvath.a(Task.o) libvath.a(ThreadMgr.o)
libvath.a: libvath.a(InputList.o)
libvath.a: clean

libvath.a(BkBarrier.o): BkBarrier.o BkBarrier.hpp
	$(AR) $(ARFLAGS) $@ $<

libvath.a(BLock.o): BLock.o BLock.hpp
	$(AR) $(ARFLAGS) $@ $<

libvath.a(NPool.o): NPool.o NPool.hpp
	$(AR) $(ARFLAGS) $@ $<

libvath.a(SPool.o): SPool.o SPool.hpp
	$(AR) $(ARFLAGS) $@ $<

libvath.a(InputList.o): InputList.o InputList.hpp
	$(AR) $(ARFLAGS) $@ $<

libvath.a(Task.o): Task.o Task.hpp
	$(AR) $(ARFLAGS) $@ $<

libvath.a(ThreadMgr.o): ThreadMgr.o ThreadMgr.hpp
	$(AR) $(ARFLAGS) $@ $<

#"""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the dynamic libraries 
# libvath.so and libvamic.so
# """"""""""""""""""""""""""""""""""""""""""""

libvath.so:  ${OBJECTS}
	${CXX} -o libvath.so -shared -fPIC ${OBJECTS}

libvamic.so:  ${OBJECTS}
	${CXX} -mmic -o libvamic.so -shared -fPIC ${OBJECTS}


clean:
	rm *.o

