
testSorted: sortedClient.c ../../src/sortedMatrix/*.c
	#  -std=gnu11 used to enable clock_gettime().  Does not work on -std=c11
	#  c11 used for the bool type
	gcc ../../src/dataMatrix/dataMatrixADT.c ../../src/sortedMatrix/*.c sortedClient.c -o testSorted -std=gnu11

clean: 
	rm testSorted 

