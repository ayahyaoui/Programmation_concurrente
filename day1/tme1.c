#include "tme1.h"

/*
*	Fichier principales contenant:
*	creation productueur/consommateur
*	conversion d'un void* vers producteur/consommateur 
*		(sert notamment a envoyer les info au thread (pthread_create))
*	routine producteur/consommateur (plus de detail dans la fonction
*/

// Variable globale
int CPT; // partage par tout les consommateur
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int DEBUG = 0;
extern int VERBOSE;

// ============================ Creation =====================================================

t_producteur	make_producteur(fifo *f, char *nom, int cible)
{
	t_producteur p;
	p.f = f;
	p.nom = strdup(nom);
	p.cible = cible;
	return (p);
}

t_consommateur	make_consommateur(fifo *f, int ref)
{
	t_consommateur t;
	t.f = f;
	t.ref = ref;
	return (t);
}


// ============================ Convertion =====================================================

t_producteur	void_to_producteur(void *produ)
{
	t_producteur *tmp = (t_producteur*) produ;
	t_producteur p;
	p.f = tmp->f;
	p.nom = tmp->nom;
	p.cible = tmp->cible;
	return p;
}

t_consommateur	void_to_consommateur(void *conso)
{
	t_consommateur *tmp = (t_consommateur*) conso;
	t_consommateur c;
	c.f = tmp->f;
	c.ref = tmp->ref;
	return c;
}


// ============================ routine =====================================================
/*
*	Methode: producteur
*	Parametre: 
*	Description:
*	todo: remplacer les sleep par des wait et envoyer des signal a chaque fois
*		qu'un producteur produit (pour reveiller les consommateur "wait")
* 	
*	derniere mise a jour: sorti de la creation des produix du miutex
*   objectif: limiter au maximum les actions non parralelle
*/
void *producteur(void *args)
{
	t_producteur 	p = void_to_producteur(args);
	char 			*str = calloc(10,1);
	char 			*nom_pacquet;
	int 			produits = 0;
	int				has_product = 0;
	int 			was_full = 0;

	while(produits < p.cible)
	{
		if (!has_product)
		{
			sprintf(str, "%d", produits);
			nom_pacquet = strcjoin((char*)p.nom, str, ' ');	
			has_product = 1;
		}
		if (!isfull(p.f))
		{
			pthread_mutex_lock(&mutex);
			if (isfull(p.f)) // tres rare mais theoriquement possible 
			{
				if (VERBOSE > 0)
					printf("Incroyable un autre producteur viens de me passer sous le nez\n");
				pthread_mutex_unlock(&mutex);
				break;	
			}	
			enfile(p.f,nom_pacquet);
			
			free(nom_pacquet); // element copier dans la file peu supp l'originale
			produits++;
			has_product = 0;
			was_full = 0;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			if (!was_full)
				printf("tapis plein reste ([%d] non produits)\n", CPT - p.f->size);
			usleep(10000);
			was_full = 1;
		}
	}

	pthread_mutex_lock(&mutex);
	DEBUG ++;
	pthread_mutex_unlock(&mutex);
	printf("Producteur de <%s> a terminer ca journee (%d ont deja terminer)\n", p.nom, DEBUG);
	free(str);
	return 0x0;
}

/**
*	Methode: consommateur
*	Parametre: un void* contenant un t_consommateur (acces a la fifo plus une ref)
*	Description:
*
*	Note: was_empty sert a ne pas afficher successivement des message a chaque fois q'un
*			conssomateur ne retrouve rien a mange
*
*	todo: remplacer les sleep par des wait et envoyer des signal a chaque fois
*		d'un consommateur mange
**/
void *consommateur(void *conso)
{
	
	t_consommateur c = void_to_consommateur(conso);
	char *s;
	int was_empty = 0;
	//printf("{address = %p}\n", c.f->tapis[0]);
	
	while(CPT > 0)
	{
		if (!isEmpty(c.f))
		{
			pthread_mutex_lock(&mutex);
			if (isEmpty(c.f)) // tres rare mais theoriquement possible 
			{
				if (VERBOSE > 0)
					printf("Dommage un autre consommateur viens de me depasser!\n");
				pthread_mutex_unlock(&mutex);
				break;	
			}
			s = defile(c.f);
			CPT--;
			printf("Consommateur [%d] a mange <%s> \n", c.ref,s);
			usleep(10000);

			//free(s); // le packet a ete alloue par la fifo (copier) et detuit par un conso
			was_empty = 0;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			if (!was_empty)
				printf("tapis vide reste [%d] actions\n", CPT);
			usleep(10000);
			was_empty = 1;
		}
	}
	printf("consommateur <%d> rentre chez lui (couvre feu oblige)\n", c.ref);
	
	return 0x0;
}


// ============================ test complet =====================================================
/**
*	Methode: full_test
*	Parametre: aucun
*	Description:
* 
* 
* 
**/
void	full_test()
{
	fifo *f= make_fifo(SIZEFIFO);
	pthread_t *thread_prod;
	pthread_t *thread_cons;
	t_consommateur  tab_conso[NBCONS];
	t_producteur  tab_produ[NBPROD];
	int i;
	char *str = calloc(20,1);
	const int nb_fruits = 6;
	char  *tab_fruit[nb_fruits + 1] = {"Banane", "Pomme", "Poire", "Fraise", "Kiwi", "orange", 0x0};

	printf("start test ...\n nombre d'action attendu: %d\n", CPT);
	if (!(thread_prod = (pthread_t*)malloc(sizeof(pthread_t) * NBPROD)))
		error_malloc("bigtest");
	if (!(thread_cons = (pthread_t*)malloc(sizeof(pthread_t) * NBCONS)))
		error_malloc("bigtest");

	for (i = 0; i < NBPROD; i++) {
		if (i < nb_fruits)
		{
			bzero(str, 20);
			str = strcpy(str, tab_fruit[i]);
		}
		else
			sprintf(str, "fruit%d", i + 1 - nb_fruits);
		tab_produ[i] = make_producteur(f, str, CIBLE);
		pthread_create(&thread_prod[i],NULL,producteur,(void*)(&tab_produ[i]));
	}
	
	for (i = 0; i < NBCONS; i++)
	{
		tab_conso[i] = make_consommateur(f, i);
		pthread_create(&thread_prod[i],NULL,consommateur,(void*)(&tab_conso[i]));
	}

	for (i = 0; i < NBCONS; i++)
		pthread_join(thread_cons[i], NULL);
	
	for (i = 0; i < NBPROD; i++)
		pthread_join(thread_prod[i], NULL);


	printf("finish\n");
	usleep(100000);
	free_fifo(f);
	free(thread_prod);
	free(thread_cons);
	free(str);
}

int main(void)
{
	CPT = CIBLE * NBPROD;
	full_test();
	return 0;
}
