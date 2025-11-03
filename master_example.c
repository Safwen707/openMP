#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void exemple_initialisation() {
    printf("=== EXEMPLE 1: INITIALISATION ===\n");
    
    int *donnees = NULL;
    int taille = 1000;
    
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        
        #pragma omp master
        {
            printf("Thread master (%d): J'alloue la mémoire pour tous\n", thread_id);
            donnees = malloc(taille * sizeof(int));
            
            // Initialisation que seul le master doit faire
            printf("Thread master (%d): J'initialise les données\n", thread_id);
            for (int i = 0; i < taille; i++) {
                donnees[i] = i * 2;
            }
        }
        
        // BARRIERE IMPLICITE : tous les threads attendent que le master finisse
        
        // Maintenant tous les threads peuvent utiliser 'donnees'
        printf("Thread %d: Je peux maintenant utiliser les données\n", thread_id);
        
        // Chaque thread traite sa partie
        int chunk = taille / 4;
        int debut = thread_id * chunk;
        int fin = (thread_id == 3) ? taille : debut + chunk;
        
        int somme_locale = 0;
        for (int i = debut; i < fin; i++) {
            somme_locale += donnees[i];
        }
        printf("Thread %d: ma somme locale = %d\n", thread_id, somme_locale);
    }
    
    free(donnees);
    printf("\n");
}




void exemple_fichier_log() {
    printf("=== EXEMPLE 2: ECRITURE DANS UN FICHIER LOG ===\n");
    
    FILE *fichier = NULL;
    
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        
        #pragma omp master
        {
            printf("Thread master (%d): J'ouvre le fichier de log\n", thread_id);
            fichier = fopen("calcul.log", "w");
            if (fichier) {
                fprintf(fichier, "=== DEBUT DU CALCUL PARALLELE ===\n");
                fprintf(fichier, "Nombre de threads: %d\n", omp_get_num_threads());
                fprintf(fichier, "Timestamp: %d\n", (int)time(NULL));
                fflush(fichier);
            }
        }
        
        // Tous les threads font leur travail
        printf("Thread %d: Je fais mon calcul...\n", thread_id);
        
        // Simulation de calcul
        int resultat = 0;
        for (int i = 0; i < 100000; i++) {
            resultat += i * thread_id;
        }
        
        printf("Thread %d: Mon résultat = %d\n", thread_id, resultat);
        
        // Seul le master écrit la conclusion
        #pragma omp master
        {
            if (fichier) {
                printf("Thread master (%d): J'écris la conclusion dans le log\n", thread_id);
                fprintf(fichier, "=== FIN DU CALCUL ===\n");
                fprintf(fichier, "Tous les threads ont terminé\n");
                fclose(fichier);
            }
        }
    }
    printf("\n");
}

void exemple_mesure_temps() {
    printf("=== EXEMPLE 3: MESURE DE TEMPS GLOBALE ===\n");
    
    double temps_debut, temps_fin;
    
    #pragma omp parallel num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        
        #pragma omp master
        {
            printf("Thread master (%d): Je démarre le chronomètre\n", thread_id);
            temps_debut = omp_get_wtime();
        }
        
        // Tous les threads font du travail
        printf("Thread %d: Je commence mon travail\n", thread_id);
        
        // Simulation de travail intensif
        volatile double calcul = 0;
        for (int i = 0; i < 1000000; i++) {
            calcul += i * 3.14159 * thread_id;
        }
        
        printf("Thread %d: J'ai terminé\n", thread_id);
        
        #pragma omp master
        {
            temps_fin = omp_get_wtime();
            printf("Thread master (%d): Temps total = %.6f secondes\n", 
                   thread_id, temps_fin - temps_debut);
        }
    }
    printf("\n");
}

int main() {
    printf("Exemples d'utilisation de #pragma omp master\n");
    printf("============================================\n\n");
    
    exemple_initialisation();
    exemple_affichage_progression();
    exemple_fichier_log();
    exemple_mesure_temps();
    
    printf("=== RESUME ===\n");
    printf("Le master est utilisé pour :\n");
    printf("• Allocation/libération de mémoire\n");
    printf("• Ouverture/fermeture de fichiers\n");
   
    printf("• Mesures de temps globales\n");
    printf("• Initialisation partagée\n");
    printf("• Logs et rapports\n");
    
    return 0;
}