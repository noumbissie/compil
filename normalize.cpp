#include <Rcpp.h>
#include <vector>
using namespace Rcpp;

//' Normalize a raw matrix
//' @param dataGeneSamples Raw matrix to normalize.
//' @param vectorOfgeneslength Vector containing the length of each gene.
//' @return Normalized count matrix.
//' @examples
//' normaFreq(Matrix, vectorOfGeneslength)
//' @export
// [[Rcpp::export]]
SEXP normFreq(const Rcpp::IntegerMatrix& dataGeneSamples,const Rcpp::IntegerVector& vectorOfGeneslength){
  const int numGenes=dataGeneSamples.nrow();
  const int numSamples=dataGeneSamples.nrow();
  Rcpp::NumericMatrix result(numGenes,numSamples);
  
  if (vectorOfGeneslength.length() != dataGeneSamples.nrow())
  {
    Rcpp::stop(" The vector has %d elements. %d expected.", vectorOfGeneslength.length(),numGenes);
  }
  
  for(int i=0,i< dataGeneSamples.ncol() ,i++){
    
    resultat[,i]=dataGeneSamples[,i]/numGenes;
    
  }
  
  for(int i=0,i< dataGeneSamples.ncol() ,i++){
    
    resultat[,i]=resultat[,i]/sum(resultat[,i]);
    
  }
  
  return wrap(result);
}
  
  
