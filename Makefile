
run := mpirun
mcc  := mpicc
cc := gcc


all: 
	$(cc) DataGenerator.c -o DataGenerator
	$(mcc) RedToroide.c -o RedToroide

generateData16:
	./DataGenerator 16

toroide_16:
	$(run) -n 16 ./RedToroide