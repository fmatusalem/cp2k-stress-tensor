# cp2k-stress-tensor
Program to extract stress tensor components from CP2K output and compute moving averages.

This program is only to extract the stress tensor components from the standart output of CP2k MD calculation. When running CP2K redirect the output to a file and use this program to extract the stress tensor components and print to a file. The program also compute the moving averages. 

-f [string] The name of the cp2k output file must be entered!! Option -f filename
-e [float] Can be entered to eliminate some first steps (in percentage). 
-p [float] Multiply the step number by a value. Useful to convert the step number to time or percentage of strain.
-a [int] Enter the number of steps to compute moving average. Default 3%% MD steps
-h print this help.


compilation:

g++ -o stress_tensor-cp2k-3.1.0.x stress_tensor-cp2k.c


