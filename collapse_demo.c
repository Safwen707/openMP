#include <omp.h>
#include <stdio.h>

void exemple_sans_collapse() {
    printf("=== SANS COLLAPSE - PROBLÈME ===\n");
    printf("Boucles: i=0..3 (4 itérations), j=0..999 (1000 itérations)\n");
    printf("8 threads disponibles\n\n");
    
    int compteur[8] = {0};  // Compteur par thread
    
    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < 4; i++) {
        int thread_id = omp_get_thread_num();
        
        for (int j = 0; j < 1000; j++) {
            compteur[thread_id]++;
            
            // Affichage seulement pour les premières itérations
            if (j < 3) {
                printf("Thread %d: i=%d, j=%d\n", thread_id, i, j);
            }
        }
    }
    
    printf("\nDistribution du travail:\n");
    for (int t = 0; t < 8; t++) {
        printf("Thread %d: %d itérations\n", t, compteur[t]);
    }
    
    printf("\n❌ PROBLÈME: Seulement 4 threads actifs sur 8!\n");
    printf("   4 threads ne font RIEN (gaspillage!)\n\n");
}

void exemple_avec_collapse() {
    printf("=== AVEC COLLAPSE(2) - SOLUTION ===\n");
    printf("Boucles aplaties: 4 × 1000 = 4000 itérations totales\n");
    printf("8 threads disponibles\n\n");
    
    int compteur[8] = {0};  // Compteur par thread
    
    #pragma omp parallel for collapse(2) num_threads(8)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {
            int thread_id = omp_get_thread_num();
            compteur[thread_id]++;
            
            // Affichage pour quelques itérations
            if ((i == 0 && j < 3) || (i == 1 && j < 3)) {
                printf("Thread %d: i=%d, j=%d\n", thread_id, i, j);
            }
        }
    }
    
    printf("\nDistribution du travail:\n");
    int total = 0;
    for (int t = 0; t < 8; t++) {
        printf("Thread %d: %d itérations\n", t, compteur[t]);
        total += compteur[t];
    }
    printf("Total: %d itérations (4×1000)\n", total);
    
    printf("\n✅ SOLUTION: TOUS les threads travaillent!\n");
    printf("   Chaque thread: ~500 itérations (4000/8)\n\n");
}

void exemple_visualisation() {
    printf("=== VISUALISATION DE L'ORDRE D'EXÉCUTION ===\n");
    printf("Ordre séquentiel: (0,0), (0,1), ..., (0,999), (1,0), ...\n\n");
    
    printf("Avec collapse(2), affichage des 20 premières itérations:\n");
    
    int count = 0;
    #pragma omp parallel for collapse(2) num_threads(8) schedule(static)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {
            #pragma omp critical
            {
                if (count < 20) {
                    printf("Itération %2d: Thread %d exécute (i=%d, j=%d)\n", 
                           count, omp_get_thread_num(), i, j);
                    count++;
                }
            }
        }
    }
    printf("\n");
}

void exemple_performance() {
    printf("=== COMPARAISON DE PERFORMANCE ===\n");
    
    // Simulation de calcul
    int tableau[4][1000];
    
    // Initialisation
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {
            tableau[i][j] = i * 1000 + j;
        }
    }
    
    // SANS COLLAPSE
    double start = omp_get_wtime();
    
    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {
            // Simulation de calcul
            volatile int calcul = tableau[i][j] * 2;
            for (int k = 0; k < 1000; k++) {
                calcul += k;
            }
        }
    }
    
    double temps_sans_collapse = omp_get_wtime() - start;
    
    // AVEC COLLAPSE
    start = omp_get_wtime();
    
    #pragma omp parallel for collapse(2) num_threads(8)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {
            // Même calcul
            volatile int calcul = tableau[i][j] * 2;
            for (int k = 0; k < 1000; k++) {
                calcul += k;
            }
        }
    }
    
    double temps_avec_collapse = omp_get_wtime() - start;
    
    printf("Temps SANS collapse: %.6f secondes\n", temps_sans_collapse);
    printf("Temps AVEC collapse: %.6f secondes\n", temps_avec_collapse);
    
    double speedup = temps_sans_collapse / temps_avec_collapse;
    printf("Speedup: %.2fx\n", speedup);
    
    if (speedup > 1.2) {
        printf("✅ collapse améliore significativement la performance!\n\n");
    } else {
        printf("⚠️  Amélioration modérée\n\n");
    }
}

void exemple_triple_collapse() {
    printf("=== COLLAPSE(3) - TROIS BOUCLES ===\n");
    printf("Boucles: i=0..1, j=0..2, k=0..1000\n");
    printf("Total: 2 × 3 × 1000 = 6000 itérations\n\n");
    
    int compteur[8] = {0};
    
    #pragma omp parallel for collapse(3) num_threads(8)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 1000; k++) {
                int thread_id = omp_get_thread_num();
                compteur[thread_id]++;
                
                if (i == 0 && j == 0 && k < 5) {
                    printf("Thread %d: (i=%d, j=%d, k=%d)\n", thread_id, i, j, k);
                }
            }
        }
    }
    
    printf("\nDistribution:\n");
    for (int t = 0; t < 8; t++) {
        printf("Thread %d: %d itérations\n", t, compteur[t]);
    }
    printf("\n");
}

int main() {
    printf("COLLAPSE : OPTIMISATION DES BOUCLES IMBRIQUÉES\n");
    printf("==============================================\n\n");
    
    exemple_sans_collapse();
    exemple_avec_collapse();
    exemple_visualisation();
    exemple_performance();
    exemple_triple_collapse();
    
    printf("=== RÈGLES D'UTILISATION ===\n");
    printf("✅ Utiliser collapse quand:\n");
    printf("   - Boucle externe a PEU d'itérations\n");
    printf("   - Boucle interne a BEAUCOUP d'itérations\n");
    printf("   - Plus de threads que d'itérations externes\n");
    printf("   - Pas de dépendances entre boucles\n\n");
    
    printf("❌ Ne PAS utiliser collapse quand:\n");
    printf("   - Boucles ont des dépendances\n");
    printf("   - Boucle externe a déjà assez d'itérations\n");
    printf("   - Break/continue/return dans les boucles\n\n");
    
    printf("=== SYNTAXE ===\n");
    printf("#pragma omp parallel for collapse(2)  // 2 boucles\n");
    printf("#pragma omp parallel for collapse(3)  // 3 boucles\n");
    printf("// collapse(n) = n boucles consécutives aplaties\n");
    
    return 0;
}