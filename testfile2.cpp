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
SEXP timereadfile(const std::string & filename,bool header) {
  std::string data;
  std::ifstream infile;
  std::string valname;
    
  std::string colNames;
  std::string rowNames;
  int nRow=0;
  int nCol;
  
  clock_t tStart = clock();
  infile.open(filename.c_str());
  
  //test existence of file 
  if(!infile.is_open())
    exit(EXIT_FAILURE);
  
  std::getline(infile,rowNames);//get first line 

  //get column dimension
  nCol=std::distance(std::istream_iterator<std::string>(std::istringstream(rowNames) >> std::ws)
                             
                             ,std::istream_iterator<std::string>()
                             
                             );//on obtient la taille de l'echantillon 

  //clear rownames string 
  rowNames.clear();
  
  //get dimension row
  while(!infile.eof()){
    getline(infile,data);
    nRow++;
    
  }
  if(header){nRow=nRow-1;}
  
  /*we need get data inside file */
  
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
