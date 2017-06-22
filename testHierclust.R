
library(data.table)
library(MASS)
#library(bigmemory)
#library(microbenchmark)
library(Matrix)
library(momr)
library(ggplot2)


data_dt<-fread("ms_vs_hs_9_9_igc_rmHosts_raw_smart_shared_reads_05042017.tsv",sep = "\t",header=T) #matrice a 150 individus

valrow<-nrow(data_dt)#taille de la strucure de données 
data_dt<-as.data.frame(data_dt[,-1])#extraction de la structure de données sans la premiere colonne

tirage<-seq(from = 10000, to = 10000000,by=100000 ) #sequence des aifferentes tailles de gènes qui pourront etre obtenu par
                                                       #tirage aléatoire

dimrow_data_plot=( ncol(data_dt)*ncol(data_dt) - ncol(data_dt) ) /2 #nombre de ligne de la matrice de correlation data_plot,cest la matrice tri_inferieure de data_dt                                            

data_plot<-as.data.frame(matrix(0,nrow = dimrow_data_plot,ncol = length(tirage)))#matrice de correlation entre differents individus
                                                                  #sur la ligne represente les correlations et les colonnes
                                                                  #on a le differentes tailles des gènes de du vecteur nbgene,obtenu
                                                                  #par tirage aléatoire
indice<-1

#fonction de tirage aleatoire(on aurait pu ne pas le definir et utiliser directement sample)
fill_list<-function(taille,valrow){
  listcor<-vector(mode = "numeric", length = taille)
  listcor=sample(1:valrow,taille)
  return (listcor)
}

repetitions = 15 #cette variable represente le nombre de répétition d'un tirage aléatoire

for(i in tirage){
  #{
  for(j in 1:repetitions){
    listgene<-fill_list(i,valrow)#appel de la fonction fill_list()
             
    hierclust<- Hmisc::rcorr(as.matrix(data_dt[listgene,]), type = "spearman")$r
                                       #calcul de la correlation entre toutes les individus de la matrice
                                       #mat avec ma méthode de spearman qui permet d'obtenir en plus la p-value
                                       #qui permettra de savoir si la corrélation est significative
    if(j>1){
      matcor<-matcor+hierclust #matrice de correlation des individus eyant la quantité listgen de gene
    }else{
      matcor<-hierclust
    }
  }
  matcor<-matcor/(repetitions) #calcul de la moyenne des correlations par le nombre de repetitions

  #}else{ #cette condition permet deviter de faire un tirage de 10M dans 10M
   # hierclust<-Hmisc::rcorr(as.matrix(data_dt[listgene,]), type ="spearman")$r
    #matcor<-hierclust
  #}

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
}
dataplot_names=vector(mode="character",length = nrow(data_plot))
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
saveRDS(data_plot, file = "matcor150_ind_bis.RDS") #pour sauvegarder la matrice data_plot
                                                                        #matcor = readRDS("/Users/gtietcheu/Desktop/matcor.RDS")
