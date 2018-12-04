# Parallel computing project

Each function (ompcumsum, rthma, rthma_lambda) has 2 others differents file .o and .so. These files are the result of the line in the terminal.
```
R CMD SHLIB file_name.cpp
```
Change to Console to load the function 
```
dyn.load("file_name.so")
```
To call out the function in Console:
```
.Call("function_name",parameter1, parameter2,...,parameter n)
```


Or these function could be simply loaded in the Console:
```
Rcpp::sourceCpp('file_name.cpp')
```
and the function is stocked in the environment of R.

