
library(data.table)
library(MASS)
library(momr)
library(dotCall64)


data_dt<-fread("ms_vs_hs_9_9_igc_rmHosts_raw_smart_shared_reads_05042017.tsv",sep="\t",header=T)

data_dt<-data_dt[,-1]
data_dt2=data_dt
nbLecture=seq(from=2,to=20,by=1) #vecteur contenant le nombre de fois que je vais multiplier la tailles des donnéesdans le fichier ms_vs_topaste.tsv ,elle est representé sous forme de vecteur 
resultime=vector(mode="numeric",length=3)
resultime=as.list(resultime)
#vecteur contenant des données recupérés pendant

readFunc<-function()
{
        data1=NULL
        for(i in nbLecture)
        {
                data1=cbind(data_dt,data_dt2)
                gc(reset=T)
                        tps<-system.time(expr={
                            # hierclust<-Hmisc::rcorr(as.matrix(data1), type ="spearman")$r
			 hierclust<-hierClust(as.matrix(data1), side = "col", dist = "correlation", cor.type = "spearman",
  hclust.method = "ward", side.col.c = NULL, side.col.r = NULL,
  plot =FALSE)$mat.rho

                        })
                rm(hierclust)
                print("clustering has been done .....")
                gcval<-gc()
                resultime<-as.list(c(i,tps["elapsed"],gcval[2,4]))

                #gc()garbagge collector vide de mémoire inocccupée
                           # i represente le nombre de fois que le fichier ms_vs* 
                               #est multiplié pour pouvoir etre lu par le lecteur de fichier 
                 #tps["elapsed"] represente le temps que mets le fichier pour
                                                    #pour finir sa lecture du fichier  
                 #gcval[] représente la valeur de la mémoire libérée par l'objet data_dt1  

                fwrite(resultime,"/export/mgps/home/gtietcheu/ShRCAn2/HierclustCor/resultimeHierclust.tsv",append=TRUE,row.names=FALSE,col.names=FALSE,sep="\t")

         }
}
readFunc()
rm(list=ls())            






