#include <Rcpp.h>
#include <omp.h>
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

// parallel analog of cumsum( ) , using OMP


// input vector x ,number of desired threads nth
RcppExport SEXP ompcumsum(SEXP x,SEXP nth)
{
  Rcpp::NumericVector xa(x) ;
  int nx = xa.size() ;
  
  // cumulative sums vector
  double csms[nx] ;
  
  // set number of threads , and allocate adjustment
  // values vector ; INTEGER is a SEXP construct
  int nthreads = INTEGER(nth)[0] ;
  omp_set_num_threads(nthreads);
  
  
  // space to store the block endpoint sums
  double adj [nthreads-1] ;
  
  int chunksize = nx / nthreads ;

  // output vector
  Rcpp::NumericVector csout(nx) ;

  #pragma omp parallel
  {
    std::cout<<omp_get_num_threads()<<std::endl;
    int me = omp_get_thread_num() ;
    int mystart = me * chunksize ,
        myend = mystart + chunksize - 1 ;
    if (me == nthreads - 1) myend = nx - 1 ;
    int i ;
    
    // do cumulat ive sums for my chunk
    double sum = 0 ;
    for ( i = mystart ; i <= myend ; i++) {
      sum += xa[i] ;
      csms [i] = sum;
    }
    
    // find adjustment values
    //
    // first , make sure all the chunk cumsusm
    // are ready
    #pragma omp barrier
    // only one thread need compute and
    // accumulate the r i ght􀀀hand endpoint s
    #pragma omp single
    {
      adj[0] = csms[chunksize - 1];
      if (nthreads > 2)
        for (i = 1 ; i < nthreads - 1; i++){
          adj[i] = adj[i - 1] + csms[(i + 1)*chunksize - 1];
        }
    }  
    // implied barrier at the end of any
    // 'single' pragma
    // do my adjustments
    double myadj ;
    if (me == 0) myadj = 0;
    else myadj = adj[me - 1];
    for (i = mystart; i <= myend; i++){
      csout[i] = csms[i] + myadj ;
    }
}
// implied barrier at the end of any
// 'parallel' pragma
    return csout ;
}
  


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically 
// run after the compilation.
//


