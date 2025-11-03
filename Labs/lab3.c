/*
 * LAB 3: Nombres Premiers avec OpenMP
 * 
 * Objectif: Trouver tous les nombres premiers jusqu'à N
 * Test avec: 10, 100, 1000, 10000, 100000
 * 
 * Méthodes comparées:
 * 1. Séquentiel (baseline)
 * 2. Parallèle avec reduction
 * 3. Parallèle avec schedule(static)
 * 4. Parallèle avec schedule(dynamic)
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Fonction pour vérifier si un nombre est premier
int est_premier(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    
    int limite = (int)sqrt((double)n);
    for (int i = 3; i <= limite; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Méthode 1: SÉQUENTIEL
int count_primes_sequential(int n) {
    int count = 0;
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            count++;
        }
    }
    return count;
}

// Méthode 2: PARALLÈLE avec REDUCTION
int count_primes_parallel_reduction(int n, int num_threads) {
    int count = 0;
    
    #pragma omp parallel for reduction(+:count) num_threads(num_threads)
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            count++;
        }
    }
    return count;
}

// Méthode 3: PARALLÈLE avec SCHEDULE STATIC
int count_primes_parallel_static(int n, int num_threads) {
    int count = 0;
    
    #pragma omp parallel for schedule(static) reduction(+:count) num_threads(num_threads)
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            count++;
        }
    }
    return count;
}

// Méthode 4: PARALLÈLE avec SCHEDULE DYNAMIC
int count_primes_parallel_dynamic(int n, int num_threads) {
    int count = 0;
    
    #pragma omp parallel for schedule(dynamic, 100) reduction(+:count) num_threads(num_threads)
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            count++;
        }
    }
    return count;
}

// Fonction pour afficher les premiers nombres premiers (pour petits N)
void afficher_premiers(int n) {
    printf("Nombres premiers jusqu'à %d: ", n);
    int count = 0;
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            printf("%d ", i);
            count++;
        }
    }
    printf("\n(Total: %d nombres premiers)\n\n", count);
}

// Test de performance pour une taille donnée
void test_performance(int n, int num_threads) {
    printf("==== N = %d ====\n", n);
    
    // Afficher les nombres premiers pour les petites valeurs
    if (n <= 100) {
        afficher_premiers(n);
    }
    
    double start, end;
    int result;
    
    // 1. SÉQUENTIEL
    start = omp_get_wtime();
    result = count_primes_sequential(n);
    end = omp_get_wtime();
    double time_seq = end - start;
    printf("1. SÉQUENTIEL:\n");
    printf("   Nombres premiers trouvés: %d\n", result);
    printf("   Temps: %.6f secondes\n\n", time_seq);
    
    // 2. PARALLÈLE avec REDUCTION (schedule par défaut)
    start = omp_get_wtime();
    result = count_primes_parallel_reduction(n, num_threads);
    end = omp_get_wtime();
    double time_red = end - start;
    printf("2. PARALLÈLE (reduction, schedule par défaut):\n");
    printf("   Nombres premiers trouvés: %d\n", result);
    printf("   Temps: %.6f secondes\n\n", time_red);
    
    // 3. PARALLÈLE avec SCHEDULE STATIC
    start = omp_get_wtime();
    result = count_primes_parallel_static(n, num_threads);
    end = omp_get_wtime();
    double time_static = end - start;
    printf("3. PARALLÈLE (schedule static):\n");
    printf("   Nombres premiers trouvés: %d\n", result);
    printf("   Temps: %.6f secondes\n\n", time_static);
    
    // 4. PARALLÈLE avec SCHEDULE DYNAMIC
    start = omp_get_wtime();
    result = count_primes_parallel_dynamic(n, num_threads);
    end = omp_get_wtime();
    double time_dyn = end - start;
    printf("4. PARALLÈLE (schedule dynamic, chunk=100):\n");
    printf("   Nombres premiers trouvés: %d\n", result);
    printf("   Temps: %.6f secondes\n\n", time_dyn);
    
    // Comparaison
    printf("COMPARAISON:\n");
    printf("   SÉQUENTIEL:        %.6f s (baseline)\n", time_seq);
    printf("   REDUCTION:         %.6f s\n", time_red);
    printf("   SCHEDULE STATIC:   %.6f s\n", time_static);
    printf("   SCHEDULE DYNAMIC:  %.6f s\n", time_dyn);
    
    // Meilleure méthode
    double best_time = time_red;
    const char* best_method = "REDUCTION";
    if (time_static < best_time) {
        best_time = time_static;
        best_method = "SCHEDULE STATIC";
    }
    if (time_dyn < best_time) {
        best_time = time_dyn;
        best_method = "SCHEDULE DYNAMIC";
    }
    printf("   Meilleure méthode: %s (%.6f s)\n", 
           best_method, best_time);
    
    printf("\n========================================\n\n");
}

// Démonstration visuelle: distribution du travail entre threads
void demo_distribution_travail() {
    printf("==== DÉMONSTRATION: Distribution du travail ====\n\n");
    
    int n = 30;
    printf("Recherche des nombres premiers jusqu'à %d avec 4 threads\n\n", n);
    
    printf("SCHEDULE STATIC (par défaut):\n");
    printf("  Divise les itérations en blocs égaux\n");
    printf("  Thread 0: vérifie [2-9]\n");
    printf("  Thread 1: vérifie [10-17]\n");
    printf("  Thread 2: vérifie [18-25]\n");
    printf("  Thread 3: vérifie [26-30]\n\n");
    
    #pragma omp parallel for schedule(static) num_threads(4)
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            #pragma omp critical
            {
                printf("  Thread %d a trouvé: %d\n", omp_get_thread_num(), i);
            }
        }
    }
    
    printf("\nSCHEDULE DYNAMIC (chunk=5):\n");
    printf("  Distribue les itérations dynamiquement par paquets de 5\n");
    printf("  Le thread qui finit prend le prochain paquet disponible\n\n");
    
    #pragma omp parallel for schedule(dynamic, 5) num_threads(4)
    for (int i = 2; i <= n; i++) {
        if (est_premier(i)) {
            #pragma omp critical
            {
                printf("  Thread %d a trouvé: %d\n", omp_get_thread_num(), i);
            }
        }
    }
    
    printf("\nNote: Avec DYNAMIC, l'équilibrage est meilleur quand le travail varie\n");
    printf("      (car tester si 29 est premier est plus rapide que tester 30)\n\n");
    printf("========================================\n\n");
}

int main() {
    printf("==== LAB 3: Nombres Premiers avec OpenMP ====\n");
    printf("Comparaison: SÉQUENTIEL vs PARALLÈLE\n");
    
    int num_threads = 4;
    omp_set_num_threads(num_threads);
    printf("Nombre de threads: %d\n\n", num_threads);
    
    // Démonstration visuelle
    demo_distribution_travail();
    
    // Tests de performance
    int sizes[] = {10, 100, 1000, 10000, 100000};
    int num_sizes = 5;
    
    for (int i = 0; i < num_sizes; i++) {
        test_performance(sizes[i], num_threads);
    }
    
    printf("\n==== EXPLICATION DES SCHEDULES ====\n\n");
    printf("1. SCHEDULE(STATIC):\n");
    printf("   - Divise les itérations en blocs égaux au démarrage\n");
    printf("   - Chaque thread reçoit un bloc fixe\n");
    printf("   - Overhead minimal, mais peut être déséquilibré\n");
    printf("   - Bon pour: travail uniforme entre itérations\n\n");
    
    printf("2. SCHEDULE(DYNAMIC, chunk):\n");
    printf("   - Distribue les itérations par paquets de 'chunk' itérations\n");
    printf("   - Thread qui finit prend le prochain paquet disponible\n");
    printf("   - Plus d'overhead, mais meilleur équilibrage\n");
    printf("   - Bon pour: travail variable entre itérations\n\n");
    
    printf("3. SCHEDULE(GUIDED):\n");
    printf("   - Comme dynamic, mais la taille des paquets diminue\n");
    printf("   - Commence avec gros paquets, finit avec petits\n");
    printf("   - Compromis entre static et dynamic\n\n");
    
    printf("POUR LES NOMBRES PREMIERS:\n");
    printf("- Les petits nombres sont rapides à tester\n");
    printf("- Les grands nombres prennent plus de temps (plus de divisions)\n");
    printf("- DYNAMIC peut être meilleur car il équilibre mieux la charge\n");
    
    return 0;
}
