#include <Rcpp.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include<iterator>
#include <vector>
using namespace Rcpp;
using namespace std;


// [[Rcpp::export]]

SEXP timereadfile(const std::string & filename,bool header) {
  
  /*Debut partie declarattion de variable */
  std::string data;
  std::ifstream infile;
  std::string valname;
    
  std::string colNames;
  std::string rowNames;
  std::string getColNames;
  
  int nRow=1;//Dimension of the matrix row
  int nCol=0;//Dimension of the matrix column
  
  /*fin Partie declaration de variable */
  
  
  clock_t tStart = std::clock();
  infile.open(filename.c_str());
  
  /*test existence of file*/ 
  if(!infile.is_open())
  exit(EXIT_FAILURE);
  
  
  /*get first line of the file*/
  std::getline(infile,getColNames);

  /*get column dimension,the size of samples*/ 
  nCol=std::distance(std::istream_iterator<std::string>(std::istringstream(getColNames) >> std::ws)
                             
                             ,std::istream_iterator<std::string>()
                             
                             );
  /*get dimension row,size of gene catalogue*/
  while(!infile.eof()){
    std::getline(infile,data);
    
    nRow++; 
  }
  if(header){nRow--;} 
  
  /*creation of colnames of our matrices */
  std::istringstream ss(getColNames) ;
    StringVector vectorOfColnames(nCol);//vector of colnames
    
    for(int i = 0 ;i < nCol ; i ++){
    	ss >> valname;
    	vectorOfColnames(i)=valname;
    }
    
    
  /*clear rownames and valname string*/
  valname.clear();
  getColNames.clear();
 

  /*we need get data inside file */
  
  infile.close();
  
  //Rcpp::Rcout<<"Time taken to read file :";
  double timeread;
  timeread=(double(std::clock() - tStart)/(CLOCKS_PER_SEC));
 // Rcpp::Rcout<<timeread;
  data.clear();
  return wrap(timeread);
}


/*** R
timereadfile("ms_vs_topaste.tsv",TRUE)
*/
