#include "tme1.h"

int CPT; // partage par tout les consommateur
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
*	Fichier principales contenant:
*	creation productueur/consommateur
*	conversion d'un void* vers producteur/consommateur (sert notamment a envoyer les info au thread (pthread_create))
*	routine producteur/consommateur
*/

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

t_producteur	void_to_producteur(void *conso)
{
	t_producteur *tmp = (t_producteur*) conso;
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
void *producteur(void *args)
{
	t_producteur p = void_to_producteur(args);
	char *str = calloc(10,1);
	char *nom_pacquet;
	int produits = 0;
	int has_product = 0; // objectif est de creer le produix en dehors du mutex pour limiter au maximum les actions non parralelle


	while(produits < p.cible)
	{
		if (!has_product)
		{
			printf("create product\n");
			sprintf(str, "%d", produits);
			nom_pacquet = strcjoin((char*)p.nom, str, ' ');
			has_product = 1;
		}
		if (!isfull(p.f))
		{
			pthread_mutex_lock(&mutex); // minimiser au maximum les actions entre lock et unlock
			enfile(p.f,nom_pacquet);
			pthread_mutex_unlock(&mutex);
			free(nom_pacquet);
			produits++;
			has_product = 0;
		}
		else
		{
			printf("tapis plein\n");
			usleep(10000);
		}
	}
	free(str);
	return 0x0;
}

void *consommateur(void *conso)
{
	t_consommateur c = void_to_consommateur(conso);
	char *s;

	while(CPT > 0)
	{
		if (!isEmpty(c.f))
		{
			pthread_mutex_lock(&mutex);
			s = defile(c.f);
			printf("C %d mange %s \n", c.ref,s);
			CPT--;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			printf("tapis vide\n");
			usleep(10000);
		}
	}
	printf("consommateur %d a fini sa vie\n", c.ref);
	return 0x0;
}


// ============================ test complet =====================================================
void	full_test()
{
	fifo *f= make_fifo(SIZEFIFO);
	pthread_t *thread_prod;
	pthread_t *thread_cons;
	t_consommateur  tab_conso[NBCONS];
	t_producteur  tab_produ[NBPROD];
	int i;
	char *str = calloc(10,1);
	const int nb_fruits = 7;
	char  *tab_fruit[nb_fruits] = {"Banane", "Pomme", "Poire", "Fraise", "Kiwi", "orange", 0x0};


	if (!(thread_prod = (pthread_t*)malloc(sizeof(pthread_t) * NBPROD)))
		error_malloc("bigtest");
	if (!(thread_cons = (pthread_t*)malloc(sizeof(pthread_t) * NBCONS)))
		error_malloc("bigtest");

	for (i = 0; i < NBPROD; i++) {
		if (i < nb_fruits)
			str = strcpy(str, tab_fruit[i]);
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
