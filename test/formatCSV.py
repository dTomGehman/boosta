#Created for crohn.csv specifically; certain lines would have to be adjusted for different files

import numpy as np
import pandas as pd


df = pd.read_csv('crohn.csv')
df = df.drop(["num","pid","id","fid","mid","sex"],axis=1) #Excludes categorical data

X, y = df.drop("crohn", axis=1), df[["crohn"]]

y=np.ravel(y)
rows, cols = len(X.index), len(X.columns)

print(rows, cols)	   #Prints number of obserations and features
print("Response", end=" ") #Response correlates to crohn
labels = [*X]
for k in range (0, len(labels)):
	print(labels[k], end=" ");  #Print feature names
print("")
for i in range(0,rows):
	print(int(y[i]/2), end=" ")  #Divides by 2 so that label is either 0 or 1
	for j in range(0,cols):
		print(X.iloc[i,j], end=" ")
	print("")
