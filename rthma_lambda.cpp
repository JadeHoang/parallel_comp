#include <thrust/device_vector.h>
#include <thrust/scan.h>
#include <thrust/transform.h>
#include <thrust/functional.h>
#include <thrust/sequence.h>
#include <Rcpp.h>
#include "backend.h" // from Rth
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
  //
  //   http://www.rcpp.org/
  //   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
  //
  
  // [[Rcpp::export]]

//struct minus_and_divide now deleted
RcppExport SEXP rthma_lambda(SEXP x, SEXP w, SEXP nthreads)
{
  Rcpp::NumericVector xa(x);
  int wa = INTEGER(w)[0];
  
  #if RTH_OMP
  omp_set_num_threads(INTEGER(nthreads)[0]);
  #elif RTH_TBB
  tbb::task_scheduler_init init(INT(nthreads));
  #endif
  
  //set up device vector and copy xa to it 
  thrust::device_vector<double> dx(xa.begin(), xa.end());
  
  int xas = xa.size();
  if (xas < wa) return 0;
  
  //allocate device storage for cumulative sims,
  //sand compute them
  thrust::device_vector<double> csums(xa.size() + 1);
  thrust::exclusive_scan(dx.begin(), dx.end(), csums.begin());
  //need one more sum at (actually past) the end
  csums[xas] = xa[xas - 1] + csums[xas - 1];
  
  //compute moving averages from cumulative sums
  Rcpp::NumericVector xb(xas - wa + 1); 
  
  //change code
  thrust::transform(csums.begin() + wa, csums.end(), csums.begin(), xb.begin(), [=] (double & a, double & b){ return((a - b)/wa);
});
  
  return xb;
}

// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//
  
  