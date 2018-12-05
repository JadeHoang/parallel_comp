# prefix scan cumsum v1
# Author: Gauthier CASTRO
# nov 2018

# sequential, for loop
cs <- function(x){
  n <- length(x)
  # number of steps
  steps <- 1:ceiling(log2(n))
  # buffer for each step
  buff <- rep(1, n)
  # x indexes to start with, for each step
  starts = 2^(steps-1)+1
  for(start in starts){
    for (i in start:n) {
      buff[i] <- x[i] + x[i - start + 1]
    }
    x <- buff
  }
  return(x)
}
#debug(cs)
cs(x)

# sapply
lcs <- function(x){
  n <- length(x)
  steps <- 1:ceiling(log2(n))
  # sapply acts as buffer
  #buff <- rep(1, n)
  starts = 2^(steps-1)+1
  for(start in starts){
    x[start:n] <- sapply(start:n, function(i) x[i] + x[i - start + 1])
  }
  return(x)
}
#debug(lcs)
lcs(x)

library(parallel)

# Parallel sapply
# !! VERY SLOW !! not microbenchmarked
plcs <- function(x){
  xlen <- length(x)
  steps <- 1:ceiling(log2(xlen))
  starts = 2^(steps-1)+1
    cl <- makeCluster(4)
  for(start in starts){
    clusterExport(cl, list("x", "start"))
    x[start:length(x)] <- parSapply(cl, start:length(x), FUN = function(i) x[i] + x[i - start + 1])
  }
    stopCluster(cl)
  return(x)
}
plcs(x)

library(foreach)
## foreach
fcs <- function(x){
  xlen <- length(x)
  steps <- 1:ceiling(log2(xlen))
  buff <- rep(1, xlen)
  starts = 2^(steps-1)+1
  for(s in starts){
    buff[s:xlen] <- foreach(i = s:xlen, .combine = "c") %dopar% (x[i] + x[i - s + 1])
    x <- buff
  }
  return(x)
}
#debug(fcs)
fcs(x)

library(doParallel)
# parallel foreach
pfcs <- function(x){
  xlen <- length(x)
  steps <- 1:ceiling(log2(xlen))
  buff <- rep(1, xlen)
  starts = 2^(steps-1)+1
  registerDoParallel(cores = 4)
  for(s in starts){
    # combine 'c' yields vector of results
    buff[s:xlen] <- foreach(i = s:xlen, .combine = "c") %dopar% (x[i] + x[i - s + 1])
    x <- buff
  }
  stopImplicitCluster()
  return(x)
}
pfcs(x)

# compare compiled version of seq verison and sapply
library(compiler)
ccs <- cmpfun(cs)
clcs <- cmpfun(lcs)
library(microbenchmark)
library(ggplot2)
x <- 1:20
sum(x)
# environ 15 sec
compare <- microbenchmark(pfcs(x), fcs(x), lcs(x), ccs(x), cs(x), sum(x), times = 50)
autoplot(compare)

