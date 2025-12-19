
testBooster: ../../src/gradientTree/gradientTreeADT.* ../../src/booster/* boosterClient.c ../../src/metrics/*
	gcc -g ../../src/booster/boosterADT.c boosterClient.c ../../src/gradientTree/gradientTreeADT.c ../../src/dataMatrix/dataMatrixADT.c ../../src/sortedMatrix/*.c ../../src/metrics/*.c -o testBooster -std=gnu11 -lm

clean: 
	rm testBooster

