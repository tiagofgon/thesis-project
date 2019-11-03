VPATH = include src

EXECS = prarea minval addvec feach foreach invoke \
        invokefct tg1 tg2 tg3 dbtbb

include makefile-compiler

addvec: AddVec.o
	${CXX} -oaddvec AddVec.o -ltbb
	rm *.o

feach:  for_each.o
	${CXX} -ofeach for_each.o -ltbb
	rm *.o

foreach: ForEach.o
	${CXX} -oforeach ForEach.o -ltbb
	rm *.o

invoke: Invoke.o
	${CXX} -oinvoke Invoke.o -ltbb
	rm *.o

invokefct: InvokeFct.o
	${CXX} -oinvokefct InvokeFct.o -ltbb
	rm *.o

prarea: PRarea.o
	${CXX} -oprarea PRarea.o -L../vath_pth -lvath -ltbb
	rm *.o

minval: MinVal.o
	${CXX} -ominval MinVal.o -ltbb
	rm *.o

tg1:    Tg1.o
	${CXX} -otg1 Tg1.o -ltbb 
	rm *.o

tg2:    Tg2.o
	${CXX} -otg2 Tg2.o -ltbb 
	rm *.o

tg3:    Tg3.o
	${CXX} -otg3 Tg3.o -ltbb 
	rm *.o

dbtbb:  DbTbb.o
	${CXX} -odbtbb DbTbb.o -ltbb 
	rm *.o

#"""""""""""""""""""""""""""""""""""
# Subsidiary targets, including test
# execution with 2 threads, to verify
# that everything is OK.
# """"""""""""""""""""""""""""""""""

clean:
	rm -f ${EXECS}

alltbb:  ${EXECS}

.PHONY: runtbb
runtbb:
	./prarea
	./minval 
	./addvec
	./feach
	./foreach
	./invoke
	./invokefct
	./tg1
	./tg2
	./tg3
	./dbtbb

