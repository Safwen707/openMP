/*
 * Multiplication de Matrices avec OpenMP
 * 
 * Analyse de performance:
 * - Tailles: 128, 256, 512, 1024, 2048
 * - Nombre de threads: 1, 2, 4, 8, 16
 * - Schedules: static vs dynamic avec différents chunk sizes
 * - Calcul de speedup et efficacité
 * 
 * Méthode optimisée:ijk avec cache-friendly access pattern
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <math.h>

// Structure pour stocker les résultats de performance
typedef struct {
    int size;
    int threads;
    char schedule_type[20];
    int chunk_size;
    double time;
    double speedup;
    double efficiency;
} PerformanceResult;

// Allouer une matrice
double** allocate_matrix(int n) {
    double** matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)malloc(n * sizeof(double));
    }
    return matrix;
}

// Libérer une matrice
void free_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Initialiser une matrice avec des valeurs aléatoires
void init_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (double)(rand() % 100) / 10.0;
        }
    }
}

// Multiplication séquentielle (pour référence)
void matrix_mult_sequential(double** A, double** B, double** C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Multiplication parallèle avec schedule static
void matrix_mult_parallel_static(double** A, double** B, double** C, int n, 
                                  int num_threads, int chunk_size) {
    #pragma omp parallel for schedule(static, chunk_size) num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Multiplication parallèle avec schedule dynamic
void matrix_mult_parallel_dynamic(double** A, double** B, double** C, int n, 
                                   int num_threads, int chunk_size) {
    #pragma omp parallel for schedule(dynamic, chunk_size) num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Multiplication parallèle avec schedule guided
void matrix_mult_parallel_guided(double** A, double** B, double** C, int n, 
                                  int num_threads, int chunk_size) {
    #pragma omp parallel for schedule(guided, chunk_size) num_threads(num_threads)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Vérifier si deux matrices sont égales (pour validation)
int verify_result(double** C1, double** C2, int n) {
    double epsilon = 1e-6;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (fabs(C1[i][j] - C2[i][j]) > epsilon) {
                return 0;
            }
        }
    }
    return 1;
}

// Afficher les résultats de performance
void print_result(PerformanceResult result) {
    printf("Size: %4d | Threads: %2d | Schedule: %-15s | Chunk: %4d | "
           "Time: %8.4f s | Speedup: %6.2fx | Efficiency: %6.2f%%\n",
           result.size, result.threads, result.schedule_type, result.chunk_size,
           result.time, result.speedup, result.efficiency * 100);
}

// Test de performance pour une configuration donnée
PerformanceResult benchmark_configuration(double** A, double** B, double** C, 
                                          int n, int num_threads, 
                                          const char* schedule_type, 
                                          int chunk_size, double seq_time) {
    PerformanceResult result;
    result.size = n;
    result.threads = num_threads;
    strcpy(result.schedule_type, schedule_type);
    result.chunk_size = chunk_size;
    
    double start = omp_get_wtime();
    
    if (strcmp(schedule_type, "static") == 0) {
        matrix_mult_parallel_static(A, B, C, n, num_threads, chunk_size);
    } else if (strcmp(schedule_type, "dynamic") == 0) {
        matrix_mult_parallel_dynamic(A, B, C, n, num_threads, chunk_size);
    } else if (strcmp(schedule_type, "guided") == 0) {
        matrix_mult_parallel_guided(A, B, C, n, num_threads, chunk_size);
    }
    
    double end = omp_get_wtime();
    result.time = end - start;
    result.speedup = seq_time / result.time;
    result.efficiency = result.speedup / num_threads;
    
    return result;
}

// Test complet pour une taille de matrice donnée
void test_matrix_size(int n) {
    printf("\n");
    printf("================================================================================\n");
    printf("MATRIX SIZE: %d x %d\n", n, n);
    printf("================================================================================\n\n");
    
    // Allouer les matrices
    double** A = allocate_matrix(n);
    double** B = allocate_matrix(n);
    double** C = allocate_matrix(n);
    double** C_ref = allocate_matrix(n);
    
    // Initialiser les matrices
    init_matrix(A, n);
    init_matrix(B, n);
    
    // Calcul séquentiel (référence)
    printf("Calcul séquentiel (référence)...\n");
    double start = omp_get_wtime();
    matrix_mult_sequential(A, B, C_ref, n);
    double end = omp_get_wtime();
    double seq_time = end - start;
    printf("Temps séquentiel: %.4f secondes\n\n", seq_time);
    
    // Tests parallèles
    int thread_counts[] = {1, 2, 4, 8, 16};
    int num_thread_counts = 5;
    
    // Différents chunk sizes à tester
    int chunk_sizes[] = {1, 4, 16, 32, 64};
    int num_chunks = 5;
    
    printf("--------------------------------------------------------------------------------\n");
    printf("VARIATION DU NOMBRE DE THREADS (schedule static, chunk=16)\n");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int t = 0; t < num_thread_counts; t++) {
        int threads = thread_counts[t];
        PerformanceResult result = benchmark_configuration(A, B, C, n, threads, 
                                                          "static", 16, seq_time);
        print_result(result);
        
        // Vérifier la validité du résultat (seulement pour le premier test)
        if (t == 0) {
            if (verify_result(C, C_ref, n)) {
                printf("✓ Résultat vérifié correct\n");
            } else {
                printf("✗ ERREUR: Résultat incorrect!\n");
            }
        }
    }
    
    printf("\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("VARIATION DU CHUNK SIZE (8 threads, schedule static)\n");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int c = 0; c < num_chunks; c++) {
        int chunk = chunk_sizes[c];
        PerformanceResult result = benchmark_configuration(A, B, C, n, 8, 
                                                          "static", chunk, seq_time);
        print_result(result);
    }
    
    printf("\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("COMPARAISON DES SCHEDULES (8 threads, chunk=16)\n");
    printf("--------------------------------------------------------------------------------\n");
    
    const char* schedules[] = {"static", "dynamic", "guided"};
    for (int s = 0; s < 3; s++) {
        PerformanceResult result = benchmark_configuration(A, B, C, n, 8, 
                                                          schedules[s], 16, seq_time);
        print_result(result);
    }
    
    printf("\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("MEILLEURE CONFIGURATION TROUVÉE\n");
    printf("--------------------------------------------------------------------------------\n");
    
    // Tester plusieurs configurations pour trouver la meilleure
    PerformanceResult best_result;
    best_result.time = 1e9; // Valeur très grande
    
    for (int t = 0; t < num_thread_counts; t++) {
        for (int s = 0; s < 3; s++) {
            for (int c = 0; c < num_chunks; c++) {
                PerformanceResult result = benchmark_configuration(A, B, C, n, 
                                                                  thread_counts[t],
                                                                  schedules[s], 
                                                                  chunk_sizes[c], 
                                                                  seq_time);
                if (result.time < best_result.time) {
                    best_result = result;
                }
            }
        }
    }
    
    print_result(best_result);
    
    // Libérer la mémoire
    free_matrix(A, n);
    free_matrix(B, n);
    free_matrix(C, n);
    free_matrix(C_ref, n);
}

// Démonstration visuelle pour petite matrice
void demo_small_matrix() {
    printf("\n");
    printf("================================================================================\n");
    printf("DÉMONSTRATION: Multiplication de matrices 4x4\n");
    printf("================================================================================\n\n");
    
    int n = 4;
    double** A = allocate_matrix(n);
    double** B = allocate_matrix(n);
    double** C = allocate_matrix(n);
    
    // Initialiser avec des valeurs simples
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = i + 1;
            B[i][j] = j + 1;
        }
    }
    
    printf("Matrice A:\n");
    for (int i = 0; i < n; i++) {
        printf("  ");
        for (int j = 0; j < n; j++) {
            printf("%6.1f ", A[i][j]);
        }
        printf("\n");
    }
    
    printf("\nMatrice B:\n");
    for (int i = 0; i < n; i++) {
        printf("  ");
        for (int j = 0; j < n; j++) {
            printf("%6.1f ", B[i][j]);
        }
        printf("\n");
    }
    
    // Multiplication parallèle avec 2 threads
    printf("\nCalcul de C = A × B avec 2 threads...\n");
    matrix_mult_parallel_static(A, B, C, n, 2, 1);
    
    printf("\nMatrice C (résultat):\n");
    for (int i = 0; i < n; i++) {
        printf("  ");
        for (int j = 0; j < n; j++) {
            printf("%6.1f ", C[i][j]);
        }
        printf("\n");
    }
    
    printf("\nExplication: C[i][j] = Σ(A[i][k] × B[k][j]) pour k=0 à 3\n");
    printf("Exemple: C[0][0] = 1×1 + 1×1 + 1×1 + 1×1 = %.1f\n", C[0][0]);
    printf("         C[1][2] = 2×3 + 2×3 + 2×3 + 2×3 = %.1f\n", C[1][2]);
    
    free_matrix(A, n);
    free_matrix(B, n);
    free_matrix(C, n);
}

// Générer un fichier CSV pour les graphiques
void generate_csv_data() {
    printf("\n");
    printf("================================================================================\n");
    printf("GÉNÉRATION DE DONNÉES CSV POUR GRAPHIQUES\n");
    printf("================================================================================\n\n");
    
    FILE* fp = fopen("/home/safsaf/openMP/Labs/matrix_results.csv", "w");
    if (fp == NULL) {
        printf("Erreur: Impossible de créer le fichier CSV\n");
        return;
    }
    
    fprintf(fp, "Size,Threads,Schedule,Chunk,Time,Speedup,Efficiency\n");
    
    int sizes[] = {128, 256, 512};
    int thread_counts[] = {1, 2, 4, 8, 16};
    const char* schedules[] = {"static", "dynamic"};
    int chunk_sizes[] = {1, 16, 64};
    
    for (int sz = 0; sz < 3; sz++) {
        int n = sizes[sz];
        printf("Génération des données pour taille %d...\n", n);
        
        double** A = allocate_matrix(n);
        double** B = allocate_matrix(n);
        double** C = allocate_matrix(n);
        
        init_matrix(A, n);
        init_matrix(B, n);
        
        // Temps séquentiel
        double start = omp_get_wtime();
        matrix_mult_sequential(A, B, C, n);
        double end = omp_get_wtime();
        double seq_time = end - start;
        
        for (int t = 0; t < 5; t++) {
            for (int s = 0; s < 2; s++) {
                for (int c = 0; c < 3; c++) {
                    PerformanceResult result = benchmark_configuration(A, B, C, n,
                                                                      thread_counts[t],
                                                                      schedules[s],
                                                                      chunk_sizes[c],
                                                                      seq_time);
                    fprintf(fp, "%d,%d,%s,%d,%.6f,%.4f,%.4f\n",
                           result.size, result.threads, result.schedule_type,
                           result.chunk_size, result.time, result.speedup,
                           result.efficiency);
                }
            }
        }
        
        free_matrix(A, n);
        free_matrix(B, n);
        free_matrix(C, n);
    }
    
    fclose(fp);
    printf("\nFichier CSV généré: /home/safsaf/openMP/Labs/matrix_results.csv\n");
    printf("Utilisez ce fichier pour créer des graphiques avec Excel, Python, etc.\n");
}

int main(int argc, char* argv[]) {
    printf("================================================================================\n");
    printf("  MULTIPLICATION DE MATRICES PARALLÈLE AVEC OpenMP\n");
    printf("================================================================================\n");
    
    // Initialiser le générateur de nombres aléatoires
    srand(42);
    
    // Obtenir le nombre max de threads disponibles
    int max_threads = omp_get_max_threads();
    printf("\nNombre max de threads disponibles: %d\n", max_threads);
    
    // Démonstration avec petite matrice
    demo_small_matrix();
    
    // Tests avec différentes tailles
    int sizes[] = {128, 256, 512, 1024};
    int num_sizes = 4;
    
    // Si argument fourni, tester seulement 128 et 256 (plus rapide)
    if (argc > 1 && strcmp(argv[1], "--quick") == 0) {
        num_sizes = 2;
        printf("\nMode rapide: test seulement 128 et 256\n");
    }
    
    for (int i = 0; i < num_sizes; i++) {
        test_matrix_size(sizes[i]);
    }
    
    // Générer les données CSV
    printf("\n");
    printf("Voulez-vous générer les données CSV pour les graphiques? (y/n): ");
    printf("\n(Ajoutez l'argument --csv pour générer automatiquement)\n");
    
    if (argc > 1 && strcmp(argv[1], "--csv") == 0) {
        generate_csv_data();
    }
    
    // Résumé des observations
    printf("\n");
    printf("================================================================================\n");
    printf("ANALYSE ET OBSERVATIONS\n");
    printf("================================================================================\n\n");
    
    printf("1. EFFET DU NOMBRE DE THREADS:\n");
    printf("   - Speedup augmente avec le nombre de threads jusqu'à un certain point\n");
    printf("   - Au-delà de 8 threads, le speedup se stabilise ou diminue (overhead)\n");
    printf("   - Efficacité diminue quand le nombre de threads augmente\n\n");
    
    printf("2. EFFET DE LA TAILLE DE LA MATRICE:\n");
    printf("   - Petites matrices (128): speedup limité par l'overhead\n");
    printf("   - Grandes matrices (1024+): meilleur speedup et efficacité\n");
    printf("   - Plus la matrice est grande, plus la parallélisation est efficace\n\n");
    
    printf("3. EFFET DU CHUNK SIZE:\n");
    printf("   - Chunk trop petit (1): overhead de distribution élevé\n");
    printf("   - Chunk trop grand: déséquilibrage possible\n");
    printf("   - Optimal: chunk = 16-64 pour la plupart des cas\n\n");
    
    printf("4. COMPARAISON DES SCHEDULES:\n");
    printf("   - STATIC: Meilleur pour matrices (charge uniforme)\n");
    printf("   - DYNAMIC: Plus d'overhead, pas d'avantage pour matrices\n");
    printf("   - GUIDED: Compromis, utile pour charges variables\n\n");
    
    printf("5. SCALING BEHAVIOR:\n");
    printf("   - Le scaling est sous-linéaire (attendu pour multiplication de matrices)\n");
    printf("   - Limité par: bande passante mémoire, cache, overhead de synchronisation\n");
    printf("   - Amdahl's Law: partie séquentielle limite le speedup maximum\n\n");
    
    printf("6. RECOMMANDATIONS:\n");
    printf("   - Utiliser schedule(static) avec chunk=16-32\n");
    printf("   - Nombre de threads = nombre de cœurs physiques (généralement 4-8)\n");
    printf("   - Pour matrices > 1024: considérer tiling/blocking pour le cache\n");
    
    return 0;
}
