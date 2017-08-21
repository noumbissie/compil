 #Hierclust
##Optimisation de la fonction Hierclust  
##Author: Gaelle T.noumbissie
#le programme a été en script R 


#--Definition de Hierclust--#

	*Hierclust est une routine du package momr de R,elle permet de calculer la correlation entre differents paires d'individus 
	de la matrice données

#---Principe de la methode ---#

	 1*definir un nombre de repetition
	 
	 2**definir une liste contenant les nombres de gènes à tester 
	 
	 3***Obtenir un nombre de gène par un ré-echantillonage et calculer la correlation avec la fonction hierclust 
	 autant de fois qu'il yaura de repetition (donc on aura autant de matrice de correlation autant de repetition)
	 
	 4****calculer la moyenne des correlations sur la matrice précedente,obtenir en sortie une liste de valeurs correspondant 
	 aux correlation des differentes paires d'individus pour le nombre de gènes defini.
	 
	 5******definir une matrice qui aura sur ses lignes les paires d'individus ,et sur les colonnes les differents nombre de genes,
	 chaque colonne n'est rien dautre que le vecteur precédent .
    
#--Execution de la Fonction--#
	1*les paramètres d'entrée sont :
		**le nombre minimum de gènes 
		***le nombre max de gènes 
		****le pas d'incrementation du nombre de gènes 
		*****le nombre de repetitition pour le re-échantillonage(ou non si le nombres de gène et egale au         catalogue de gène c'est-à-dire le nombre de ligne de la matrice)
		
 2**Il faudrait au préalable installé le package *momr de base 

 3***Le nom de la fonction est testHierclustNumberOfgene.R
 
 4****Pour l'utiliser dans son script R que 
