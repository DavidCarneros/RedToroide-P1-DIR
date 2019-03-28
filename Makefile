
run := mpirun
mcc  := mpicc
cc := gcc



generateData16:
	$(cc) DataGenerator.c -o DataGenerator
	./DataGenerator 16

toroide_L4:
	$(mcc) RedToroide.c -o RedToroide
	$(run) -n 16 ./RedToroide