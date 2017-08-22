 #Hierclust
## re-echantillonage sur le nombre de gène pour le calcul de la corrélation avec hierClust
##Author: Gaelle T.noumbissie
#le programme a été en script R 


A--Definition de Hierclust--#

	*Hierclust est une routine du package momr de R,elle permet de calculer la correlation entre differents paires d'individus 
	de la matrice données

B---Principe de la methode ---#

	 1. définir un nombre de repetition.
	 
	 2. définir une liste L contenant des valeurs correspondant à une quantité de gène.
	 
	 3. Pour chaque valeur à l'indice i de la liste L, faire un ré-echantillonage,puis calculer la correlation "matcor" avec         la fonction hierclust
	 autant de fois qu'il yaura de repetition (si repetition=n ,nous aurions n addition de matcor).
	 
	 4. Ensuite, calculer la moyenne de la matrice "matcor",obtenir en sortie une liste de valeurs correspondant 
	 aux correlation des différentes paires d'individus pour le nombre de gènes à lindice i de L.
	 
	 5. définir une matrice qui aura sur ses lignes les paires d'individus, et sur les colonnes les différents nombre de genes de la liste L.
    
C--Execution de la Fonction--#
	1. les paramètres d'entrée sont :
	    *le nombre de gène(numberOfGene)
	    *la matrice de donnée (data.gene.sample)
		*le nombre minimum de gènes(numMinGene),par défaut égale à 1 
		*le nombre max de gènes(numMaxGene),par défaut égale à 1
		*le pas d'incrementation du nombre de gènes(step),par défaut égale à 1 
		*le nombre de repetitition pour le re-échantillonage(repetitions)
		(ou non si le nombres de gène et egale au catalogue de gène c'est-à-dire le nombre de ligne de la matrice)
		
	il est à noter ici que numberOfGene intervient lorque nous ne voulions faire que le re-échantillonage d'un seul nombre     de gène(par exemple le calcul normal de Hierclust).
	dans ce cas, repetitions,step,numMinGene,numMaxGene doivent rester égale à 1 
	
	2. Il faudrait au préalable installé le package *momr de base 

	3. Le nom de la fonction est testHierclustNumberOfgene.R
 
    4. Pour l'utiliser dans son script R que 
