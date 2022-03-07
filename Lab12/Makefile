.PHONY: all
.PHONY: mem
.PHONY: test
.PHONY: help
.PHONY: base
.PHONY: correction
.PHONY: x
.PHONY: plot

.SILENT: 

VAL = valgrind --tool=memcheck --leak-check=yes --track-origins=yes
O = out.txt
TST = intTest
VERBOSE=
HW = hw12
CODE1= rootfinding.c
cc = gcc -Wall -std=c99 -lm
MEM = mem.txt
RD = realDevice
DE =detError
COR = correction
CMOD = chmod +x
RAND = rand.txt
AD = alldata.txt
FILE = @echo "proccessing file : 
clean:
	-rm -f *.o $(HW) $(MEM)  $(RAND) $(O) 

all: 
	$(cc)  $(HW).c  -lgsl -lgslcblas -g  -o $(HW)
tests: all
	$(FILE) 0   ">$(O)
	-./$(HW) -i ge0.txt>>$(O) 2>&1
	$(FILE) 1  ">>$(O)

	-./$(HW) -i ge1.txt>>$(O) 2>&1
	$(FILE) 2  ">>$(O)

	-./$(HW) -i ge2.txt>>$(O) 2>&1
	$(FILE) 3  ">>$(O)

	-./$(HW) -i ge3.txt>>$(O) 2>&1
	$(FILE) 4  ">>$(O)

	-./$(HW) -i ge4.txt>>$(O) 2>&1
	$(FILE) 5  ">>$(O)

	-./$(HW) -i ge5.txt>>$(O) 2>&1
	$(FILE) 6  ">>$(O)

	-./$(HW) -i ge6.txt>>$(O) 2>&1
	$(FILE) 7  ">>$(O)

	-./$(HW) -i ge7.txt>>$(O) 2>&1
	$(FILE) 8  ">>$(O)

	-./$(HW) -i ge8.txt>>$(O) 2>&1
	$(FILE) 9  ">>$(O)

	-./$(HW) -i ge9.txt>>$(O) 2>&1
	$(FILE) 10  ">>$(O)

	-./$(HW) -i ge10.txt>>$(O) 2>&1
	$(FILE) 11  ">>$(O)

	-./$(HW) -i ge11.txt>>$(O) 2>&1
	$(FILE) 12  ">>$(O)

	-./$(HW) -i ge12.txt>>$(O) 2>&1
	FILE 0  ">>$(O)

help:
	@echo "all: to get an executable of the hw files"
	@echo "runs a test of everything"
	@echo "clean: deletes everything made by the make"
	@echo "mem: runs Valgrind on a randomly generated piece of data"
	# @echo " VERBOSE=DVERBOSE will give u all the the iteratoins, not recc to run w/ timing enabled "

mem: all
	./genRand>$(RAND)
	$(VAL) ./$(HW) -i $(RAND)>$(MEM)