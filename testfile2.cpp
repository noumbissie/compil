#include <Rcpp.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include<iterator>
using namespace Rcpp;
using namespace std;


// [[Rcpp::export]]
SEXP timereadfile(const std::string & filename ) {
  std::string data;
  std::ifstream infile;
  std::string valname;
    
  std::string colNames;
  std::string rowNames;
  int nRow;
  int nCol;
  
  clock_t tStart = clock();
  infile.open(filename.c_str());
  
  std::getline(infile,rowNames);
  //Rcout<<lineNames<<std::endl;

  nCol=std::distance(std::istream_iterator<std::string>(std::istringstream(rowNames) >> std::ws)
                             
                             ,std::istream_iterator<std::string>()
                             
                             );//on obtient la taille de l'echantillon 

  //std::cout<<rowNames[3]<< std::endl;
  rowNames.clear();
  
  //getline(infile,data);
  if(!infile.is_open())
    exit(EXIT_FAILURE);
  
  while(!infile.eof()){
    getline(infile,data);
    
  }
  infile.close();
  
  Rcpp::Rcout<<"Time taken to read file :";
  double timeread;
  timeread=(double(clock() - tStart)/(CLOCKS_PER_SEC));
  Rcpp::Rcout<<timeread;
  data.clear();
  return wrap(timeread);
}




/*** R
timereadfile("debug_ms_vs_igc_100000_10ind.tsv")
*/
