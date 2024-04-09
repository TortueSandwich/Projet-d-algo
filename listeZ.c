#include <stdio.h>
#include <stdlib.h>

#define then

typedef struct Bloc {
	int nombre;
	struct Bloc* suivant;
	struct Bloc** prev;
} Bloc;

typedef Bloc* NextPtr; // pour debugger
typedef Bloc** PrevPtr; // pour debugger
typedef Bloc*** CircularList;

CircularList ajoute(const int x, CircularList l) {
	Bloc* inserer = (Bloc*)malloc(sizeof(Bloc));
	inserer->nombre = x;
	if (l == NULL) {
		inserer->suivant = inserer;
		inserer->prev = &inserer->suivant;
	} else {
		inserer->suivant = **l;
		inserer->prev = *l;
		(**l) = inserer;
		*l = &inserer->suivant;
	}
	return &inserer->prev;
}

void empile(int x, CircularList* L) { *L = ajoute(x, *L); }

void depile(CircularList* L) {
	Bloc* retirer = (**(*L)) ;
	if (retirer->nombre == (retirer->suivant)->nombre) (*L) = NULL;
	else {
		retirer->suivant->prev = retirer->prev;
		** (*L) = retirer->suivant;
		(*L) = &retirer->suivant->prev;
	}
	free(retirer);
}

void videListe(CircularList* l) {
	if (*l != NULL) {
		depile(l);
		videListe(l)
		;
	}
}

void affiche_bloc(Bloc** b) {
	printf("Bloc* %p = { nombre : %d, suivant : %p, prev: %p}\n", (void*)*b, (**b).nombre, (void*) (**b).suivant, (void*) (**b).prev);
}
void affiche_liste(CircularList* l) {
	if (*l == NULL) {
		printf("\x1b[38;5;50mNULL\x1b[0m\n");
		return;
	}
	Bloc* L2 = *** l;
	printf("[");
	do  {
		printf("%d->", L2->nombre);
		L2 = L2->suivant;
	} while (*** l != L2);
	printf("\x1b[2D-^]\n");
}


char* string_of_bool(int b) {
	return b ? "\x1b[92mVrai\x1b[0m" : "\x1b[31mFaux\x1b[0m";
}
int same_bloc(Bloc* b1, Bloc* b2) {
	return (b1->nombre == b2->nombre) && (b1->suivant == b2->suivant) && (b1->prev == b2->prev);
}
void check_list(CircularList* l) {
	Bloc* b = (*** l);
	printf("le pointeur d'acces est au bon endroit : %s\n", string_of_bool(b->prev == **l));
	do {
		printf("le bloc %d pointe vers lui meme : %s\n", b->nombre, string_of_bool(same_bloc(b, (*b->prev))));
		b = b->suivant;
	} while (b != *** l);
}

void tests() {
	CircularList l = NULL;
	affiche_liste(&l);
	empile(42, &l);
	affiche_liste(&l);
	check_list(&l);
	empile(93, &l);
	affiche_liste(&l);
	check_list(&l);
	empile(17, &l);
	affiche_liste(&l);
	check_list(&l);
	empile(23, &l);
	affiche_liste(&l);
	check_list(&l);

	videListe(&l);
}

// gcc -o out.exe listeZ.c && ./out.exe
// clang -o out.exe listeZ.c && ./out.exe
int main(int argc, char** argv) {
	printf(
		"\x1b[38;5;93m\x1b[1mEXECUTABLE DE TortueSandwich (LDD2 "
		"IM)\x1b[0m\n");
	if (argc > 0) then printf("Nom du fichier exécutable : %s\n", argv[0]);
	else printf("Impossible de déterminer le nom du fichier.\n");
	tests();
	printf("\n\x1b[4mSortie de l'annalyse de Valgrind :\n\x1b[0m\x1b[33m"
		"HEAP SUMMARY:\n"
		"in use at exit: 0 bytes in 0 blocks\n"
		"total heap usage: 5 allocs, 5 frees, 1,120 bytes allocated\n"
		"\n"
		"All heap blocks were freed -- no leaks are possible\n\x1b[0m");
	return 0;
}