#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define AND &&
#define OR ||
#define ISNOT !=
#define NOT !
#define then

typedef enum { FALSE, TRUE } bool;

// arbre valuation feuille
typedef struct Bloc_image {
	bool quatre;
	struct Bloc_image* hg, *hd, *bg, *bd;
	int refcount;
} Bloc_image;
typedef Bloc_image* Image;

////////////  DECLARATIONS ////////////////
Image blanc(); // Blanc()
Image noir(); // Noir()
Image compose(Image i0, Image i1, Image i2, Image i3); // Compose(i0,i1,i2,i3)
void affichage(const Bloc_image* const b); // Affichage(image)
void affichage_profondeur(const Bloc_image* const b); // AffichageProfondeur(image)
Image lecture(char s[], int len); // Lecture()
bool est_noire(const Bloc_image* const b); // EstNoire(image)
bool est_blanche(const Bloc_image* const b); // EstBlanche(image)
float aire(const Bloc_image* const b); // Aire(image)
Image triangle_BD(int p); // TriangleBD(int p)
void arronditP(Image* b, int p); // Arrondit(inout image, int p)
bool meme_dessin(Image b1, Image b2); // MemeDessin(image1, image2)
void inter_union(Image* image1, Image* image2); // InterUnion(inout image1, inout image2)
int compte_damiers(Image b); // CompteDamiers(image)
void compresse(Image* b); // Compresse(inout image)
void dilate(Image* b); // Dilate(inout image)
void affiche(const Bloc_image* const b); // NON DEMANDéE
void libere(Image* b);

////////////  DEFINITIONS ////////////////

// Blanc()
Image blanc() {
	Image feuille_blanche = (Image)malloc(sizeof(Bloc_image));
	feuille_blanche->quatre = FALSE;
	feuille_blanche->hg = NULL;
	feuille_blanche->hd = NULL;
	feuille_blanche->bg = NULL;
	feuille_blanche->bd = NULL;
	feuille_blanche->refcount = 1;
	return feuille_blanche;
}
// Noir()
Image noir() {
	return NULL;
}
// Compose(i0,i1,i2,i3)
Image compose(Image i0, Image i1, Image i2, Image i3) {
	Image res = (Image)malloc(sizeof(Bloc_image));
	res->quatre = TRUE;
	res->hg = i0;
	res->hd = i1;
	res->bg = i2;
	res->bd = i3;
	res->refcount = 1;
	return res;
}

void print_x_o(const Bloc_image* const b) {
	if (b == NULL)
		printf("X");
	else if (!b->quatre)
		printf("o");
	else {
		printf("erreur lors de l'affichage");
		exit(1);
	}
}
// Affichage(image)
void affichage(const Bloc_image* const b) {
	//  blanc         not image
	if (b == NULL || !b->quatre)
		print_x_o(b);
	else {
		affichage(b->hg);
		affichage(b->hd);
		affichage(b->bg);
		affichage(b->bd);
		printf("*");
	}
}
void affichage_dans_ex(char* buffer, const Bloc_image* const b) {
	//  blanc         not image
	if (b == NULL || !b->quatre)
		if (b == NULL)
			strcat(buffer, "X");
		else if (!b->quatre)
			strcat(buffer, "o");
		else
			strcat(buffer, "?");
	else {
		affichage_dans_ex(buffer, b->hg);
		affichage_dans_ex(buffer, b->hd);
		affichage_dans_ex(buffer, b->bg);
		affichage_dans_ex(buffer, b->bd);
		strcat(buffer, "*");
	}
}
void affichage_dans(char* buffer, const Bloc_image* const b) {
	strcpy(buffer, "");
	affichage_dans_ex(buffer, b);
}

void affiche_suff_profondeur_a(const Bloc_image* const b, int* p) {
	if (b == NULL || !b->quatre) {
		print_x_o(b);
		printf("%d ", *p);
	} else {
		*p += 1;
		affiche_suff_profondeur_a(b->hg, p);
		affiche_suff_profondeur_a(b->hd, p);
		affiche_suff_profondeur_a(b->bg, p);
		affiche_suff_profondeur_a(b->bd, p);
		printf("*");
		printf("%d ", *p);
		*p -= 1;
	}
}
// AffichageProfondeur(image)
void affichage_profondeur(const Bloc_image* const b) {
	int p = 0;
	affiche_suff_profondeur_a(b, &p);
}

Image lire_noeud(char s[], int* pos) {
	while (s[*pos] != '*' && s[*pos] != 'X' && s[*pos] != 'o')
		*pos -= 1;

	if (s[*pos] != '*') {
		printf("dead commence pas avec * (commence avec %c)\n", s[*pos]);
		exit(1);
	}
	*pos -= 1;
	Image i[4];
	for (int j = 3; j >= 0; j--) {
		if (s[*pos] == '*') {
			i[j] = lire_noeud(s, pos);
			*pos += 1;
		} else {
			if (s[*pos] == 'X') i[j] = noir();
			else if (s[*pos] == 'o') i[j] = blanc();
			else j += 1;
		}
		*pos -= 1;
	}

	return compose(i[0], i[1], i[2], i[3]);
}

// Lecture()
Image lecture(char s[], int len) {
	int pos = len - 1;
	return lire_noeud(s, &pos);
}

// EstNoire(image)
bool est_noire(const Bloc_image* const b) {
	return b == NULL || (est_noire(b->hg) && est_noire(b->hd) &&
			est_noire(b->bg) && est_noire(b->bd));
}
// EstBlanche(image)
bool est_blanche(const Bloc_image* const b) {
	if (b == NULL) return FALSE;
	else
		return (b != NULL && !b->quatre) ||
			(est_blanche(b->hg) && est_blanche(b->hd) && est_blanche(b->bg) &&
				est_blanche(b->bd));
}

void aire_aux(const Bloc_image* const b, float current_area, float* noir) {
	if (b == NULL)
		*noir += current_area;
	else if (!b->quatre)
		return;
	else {
		float nouvelle_aire = current_area / 4;
		aire_aux(b->hg, nouvelle_aire, noir);
		aire_aux(b->hd, nouvelle_aire, noir);
		aire_aux(b->bg, nouvelle_aire, noir);
		aire_aux(b->bd, nouvelle_aire, noir);
	}
}
// Aire(image)
float aire(const Bloc_image* const b) {
	float nb_de_noir = 0;
	aire_aux(b, 1, &nb_de_noir);
	return nb_de_noir;
}

// TriangleBD(int p)
// On peut aussi faire une image compressée en ne creant que les 3 blocs differant
Image triangle_BD(int p) {
	if (p == 0)
		return blanc();
	else
		return compose(blanc(), triangle_BD(p - 1), triangle_BD(p - 1), noir());
}

typedef enum { NOIR, BLANC, AUCUN } couleur;

couleur get_couleur(Image* image) {
	if (*image == NULL)
		return NOIR;
	return (*image)->quatre ? AUCUN : BLANC;
}
bool est_noeud(couleur c) {
	return c == AUCUN;
}

// Arrondit(inout image, int p)
// O(n)
void arronditP(Image* b, int p) {
	if (p == 0) {
		float taux = aire(*b);
		libere(b);
		if (taux >= 0.5)
			*b = noir();
		else
			*b = blanc();
	} else if (est_noeud(get_couleur(b))) {
		arronditP(&(*b)->hg, p - 1);
		arronditP(&(*b)->hd, p - 1);
		arronditP(&(*b)->bg, p - 1);
		arronditP(&(*b)->bd, p - 1);
	} else {
	}
}

// MemeDessin(image1, image2)
bool meme_dessin(Image b1, Image b2) {
	couleur val1 = get_couleur(&b1);
	couleur val2 = get_couleur(&b2);
	bool b1f = est_noeud(val1); // (b1 == NULL || !b1->quatre);
	bool b2f = est_noeud(val2); //(b2 == NULL || !b2->quatre);
	if (!b1f && b2f) {
		return meme_dessin(b1, b2->hg) && meme_dessin(b1, b2->hd) &&
			meme_dessin(b1, b2->bg) && meme_dessin(b1, b2->bd);
	} else if (b1f && !b2f) {
		return meme_dessin(b1->hg, b2) && meme_dessin(b1->hd, b2) &&
			meme_dessin(b1->bg, b2) && meme_dessin(b1->bd, b2);
	} else if (b1f && b2f) {
		return meme_dessin(b1->hg, b2->hg) && meme_dessin(b1->hd, b2->hd) &&
			meme_dessin(b1->bg, b2->bg) && meme_dessin(b1->bd, b2->bd);
	} else   // if b1f et b2f
		return (val1 == val2);
}

// InterUnion(inout image1, inout image2)
void inter_union(Image* image1, Image* image2) {
	couleur val1 = get_couleur(image1);
	couleur val2 = get_couleur(image2);

	if (val2 == BLANC) {
		Image tmp = *image2;
		*image2 = *image1;
		*image1 = tmp;
	} else if (val1 == NOIR) {
		Image tmp = *image1;
		*image1 = *image2;
		*image2 = tmp;
	} else if (est_noeud(val1) && est_noeud(val2)) {
		inter_union(&(*image1)->hg, &(*image2)->hg);
		inter_union(&(*image1)->hd, &(*image2)->hd);
		inter_union(&(*image1)->bg, &(*image2)->bg);
		inter_union(&(*image1)->bd, &(*image2)->bd);
	} else {
	}
}

// O(n)
void compte_damiers_aux(Image b, int* h, bool* was_dammier, int* cpt) {
	if (b == NULL || !b->quatre) {
		*h = 0;
		return;
	}

	couleur val1 = get_couleur(&b->hg);
	couleur val2 = get_couleur(&b->hd);
	couleur val3 = get_couleur(&b->bg);
	couleur val4 = get_couleur(&b->bd);
	if (val1 == BLANC && val2 == NOIR && val3 == NOIR && val4 == BLANC) {
		*cpt += 1;
		*was_dammier = TRUE;
		*h = 1;
		return;
	}

	compte_damiers_aux(b->hg, h, was_dammier, cpt);
	bool fhg = *was_dammier;
	int h1 = *h;
	*was_dammier = FALSE;

	compte_damiers_aux(b->hd, h, was_dammier, cpt);
	bool fhd = *was_dammier;
	int h2 = *h;
	*was_dammier = FALSE;

	compte_damiers_aux(b->bg, h, was_dammier, cpt);
	bool fbg = *was_dammier;
	int h3 = *h;
	*was_dammier = FALSE;

	compte_damiers_aux(b->bd, h, was_dammier, cpt);
	bool fbd = *was_dammier;
	int h4 = *h;
	*was_dammier = FALSE;

	if (fhg && fhd && fbg && fbd)
		*was_dammier = TRUE;

	if (*was_dammier && h1 == h2 && h2 == h3 && h3 == h4)
		*cpt += 1;
	else
		*was_dammier = FALSE;
	*h += 1;

}

// CompteDamiers(image)
int compte_damiers(Image b) {
	int h = 0; // Hauteur
	bool was_damier = FALSE; // Hauteur precédente etait damier ?
	int cpt = 0;
	compte_damiers_aux(b, &h, &was_damier, &cpt);
	return cpt;
}

typedef struct Bloc {
	Image* pointeur_vers_truc;
	struct Bloc* suivant;
} Bloc;

typedef Bloc* Liste;

Bloc* creer_element(Image* truc) {
	Bloc* nouvel_element = malloc(sizeof(Bloc));
	*nouvel_element = (Bloc) {
		.pointeur_vers_truc = truc,
		.suivant = NULL,
	};
	return nouvel_element;
}
void ajouter_element(Liste* liste, Image* truc) {
	Bloc* nouvel_element = creer_element(truc);
	if (nouvel_element != NULL) {
		nouvel_element->suivant = *liste;
		*liste = nouvel_element;
	}
}
bool pareil(Image b1, Image b2) {
	couleur val1 = get_couleur(&b1);
	couleur val2 = get_couleur(&b2);
	bool b1f = est_noeud(val1); // (b1 == NULL || !b1->quatre);
	bool b2f = est_noeud(val2); //(b2 == NULL || !b2->quatre);
	if ((!b1f && b2f) || (b1f && !b2f))
		return FALSE;
	else if (b1f && b2f) {
		return pareil(b1->hg, b2->hg) && pareil(b1->hd, b2->hd) &&
			pareil(b1->bg, b2->bg) && pareil(b1->bd, b2->bd);
	} else   // if b1f et b2f
		return (val1 == val2);
}
Image* trouver_element(Liste* liste, Image* i) {
	Bloc* courant = *liste;
	while (courant != NULL) {
		if (pareil(*(courant->pointeur_vers_truc), *i))
			return courant->pointeur_vers_truc;
		courant = courant->suivant;
	}
	return NULL;
}

void liberer_liste_sans_truc(Liste liste) {
	while (liste != NULL) {
		Bloc* suivant = liste->suivant;
		free(liste);
		liste = suivant;
	}
}

void compresse_a(Bloc_image** b, Liste* l) {
	if (*b == NULL)   /* noir */
		return;
	else {
		compresse_a(&(*b)->hg, l);
		compresse_a(&(*b)->hd, l);
		compresse_a(&(*b)->bg, l);
		compresse_a(&(*b)->bd, l);
	}
	Image* ptr = trouver_element(l, b);
	if (ptr == NULL)
		ajouter_element(l, b);
	else {
		(*ptr)->refcount = (*ptr)->refcount + 1;
		free(*b);
		*b = *ptr;
	}
}

// Compresse(inout image)
// perte de 3 blocs dans l'exemple du sujet
void compresse(Image* b) {
	Liste l = NULL;
	compresse_a(b, &l);
	liberer_liste_sans_truc(l);
}

Bloc_image* dilate_a(const Bloc_image* original) {
	if (original == NULL)
		return NULL;

	Bloc_image* copie = (Bloc_image*)malloc(sizeof(Bloc_image));
	*copie = (Bloc_image) {
		.quatre = original->quatre,
		.hg = dilate_a(original->hg),
		.hd = dilate_a(original->hd),
		.bg = dilate_a(original->bg),
		.bd = dilate_a(original->bd),
		.refcount = 1,
	};

	return copie;
}

// Dilate(inout image)
// Pas opti, au moins il n'y a pas de fuite
void dilate(Image* b) {
	Image tmp = *b;
	*b = dilate_a(*b);
	libere(&tmp);
}

void libere(Image* b) {
	if (*b != NULL && (*b)->refcount > 1)
		(*b)->refcount = (*b)->refcount - 1;
	else if (!est_noeud(get_couleur(b)))
		free(*b);
	else {
		libere(&(*b)->hg);
		libere(&(*b)->hd);
		libere(&(*b)->bg);
		libere(&(*b)->bd);
		free(*b);
	}
}




//////////////////////////////////////
// TESTS // TESTS // TESTS // TESTS //
//////////////////////////////////////

#define COULEUR_CHIFFRE(valeur, attendu)                                       \
	if (valeur == attendu) {                                                     \
		printf("\x1b[92m");                                                        \
	} else {                                                                     \
		printf("\x1b[91m");                                                        \
	}                                                                            \
	printf("%d", valeur);                                                        \
	printf("\x1b[0m\n");

#define COULEUR_FLOAT(valeur, attendu)                                         \
	if (valeur == attendu) {                                                     \
		printf("\x1b[92m");                                                        \
	} else {                                                                     \
		printf("\x1b[91m");                                                        \
	}                                                                            \
	printf("%f", valeur);                                                        \
	printf("\x1b[0m\n");

#define COULEUR_BOOL(valeur, attendu)                                          \
	if (valeur == attendu) {                                                     \
		printf("\x1b[92m");                                                        \
	} else {                                                                     \
		printf("\x1b[91m");                                                        \
	}                                                                            \
	printf(string_of_bool(valeur));                                              \
	printf("\x1b[0m\n");

#define COULEUR_STR(valeur, attendu)                                           \
	if (strcmp(valeur, attendu) == 0) {                                          \
		printf("\x1b[92m");                                                        \
		printf(valeur);                                                            \
	} else {                                                                     \
		printf("\x1b[91m");                                                        \
		printf(valeur);                                                            \
		printf("    attendait : ");                                                \
		printf(attendu);                                                           \
	} \
	printf("\x1b[0m\n");

char* string_of_bool(bool b) {
	return b ? "Vrai" : "Faux";
}
void afficher_adresses_images(Image image) {
	if (image == NULL)
		return;
	if (get_couleur(&image) == BLANC)
		printf("%p : blanc\n", image);
	else {

		printf("Adresse de l'image : %p\n", (void*)image);
		printf(":  %p - %p - %p - %p\n", image->hg, image->hd, image->bg,
			image->bd);
	}
	afficher_adresses_images(image->hg);
	afficher_adresses_images(image->hd);
	afficher_adresses_images(image->bg);
	afficher_adresses_images(image->bd);
}

void check(const void* address1, const void* address2) {
	if (address1 != address2) {
		fprintf(stderr, "Erreur : Les adresses ne sont pas égales !\n");
		exit(EXIT_FAILURE);
	}
}
void check_diff(const void* address1, const void* address2) {
	if (address1 == address2) {
		fprintf(stderr, "Erreur : Les adresses sont égales !\n");
		exit(EXIT_FAILURE);
	}
}
//////////////////////////////////////
// TESTS // TESTS // TESTS // TESTS //
//////////////////////////////////////
void test() {
	char buffer[128] = "";
	if (TRUE) { // Blanc Noir Compose
		printf("----------------- Blanc Noir Compose ------------\n");
		Image b = blanc();
		affichage_dans(buffer, b);
		printf("blanc() = ");
		COULEUR_STR(buffer, "o");
		libere(&b);
		b = noir();
		affichage_dans(buffer, b);
		printf("noir() = ");
		COULEUR_STR(buffer, "X");
		libere(&b);
		b = compose(noir(), blanc(), blanc(), noir());
		affichage_dans(buffer, b);
		printf("compose(noir(), blanc(), blanc(), noir()) = ");
		COULEUR_STR(buffer, "XooX*");
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // Affichage
		printf("----------------- Affichage ------------\n");
		Image trois = compose(noir(), blanc(), blanc(), noir());
		Image deux = compose(blanc(), noir(), trois, blanc());
		Image un = compose(blanc(), noir(), noir(), deux);
		printf("afichage(/* oXXoXXooX*o** */) = ");
		affichage(un);
		printf("\n");
		libere(&un);
		printf("\n\n");
	}
	if (TRUE) { // AffichageProfondeur
		printf("----------------- AffichageProfondeur ------------\n");
		char s[] = "XooXo*oXXXoXoXX**oX**";
		printf("afichageProfondeur(%s) = ", s);
		Image b = lecture(s, 21);
		affichage_profondeur(b);
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // Lecture
		printf("----------------- Lecture ------------\n");
		char s[] = "XooXo*oXXXoXoXX**oX**";
		printf("lecture(%s, 21) = ", s);
		Image b = lecture(s, 21);
		affichage_dans((char*)&buffer, b);
		COULEUR_STR(buffer, s);
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // EstBlanche EstNoire
		printf("----------------- EstBlanche EstNoire ------------\n");
		char s1[] = "ooooo*oooo*o*oooooo**";
		printf("est_blanche(%s) = ", s1);
		Image b = lecture(s1, 21);
		COULEUR_BOOL(est_blanche(b), TRUE);
		libere(&b);
		char s2[] = "ooooo*oooo*o*ooXooo**";
		printf("est_blanche(%s) = ", s2);
		b = lecture(s2, 21);
		COULEUR_BOOL(est_blanche(b), FALSE);
		libere(&b);
		char s3[] = "XXXXX*XXXX*X*XXXXXX**";
		printf("est_noire(%s) = ", s3);
		b = lecture(s3, 21);
		COULEUR_BOOL(est_noire(b), TRUE);
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // Aire
		printf("----------------- Aire ------------\n");
		char s1[] = "XoXoo*XXXXo**";
		printf("aire(%s) = ", s1);
		Image b = lecture(s1, 13);
		COULEUR_FLOAT(aire(b), 0.75);
		libere(&b);

		char s2[] = "XXooo*ooXXX**";
		printf("aire(%s) = ", s2);
		b = lecture(s2, 13);
		COULEUR_FLOAT(aire(b), 0.5);
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // TriangleBD
		printf("----------------- TriangleBD ------------\n");
		strcpy(buffer, "");
		printf("triangle_BD(0) = ");
		Image b = triangle_BD(0);
		affichage_dans(buffer, b);
		COULEUR_STR(buffer, "o");
		libere(&b);

		strcpy(buffer, "");
		printf("triangle_BD(1) = ");
		b = triangle_BD(1);
		affichage_dans(buffer, b);
		COULEUR_STR(buffer, "oooX*");
		libere(&b);

		strcpy(buffer, "");
		printf("triangle_BD(3) = ");
		b = triangle_BD(3);
		affichage_dans(buffer, b);
		COULEUR_STR(buffer, "oooooX*oooX*X*ooooX*oooX*X*X*");
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // Arrodit
		printf("----------------- Arrondit ------------\n");
		strcpy(buffer, "");

		char s1[] = "XooXo*oXXXoXoXX**oX**";
		printf("arronditP(%s, 0) = ", s1);
		Image b = lecture(s1, 21);
		arronditP(&b, 0);
		affichage_dans(buffer, b);
		COULEUR_STR(buffer, "o");
		libere(&b);

		strcpy(buffer, "");
		char s2[] = "XXXooX*oXXo*oooX**oXo*oXXoooXXX**oX**";
		printf("arronditP(%s, 2) = ", s2);
		b = lecture(s2, 37);
		arronditP(&b, 2);
		affichage_dans(buffer, b);
		COULEUR_STR(buffer, "XXoXo*oXooX**");
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // MemeDessin
		printf("----------------- MemeDessin ------------\n");
		strcpy(buffer, "");
		char s1[] = "Xooooo*ooo*oooo*o*XoooXXXX***";
		Image b1 = lecture(s1, 29);
		char s2[] = "XXXXXXXXXX***oooo*Xooooo*oX**";
		Image b2 = lecture(s2, 29);
		char s3[] = "XXXXXXoooo***oooo*Xooooo*oX**";
		Image b3 = lecture(s3, 29);
		Image b4 = blanc();
		Image b5 = noir();
		Image b6 = compose(noir(), noir(), noir(), noir());
		Image b7 = compose(noir(), blanc(), noir(), noir());
		Image b8 = compose(compose(noir(), noir(), noir(), noir()), noir(), noir(),
				noir());
		Image b9 = compose(compose(noir(), blanc(), noir(), noir()), noir(), noir(),
				noir());
		printf("MemeDessin(%s, %s) = ", s1, s2);
		COULEUR_BOOL(meme_dessin(b1, b2), TRUE);
		printf("MemeDessin(%s, %s) = ", s2, s3);
		COULEUR_BOOL(meme_dessin(b2, b3), FALSE);
		printf("MemeDessin(%s, X) = ", s1);
		COULEUR_BOOL(meme_dessin(b1, b5), FALSE);
		printf("MemeDessin(o, X) = ");
		COULEUR_BOOL(meme_dessin(b4, b5), FALSE);
		printf("MemeDessin(X, XXXX*) = ");
		COULEUR_BOOL(meme_dessin(b5, b6), TRUE);
		printf("MemeDessin(X, XoXX*) = ");
		COULEUR_BOOL(meme_dessin(b5, b7), FALSE);
		printf("MemeDessin(X, XXXX*XXX*) = ");
		COULEUR_BOOL(meme_dessin(b5, b8), TRUE);
		printf("MemeDessin(X, XoXX*XXX*) = ");
		COULEUR_BOOL(meme_dessin(b5, b9), FALSE);
		libere(&b1);
		libere(&b2);
		libere(&b3);
		libere(&b4);
		libere(&b5);
		libere(&b6);
		libere(&b7);
		libere(&b8);
		libere(&b9);
		printf("\n\n");
	}
	if (TRUE) { // InterUnion
		printf("----------------- InterUnion ------------\n");
		char s1[] = "oXoXXo*oXoX*oXX**";
		Image i1 = lecture(s1, 17);
		char s2[] = "XoXoXooX*ooXX*X**";
		Image i2 = lecture(s2, 17);
		printf("InterUnion(%s, %s)\n", s1, s2);
		inter_union(&i1, &i2);
		affichage_dans(buffer, i1);
		printf("image1 : ");
		COULEUR_STR(buffer, "oooXXo*ooooXX*X**");
		affichage_dans(buffer, i2);
		printf("image2 : ");
		COULEUR_STR(buffer, "XXXoXoX*XooX*XX**");
		libere(&i1);
		libere(&i2);
		printf("\n\n");
	}
	if (TRUE) { // CompteDamiers
		printf("----------------- CompteDamiers ------------\n");
		char h1[] = "oXXo*";
		char h2[] = "oXXo*oXXo*oXXo*oXXo**";
		char h3[] = "oXXo*oXXo*oXXo*oXXo**oXXo*oXXo*oXXo*oXXo**oXXo*oXXo*oXXo*oXXo*"
			"*oXXo*oXXo*oXXo*oXXo***";
		Image b1 = lecture(h1, 5);
		Image b2 = lecture(h2, 21);
		Image b3 = lecture(h3, 85);
		printf("compte_damiers(%s) = ", h1);
		COULEUR_CHIFFRE(compte_damiers(b1), 1);
		printf("compte_damiers(%s) = ", h2);
		COULEUR_CHIFFRE(compte_damiers(b2), 5);
		printf("compte_damiers(%s) = ", h3);
		COULEUR_CHIFFRE(compte_damiers(b3), 21);
		libere(&b1);
		libere(&b2);
		libere(&b3);

		char s[] = "oXXo*oXXo*oXXo*oXXo**oXXo*oXXo*oXXo*oXXo**oXXo*oXXo*oXXo**oXXo*"
			"oXXo*XX*oooo*XXoooooo*o***";
		Image b = lecture(s, 89);
		printf("compte_damiers(%s) = ", s);
		COULEUR_CHIFFRE(compte_damiers(b), 15);
		libere(&b);
		printf("\n\n");
	}
	if (TRUE) { // Compresse Dilate
		printf("----------------- Compresse et Dilate ------------\n");

		char s[] = "(oXXo*)(X(oXXo*)(oXXo*)(XooX*)*)(X(oXXo*)(oXXo*)(XooX*)*)(X("
			"XooX*)(XooX*)o*)*";
		Image b = lecture(s, 77);
		// affiche(b);
		printf("Compresion : ");
		compresse(&b); // perte de 3 blocs
		check(b->hd, b->bg);
		check(b->hd->hd, b->bg->bg);
		check(b->hd->hd, b->bg->hd);
		check(b->hd->bd, b->bd->hd);
		check(b->hg, b->hd->hd);
		check(b->hg->hg, b->hd->hd->bd);
		printf("L'image à bien été compressée\n");
		// afficher_adresses_images(b);
		printf("Dilatation : ");
		dilate(&b);
		check_diff(b->hd, b->bg);
		check_diff(b->hd->hd, b->bg->bg);
		check_diff(b->hd->hd, b->bg->hd);
		check_diff(b->hd->bd, b->bd->hd);
		check_diff(b->hg, b->hd->hd);
		check_diff(b->hg->hg, b->hd->hd->bd);
		// afficher_adresses_images(b);
		printf("L'image à bien été dilatée\n");
		libere(&b);
	}
}

// gcc -o out.exe quadtree.c && ./out.exe
int main(int argc, const char** argv) {
	printf( //"\x1b[?1049h
		"\x1b[38;5;93m\x1b[1mEXECUTABLE DE TortueSandwich (LDD2 "
		"IM)\x1b[0m\n");

	test();
	printf(
		"\n\x1b[4mSortie de l'annalyse de Valgrind :\n\x1b[0m\x1b[33m"
		"HEAP SUMMARY:\n"
		"in use at exit: 144 bytes in 3 blocks\n"
		"total heap usage: 453 allocs, 450 frees, 22,528 bytes allocated\n"
		"\n\x1b[0m");
	printf("Perte de 3 blocs lors de compresse\n");

	return EXIT_SUCCESS;
}

/// FONCTIONS NON DEMANDéES /// (un peu begué tho)
/// FONCTIONS NON DEMANDéES /// (un peu begué tho)
/// FONCTIONS NON DEMANDéES /// (un peu begué tho)

const int Profondeur_MAX = 5;
const char* const BLANC_c = "\x1b[48;5;255m";
const char* const NOIR_c = "\x1b[48;5;236m";
const char* const ROUGE_c = "\x1b[41m";
const char* const FOND_c = "\x1b[0m";
// https://www.lihaoyi.com/post/Ansi/RainbowBackground256.png
void carre(const int L, const char* const c);
void affiche_aux(const Bloc_image* const b, const int PMAX, int* const profond,
	int* const p_puissance);
void carre(int L, const char* const c) {
	for (int y = 0; y < L; ++y, printf("\x1b[%dD\x1b[B", 2 * L))
		for (int x = 0; x < L; ++x)
			printf("%s  %s", c, FOND_c);
	printf("\x1b[%dA\x1b[%dC", L, 2 * L);
}
void affiche_aux(const Bloc_image* const b, const int PMAX, int* const p,
	int* const p2) {
	// if (*p2 > PMAX*2) {
	//         carre(1, ROUGE_c);
	//         return;
	// }
	int L = PMAX / *p2; // taille petit carre
	if (b == NULL)
		carre(L, NOIR_c);
	else if (!(b->quatre))   /*est feuille*/
		carre(L, BLANC_c);
	else { // Backtracking
		(*p2) *= 2;
		++(*p);
		affiche_aux(b->hg, PMAX, p, p2);
		affiche_aux(b->hd, PMAX, p, p2);
		printf("\x1b[%dB\x1b[%dD", L / 2, 2 * L);
		affiche_aux(b->bg, PMAX, p, p2);
		affiche_aux(b->bd, PMAX, p, p2);
		printf("\x1b[%dA", L / 2);
		--(*p);
		(*p2) /= 2;
	}
}
void affiche(const Bloc_image* const b) {
	int PMAX = 1;
	printf("%s", FOND_c);
	for (int cpt = 0; cpt < Profondeur_MAX; cpt++)
		PMAX *= 2;
	for (int i = 0; i < PMAX + 2; ++i)
		printf("\n");
	printf("\x1b[%dA%s", PMAX, FOND_c);
	int p = 0, p2 = 1;
	affiche_aux(b, PMAX, &p, &p2);
	printf("\x1b[0m\x1b[%dE\n", PMAX);
}

/// FONCTIONS NON DEMANDéES /// (un peu begué tho)
/// FONCTIONS NON DEMANDéES /// (un peu begué tho)
/// FONCTIONS NON DEMANDéES /// (un peu begué tho)

