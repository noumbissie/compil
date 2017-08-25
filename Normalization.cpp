#include <Rcpp.h>
#include <vector>
#include <iostream>

using namespace Rcpp;

//' Normalize a raw matrix
//' @param dataGeneSamples Raw matrix to normalize.
//' @param vectorOfgeneslength Vector containing the length of each gene.
//' @return Normalized count matrix.
//' @examples
//' normaFreq(Matrix, vectorOfGeneslength)
//' @export
// [[Rcpp::export]]
SEXP normFreqCpp(const Rcpp::NumericMatrix& dataGeneSamples,const Rcpp::IntegerVector& vectorOfGeneslength){
  const int numGenes=dataGeneSamples.nrow();
  const int numSamples=dataGeneSamples.ncol();
  Rcpp::NumericMatrix result (numGenes,numSamples);

  int i=0,j=0;

  if (vectorOfGeneslength.length() != numGenes)
  {
    Rcpp::stop(" The vector has %d elements. %d expected.", vectorOfGeneslength.length(),numGenes);
  }

  for(j=0 ; j <numSamples ; j++){

    double sumcol=0.00000;

    for(i=0 ; i <numGenes ; i++){

      result(i,j)=static_cast<double>(dataGeneSamples(i,j))/static_cast<double>(vectorOfGeneslength[i]);
      sumcol+=static_cast<double>(result(i,j));

    }
    for(i=0 ; i<  numGenes; i++ )

      result(i,j)=result(i,j)/sumcol;

  }

  return wrap(result);

}


  // for(j=0 ; j< numSamples; j++ ){
  //
  //
  //   for(i=0 ; i<  numGenes; i++ ){
  //
  //     sumcol+=std::static_cast<double>result(i,j);
  //
  //   }
  //   for(i=0 ; i<  numGenes; i++ ){
  //     result(i,j)=result(i,j)/sumcol;
  //   }
  // }

  // for(i=0 ; i<  numGenes; i++ ){
  //   for(j=0 ; j< numSamples ; j++ ) std::cout<<result(i,j)<<" ";
  //   std::cout<<std::endl;
  // }

