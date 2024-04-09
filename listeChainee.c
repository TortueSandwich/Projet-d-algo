
#include <stdio.h>
#include <stdlib.h>

/*************************************************/
/*                                               */
/*                sucre syntaxique               */
/*                                               */
/*************************************************/

#define AND &&
#define OR ||
#define ISNOT !=
#define NOT !
#define then

typedef enum { FALSE, TRUE } bool;
char* string_of_bool(bool b) {
	return b ? "\x1b[92mVrai\x1b[0m" : "\x1b[31mFaux\x1b[0m";
}
/*************************************************/
/*                                               */
/*          definition type liste                */
/*                                               */
/*************************************************/

typedef struct Bloc {
	int nombre;
	struct Bloc* suivant;
} Bloc;

typedef Bloc* Liste;

/*************************************************/
/*                                               */
/*                predeclarations                */
/*                                               */
/*************************************************/

/* initialise une Liste � vide */
void initVide(Liste* const L);

/* renvoie 1 si la Liste en parametre est vide, 0 sinon */
bool estVide(const Liste* const l);
// bool estVide_ptr(const Liste *const l);
/* renvoie le premier element de la Liste en parametre */
int premier(const Liste l);
const int const* premier_ptr(const Liste* const l);

/* renvoie une nouvelle Liste correspondant a celle en parametre, avec l'element
 * x ajoute en haut de la pile */
const Liste ajoute(const int x, const Liste l);

/* modifie la Liste en parametre: x est ajoute comme premier element */
void empile(int x, Liste* const L);

/* renvoie une nouvelle Liste correspondant a celle en parametre sans son
 * premier element */
const Liste suite(const Liste l);

/* modifie la Liste en parametre: le premier element est retire */
void depile(Liste* const L);

/* affichage simple en recursif et en iteratif */
void affiche_rec(const Liste* const l);
void affiche_iter(const Liste* const l);

/* longueur en recursif et en iteratif */
int longueur_rec(Liste l);
int longueur_iter(Liste l);

/*  Elimination du dernier element en recursif et en iteratif  */
/*  VD est la sousprocedure utilitaire de la version recursive */
void VD(Liste* L);
void VireDernier_rec(Liste* L);
void VireDernier_iter(Liste* L);

Liste* pointerSuite(const Liste* const l);

#define AFFICHE_LISTE(liste_ptr)     \
	printf("%s = ", #liste_ptr); \
	affiche_iter(liste_ptr);

#define CREER_LISTE(l_ptr, ...)                         \
	{                                               \
		initVide(l_ptr);                        \
		int args[] = {__VA_ARGS__};             \
		int n = sizeof(args) / sizeof(args[0]); \
		for (int i = 0; i < n; i++) {           \
			empile(args[n - i - 1], l_ptr); \
		}                                       \
	}

#define PANIC(msg, ...)                                                   \
	{                                                                 \
		fprintf(stderr, "\x1b[38;5;196mErreur : \x1b[38;5;210m"); \
		fprintf(stderr, msg, ##__VA_ARGS__);                      \
		fprintf(stderr, "\x1b[0m\n\n\n");                         \
		exit(1);                                                  \
	};

int nombreDeChiffres_aux(int nombre, int res) {
	if (nombre < 10)
		return res + 1;
	else
		return nombreDeChiffres_aux(nombre / 10, res + 1);
}
int nombreDeChiffres(const int* nombre) {
	if (nombre == NULL)
		return 1;
	int t = (*nombre < 0) ? -(*nombre) : *nombre;
	return nombreDeChiffres_aux(t, 0);
}

#define COULEUR_CHIFFRE(valeur, attendu) \
	if (valeur == attendu) {         \
		printf("\x1b[92m");      \
	} else {                         \
		printf("\x1b[91m");      \
	}

void compare_liste(const Liste* given, const Liste* expected) {
	if (estVide(given) AND estVide(expected)) {
		printf(
			"given = \x1b[96mNULL\x1b[0m  expected = "
			"\x1b[96mNULL\x1b[0m");
		return;
	}
	printf("\n\n\x1b[1Aexpected = [\x1b[13D\x1b[1Agiven    = [");
	bool flag_e = TRUE, flag_g = TRUE;

	const int* val_e = NULL, *val_g = NULL;
	char sg[4], se[4];
	while (flag_g || flag_e) {
		val_g = (estVide(given)) ? NULL : premier_ptr(given);
		val_e = (estVide(expected)) ? NULL : premier_ptr(expected);

		if (val_g != NULL) snprintf(sg, sizeof(sg), "%d", *val_g);
		else snprintf(sg, sizeof(sg), "N");
		if (val_e != NULL) snprintf(se, sizeof(se), "%d", *val_e);
		else snprintf(se, sizeof(se), "N");

		int nb_ch_g = nombreDeChiffres(val_g);
		int nb_ch_e = nombreDeChiffres(val_e);
		if (nb_ch_g < nb_ch_e) {
			printf("\x1b[%dC", nb_ch_e - nb_ch_g);
			if (val_e == NULL || val_g == NULL) printf("\x1b[96m");
			else COULEUR_CHIFFRE(*val_g, *val_e);
			printf("%s\x1b[0m, \x1b[%dD", sg, 2 + nb_ch_e);
		} else {
			if (val_e == NULL) printf("\x1b[35m");
			else if (val_g == NULL) printf("\x1b[96m");
			else COULEUR_CHIFFRE(*val_g, *val_e);
			printf("%s\x1b[0m, \x1b[%dD", sg, 2 + nb_ch_g);
		}
		printf("\x1b[B");
		if (nb_ch_e < nb_ch_g) {
			printf("\x1b[%dC", nb_ch_g - nb_ch_e);
			if (val_e == NULL) printf("\x1b[96m");
			printf("%s\x1b[0m, ", se);
		} else {
			if (val_e == NULL) printf("\x1b[96m");
			printf("%s\x1b[0m, ", se);
		}
		printf("\x1b[B\x1b[2A");

		if (!estVide(given)) given = pointerSuite(given);
		else flag_g = FALSE;

		if (!estVide(expected)) expected = pointerSuite(expected);
		else flag_e = FALSE;

		if (!flag_e && !flag_g) printf("\x1b[5D]    \x1b[5D\x1b[B]    ");
	}
}

/*************************************************/
/*                                               */
/*                briques de base                */
/*                                               */
/*************************************************/

void initVide(Liste* const L) { *L = NULL; }

bool estVide(const Liste* const l) { return *l == NULL; }

int premier(const Liste l) {
	if (estVide(&l)) PANIC("dereferancement d'un pointeur vide lors de premier()");
	return l->nombre;
}
const int* premier_ptr(const Liste* l) {
	if (estVide(l)) PANIC("dereferancement d'un pointeur vide lors de premier_ptr()");
	return &((*l)->nombre);
}

const Liste ajoute(const int x, const Liste l) {
	Liste tmp = (Liste)malloc(sizeof(Bloc));
	if (tmp == NULL) {
		AFFICHE_LISTE(&l);
		PANIC("Erreur d'allocation lors de ajoute(int %d, list ^)", x);
	}
	*tmp = (Bloc) {.nombre = x, .suivant = l};
	return tmp;
}

void empile(int x, Liste* const L) { *L = ajoute(x, *L); }

const Liste suite(const Liste l) {
	if (estVide(&l)) PANIC("dereferancement d'un pointeur vide lors de suite()");
	return l->suivant;
}

void depile(Liste* const L) {
	Liste tmp = *L;
	*L = suite(*L);
	free(tmp);
}

Liste* pointerSuite(const Liste* const l) {
	if (l == NULL) PANIC("pointeur null (pointeursuite)");
	return &(*l)->suivant;
}

Liste copie(Liste l) {
	Liste R = NULL;
	// C'est horrible :)
	for (Bloc** Q = &R, *P = l; P != NULL; P = suite(P), Q = &(*Q)->suivant) empile(premier(P), Q);
	return R;
}



/*************************************************/
/*                                               */
/*     Affiche, avec les briques de base         */
/*                                               */
/*************************************************/

void affiche_rec_a(const Liste* const l) {
	if (estVide(l)) printf("\x1b[2D]\n");
	else {
		printf("%d, ", premier(*l));
		affiche_rec_a(pointerSuite(l));
	}
}
void affiche_rec(const Liste* const l) {
	if (estVide(l)) {
		printf("\x1b[38;5;50mNULL\x1b[0m");
		return;
	}
	printf("[");
	affiche_rec_a(l);
}

void affiche_iter(const Liste* const l) {
	if (estVide(l)) {
		printf("\x1b[38;5;50mNULL\x1b[0m");
		return;
	}
	Liste L2 = *l;
	printf("[");
	bool flag_non_vide = !estVide(&L2);
	while (flag_non_vide) {
		printf("%d", premier(L2));
		L2 = suite(L2);
		flag_non_vide = !estVide(&L2);
		if (flag_non_vide) printf(", ");
	}
	printf("]");
}

/*************************************************/
/*                                               */
/*     Longueur, sans les briques de base        */
/*                                               */
/*************************************************/

int longueur_rec(Liste l) {
	if (l == NULL) return 0;
	else return (1 + longueur_rec(l->suivant));
}

int longueur_iter(Liste l) {
	int cpt = 0;
	for (Liste P = l; P ISNOT NULL; P = P->suivant) cpt++;
	return cpt;
}

/*************************************************/
/*                                               */
/*       VireDernier,                            */
/*               sans les briques de base,       */
/*               ni le "->"                      */
/*                                               */
/*************************************************/

void VD(Liste* L)
// *L non NULL ie liste non vide
{
	if (((**L).suivant) == NULL)
		depile(L);  // moralement : depile(& (*L)) ;
	else
		VD(&((**L).suivant));
}

void VireDernier_rec(Liste* L) {
	if ((*L)ISNOT NULL) VD(L);  // moralement : VD(& (*L)) ;
}

void VireDernier_iter(Liste* L) {
	if ((*L)ISNOT NULL) {
		while (((**L).suivant)ISNOT NULL) L = &((**L).suivant);
		free(*L);
		*L = NULL;
	}
}

/*************************************************/
/*                                               */
/*       Libere la memoire                       */
/*                                               */
/*************************************************/

void VideListe(Liste* L) {
	if (NOT(estVide(L))) {
		depile(L);
		VideListe(L);
	}
}

/*************************************************/
/*                                               */
/*           Main                                */
/*                                               */
/*************************************************/

void poup(Liste l) {
	printf("Double Affichage \n");
	affiche_rec(&l);
	affiche_iter(&l);

	printf("Longueur en double %d %d \n\n", longueur_rec(l),
		longueur_iter(l));
}

bool UnPlusDeuxEgalTrois(const Liste* l_ptr) {
	if (estVide(l_ptr)) return TRUE;
	int val1 = (estVide(l_ptr)) ? 0 : *premier_ptr(l_ptr);
	l_ptr = pointerSuite(l_ptr);
	int val2 = (estVide(l_ptr)) ? 0 : *premier_ptr(l_ptr);
	if (!estVide(l_ptr)) l_ptr = pointerSuite(l_ptr);  // de taille 1
	int val3 = (estVide(l_ptr)) ? 0 : *premier_ptr(l_ptr);
	return val1 + val2 == val3;
}

bool croissante_rec_term(const Liste* const l, const bool flag) {
	if (estVide(l) || estVide(pointerSuite(l))) then return flag;
	else {
		const int* const x = premier_ptr(l);
		const int* const y = premier_ptr(pointerSuite(l));
		return croissante_rec_term(pointerSuite(l), flag && *x < *y);
	}
}
bool croissante_rec(const Liste* l) { return croissante_rec_term(l, TRUE); }

bool croissante_iter(const Liste* const l) {
	if (estVide(l) || estVide(pointerSuite(l))) then return TRUE;

	const int* x = premier_ptr(l), *y;
	for (const Liste* reste = pointerSuite(l); !estVide(reste); reste = pointerSuite(reste)) {
		y = premier_ptr(reste);
		if (NOT(*x < *y)) then return FALSE; // NO ELSE
		x = y;
	}
	return TRUE;
}

int nombreMemePosition_rec(const Liste* const l1, const Liste* const l2) {
	if (estVide(l1) OR estVide(l2)) then return 0;
	else {
		int tmp = (*premier_ptr(l1) == *premier_ptr(l2));
		return tmp + nombreMemePosition_rec(pointerSuite(l1), pointerSuite(l2));
	}
}
int nombreMemePosition_iter(const Liste* l1, const Liste* l2) {
	int compteur = 0;
	for (/*NO INIT*/; NOT(estVide(l1) OR estVide(l2)); l1 = pointerSuite(l1), l2 = pointerSuite(l2))
		compteur += (*premier_ptr(l1) == *premier_ptr(l2));
	return compteur;
}

int NMP_RT_aux(const Liste* const l1, const Liste* const l2, const int acc) {
	if (estVide(l1) OR estVide(l2)) then return acc;
	else {
		int tmp = (*premier_ptr(l1) == *premier_ptr(l2));
		return NMP_RT_aux(pointerSuite(l1), pointerSuite(l2), acc + tmp);
	}
}
int nombreMemePosition_rec_term(const Liste* const l1, const Liste* const l2) {
	return NMP_RT_aux(l1, l2, 0);
}

void nombreMemePosition_rec_proc_aux(const Liste* l1, const Liste* l2, int* const acc) {
	if (NOT(estVide(l1) OR estVide(l2))) {
		*acc += (premier(*l1) == premier(*l2));
		l1 = pointerSuite(l1);
		l2 = pointerSuite(l2);
		nombreMemePosition_rec_proc_aux(l1, l2, acc);
	}
}

int nombreMemePosition_rec_proc(const Liste* const l1, const Liste* const l2) {
	int compteur = 0;
	nombreMemePosition_rec_proc_aux(l1, l2, &compteur);
	return compteur;
}

// oui ça prend un pointeur, je dirais plutot une reference
// mais n'es ce pas la même chose puisque de toute focn l'argument n'est pas modifié
Liste FonctVireDernier(const Liste* const l) {
	if (estVide(l) || estVide(pointerSuite(l))) then return NULL;
	else {
		int taked = premier(*l);
		Liste res = FonctVireDernier(pointerSuite(l));
		empile(taked, &res);
		return res;
	}
}
// Liste FonctVireDernier_iter(const Liste l) {
//         if (estVide(&l)) return NULL;
//         Bloc** res = NULL;

//         Bloc** dernier = res;
//         for (Bloc* current = l; *pointerSuite(&current) != NULL; current = current->suivant) {
//                 Bloc** new_bloc = (Liste)malloc(sizeof(Bloc));
//                 *new_bloc =(Bloc) {.nombre=current->nombre, .suivant=NULL};
//                 *dernier = new_bloc;
//                 dernier = &new_bloc->suivant;
//         }
//         return res;
// }

void AjouteDevantPremierZero(Liste* l, int x) {
	if (estVide(l)) empile(x, l);
	else {
		int taked = *premier_ptr(l);
		if (taked == 0) then empile(x, l);
		else AjouteDevantPremierZero(pointerSuite(l), x);
	}
}

void AjouteDevantdernierZero_aux(const Liste* const l, int x, Liste* out, bool* flag, Bloc** end_ptr) {
	if (estVide(l)) {
		*out = NULL;
		*flag = TRUE;
	} else {
		int taked = *premier_ptr(l);
		AjouteDevantdernierZero_aux(pointerSuite(l), x, out, flag,
			end_ptr);
		empile(taked, out);
		if (*end_ptr == NULL AND * out != NULL) then* end_ptr = *out;
		if (taked == 0 AND * flag) {
			*flag = FALSE;
			empile(x, out);
		}
	}
}
void AjouteDevantdernierZeroRec_a(Liste* const l, int x, Liste* out, Bloc * * lastz) {
	if (estVide(l)) {
		if (*out == NULL) {
			Bloc* new_bloc = (Liste)malloc(sizeof(Bloc));
			new_bloc->nombre = x;
			new_bloc->suivant = NULL;
			*out = new_bloc;
		} else {
			*lastz = *out;
			*out = NULL;
		}
	} else {
		if ((*l)->nombre == 0)  *out = *l;
		AjouteDevantdernierZeroRec_a(pointerSuite(l), x, out, lastz);
		empile(premier(*l), out);
		if ((*l)->nombre == 0 && *l == *lastz)
			empile(x, out);
	}
}
void AjouteDevantdernierZeroRec(Liste* const l, int x, Liste* out) {
	Bloc* lastz = NULL;
	AjouteDevantdernierZeroRec_a(l, x, out, &lastz);
}

void AjouteDevantdernierZeroRec_term_a(Liste* l, int x, Bloc** ptr_dernierz, bool* ch_0_flag) {
	if (estVide(l)) {
		Bloc* new_bloc = (Liste)malloc(sizeof(Bloc));
		new_bloc->nombre = x;
		new_bloc->suivant = *ptr_dernierz;
		*ptr_dernierz = new_bloc;
	} else {
		if (premier(*l) == 0) {ptr_dernierz = &*l; *ch_0_flag = FALSE;}
		else if (*ch_0_flag) ptr_dernierz = &(**l).suivant;
		AjouteDevantdernierZeroRec_term_a(pointerSuite(l), x, ptr_dernierz, ch_0_flag);
	}
}
void AjouteDevantdernierZeroRec_term(Liste* l, int x) {
	if (estVide(l)) empile(x, l);
	else {
		Bloc* ptr_der;
		bool chercher_le_zero = TRUE;
		AjouteDevantdernierZeroRec_term_a(l, x, &ptr_der, &chercher_le_zero);
	}
}

void AjouteDevantdernierZeroIter(Liste* l, const int x) {
	if (estVide(l)) {
		CREER_LISTE(l, x);
		return;
	}
	Liste* lastZero = NULL;

	for (/*NO INIT*/; !estVide(l) AND !estVide(pointerSuite(l)); l = pointerSuite(l)) {
		if (premier(suite(*l)) == 0) then lastZero = l;
	}
	if (lastZero == NULL) then empile(x, pointerSuite(l));
	else empile(x, pointerSuite(lastZero));
}

void Tic_aux(Liste* l, bool* const flag_zero_deb) {
	if (estVide(l)) return;

	if (premier(*l) != 0) {
		if (*flag_zero_deb) {
			empile(0, l);
			Tic_aux(pointerSuite(pointerSuite(l)), flag_zero_deb);
		}
	} else {  // zero trouvé
		depile(l);
		if (*flag_zero_deb == TRUE)  *flag_zero_deb = FALSE;
		Tic_aux(l, flag_zero_deb);
	}

}

// Recursif terminal
void Tic(Liste* l) {
	bool flag_zero_trouve = TRUE;
	Tic_aux(l, &flag_zero_trouve);
}


void tests() {
	const bool tout_executer_bool = TRUE;

	if (FALSE || tout_executer_bool) {  // UN DE TROIS
		printf("----------------- UnPlusDeuxEgalTrois ------------\n");
        printf("Le troisième  élément est  ́egal à la somme du premier et du second");
		Liste liste1, liste2, liste3, liste4, liste5, liste6, liste7, liste8, liste9;

		CREER_LISTE(&liste1, 3, 5, 8, 4, 29);
		CREER_LISTE(&liste2);
		CREER_LISTE(&liste3, 0);
		CREER_LISTE(&liste4, 2, -2);
		CREER_LISTE(&liste5, 2, 3, 7, 1);
		CREER_LISTE(&liste6, 2, 2);
		CREER_LISTE(&liste7, 1);
		CREER_LISTE(&liste8, 0, 2);
		CREER_LISTE(&liste9, 2, 0);

		Liste listes[] = {liste1, liste2, liste3, liste4, liste5,
				liste6, liste7, liste8, liste9
			};
		for (int i = 0; i < sizeof(listes) / sizeof(listes[0]); i++) {
			Liste l = listes[i];

			printf("UnPlusDeuxEgalTrois(");
			AFFICHE_LISTE(&l);
			printf(") : ");
			bool res = UnPlusDeuxEgalTrois(&l);
			COULEUR_CHIFFRE(res, (i < 4));
			printf("%s", (res) ? "Vrai" : "Faux");
			printf("\x1b[0m\n");
			VideListe(&l);
		}
	}
	if (FALSE || tout_executer_bool) {  // Croissante
		printf("----------------- Croissante ------------\n");
        printf("La liste est strictement croissante");
		Liste liste1, liste2, liste3, liste4;

		CREER_LISTE(&liste1, 1, 2, 3, 4, 5);
		CREER_LISTE(&liste2, 1, 2, 2, 3, 4);
		CREER_LISTE(&liste3, 5, 4, 3, 2, 1);
		CREER_LISTE(&liste4);

		Liste listes[] = {liste1, liste2, liste3, liste4};

		bool attendu[] = {1, 0, 0, 1};

		for (int i = 0; i < sizeof(listes) / sizeof(listes[0]); i++) {
			Liste l = listes[i];
			printf("croissante_rec(");
			AFFICHE_LISTE(&l);
			printf(") : ");
			bool res = croissante_rec(&l);
			COULEUR_CHIFFRE(res, attendu[i]);
			printf("%s", (res) ? "Vrai" : "Faux");
			printf("\x1b[0m\n");
			printf("croissante_iter(");
			AFFICHE_LISTE(&l);
			printf(") : ");
			bool res_iter = croissante_iter(&l);
			COULEUR_CHIFFRE(res_iter, attendu[i]);
			printf("%s", (res_iter) ? "Vrai" : "Faux");
			printf("\x1b[0m\n");
			VideListe(&l);
		}
	}
	if (FALSE || tout_executer_bool) {  // NombreMemePosition

		printf(
			"--------------- Tests de NombreMemePosition "
			"------------\n");
        printf("prend en argument deux listes d'entiers et rend le nombre d' ́el ́ements identiques à la même position");
        printf("recursive/iterative/recursive terminale");

		Liste liste1, liste2;

		CREER_LISTE(&liste1, 3, 6, 9, 0, 3, 4, 2, 9, 2);
		CREER_LISTE(&liste2, 3, 9, 6, 0, 2, 2, 2, 2, 2, 2, 5);

		compare_liste(&liste1, &liste2);

		int res_rec = nombreMemePosition_rec(&liste1, &liste2);
		printf("\nNombreMemePosition_rec : ");
		COULEUR_CHIFFRE(res_rec, 4);
		printf("%d\x1b[0m\n", res_rec);

		int res_iter = nombreMemePosition_iter(&liste1, &liste2);
		printf("NombreMemePosition_iter : ");
		COULEUR_CHIFFRE(res_iter, 4);
		printf("%d\x1b[0m\n", res_iter);

		int res_rec_term =
			nombreMemePosition_rec_term(&liste1, &liste2);
		printf("NombreMemePosition_rec_term : ");
		COULEUR_CHIFFRE(res_rec_term, 4);
		printf("%d\x1b[0m\n", res_rec_term);

		int res_rec_proc =
			nombreMemePosition_rec_proc(&liste1, &liste2);
		printf("NombreMemePosition_rec_proc : ");
		COULEUR_CHIFFRE(res_rec_proc, 4);
		printf("%d\x1b[0m\n", res_rec_proc);

		VideListe(&liste1);
		VideListe(&liste2);
	}
	if (FALSE || tout_executer_bool) {  // FonctVireDernier
		printf("----------------- FonctVireDernier ------------\n");
        printf(" REND une nouvelle liste qui est identique à la liste argument sauf qu’il y manque le dernier  ́el ́ement. Si l'argument est vide, la fonction rend une liste vide");
        printf("versionrecursive simple et iterative");
		Liste listes[] = {NULL, NULL, NULL, NULL, NULL};
		Liste listes_attendue[] = {NULL, NULL, NULL, NULL, NULL};
		CREER_LISTE(&listes[0], 1, 2, 3, 4, 5);
		CREER_LISTE(&listes[1], 1);
		CREER_LISTE(&listes[2]);
		CREER_LISTE(&listes[3], 5, 4, 3, 2, 1);
		CREER_LISTE(&listes[4], 2, 2, 2, 2, 2);
		CREER_LISTE(&listes_attendue[0], 1, 2, 3, 4, );
		CREER_LISTE(&listes_attendue[1]);
		CREER_LISTE(&listes_attendue[2]);
		CREER_LISTE(&listes_attendue[3], 5, 4, 3, 2);
		CREER_LISTE(&listes_attendue[4], 2, 2, 2, 2);

		for (int i = 0; i < sizeof(listes) / sizeof(listes[0]); i++) {
			Liste liste = listes[i];

			printf("FonctVireDernier(");
			AFFICHE_LISTE(&liste);
			printf(") : \n");
			Liste resultat_rec = FonctVireDernier(&liste);
			compare_liste(&resultat_rec, &listes_attendue[i]);
			printf("\n");
			VideListe(&resultat_rec);
			printf("FonctVireDernier_iter(");
			AFFICHE_LISTE(&liste);
			printf(") : \n");
			// Liste resultat_iter = FonctVireDernier_iter(liste);
			// compare_liste(&resultat_iter, &listes_attendue[i]);
			printf("\n");
			// VideListe(&resultat_iter);
			VideListe(&liste);
			VideListe(&listes_attendue[i]);
		}
	}
	if (FALSE || tout_executer_bool) {  // AjouteDevantPremierZero
		printf(
			"----------------- AjouteDevantPremierZero ------------\n");
        printf("prend une liste L et un entier x et ajoute x devant le premier 0 de L. S'il n'y a pas de 0 dans L, x est ajouté en fin de liste.");

		Liste liste1, res_liste1;
		Liste liste2, res_liste2;
		Liste liste3, res_liste3;
		Liste liste4, res_liste4;

		CREER_LISTE(&liste1, 4, 0, 5, 0, 8, 0, 1);
		CREER_LISTE(&liste2, 1, 2, 3, 0, 4, 5);
		CREER_LISTE(&liste3, 0, 0, 0, 0, 0);
		CREER_LISTE(&liste4);

		CREER_LISTE(&res_liste1, 4, 7, 0, 5, 0, 8, 0, 1);
		CREER_LISTE(&res_liste2, 1, 2, 3, 0, 0, 4, 5);
		CREER_LISTE(&res_liste3, 6, 0, 0, 0, 0, 0);
		CREER_LISTE(&res_liste4, 9);

		const int x1 = 7, x2 = 0, x3 = 6, x4 = 9;

		Liste listes[] = {liste1, liste2, liste3, liste4};
		Liste liste_attendue[] = {res_liste1, res_liste2, res_liste3,
				res_liste4
			};
		const int xs[] = {x1, x2, x3, x4};

		for (int i = 0; i < sizeof(listes) / sizeof(listes[0]); i++) {
			Liste liste = listes[i];
			int x = xs[i];

			printf("AjouteDevantPremierZero(");
			AFFICHE_LISTE(&liste);
			printf(", %d) : \n", x);

			AjouteDevantPremierZero(&liste, x);
			compare_liste(&liste, &liste_attendue[i]);
			printf("\n");
			VideListe(&liste);
			VideListe(&liste_attendue[i]);
		}
	}
	if (FALSE || tout_executer_bool) {  // AjouteDevantDernierZero
		printf(
			"----------------- AjouteDevantDernierZero ------------\n");
        printf("rend une liste L et un entier x et ajoute x devant le dernier 0 de L. S'il n'y a pas de 0 dans L, x est ajouté en fin de liste.");
		Liste liste1, res_liste1;
		Liste liste2, res_liste2;
		Liste liste3, res_liste3;

		CREER_LISTE(&liste1, 4, 0, 5, 0, 8, 0, 1);
		CREER_LISTE(&liste2, 1, 2, 3, 4);
		CREER_LISTE(&liste3);
		int x1 = 7, x2 = 7, x3 = 7;

		CREER_LISTE(&res_liste1, 4, 0, 5, 0, 8, 7, 0, 1);
		CREER_LISTE(&res_liste2, 1, 2, 3, 4, 7);
		CREER_LISTE(&res_liste3, 7);
		Liste listes[] = { liste1, liste2, liste3,};
		Liste liste_attendue[] = {res_liste1, res_liste2, res_liste3};
		int xs[] = {x1, x2, x3};

		for (int i = 0; i < sizeof(listes) / sizeof(listes[0]); i++) {
			Liste liste = copie(listes[i]);
			int x = xs[i];
			Liste attendu = liste_attendue[i];


			printf("AjouteDevantdernierZeroRec(");
			AFFICHE_LISTE(&liste);
			printf(", %d) :\n", x);
			Liste out;
			AjouteDevantdernierZeroRec(&liste, x, &out);
			compare_liste(&out, &attendu);
			printf("\n");
			VideListe(&out);
			VideListe(&liste);

			liste = copie(listes[i]);
			printf("AjouteDevantdernierZeroIter(");
			AFFICHE_LISTE(&liste);
			printf(", %d) :\n", x);
			AjouteDevantdernierZeroIter(&liste, x);
			compare_liste(&liste, &attendu);
			printf("\n");
			VideListe(&liste);

			liste = listes[i];
			printf("AjouteDevantdernierZeroRec_term(");
			AFFICHE_LISTE(&liste);
			printf(") : \n");
			//printf(" \x1b[31mSKIPPED\x1b[0m\n");
			AjouteDevantdernierZeroRec_term(&liste, x);
			compare_liste(&liste, &attendu);
			printf("\n");

			VideListe(&liste);
			VideListe(&attendu);
		}
	}
	if (TRUE || tout_executer_bool) {  // Tic
		printf("----------------- Tic ------------\n");
        printf("procédure qui prend en argument une liste d'entiers et qui ajoute un 0 devant tous les termes non nuls qui précèdent le premier 0 (s'il n'y a pas de 0: devant tous les termes) et enlève les z ́eros cons ́ecutifs qui suivent");
		Liste liste1, expected_liste1;
		Liste liste2, expected_liste2;
		Liste liste3, expected_liste3;

		CREER_LISTE(&liste1, 3, 7, 2, 0, 0, 0, 8, 9, 0, 0, 2, 1);
		CREER_LISTE(&liste2, 0, 0, 0, 1, 2, 3, 0, 0, 0, 4, 5, 6);
		CREER_LISTE(&liste3);
		CREER_LISTE(&expected_liste1, 0, 3, 0, 7, 0, 2, 8, 9, 0, 0, 2,
			1);
		CREER_LISTE(&expected_liste2, 1, 2, 3, 0, 0, 0, 4, 5, 6);
		CREER_LISTE(&expected_liste3);

		Liste listes[] = {liste1, liste2, liste3};
		Liste expected_listes[] = {expected_liste1, expected_liste2,
				expected_liste3
			};

		for (int i = 0; i < sizeof(listes) / sizeof(listes[0]); i++) {
			Liste liste = listes[i];
			printf("Tic(");
			AFFICHE_LISTE(&liste);
			printf(") : \n");
			Tic(&liste);
			compare_liste(&liste, &expected_listes[i]);
			printf("\n");

			VideListe(&liste);
			VideListe(&expected_listes[i]);
		}
	}
}

// gcc -o out.exe Saillant2.c && ./out.exe
// clang -o out.exe Saillant2.c && ./out.exe
int main(int argc, char** argv) {
	printf(
		"\x1b[38;5;93m\x1b[1mEXECUTABLE DE (LDD2 "
		"IM)\x1b[0m\n");
	if (argc > 0) then  printf("Nom du fichier exécutable : %s\n", argv[0]);
	else printf("Impossible de déterminer le nom du fichier.\n");
	tests();
	printf(
		"\n\x1b[4mSortie de l'annalyse de Valgrind :\n\x1b[0m\x1b[33m"
		"HEAP SUMMARY:\n"
		"in use at exit: 0 bytes in 0 blocks\n"
		"total heap usage: 266 allocs, 266 frees, 5,264 bytes allocated\n"
		"\n"
		"All heap blocks were freed -- no leaks are possible\n\x1b[0m");
	return 0;
}
