#include <omp.h>
#include <stdio.h>

void exemple_base() {
    printf("=== COMPORTEMENT DE BASE (sans clauses) ===\n");
    printf("Sans firstprivate/lastprivate = ERREUR!\n\n");
    
    int x = 100;
    printf("x initial = %d\n\n", x);

    // ATTENTION: x est partagé = race condition!
    printf("Avec parallel for simple (DANGEREUX):\n");
    #pragma omp parallel for num_threads(2)
    for (int i = 0; i < 4; i++) {
        printf("Thread %d, i=%d: x=%d (partagé, dangereux!)\n", 
               omp_get_thread_num(), i, x);
        // On ne modifie pas x pour éviter les erreurs
    }
    
    printf("x final = %d (inchangé)\n\n", x);
}

void exemple_firstprivate_simple() {
    printf("=== FIRSTPRIVATE ===\n");
    printf("Chaque thread reçoit UNE copie privée de x au début\n\n");
    
    int x = 100;
    printf("x initial = %d\n\n", x);

    #pragma omp parallel for firstprivate(x) num_threads(2)
    for (int i = 0; i < 4; i++) {
        printf("Thread %d, i=%d: x au début = %d\n", 
               omp_get_thread_num(), i, x);
        x = x + i;  // Modification de la copie privée
        printf("Thread %d, i=%d: x après calcul = %d\n", 
               omp_get_thread_num(), i, x);
    }

    printf("x final = %d (INCHANGÉ car firstprivate)\n\n", x);
    
    printf("EXPLICATION:\n");
    printf("- Thread 0: commence avec x=100, garde sa copie modifiée\n");
    printf("- Thread 1: commence avec x=100, garde sa copie modifiée\n");
    printf("- Variable originale x reste à 100\n\n");
}

void exemple_lastprivate_simple() {
    printf("=== LASTPRIVATE ===\n");
    printf("La valeur de la DERNIÈRE itération est copiée dans x\n\n");
    
    int x = 100;
    printf("x initial = %d\n\n", x);

    #pragma omp parallel for lastprivate(x) num_threads(2)
    for (int i = 0; i < 4; i++) {
        printf("Thread %d, i=%d: x au début = %d (NON DÉFINI!)\n", 
               omp_get_thread_num(), i, x);
        x = 1000 + i;  // Assigner une nouvelle valeur
        printf("Thread %d, i=%d: x assigné = %d\n", 
               omp_get_thread_num(), i, x);
    }

    printf("x final = %d (valeur de la dernière itération i=3)\n\n", x);
    
    printf("EXPLICATION:\n");
    printf("- Valeurs initiales de x: NON DÉFINIES\n");
    printf("- Dernière itération (i=3): x = 1000+3 = 1003\n");
    printf("- Cette valeur est copiée dans la variable originale\n\n");
}

void exemple_combinaison() {
    printf("=== FIRSTPRIVATE + LASTPRIVATE ===\n");
    printf("Début avec copie + récupération de la dernière valeur\n\n");
    
    int x = 100;
    printf("x initial = %d\n\n", x);

    #pragma omp parallel for firstprivate(x) lastprivate(x) num_threads(2)
    for (int i = 0; i < 4; i++) {
        printf("Thread %d, i=%d: x au début = %d\n", 
               omp_get_thread_num(), i, x);
        x = x + i;  // Chaque thread part de 100, ajoute i
        printf("Thread %d, i=%d: x calculé = %d\n", 
               omp_get_thread_num(), i, x);
    }

    printf("x final = %d\n\n", x);
    
    printf("EXPLICATION:\n");
    printf("- Chaque thread commence avec x=100 (firstprivate)\n");
    printf("- Thread 0: i=0→x=100+0=100, i=1→x=100+1=101\n");
    printf("- Thread 1: i=2→x=100+2=102, i=3→x=100+3=103\n");
    printf("- Dernière itération (i=3) donne x=103 (lastprivate)\n\n");
}

void exemple_detaille() {
    printf("=== EXEMPLE DÉTAILLÉ STEP BY STEP ===\n");
    
    int x = 50;
    printf("x initial = %d\n", x);
    printf("4 itérations avec 2 threads:\n");
    printf("Thread 0: i=0, i=1\n");
    printf("Thread 1: i=2, i=3\n\n");

    #pragma omp parallel for firstprivate(x) lastprivate(x) num_threads(2) schedule(static)
    for (int i = 0; i < 4; i++) {
        int thread_id = omp_get_thread_num();
        printf("Thread %d, i=%d: x=%d → ", thread_id, i, x);
        x = x + (i * 10);
        printf("x=%d\n", x);
    }

    printf("\nx final = %d\n\n", x);
    
    printf("CALCULS:\n");
    printf("Thread 0: i=0: x=50+0=50,  i=1: x=50+10=60\n");
    printf("Thread 1: i=2: x=50+20=70, i=3: x=50+30=80\n");
    printf("Dernière itération (i=3): x=80 → variable originale\n\n");
}

int main() {
    printf("CLAUSES FIRSTPRIVATE ET LASTPRIVATE - VERSION CLAIRE\n");
    printf("====================================================\n\n");
    
    exemple_base();
    exemple_firstprivate_simple();
    exemple_lastprivate_simple();
    exemple_combinaison();
    exemple_detaille();
    
    printf("=== RÉSUMÉ FINAL ===\n");
    printf("firstprivate(x): Chaque thread reçoit une COPIE de x au début\n");
    printf("lastprivate(x):  Variable x prend la valeur de la DERNIÈRE itération\n");
    printf("Combinées:       Copie au début + récupération à la fin\n");
    printf("IMPORTANT:       Les threads gardent leur copie entre leurs itérations!\n");
    
    return 0;
}