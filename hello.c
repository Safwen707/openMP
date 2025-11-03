#include <omp.h>
#include <stdio.h>

int main() {
    omp_set_num_threads(4);  // On veut 4 threads
    
    #pragma omp parallel  // Ici commence la zone parallèle
    {
        int id = omp_get_thread_num();  // Chaque thread a son numéro
        printf("Hello from thread %d\n", id);
    }  // Fin de la zone parallèle
    
    return 0;
}