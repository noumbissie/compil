#Hierclust
##Optimisation de la fonction Hierclust  
##Author: Gaelle T.noumbissie
#le programme a été en script R 


#--Definition de Hierclust--#

	*Hierclust est une routine du package momr de R,elle permet de calculer la correlation entre differents paires d'individus 
	de la matrice données

#---Principe de la methode ---#

	 *definir un nombre de repetition
	 
	 **definir une liste contenant les nombres de gènes à tester 
	 
	 ***Obtenir un nombre de gène par un ré-echantillonage et calculer la correlation avec la fonction hierclust 
	 autant de fois qu'il yaura de repetition (donc on aura autant de matrice de correlation autant de repetition)
	 
	 ****calculer la moyenne des correlations sur la matrice précedente,obtenir en sortie une liste de valeurs correspondant 
	 aux correlation des differentes paires d'individus pour le nombre de gènes defini.
	 
	 ******definir une matrice qui aura sur ses lignes les paires d'individus ,et sur les colonnes les differents nombre de genes,
	 chaque colonne n'est rien dautre que le vecteur precédent .
    
#--Execution de la Fonction--#
	*les paramètres d'entrée sont :
		*le nom fichier contenant la matrice
		**le nombre minimum de gènes 
		***le nombre max de gènes 
		****le pas d'incrementation 
		*****le nombre de repetitition pour le re-échantillonage
	**Il faudrait au prealable installé le package *momr de base 
	
	
	
		*
