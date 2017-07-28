library(data.table)
library(MASS)
library(Matrix)
library(momr)
library(stringr)

#enregistrement des paramètres

string_args=commandArgs(TRUE)
filepath=as.character(string_args[1]) #contient le nom du fichier ainsi que son chemin 
min=as.numeric(string_args[2]) #le nombre min de gènes 
max=as.numeric(string_args[3])#le nombre max de gènes 
step=as.numeric(string_args[4]) #le pas d'incremaentation du nombre de genes
repetitions=as.numeric(string_args[5])
print("#---DEBUT---#")

data_dt<-fread(filepath,sep = "\t",header=T) #matrice a 150 individus

valrow<-nrow(data_dt)#taille de la strucure de données 

filename=

data_dt<-data_dt[,-1]#extraction de la structure de données sans la premiere colonne

tirage<-seq(from = min,to =max, by = step ) #sequence des aifferentes tailles de gènes qui pourront etre obtenu par
                                                       #tirage aléatoire

dimrow_data_plot=( ncol(data_dt)*ncol(data_dt) - ncol(data_dt) ) /2 #nombre de ligne de la matrice de correlation data_plot,cest la matrice tri_inferieure de data_dt                                            

data_plot<-as.data.frame(matrix(0,nrow = dimrow_data_plot,ncol = length(tirage)))#matrice de correlation entre differents individus
                                                                  #sur la ligne represente les correlations et les colonnes
                                                                  #on a le differentes tailles des gènes de du vecteur nbgene,obtenu
                                                                  #par tirage aléatoire
indice<-1

#cette variable represente le nombre de répétition d'un tirage aléatoire

for(i in tirage){
  if(i < nrow(data_dt)){
  for(j in 1:repetitions){
	  
    listgene<-sample(1:valrow,i)#appel de la fonction fill_list()
    print("outside of hierclust .....!!")
    hierclust<-hierClust(data=data_dt[listgene,], side = "col", dist = "correlation", cor.type = "spearman",
                         hclust.method = "ward", side.col.c = NULL, side.col.r = NULL,
                         plot = FALSE)
    print("I am there already .....!! ")
                                       #calcul de la correlation entre toutes les individus de la matrice
                                       #mat avec ma méthode de spearman qui permet d'obtenir en plus la p-value
                                       #qui permettra de savoir si la corrélation est significative
    if(j>1){
      matcor<-matcor+hierclust$mat.rho #matrice de correlation des individus eyant la quantité listgen de gene
    }else{
      matcor<-hierclust$mat.rho
    }
  }
  matcor<-matcor/(repetitions) #calcul de la moyenne des correlations par le nombre de repetitions

  }else{ #cette condition permet deviter de faire un tirage de 10M dans 10M
    hierclust<-hierClust(data=data_dt[listgene,], side = "col", dist = "correlation", cor.type = "spearman",
                         hclust.method = "ward", side.col.c = NULL, side.col.r = NULL,
                         plot = FALSE)
    matcor<-hierclust$mat.rho
  }

  liste=c()
  s=1 #indice d'incrementation du vecteur liste 
  for(l in 2:nrow(matcor)){
    for(k in 1:ncol(matcor)){
      if(k<l){
        liste[s]=matcor[l,k]#cette liste recupère la valeur des correltions dans la matrice de correlation matcor
                            #seulla partie triangulaire est recuperée vu qu'elle est symetrique
        s=s+1
      }
    }
  }
  data_plot[,indice]=liste
  indice=indice+1

  print(i)
  print(object.size(matcor))
}
#print

dataplot_names=as.list(vector(mode="character",length = nrow(data_plot)))
e=1
for(l in 1:ncol(data_dt)){
  for(k in 1:ncol(data_dt)){
    if(k<l){
      dataplot_names[e]=paste(colnames(data_dt)[l],colnames(data_dt)[k],sep=".")#cette instructon permet de renommer les ligne dela
      e=e+1                                                                      #matrice de correlation data_plot,permettant de connaitre quelle
                                                                            #correlation pour quel nombre de gène
    }
  }
}
rownames(data_plot) = make.names(dataplot_names, unique=TRUE)#nommer la correlation pour situer sur
                                                             #quelle paire d'individu il ya correlation                                                                                                                         
colnames(data_plot)=make.names(as.character(tirage),unique=TRUE) 
 
#définition du nom de la matrice de corelation en fonction du parametre entré
filename=str_c(str_c(str_c("matCorr",as.character(ncol(data_dt)),sep="_"),str_c(as.character(min),as.character(max),sep="_"),sep="_"),"RDS",sep=".")

#enregistrement en fichier RDS de la matrice de correlation dataplot
saveRDS(data_plot, file = filename) #pour sauvegarder la matrice data_plot
                                                                        #matcor = readRDS("/Users/gtietcheu/Desktop/matcor.RDS")
print("#---FIN---#")
