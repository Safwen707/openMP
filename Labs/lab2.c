#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Fonction pour initialiser un tableau
void init_array(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = i + 1;  // Valeurs 1, 2, 3, ..., size
    }
}

// Méthode 1: Avec reduction (recommandée)
/*
 * PRINCIPE DE REDUCTION:
 * 
 * ┌─────────────────────────────────────────────┐
 * │         Variable originale: sum = 0         │
 * └─────────────────────────────────────────────┘
 *                     │
 *         ┌───────────┴───────────┐
 *         │  OpenMP crée copies   │
 *         └───────────┬───────────┘
 *                     │
 *     ┌───────┬───────┼───────┬───────┐
 *     │       │       │       │       │
 *     ▼       ▼       ▼       ▼       ▼
 * ┌──────┐┌──────┐┌──────┐┌──────┐
 * │ T0   ││ T1   ││ T2   ││ T3   │  Calculs
 * │sum=0 ││sum=0 ││sum=0 ││sum=0 │  parallèles
 * └──┬───┘└──┬───┘└──┬───┘└──┬───┘  indépendants
 *    │      │      │      │
 *    │+=i0  │+=i1  │+=i2  │+=i3    
 *    │+=... │+=... │+=... │+=...
 *    │      │      │      │
 *    ▼      ▼      ▼      ▼
 * ┌──────┐┌──────┐┌──────┐┌──────┐
 * │local0││local1││local2││local3│
 * └──┬───┘└──┬───┘└──┬───┘└──┬───┘
 *    │       │       │       │
 *    └───────┴───┬───┴───────┘
 *                │ Réduction (+)
 *                ▼
 *         ┌─────────────┐
 *         │  sum finale │
 *         └─────────────┘
 */
long long sum_with_reduction(int *arr, int size) {
    long long sum = 0;
    
    // Chaque thread aura une copie privée de sum initialisée à 0
    // À la fin, OpenMP combine automatiquement toutes les copies
    #pragma omp parallel for reduction(+:sum) num_threads(4)
    for (int i = 0; i < size; i++) {
        sum += arr[i];  // Chaque thread ajoute à SA copie locale
    }
    // Ici, sum contient la somme finale (combinaison automatique)
    
    return sum;
}

// Méthode 2: Avec atomic
/*
 * PRINCIPE ATOMIC:
 * - Tous les threads partagent la MÊME variable sum
 * - Chaque addition est une opération atomique (indivisible)
 * - Plus lent car contention sur la variable partagée
 * 
 * POURQUOI ATOMIC EST NÉCESSAIRE:
 * L'opération "sum += arr[i]" n'est PAS atomique en assembleur:

 * lab2.png
 
 * SOLUTION: #pragma omp atomic rend toute l'opération indivisible
 */
long long sum_with_atomic(int *arr, int size) {
    long long sum = 0;
    
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < size; i++) {
        #pragma omp atomic
        sum += arr[i];  // Opération atomique à chaque itération
    }
    
    return sum;
}

// Méthode 3: Avec critical
/*
 * PRINCIPE CRITICAL:
 * - Tous les threads partagent la MÊME variable sum
 * - Un SEUL thread peut entrer dans la section critique à la fois
 * - Les autres threads attendent → sérialisation complète
 * - Le plus lent des trois méthodes
 */
long long sum_with_critical(int *arr, int size) {
    long long sum = 0;
    
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < size; i++) {
        #pragma omp critical
        {
            sum += arr[i];  // Un seul thread à la fois ici
        }
    }
    
    return sum;
}

// Fonction de test pour une taille donnée
void test_size(int size) {
    printf("\n==== Taille du tableau: %d éléments ====\n", size);
    
    // Allocation et initialisation
    int *arr = (int*)malloc(size * sizeof(int));
    init_array(arr, size);
    
    // Calcul de la somme attendue: 1+2+...+n = n*(n+1)/2
    long long expected_sum = (long long)size * (size + 1) / 2;
    printf("Somme attendue: %lld\n\n", expected_sum);
    
    // Test 1: Reduction
    double start = omp_get_wtime();
    long long sum1 = sum_with_reduction(arr, size);
    double time1 = omp_get_wtime() - start;
    printf("1. REDUCTION:\n");
    printf("   Résultat: %lld %s\n", sum1, (sum1 == expected_sum) ? "✓" : "✗");
    printf("   Temps: %.6f secondes\n\n", time1);
    
    // Test 2: Atomic
    start = omp_get_wtime();
    long long sum2 = sum_with_atomic(arr, size);
    double time2 = omp_get_wtime() - start;
    printf("2. ATOMIC:\n");
    printf("   Résultat: %lld %s\n", sum2, (sum2 == expected_sum) ? "✓" : "✗");
    printf("   Temps: %.6f secondes\n", time2);
    printf("   Ratio vs reduction: %.2fx plus lent\n\n", time2 / time1);
    
    // Test 3: Critical
    start = omp_get_wtime();
    long long sum3 = sum_with_critical(arr, size);
    double time3 = omp_get_wtime() - start;
    printf("3. CRITICAL:\n");
    printf("   Résultat: %lld %s\n", sum3, (sum3 == expected_sum) ? "✓" : "✗");
    printf("   Temps: %.6f secondes\n", time3);
    printf("   Ratio vs reduction: %.2fx plus lent\n\n", time3 / time1);
    
    // Comparaison
    printf("COMPARAISON:\n");
    printf("   REDUCTION est le plus rapide (baseline)\n");
    printf("   ATOMIC est %.2fx plus lent\n", time2 / time1);
    printf("   CRITICAL est %.2fx plus lent\n", time3 / time1);
    
    free(arr);
}

// Démonstration visuelle du principe de reduction
void demo_reduction_visuelle() {
    printf("\n==== DÉMONSTRATION VISUELLE DE REDUCTION ====\n\n");
    
    int arr[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    long long sum = 0;
    
    printf("Tableau: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]\n");
    printf("Somme attendue: 136\n\n");
    
    printf("ÉTAPE 1: Création des copies privées\n");
    printf("  sum initial = 0\n");
    printf("  OpenMP crée une copie par thread (4 threads)\n\n");
    
    printf("ÉTAPE 2: Calcul parallèle\n");
    
    #pragma omp parallel num_threads(4) reduction(+:sum)
    {
        int id = omp_get_thread_num();
        int chunk_size = 16 / 4;
        int start = id * chunk_size;
        int end = start + chunk_size;
        
        long long local_sum = 0;
        
        // Affichage de ce que fait chaque thread
        #pragma omp critical
        {
            printf("  Thread %d traite indices [%d-%d]: ", id, start, end-1);
            for (int i = start; i < end; i++) {
                printf("%d ", arr[i]);
                local_sum += arr[i];
            }
            printf("→ somme locale = %lld\n", local_sum);
        }
        
        // Calcul réel pour reduction
        for (int i = start; i < end; i++) {
            sum += arr[i];
        }
    }
    
    printf("\nÉTAPE 3: Réduction finale\n");
    printf("  OpenMP combine automatiquement toutes les sommes locales\n");
    printf("  Résultat final: sum = %lld\n\n", sum);
}

// Démonstration du principe atomic
void demo_atomic_visuelle() {
    printf("\n==== DÉMONSTRATION VISUELLE DE ATOMIC ====\n\n");
    
    int arr[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    long long sum = 0;
    
    printf("Tableau: [10, 20, 30, 40, 50, 60, 70, 80]\n");
    printf("Somme attendue: 360\n\n");
    
    printf("PRINCIPE: Tous les threads accèdent à la MÊME variable sum\n");
    printf("Chaque opération += est atomique (indivisible)\n\n");
    
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        int chunk_size = 8 / 4;
        int start = id * chunk_size;
        int end = start + chunk_size;
        
        for (int i = start; i < end; i++) {
            // Affichage avant atomic
            #pragma omp critical
            {
                printf("  Thread %d: sum=%lld, va ajouter arr[%d]=%d", 
                       id, sum, i, arr[i]);
            }
            
            // Opération atomique
            #pragma omp atomic
            sum += arr[i];
            
            // Affichage après atomic
            #pragma omp critical
            {
                printf(" → sum=%lld\n", sum);
            }
        }
    }
    
    printf("\nRésultat final: sum = %lld\n", sum);
    printf("Note: L'ordre d'affichage peut varier car les threads travaillent en parallèle\n");
}

// Démonstration du principe critical
void demo_critical_visuelle() {
    printf("\n==== DÉMONSTRATION VISUELLE DE CRITICAL ====\n\n");
    
    int arr[8] = {5, 10, 15, 20, 25, 30, 35, 40};
    long long sum = 0;
    
    printf("Tableau: [5, 10, 15, 20, 25, 30, 35, 40]\n");
    printf("Somme attendue: 180\n\n");
    
    printf("PRINCIPE: UN SEUL thread peut entrer dans la section critique à la fois\n");
    printf("Les autres threads ATTENDENT → sérialisation\n\n");
    
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        int chunk_size = 8 / 4;
        int start = id * chunk_size;
        int end = start + chunk_size;
        
        for (int i = start; i < end; i++) {
            #pragma omp critical
            {
                printf("  Thread %d entre dans critical: sum=%lld, ajoute arr[%d]=%d", 
                       id, sum, i, arr[i]);
                sum += arr[i];
                printf(" → sum=%lld\n", sum);
                // Simulation d'un petit délai pour mieux voir la sérialisation
                for (volatile int k = 0; k < 100000; k++);
            }
        }
    }
    
    printf("\nRésultat final: sum = %lld\n", sum);
    printf("Note: Un seul thread à la fois dans la section critique → le plus lent!\n");
}

int main() {
    printf("==== LAB 2: Somme de tableau avec OpenMP ====\n");
    printf("Comparaison: REDUCTION vs ATOMIC vs CRITICAL\n");
    printf("Nombre de threads: 4\n");
    
    // Démonstrations visuelles des trois principes
    demo_reduction_visuelle();
    demo_atomic_visuelle();
    demo_critical_visuelle();
    
    printf("\n");
    printf("========================================\n");
    printf("MAINTENANT: Tests de performance\n");
    printf("========================================\n");
    
    // Test avec différentes tailles
    test_size(1000);        // Petit tableau
    test_size(100000);      // Moyen tableau
    test_size(10000000);    // Grand tableau
    
    printf("\n==== CONCLUSION ====\n");
    printf("✅ REDUCTION: Le plus rapide et le plus simple\n");
    printf("   - OpenMP optimise automatiquement\n");
    printf("   - Pas de contention entre threads\n");
    printf("   - À UTILISER en priorité\n\n");
    
    printf("⚠️  ATOMIC: Plus lent que reduction\n");
    printf("   - Chaque addition nécessite une opération atomique\n");
    printf("   - Contention sur la variable partagée\n");
    printf("   - Utiliser seulement si reduction impossible\n\n");
    
    printf("❌ CRITICAL: Le plus lent\n");
    printf("   - Sérialise complètement les accès\n");
    printf("   - Un seul thread à la fois dans la section\n");
    printf("   - À ÉVITER pour ce type de calcul\n");
    
    return 0;
}
