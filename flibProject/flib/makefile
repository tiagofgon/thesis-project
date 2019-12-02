# This makefile builds the C++14, implementation of the "libflib" library
#
# ============================================================

VPATH = src include 

OBJECTS = BLock.o BkBarrier.o InputList.o TaskCentricPool.o  ThreadCentricPool.o  ThreadMgr.o  Task.o 

#"""""""""""""""""""""""""""""""""""""""""""""""
# Variables concerning include and library paths
# """"""""""""""""""""""""""""""""""""""""""""""
PATHVA = ../libflib/ 
CPPFLAGS = -I include -I ../libflib/include 


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
# This part deals with the static library libflib
# """""""""""""""""""""""""""""""""""""""""""""""""""

libva:  libflib.a
libflib.a: libflib.a(BLock.o) libflib.a(ThreadCentricPool.o)
libflib.a: libflib.a(TaskCentricPool.o) libflib.a(BkBarrier.o) 
libflib.a: libflib.a(Task.o) libflib.a(ThreadMgr.o)
libflib.a: libflib.a(InputList.o)
libflib.a: clean

libflib.a(BkBarrier.o): BkBarrier.o BkBarrier.hpp
	$(AR) $(ARFLAGS) $@ $<

libflib.a(BLock.o): BLock.o BLock.hpp
	$(AR) $(ARFLAGS) $@ $<

libflib.a(TaskCentricPool.o): TaskCentricPool.o TaskCentricPool.hpp
	$(AR) $(ARFLAGS) $@ $<

libflib.a(ThreadCentricPool.o): ThreadCentricPool.o ThreadCentricPool.hpp
	$(AR) $(ARFLAGS) $@ $<

libflib.a(InputList.o): InputList.o InputList.hpp
	$(AR) $(ARFLAGS) $@ $<

libflib.a(Task.o): Task.o Task.hpp
	$(AR) $(ARFLAGS) $@ $<

libflib.a(ThreadMgr.o): ThreadMgr.o ThreadMgr.hpp
	$(AR) $(ARFLAGS) $@ $<

#"""""""""""""""""""""""""""""""""""""""""""""
# This part deals with the dynamic libraries 
# libflib.so and libvamic.so
# """"""""""""""""""""""""""""""""""""""""""""

libflib.so:  ${OBJECTS}
	${CXX} -o libflib.so -shared -fPIC ${OBJECTS}

libvamic.so:  ${OBJECTS}
	${CXX} -mmic -o libvamic.so -shared -fPIC ${OBJECTS}


clean:
	rm *.o

