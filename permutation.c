#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef enum { FALSE, TRUE } bool;
#define then
#define PANIC(msg, ...)                                                   \
        {                                                                 \
                fprintf(stderr, "\x1b[38;5;196mErreur : \x1b[38;5;210m"); \
                fprintf(stderr, msg, ##__VA_ARGS__);                      \
                fprintf(stderr, "\x1b[0m\n\n\n");                         \
                /* force le segfault pour debug*/                         \
                int *ptr = NULL;                                          \
                *ptr = 42;                                                \
        };

#define CREER_LISTE(l_ptr, ...)                         \
        {                                               \
                *l_ptr = NULL;                        \
                int args[] = {__VA_ARGS__};             \
                int n = sizeof(args) / sizeof(args[0]); \
                for (int i = 0; i < n; i++) {           \
                        empile(args[n - i - 1], l_ptr); \
                }                                       \
        }


int compteur_malloc = 0, compteur_free = 0;
int compteur_mallocll = 0, compteur_freell = 0;



// Reference counting
typedef struct Rc {
        // pointeur vers la fonction pour liberer le type
        void (*free)(struct Rc*); 
        int count;
} Rc;

#define rc_owner(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

void rc_inc(Rc *rc) { rc->count++; }

void ref_dec(Rc *rc) {
    if (--(rc->count) == 0) rc->free(rc);
}



/////////////////// LISTE de Int ///////////////////////

// Liste chainée d'entier
typedef struct BlocInt {
        int nombre;
        struct BlocInt *suivant;
        Rc refcount;
} BlocInt;
typedef BlocInt* ListeInt;

typedef enum TypeAffichage { Valeurs, Adresses, CompteurRef} TypeAffichage;
void valeur(BlocInt* b) {printf("%d, ", b->nombre);}
void pointeur(BlocInt* b) {printf("%p, ", b);}
void refcounter(BlocInt* b) {printf("%d, ", b->refcount);}

bool estVide(const ListeInt l) { return l == NULL; }
int premier(const ListeInt l) { 
    if (l == NULL) { PANIC("pointeur null (premier)");} 
    else return l->nombre ;
}
void blocint_free(Rc *Rc) {
    BlocInt* bloc = rc_owner(Rc, struct BlocInt, refcount);
    BlocInt* suiv = bloc->suivant;
    free(bloc);
    compteur_free++;
    if (suiv != NULL) ref_dec(&suiv->refcount);
}

ListeInt ajoute(int x, const ListeInt l) {
        ListeInt tmp = (ListeInt) malloc(sizeof(BlocInt));
        compteur_malloc++;
        Rc rc = (Rc){.free=blocint_free, .count=1};
        *tmp = (BlocInt) {.nombre=x, .suivant=l, .refcount=rc};
        return tmp;
}
void empile(int x, ListeInt *L) { *L = ajoute(x,*L); }
ListeInt suite(const ListeInt l) { if (l == NULL) { PANIC("pointeur null (suite)");} else return l->suivant; }
ListeInt* pointerSuite(const ListeInt* const  l) { if (l == NULL) { PANIC("pointeur null (pointeursuite)");} else return &(*l)->suivant; }

typedef struct { ListeInt* l; void (*fct_acces_aff)(BlocInt*); } paramAffichage_liste;
void affiche_liste_int(paramAffichage_liste paf) {
        if (estVide(*paf.l)) { printf("\x1b[38;5;50mNULL\x1b[0m"); return; }
        printf("["); 
        for(BlocInt* c = *paf.l; c != NULL; c=c->suivant) 
                paf.fct_acces_aff(c);
        printf("\x1b[2D]"); 
}
#define affiche_liste_int(...) affiche_liste_int((paramAffichage_liste){__VA_ARGS__});


// clone la liste d'entier sans la copier
BlocInt* rc_clone(BlocInt** bloc) {
        if (*bloc == NULL) return NULL;
        else {
                rc_inc(&(*bloc)->refcount);
                return *bloc;
        }
}

// free la liste d'entiers
void VideListe(ListeInt *const L) {
        BlocInt** curr = L;
        ref_dec(&(*curr)->refcount);
        *L = NULL;
}



/////////////////// LISTE de LISTE ///////////////////////

// Liste chainée de Liste
typedef struct BlocListe {
        ListeInt liste;
        struct BlocListe *suivant;
} BlocListe;
typedef BlocListe* ListeListe;

void initVidell(ListeListe*L) { *L = NULL; }
bool estVidell(const ListeListe* l) { return *l == NULL; }
ListeInt premierll(const ListeListe l) {
        if (l == NULL) then {PANIC("pointeur null (premierll)");}
        return l->liste;
}
ListeInt* pointeurpremierll(const ListeListe* const l) {
        if (*l == NULL) then {PANIC("pointeur null (pointeurpremierll) (aussi le parametre est une Rc)");}
        return &(*l)->liste;
}
ListeListe ajoutell(const ListeInt* const x, const ListeListe l) {
        ListeListe tmp = (ListeListe) malloc(sizeof(BlocListe)) ;
        compteur_mallocll++;
        *tmp = (BlocListe){.liste=*x, .suivant=l};
        return tmp;
}
void empilell(ListeInt* x, ListeListe *L) { *L = ajoutell(x,*L) ; }
ListeListe suitell(const ListeListe l) {
        if (l == NULL) then PANIC("pointeur null (suitell)");
        return l->suivant ;
}
ListeListe* pointerSuitell(const ListeListe* const  l) {
        if (l == NULL || *l == NULL) then PANIC("pointeur null (pointeursuitell)");
        return &(*l)->suivant;
}
// parametre fct d'afichage
typedef struct { ListeListe* l; bool espace; TypeAffichage type_affichage; } paramAffichage_LL;
void affiche_LL(paramAffichage_LL pll) {
        if (*pll.l == NULL) { printf("\x1b[38;5;50mNULL\x1b[0m"); if (pll.espace) then printf("\n"); return; }
        printf("[");
        if (pll.espace) then printf("\n\t");
        for (BlocListe* c = *pll.l; c != NULL; c = c->suivant) {
                switch (pll.type_affichage) {
                case Valeurs: affiche_liste_int(&c->liste, valeur); break;
                case Adresses: affiche_liste_int(&c->liste, pointeur); break;
                case CompteurRef: affiche_liste_int(&c->liste, refcounter); break;
                }
                printf(", ");
                if (pll.espace) then printf("\n\t");
        }
        printf("\x1b[E]\n");
}
// espace affiche une liste sur chaque lignes
#define affiche_LL(...) affiche_LL((paramAffichage_LL){__VA_ARGS__});

void depilell(ListeListe *L) {
        ListeInt* o = &(*L)->liste;
        ListeListe tmp = *L;
        *L = suitell(*L) ;
        VideListe(o);
        free(tmp) ;
        compteur_freell++;
}

void VideListell(ListeListe *L) {
        if (*L != NULL) {
                depilell(L);
                VideListell(L);
        }
}

int longueur(ListeListe* l) {
        if (l == NULL) return 0;
        int res = 0;
        for (BlocListe* c = *l; c != NULL; c = c->suivant) res++;
        return res;
}

// Vide seulement les bloc de liste de liste (ne vide pas les liste d'entier)
void videOnlyll(ListeListe *L) {
        if (estVidell(L)) return;
        ListeListe tmp = *L;
        *L = suitell(*L) ;
        compteur_freell++;
        free(tmp) ;
        videOnlyll(L);
}

////////////*/*/*/*/*/*/*/*/*/*/*//////////
////////////       ALGO      //////////////
////////////*/*/*/*/*/*/*/*/*/*/*//////////


ListeListe permutations( int n);
// Ajoute Toute Position
ListeListe ATP( int x, ListeInt* L);
//  Ajoute Toute Liste Toute Position
//  ATLTP( 4, [[5,7,9],[2],[3,8],[]] )
//  -> [[4,5,7,9],[5,4,7,9],[5,7,4,9],[5,7,9,4],[4,2],[2,4],[4,3,8],[3,4,8],[3,8,4],[4]]
ListeListe ATLTP ( int x, ListeListe* L);
// Ajoute En Tete Toute Liste
// AETTLTP( 6, [[3,7,9],[2],[4,8],[]] ) -> [[6,3,7,9],[6,2],[6,4,8],[6]]
void AETTL( int x, ListeListe* L);
// ListeListe concatll( ListeListe L1,  ListeListe ListeListe);
void concatll(ListeListe * L1,  ListeListe * L2);


ListeListe permutations(const int n) {
        if (n == 0) {
                ListeInt li = NULL;
                ListeListe r = NULL;
                empilell(&li,&r);
                return r; // [[]]
        } 
        else {
                ListeListe llperm = permutations(n-1);
                return ATLTP( n, &llperm );
        }
}

//  Ajoute Toute Liste Toute Position
//  ATLTP( 4, [[5,7,9],[2],[3,8],[]] )
//  -> [[4,5,7,9],[5,4,7,9],[5,7,4,9],[5,7,9,4],[4,2],[2,4],[4,3,8],[3,4,8],[3,8,4],[4]]
// NO LEAK
ListeListe ATLTP( int x, ListeListe* L) {
        if (*L == NULL) { 
                ListeListe ll;  
                initVidell(&ll);
                return ll;
        } else { 
                ListeListe llatp = ATP(x, &(*L)->liste);
                ListeListe llatltp = ATLTP(x,pointerSuitell(L));
                videOnlyll(L);
                concatll(&llatp, &llatltp);         
                return llatp;

        }
}

// ATTENTION L2 EST VIDé
void concatll(ListeListe* L1,  ListeListe* L2) {
        if (*L1 == NULL) *L1 = *L2;
        else {
                BlocListe** i = &(*L1)->suivant;
                for (/*NO local INIT*/;*i != NULL; i = &(*i)->suivant) {}
                *i = *L2;  // Concaténation de L2 à la fin de L1
        }
}
// L1 est vidé
// void concatll(ListeListe* L1,  ListeListe* L2) {
//         if (*L1 == NULL) {}
//         else { 
//                 BlocListe* l1 = *L1;
//                 *L1 = (*L1)->suivant;
//                 concatll(L1, L2);
//                 l1->suivant = *L2;
//                 *L2 = l1;
//         }
// }


// Ajoute Toute Position
// ATP(4, [5,7,9]) -> [[4,5,7,9], [5,4,7,9], [5,7,4,9], [5,7,9,4]]
// NO LEAK
ListeListe ATP( int x,  ListeInt* L) {
        if (*L == NULL) {
                ListeInt li = NULL; empile(x, &li);
                ListeListe ll = NULL; empilell(&li, &ll);
                return ll;
        } else { 
                int p = premier(*L);
                ListeInt t = rc_clone(pointerSuite(L));
                ListeListe inter = ATP(x, &t);
                AETTL(p, &inter);
                ListeInt new_l = ajoute(x, *L);
                empilell(&new_l, &inter);
                return inter;
        }
}

// Ajoute En Tete Toute Liste
// AETTLTP( 6, [[3,7,9],[2],[4,8],[]] ) -> [[6,3,7,9],[6,2],[6,4,8],[6]]
// NO LEAK
void AETTL( int x,  ListeListe* L) {
        if (*L == NULL) { return;}
        else { 
                AETTL(x, pointerSuitell(L));
                empile(x, pointeurpremierll(L));
        }
}


void tests();
// gcc -o out.exe permutation.c && ./out.exe
// clang -o out.exe permutation.c && ./out.exe
int main(int argc, char** argv) {
        printf(
            "\x1b[38;5;93m\x1b[1mEXECUTABLE DE TortueSandwich (LDD2 "
            "IM)\x1b[0m\n");
        if (argc > 0) then printf("Nom du fichier exécutable : %s\n", argv[0]);
        else printf("Impossible de déterminer le nom du fichier.\n");
        // tests();
        int n = 5;
        ListeListe liste_perm = NULL;
        liste_perm = permutations(n);
        if (liste_perm == NULL) then printf("VIDE\n");
        affiche_LL(&liste_perm, .espace=FALSE, .type_affichage = Valeurs);

        printf("Permutations de taille : %d\n", n);
        printf("Nombre de permutation (longueur liste) : %d\n", (longueur(&liste_perm)));
        if (n < 9) VideListell(&liste_perm);

        printf("\nBLOCINT\nNombre de malloc (comptage à la mains) : %d\n", compteur_malloc);
        printf("Nombre de free (comptage à la mains) : %d\n", compteur_free);
        printf("Nombre de bloc en fuite : %d\n", (compteur_malloc - compteur_free));

        printf("\nBLOCLISTE\nNombre de malloc (comptage à la mains) : %d\n", compteur_mallocll);
        printf("Nombre de free (comptage à la mains) : %d\n", compteur_freell);
        printf("Nombre de bloc en fuite : %d\n\n", (compteur_mallocll - compteur_freell));

        int accf = 1;
        for (int i = 1; i < 10; i++) {
                accf *= i;
                compteur_malloc = 0; compteur_free = 0;
                compteur_mallocll = 0; compteur_freell = 0;
                ListeListe new_perm = NULL;
                new_perm = permutations(i);
                if (i < 9) VideListell(&new_perm);
                printf("n = %d : (n! = %d) ((n+1)!-n!) = %d\n", i, accf, accf*(i+1) - accf);
                printf("BlocInt : %d alloc, %d free, %d en fuite\n", compteur_malloc, compteur_free, (compteur_malloc - compteur_free));
                printf("BlocListe : %d alloc, %d free, %d en fuite\n", compteur_mallocll, compteur_freell, (compteur_mallocll - compteur_freell));
                printf("Total : %d alloc, %d free, %d en fuite\n\n", (compteur_malloc+compteur_mallocll), (compteur_free+compteur_freell), ((compteur_malloc+compteur_mallocll)-(compteur_free+compteur_freell)));
        }
        printf("Je pense ne pas avoir assez de place pour liberer n=9\n");

        printf(
            "\n\x1b[4mSortie de l'analyse de Valgrind en fonction de n:\n\x1b[0m\x1b[33m"
            "HEAP SUMMARY:\n"
            "in use at exit: 0 bytes in 0 blocks\n"
            "n = 1 : total heap usage: 4 allocs, 4 frees, 1,088 bytes allocated\n"
            "n = 2 : total heap usage: 9 allocs, 9 frees, 1,216 bytes allocated\n"
            "n = 3 : total heap usage: 27 allocs, 27 frees, 1,696 bytes allocated\n"
            "n = 4 : total heap usage: 111 allocs, 111 frees, 4,000 bytes allocated\n"
            "n = 5 : total heap usage: 591 allocs, 591 frees, 17,440 bytes allocated\n"
            "n = 6 : total heap usage: 3,831 allocs, 3,831 frees, 109,600 bytes allocated\n"
            "n = 7 : total heap usage: 29,031 allocs, 29,031 frees, 835,360 bytes allocated\n"
            "n = 8 : total heap usage: 250,791 allocs, 250,791 frees, 7,286,560 bytes allocated\n"
            "n = 9 : total heap usage: 2,428,071 allocs, 46,235 frees, 71,153,440 bytes allocated\n"
            "n = 10 : \x1b[38;5;196mSegmentation fault\x1b[33m\n"
            "\n"
            "All heap blocks were freed -- no leaks are possible\n\x1b[0m");

        return 0;
}


void tests() {
        {
                ListeInt l1, l2, l3, l4;
                CREER_LISTE(&l1, 3, 7, 9);
                CREER_LISTE(&l2, 2);
                CREER_LISTE(&l3, 4,8);
                CREER_LISTE(&l4);

                ListeListe L = NULL;
                empilell(&l4,&L);
                empilell(&l3,&L);
                empilell(&l2,&L);
                empilell(&l1,&L);

                printf("AETTL(6,"); affiche_LL(&L); printf(") = ");
                AETTL(6, &L);
                affiche_LL(&L); printf("\n");

        }
        {
                ListeInt l1 = NULL; CREER_LISTE(&l1, 5, 7, 9);
                printf("ATP(4,"); affiche_liste_int(&l1); printf(") = ");
                ListeListe res = ATP(4, &l1);
                affiche_LL(&res); printf("\n");
        }
        {
                ListeInt l1,l2,l3,l4;
                CREER_LISTE(&l1, 1,2,3);
                CREER_LISTE(&l2, 4);
                CREER_LISTE(&l3, 5,6);
                CREER_LISTE(&l4, 7,8,9);


                ListeListe L1 = NULL;
                empilell(&l2,&L1);
                empilell(&l1,&L1);
                ListeListe L2 = NULL;
                empilell(&l4,&L2);
                empilell(&l3,&L2);

                printf("concatll("); affiche_LL(&L1); printf(", "); affiche_LL(&L2); printf(") = ");
                concatll(&L1, &L2);
                affiche_LL(&L1); printf(" / "); affiche_LL(&L2); printf("\n");
        }
        {
                ListeInt l1,l2,l3,l4;
                CREER_LISTE(&l1, 5, 7, 9);
                CREER_LISTE(&l2,2);
                CREER_LISTE(&l3, 3, 8);
                CREER_LISTE(&l4);
                ListeListe L = NULL; empilell(&l4,&L); empilell(&l3,&L); empilell(&l2,&L); empilell(&l1,&L);
                printf(" ATLTP(4,"); affiche_LL(&L); printf(") = ");
                ListeListe res = ATLTP(4, &L);
                affiche_LL(&res); printf("\n");
        }
        {
                long int f = 1; 
                for(int n = 1; n <= 4; n++) {
                        ListeListe p;
                        initVidell(&p);
                        p = permutations(n);
                        int y = (longueur(&p));
                        printf("len(permutations(%d)) : %d =?= %d\n", n, y, f);
                }
                printf("\n");
        }
}
