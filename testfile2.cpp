#include <Rcpp.h>
//#include <Rcpp11.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iterator>
#include <vector>
using namespace Rcpp;
using namespace std;



// [[Rcpp::export]]

SEXP timereadfile(const std::string & filename,bool header) {
  
  /*Debut partie declarattion de variable */
  std::string data;
  std::ifstream infile;
  std::string valname;
  double val;
  
  std::string colNames;
  std::string rowNames;
  std::string getColNames;
  //Rcpp::NumericMatrix
  int nRow=1;//Dimension of the matrix row
  int nCol=0; //Dimension of the matrix column
  
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
  
  /*creation of colnames of our matrices */
  std::istringstream ss(getColNames) ;
  StringVector vectorOfColnames(nCol);//vector of colnames
  StringVector vectorOfRownames;//vector of colnames
  
  for(int i = 0 ;i < nCol ; i ++){
    ss >> valname;
    vectorOfColnames(i)=valname;
  }
  
  
  /*clear Colnames and valname string*/
  valname.clear();
  getColNames.clear();
  
  
  /*get dimension row,size of gene catalogue and fill directly matrice*/
  std::vector<std::vector<double> > vectorOfval;
  
  while(!infile.eof()){
    std::getline(infile,data);
    std::istringstream ss(data);
    std::string::size_type sz; 
    ss >> valname;
    vectorOfRownames.push_back(valname);
    // on ajoute un nouveau vecteur de double pour la ligne en cours
    vectorOfval.push_back(std::vector<double>(nCol-1));
    //ss.flush();
    for(int i = 1 ;i < nCol ; i ++){
      ss >> val;
      // on affecte la valeur de la colonne en cours (i-1)
      vectorOfval.back().at(i-1) = val;
      //ss.flush();
      Rcout<<vectorOfval[i][i];
    }
    Rcout<<" "<<std::endl;
    //nRow++;
  }
  Rcout<<"size "<<vectorOfval.size()<<endl;
  Rcout<<"size of row "<<vectorOfval.size()<<endl;
  //if(header){nRow--;}
  printf("v[3][8]: %.20lf\n",vectorOfval[1][5]); 
  Rcout<<"v[3][8]: "<<vectorOfval[3][3]<<endl;
  
  /*we need get data inside file */
  
  infile.close();
  
  Rcpp::Rcout<<"Time taken to read file :";
  double timeread;
  timeread=(double(std::clock() - tStart)/(CLOCKS_PER_SEC));
  Rcpp::Rcout<<timeread;
  data.clear();
  return wrap(timeread);
}


mult2_code = "

  Rcpp::NumericMatrix xcpp(x);
  int nr = xcpp.nrow();
  int nc = xcpp.ncol();
  double kcpp = Rcpp::as<double>(k);

  for (int i = 0; i < nr; i++){
    for (int j = 0; j < nc; j++){
        xcpp[nr * j + i] *= kcpp;
    }
  }
  "

/*Partie du code en R */

/*** R
data<-timereadfile("head_6testfile.tsv",TRUE)
*/

