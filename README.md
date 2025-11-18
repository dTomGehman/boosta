# boosta
Reimplementation of XGBoost

This is still in progress.  

Right now, we have a simple working gradient booster.  To test it, run the makefile at test/booster/makefile, and run testBooster \[file\].  test/dat4 is the Crohn's disease data formatted for our program.  

The program will print out the structures of each tree it constructs and the weights, predicted labels, and actual labels of each training and testing observation.  It also prints the accuracy of the training and testing sets.  We'll add in some time measurements and other measurements of correctness shortly, as well as other hyperparameters to adjust the model.  
