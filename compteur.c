#include <omp.h>
#include <stdio.h>

int compteur = 0;  // Ressource partagée

int main() {
    #pragma omp parallel num_threads(4)
    {
        // ❌ DANGER : Pas de protection
        compteur = compteur + 1;
    }
    
    printf("Compteur = %d (devrait être 4)\n", compteur);
    return 0;
}
// Le problème vient du fait que plusieurs threads peuvent
//  lire la même valeur avant qu'elle soit mise à jour, puis écrire des valeurs obsolètes. 