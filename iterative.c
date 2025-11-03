#include <omp.h>
#include <stdio.h>
#include <time.h>

void version_sequentielle() {
    printf("=== VERSION SEQUENTIELLE ===\n");
    int a[100], b[100], c[100];
    
    // Initialisation
    for (int i = 0; i < 100; i++) {
        a[i] = i;
        b[i] = i * 2;
    }
    
    // Mesure du temps
    clock_t start = clock();
    
    // Calcul séquentiel
    for (int i = 0; i < 100; i++) {
        c[i] = a[i] + b[i];
    }
    
    clock_t end = clock();
    double temps = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Calcul séquentiel terminé\n");
    printf("Temps séquentiel: %.6f secondes\n\n", temps);
}

void version_parallele() {
    printf("=== VERSION PARALLELE (4 threads) ===\n");
    int a[100], b[100], c[100];
    
    // Initialisation
    for (int i = 0; i < 100; i++) {
        a[i] = i;
        b[i] = i * 2;
    }
    
    // Configuration OpenMP - FIXE à 4 threads
    omp_set_num_threads(4);
    
    // Mesure du temps
    double start = omp_get_wtime();
    
    // Calcul parallèle avec affichage par plages
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        int num_threads = 4; // Fixé à 4 threads
        int chunk_size = 25; // 100/4 = 25 éléments par thread
        int start_idx = thread_id * chunk_size;
        int end_idx = (thread_id == 3) ? 100 : start_idx + chunk_size; // Thread 3 va jusqu'à 100
        
        // Affichage de la plage assignée au thread
        printf("Thread %d: c[%d]=a[%d]+b[%d], c[%d]=a[%d]+b[%d], ..., c[%d]=a[%d]+b[%d]\n",
               thread_id, 
               start_idx, start_idx, start_idx,
               start_idx+1, start_idx+1, start_idx+1,
               end_idx-1, end_idx-1, end_idx-1);
        
        // Calcul effectif
        for (int i = start_idx; i < end_idx; i++) {
            c[i] = a[i] + b[i];
        }
    }
    
    double end = omp_get_wtime();
    double temps = end - start;
    printf("Temps parallèle: %.6f secondes\n\n", temps);
}

int main() {
    printf("Comparaison version séquentielle vs parallèle\n");
    printf("Addition de vecteurs: c[i] = a[i] + b[i] pour i=0..99\n\n");
    
    version_sequentielle();
    version_parallele();
    
    return 0;
}