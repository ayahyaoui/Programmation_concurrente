#include "tme1.h"


// malloc failed (ca ne met jamais arrive mais on sait jamais)
void	error_malloc(const char *s)
{
	printf("malloc failed in %s\n", s);
	exit(1);
}

/**
 *  Methode: strcjoin
 *  Paranetre: prend 2 string et un caractere
 * 	Description: va creer une nouvelle string (allouer) qui seras
 * 		la concataination des 2 string plus le caractere entre ces derniers
 * 	exemple: strcjoin("Anas", "Yahyaoui", ' ') retourneras "Anas Yahyaoui"
 **/
char	*strcjoin(char *w1, char*w2, char c)
{
	char *res;

	res = (char*)calloc(strlen(w1)+strlen(w2)+2, sizeof(char));
	res = strcat(res, w1);
	res[strlen(w1)] = c;
	res = strcat(res, w2);
	return res;
}
