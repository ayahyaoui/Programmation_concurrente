#include "tme1.h"


int VERBOSE = 0; // sert seulement au debug
/*
*	Ensemble de fonction primitive sur notre liste fifo 
*	creation, destruction
*	est plein , est vide
*	ajouter, et enlever un element
*	enfin fonctions d'affichage (principalement pour le debug)
*/

fifo	*make_fifo(int size)
{
	fifo *f;
	if(!(f = (fifo*)calloc(1, sizeof(fifo))))
	{
		error_malloc("make_fifo");
		return 0x0;
	}
	if (!(f->tapis=(char **) calloc(size+1,sizeof(char*))))
	{
		free(f);
		error_malloc("make_fifo");
		return 0x0;
	}	
	f->start=0;
	f->end=0;
	f->sizemax=size;
	f->size=0;
	printf("fifo create \n");
	usleep(1000);
	return f;
}

void	free_fifo(fifo* f)
{
	int i;
	if (f)
	{
		for(i = f->start; i < f->end; i++)
		{
			if(f->tapis[i])
				free(f->tapis[i]);
		}
		free(f);
		f = 0x0;
	}
}

int		isfull(fifo *f)
{
	
	if (f->size == f->sizemax)
		return 1;
	return 0;
}

int		isEmpty(fifo *f)
{
	
	if (f->size == 0)
		return 1;
	return 0;
}

void	enfile(fifo* f, char* element)
{
	if (f->size == f->sizemax)
	{
		printf("Probleme essai d'ajouter sur une pile Pleine !!\n");
		return;
	}
	if (f->end >= f->sizemax)
	{
		memmove(f->tapis, &f->tapis[f->start], sizeof(char*) * f->size);
		f->start = 0;
		f->end = f->size;
	}
	if (VERBOSE > 0)
	{
		printf("start  probleme enfile\n");
		print_fifo(f);
	}
	f->tapis[f->end] = strdup(element);
	if (VERBOSE > 0)
	{
		printf("milieux  probleme enfile\n");
		printf("{%s} dupliquer pour {%s}\n", element, f->tapis[f->end]);
	}
	f->end+=1;
	f->size+=1;
	if (VERBOSE > 0)
	{
		printf("pile a enfiler <%s> en pos %d\n", f->tapis[f->end-1], f->end-1);
		print_fifo(f);
		printf("end enfile\n");
	}
}

char	*defile(fifo *f)
{
	char *res;
	if (VERBOSE > 0)
	{
		printf("start defile\n");
		print_fifo(f);
	}
	if(f->size <= 0)
	{
		printf("Desoler fifo vide\n");
		return 0x0;
	}
	res = f->tapis[f->start];
	//free(f->tapis[f->start]); // au consomateur de detruire !!

	//f->tapis[f->start] = 0x0;
	f->start++;
	f->size--;
	if (VERBOSE > 0)
	{
		printf("pile defile <%s> en pos %d\n", res, f->start);
		print_fifo(f);
		printf("end defile\n");
	}
	return res;
}

void 	print_fifo(fifo *f)
{
	int i;
	
	if (!f)
	{
		printf("Fifo effacé ou non cree\n");
		return;
	}
	printf("Il y a %d elements [%d a %d] dans la file (address = %p and %p):\n", \
	f->size, f->start, f->end, f->tapis, f->tapis[0]);
	for(i = f->start ; i < f->end; i++)
	{
		printf("[%s] ", f->tapis[i]);
	}
	printf("\n");
}
