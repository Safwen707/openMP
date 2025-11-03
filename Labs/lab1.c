#include <omp.h>
#include <stdio.h>

int main() {
    omp_set_num_threads(4);
    printf("==== OpenMP HelloWorld: Comparaison des clauses ====""\n\n");

    // 1. Cas de base : sans clause
    printf("[1] Sans clause (base):\n");
    int a = 10;
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Thread %d: a = %d\n", id, a);
    }
    printf("\n");

    // 2. Avec private
    printf("[2] Avec private:\n");
    a = 20;
    #pragma omp parallel private(a)

    {
        int id = omp_get_thread_num();
        printf("Thread %d: a (garbage) = %d\n", id, a);
        
        a = id * 100; // chaque thread a sa propre copie
        printf("Thread %d: a (private) = %d\n", id, a);
    }
    printf("Après la région: a = %d\n\n", a);

    // 3. Avec firstprivate
    printf("[3] Avec firstprivate:\n");
    a = 30;
    #pragma omp parallel firstprivate(a)
    {
        int id = omp_get_thread_num();
        printf("Thread %d: a (firstprivate, avant) = %d\n", id, a);
        a += id;
        printf("Thread %d: a (firstprivate, après) = %d\n", id, a);
    }
    printf("Après la région: a = %d\n\n", a);


    // 4. Exemple avec lastprivate
    printf("[4] Exemple avec lastprivate :\n");
    int b = 0;
    #pragma omp parallel for lastprivate(b) num_threads(4)
    for (int i = 0; i < 8; i++) {
        b = 100 + i;
        printf("Thread %d, i=%d : b = %d\n", omp_get_thread_num(), i, b);
    }
    printf("Après la boucle : b = %d (valeur de la dernière itération i=7)\n\n", b);


    // 5. Avec shared: race condition pb
    printf("[5] Avec shared:\n");
    a = 40;
    #pragma omp parallel shared(a)
    {
        int id = omp_get_thread_num();
        a += id;
        printf("Thread %d: a (shared) = %d\n", id, a);
    }
    printf("Après la région: a = %d\n\n", a);

    

printf("Après la région: a = %d\n\n", a);


    // 6. Variable modifiée hors/sous la région parallèle
    printf("[6] Variable modifiée hors/sous la région parallèle:\n");
    a = 50;
    printf("Avant la région: a = %d\n", a);
    #pragma omp parallel private(a)
    {
        int id = omp_get_thread_num();
        a = 100 + id;
        printf("Thread %d: a (private) = %d\n", id, a);
    }
    printf("Après la région: a = %d\n\n", a);

    // 7. Variable déclarée dans la région parallèle
    printf("[7] Variable déclarée dans la région parallèle:\n");
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int b = 200 + id;
        printf("Thread %d: b (locale à la région) = %d\n", id, b);
    }
    printf("\n");



  
    // Résumé/conclusion
    printf("==== CONCLUSION ====\n");
printf("- private : copie locale par thread, valeur INITIALE NON DÉFINIE\n");
printf("- firstprivate : copie locale par thread, INITIALISÉE avec la valeur d'origine\n");  
printf("- lastprivate : copie locale par thread, et la variable ORIGINALE prend la valeur de la DERNIÈRE itération\n");
printf("- shared : tous les threads utilisent la MÊME variable (risque de race condition)\n");
printf("- reduction : chaque thread a sa copie, combinée à la fin avec une opération (+, *, max, etc.)\n");
printf("\nÀ retenir : private/firstprivate = sécurité, shared = performance (mais danger !)\n");
    return 0;
}
