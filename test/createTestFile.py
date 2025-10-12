#Generate random file for testing; takes in the number of observations and features
#run: python createTestFile.py numObs numFeats [seed]

from random import randint, seed, uniform
from sys import argv

obs, feats = argv[1], argv[2]
print(obs, feats)
print("Response", end=' ')
if (len(argv) > 3): seed(argv[3])
for i in range(1,int(feats)+1): 
	print(f"Pred{i}", end=' ')
for j in range(0,int(obs)):
	print()
	response = randint(0,1)
	print(response, end=' ')
	for k in range(0,int(feats)):
		print(f"{uniform(1,5):.2f}", end=' ')         #1 and 5 are arbitrary; can be adjusted to whatever needed
print()
