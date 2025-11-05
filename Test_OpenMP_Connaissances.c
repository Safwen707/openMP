#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * ╔═══════════════════════════════════════════════════════════════════════╗
 * ║              TEST DE CONNAISSANCES - OpenMP                           ║
 * ║                    Basé sur le cours AAPP                             ║
 * ╚═══════════════════════════════════════════════════════════════════════╝
 *
 * INSTRUCTIONS:
 * 1. Lis chaque question attentivement
 * 2. Prédis le résultat AVANT d'exécuter
 * 3. Compare avec le résultat réel
 * 4. Note tes réponses (bonnes/mauvaises)
 * 5. Vérifie les explications à la fin
 */

// ============================================================================
// SECTION 1: MODÈLE FORK-JOIN (10 points)
// ============================================================================

void test_section1() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 1: MODÈLE FORK-JOIN ET THREADS                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 1.1 (2 points)
    printf("QUESTION 1.1: Combien de threads seront créés?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("int main() {\n");
    printf("    omp_set_num_threads(6);\n");
    printf("    #pragma omp parallel\n");
    printf("    { /* code */ }\n");
    printf("}\n\n");
    printf("Réponse: ____ threads (incluant le master)\n");
    printf("APPUIE SUR ENTRÉE pour voir la réponse...");
    getchar();
    
    int count1 = 0;
    omp_set_num_threads(6);
    #pragma omp parallel
    {
        #pragma omp atomic
        count1++;
    }
    printf("✓ Réponse correcte: %d threads\n\n", count1);
    
    // QUESTION 1.2 (3 points)
    printf("QUESTION 1.2: Quel est le numéro du thread master?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("a) 1\n");
    printf("b) 0\n");
    printf("c) Le dernier thread\n");
    printf("d) Ça dépend\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse correcte: b) 0\n");
    printf("  Le master thread est TOUJOURS le thread 0\n\n");
    
    // QUESTION 1.3 (5 points)
    printf("QUESTION 1.3: Ordonnancement - Prédis l'ordre d'affichage\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  #pragma omp parallel num_threads(3)\n");
    printf("  {\n");
    printf("      int id = omp_get_thread_num();\n");
    printf("      printf(\"Thread %%d\\n\", id);\n");
    printf("  }\n\n");
    printf("L'ordre sera-t-il toujours: 0, 1, 2 ?\n");
    printf("a) Oui, toujours dans l'ordre\n");
    printf("b) Non, l'ordre est non-déterministe\n");
    printf("c) Oui, si on utilise schedule(static)\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("Démonstration (5 exécutions):\n");
    for (int run = 0; run < 5; run++) {
        printf("  Exécution %d: ", run + 1);
        #pragma omp parallel num_threads(3)
        {
            int id = omp_get_thread_num();
            #pragma omp critical
            printf("%d ", id);
        }
        printf("\n");
    }
    printf("\n✓ Réponse: b) L'ordre est NON-DÉTERMINISTE\n");
    printf("  Il n'y a AUCUNE garantie sur l'ordre d'exécution!\n\n");
}

// ============================================================================
// SECTION 2: CLAUSES DE PORTÉE (20 points)
// ============================================================================

void test_section2() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 2: CLAUSES PRIVATE/SHARED/FIRSTPRIVATE                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 2.1 (5 points)
    printf("QUESTION 2.1: Quelle sera la valeur de x après la région?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  int x = 10;\n");
    printf("  #pragma omp parallel private(x) num_threads(4)\n");
    printf("  {\n");
    printf("      x = omp_get_thread_num() * 5;\n");
    printf("  }\n");
    printf("  printf(\"x = %%d\", x);\n\n");
    printf("Réponse: x = ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    int x = 10;
    #pragma omp parallel private(x) num_threads(4)
    {
        x = omp_get_thread_num() * 5;
    }
    printf("✓ Réponse: x = %d\n", x);
    printf("  PRIVATE: les modifications ne sortent PAS de la région\n\n");
    
    // QUESTION 2.2 (5 points)
    printf("QUESTION 2.2: Quelle sera la valeur INITIALE de y dans chaque thread?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  int y = 100;\n");
    printf("  #pragma omp parallel private(y) num_threads(2)\n");
    printf("  {\n");
    printf("      printf(\"Thread %%d: y = %%d\", omp_get_thread_num(), y);\n");
    printf("  }\n\n");
    printf("a) y = 100 dans tous les threads\n");
    printf("b) y = 0 dans tous les threads\n");
    printf("c) y = valeur INDÉFINIE (garbage)\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    int y = 100;
    printf("Résultat réel:\n");
    #pragma omp parallel private(y) num_threads(2)
    {
        #pragma omp critical
        printf("  Thread %d: y = %d (INDÉFINIE!)\n", omp_get_thread_num(), y);
    }
    printf("\n✓ Réponse: c) Valeur INDÉFINIE\n");
    printf("  PRIVATE ne copie PAS la valeur initiale!\n\n");
    
    // QUESTION 2.3 (5 points)
    printf("QUESTION 2.3: FIRSTPRIVATE vs PRIVATE\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  int z = 50;\n");
    printf("  #pragma omp parallel firstprivate(z) num_threads(3)\n");
    printf("  {\n");
    printf("      printf(\"z = %%d\", z);\n");
    printf("  }\n\n");
    printf("Quelle sera la valeur de z dans chaque thread?\n");
    printf("Réponse: z = ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    int z = 50;
    printf("Résultat:\n");
    #pragma omp parallel firstprivate(z) num_threads(3)
    {
        #pragma omp critical
        printf("  Thread %d: z = %d\n", omp_get_thread_num(), z);
    }
    printf("\n✓ Réponse: z = 50 dans tous les threads\n");
    printf("  FIRSTPRIVATE copie la valeur initiale!\n\n");
    
    // QUESTION 2.4 (5 points)
    printf("QUESTION 2.4: RACE CONDITION - Spot the bug!\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  int counter = 0;\n");
    printf("  #pragma omp parallel num_threads(4)\n");
    printf("  {\n");
    printf("      for (int i = 0; i < 1000; i++) {\n");
    printf("          counter++;  // <-- Problème ici?\n");
    printf("      }\n");
    printf("  }\n");
    printf("  printf(\"counter = %%d\", counter);\n\n");
    printf("Valeur attendue: 4000 (4 threads × 1000)\n");
    printf("Que va-t-il se passer?\n");
    printf("a) counter = 4000 (correct)\n");
    printf("b) counter < 4000 (race condition)\n");
    printf("c) Erreur de compilation\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    int counter = 0;
    #pragma omp parallel num_threads(4)
    {
        for (int i = 0; i < 1000; i++) {
            counter++;
        }
    }
    printf("✓ Résultat: counter = %d\n", counter);
    printf("  Attendu: 4000\n");
    if (counter < 4000) {
        printf("  ⚠️  RACE CONDITION détectée!\n");
        printf("  Solution: utiliser #pragma omp atomic ou reduction\n");
    }
    printf("\n");
}

// ============================================================================
// SECTION 3: SCHEDULING (15 points)
// ============================================================================

void test_section3() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 3: SCHEDULING (STATIC/DYNAMIC/GUIDED)                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 3.1 (5 points)
    printf("QUESTION 3.1: Quel scheduling choisir?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Scénario: Boucle où chaque itération prend le MÊME temps\n");
    printf("for (i = 0; i < 1000; i++) {\n");
    printf("    result[i] = compute(i);  // Temps constant\n");
    printf("}\n\n");
    printf("Quel scheduling est le MEILLEUR?\n");
    printf("a) static\n");
    printf("b) dynamic\n");
    printf("c) guided\n");
    printf("d) Aucune différence\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: a) STATIC\n");
    printf("  Raisons:\n");
    printf("  - Overhead minimal (décisions à la compilation)\n");
    printf("  - Prévisible et cache-friendly\n");
    printf("  - Pas besoin de dynamic si charge équilibrée\n\n");
    
    // QUESTION 3.2 (5 points)
    printf("QUESTION 3.2: Distribution avec schedule(static, 2)\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  #pragma omp parallel for schedule(static, 2) num_threads(3)\n");
    printf("  for (int i = 0; i < 12; i++) {\n");
    printf("      printf(\"%%d \", i);\n");
    printf("  }\n\n");
    printf("Comment les itérations seront distribuées?\n");
    printf("Chunk size = 2, 3 threads, 12 itérations\n\n");
    printf("Thread 0: ____________\n");
    printf("Thread 1: ____________\n");
    printf("Thread 2: ____________\n\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Distribution correcte (Round-Robin avec chunk=2):\n");
    printf("  Thread 0: 0,1  6,7\n");
    printf("  Thread 1: 2,3  8,9\n");
    printf("  Thread 2: 4,5  10,11\n\n");
    printf("  Pattern: Chaque thread prend 2 itérations à tour de rôle\n\n");
    
    // QUESTION 3.3 (5 points)
    printf("QUESTION 3.3: Quand utiliser DYNAMIC?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Quel scénario nécessite schedule(dynamic)?\n\n");
    printf("a) for (i=0; i<n; i++) array[i] = i*2;\n");
    printf("b) for (i=0; i<n; i++) {\n");
    printf("       for (j=0; j<=i; j++) work(i,j);  // Charge variable!\n");
    printf("   }\n");
    printf("c) for (i=0; i<n; i++) sum += array[i];\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) Boucle avec charge variable\n");
    printf("  Explication:\n");
    printf("  - i=0: 1 opération\n");
    printf("  - i=99: 100 opérations\n");
    printf("  - STATIC causerait un déséquilibre massif\n");
    printf("  - DYNAMIC équilibre la charge dynamiquement\n\n");
}

// ============================================================================
// SECTION 4: RÉDUCTIONS (15 points)
// ============================================================================

void test_section4() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 4: RÉDUCTIONS                                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 4.1 (5 points)
    printf("QUESTION 4.1: Quelle est la valeur initiale pour reduction?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  int sum = 10;\n");
    printf("  #pragma omp parallel for reduction(+:sum) num_threads(2)\n");
    printf("  for (int i = 0; i < 4; i++) {\n");
    printf("      sum += i;  // 0+1+2+3 = 6\n");
    printf("  }\n");
    printf("  printf(\"sum = %%d\", sum);\n\n");
    printf("Quelle sera la valeur finale de sum?\n");
    printf("a) 6\n");
    printf("b) 10\n");
    printf("c) 16\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    int sum_test = 10;
    #pragma omp parallel for reduction(+:sum_test) num_threads(2)
    for (int i = 0; i < 4; i++) {
        sum_test += i;
    }
    printf("✓ Réponse: c) sum = %d\n", sum_test);
    printf("  Calcul: 10 (valeur initiale) + 0+1+2+3 = 16\n");
    printf("  La valeur d'origine est PRÉSERVÉE!\n\n");
    
    // QUESTION 4.2 (5 points)
    printf("QUESTION 4.2: Opérateurs de réduction\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Quelle est la valeur initiale pour reduction(*:product)?\n");
    printf("a) 0\n");
    printf("b) 1\n");
    printf("c) Indéfinie\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) 1\n");
    printf("  Valeurs initiales selon l'opérateur:\n");
    printf("  +  →  0\n");
    printf("  *  →  1\n");
    printf("  -  →  0\n");
    printf("  &  → ~0 (tous bits à 1)\n");
    printf("  |  →  0\n");
    printf("  && →  1 (true)\n");
    printf("  || →  0 (false)\n\n");
    
    // QUESTION 4.3 (5 points)
    printf("QUESTION 4.3: Correction de code\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code INCORRECT:\n");
    printf("  int max_val = 0;\n");
    printf("  #pragma omp parallel for reduction(max:max_val)\n");
    printf("  for (int i = 0; i < n; i++) {\n");
    printf("      if (array[i] > max_val) max_val = array[i];\n");
    printf("  }\n\n");
    printf("Pourquoi ce code est INCORRECT?\n");
    printf("a) Syntaxe reduction invalide\n");
    printf("b) max n'est pas un opérateur de reduction standard\n");
    printf("c) max_val devrait être private\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) 'max' n'est PAS un opérateur standard\n");
    printf("  Opérateurs valides: +, *, -, &, |, ^, &&, ||\n");
    printf("  Pour max/min: utiliser CRITICAL ou approche manuelle\n\n");
    printf("  Solution:\n");
    printf("    int max_val = array[0];\n");
    printf("    #pragma omp parallel for\n");
    printf("    for (int i = 1; i < n; i++) {\n");
    printf("        #pragma omp critical\n");
    printf("        if (array[i] > max_val) max_val = array[i];\n");
    printf("    }\n\n");
}

// ============================================================================
// SECTION 5: SYNCHRONISATION (20 points)
// ============================================================================

void test_section5() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 5: SYNCHRONISATION (CRITICAL/ATOMIC/BARRIER)            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 5.1 (5 points)
    printf("QUESTION 5.1: CRITICAL vs ATOMIC\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Lequel est le PLUS RAPIDE?\n\n");
    printf("Version A:\n");
    printf("  #pragma omp critical\n");
    printf("  counter++;\n\n");
    printf("Version B:\n");
    printf("  #pragma omp atomic\n");
    printf("  counter++;\n\n");
    printf("Réponse: Version ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: Version B (ATOMIC) est plus rapide\n");
    printf("  - ATOMIC: ~10-20 cycles CPU\n");
    printf("  - CRITICAL: ~100+ cycles CPU\n");
    printf("  MAIS: ATOMIC limité aux opérations simples\n\n");
    
    // QUESTION 5.2 (5 points)
    printf("QUESTION 5.2: Code valide avec ATOMIC?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Lequel est VALIDE?\n\n");
    printf("a) #pragma omp atomic\n");
    printf("   x = x + y * 2;\n\n");
    printf("b) #pragma omp atomic\n");
    printf("   x++;\n\n");
    printf("c) #pragma omp atomic\n");
    printf("   x = compute(y);\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) x++; SEULEMENT\n");
    printf("  ATOMIC ne supporte que:\n");
    printf("  - x++, ++x, x--, --x\n");
    printf("  - x = x op expr (où op = +,-,*,/,&,|,^,<<,>>)\n");
    printf("  - x binop= expr\n");
    printf("  Pour a) et c): utiliser CRITICAL\n\n");
    
    // QUESTION 5.3 (5 points)
    printf("QUESTION 5.3: Quand utiliser BARRIER?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  #pragma omp parallel num_threads(4)\n");
    printf("  {\n");
    printf("      phase1_compute();  // Écrit dans buffer[]\n");
    printf("      // Barrière nécessaire ici?\n");
    printf("      phase2_read();     // Lit buffer[]\n");
    printf("  }\n\n");
    printf("Faut-il une barrière?\n");
    printf("a) Non\n");
    printf("b) Oui\n");
    printf("c) Ça dépend\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) OUI, barrière NÉCESSAIRE\n");
    printf("  Raison:\n");
    printf("  - phase2 DÉPEND des résultats de phase1\n");
    printf("  - Sans barrière: certains threads lisent avant que\n");
    printf("    d'autres aient fini d'écrire\n");
    printf("  - Race condition garantie!\n\n");
    printf("  Code correct:\n");
    printf("    phase1_compute();\n");
    printf("    #pragma omp barrier\n");
    printf("    phase2_read();\n\n");
    
    // QUESTION 5.4 (5 points)
    printf("QUESTION 5.4: MASTER vs SINGLE\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Quelle est la différence?\n\n");
    printf("Version A:\n");
    printf("  #pragma omp master\n");
    printf("  { init(); }\n\n");
    printf("Version B:\n");
    printf("  #pragma omp single\n");
    printf("  { init(); }\n\n");
    printf("a) Aucune différence\n");
    printf("b) A: toujours thread 0, pas de barrière\n");
    printf("   B: n'importe quel thread, barrière implicite\n");
    printf("c) A plus rapide que B\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) Différences importantes\n");
    printf("  MASTER:\n");
    printf("  - TOUJOURS le thread 0\n");
    printf("  - PAS de barrière implicite\n");
    printf("  - Autres threads continuent immédiatement\n\n");
    printf("  SINGLE:\n");
    printf("  - N'importe quel thread (le premier arrivé)\n");
    printf("  - Barrière implicite à la fin\n");
    printf("  - Autres threads ATTENDENT\n\n");
}

// ============================================================================
// SECTION 6: RÈGLES DES BOUCLES (10 points)
// ============================================================================

void test_section6() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 6: RÈGLES DES BOUCLES FOR                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 6.1 (5 points)
    printf("QUESTION 6.1: Boucles parallélisables?\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Lesquelles peuvent être parallélisées avec #pragma omp for?\n\n");
    printf("A) for (int i = 0; i < n; i++)\n");
    printf("       a[i] = a[i-1] + 1;\n\n");
    printf("B) for (int i = 0; i < n; i++)\n");
    printf("       a[i] = b[i] * 2;\n\n");
    printf("C) int i = 0;\n");
    printf("   while (i < n) {\n");
    printf("       work(i);\n");
    printf("       i++;\n");
    printf("   }\n\n");
    printf("Réponses: ____ (ex: A,B)\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: B SEULEMENT\n");
    printf("  A) ❌ Dépendance: a[i] dépend de a[i-1]\n");
    printf("  B) ✓ Pas de dépendance, parallélisable\n");
    printf("  C) ❌ while pas supporté, seulement for\n\n");
    
    // QUESTION 6.2 (5 points)
    printf("QUESTION 6.2: Modification de l'indice\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  #pragma omp parallel for\n");
    printf("  for (int i = 0; i < n; i++) {\n");
    printf("      if (condition) i += 2;  // <-- Autorisé?\n");
    printf("      work(i);\n");
    printf("  }\n\n");
    printf("Ce code est:\n");
    printf("a) Valide\n");
    printf("b) Erreur de compilation\n");
    printf("c) Comportement indéfini\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) ERREUR de compilation\n");
    printf("  Règle: L'indice ne peut être modifié que par\n");
    printf("  l'expression de changement (i++, i+=2, etc.)\n");
    printf("  INTERDICTION de modifier i dans le corps de la boucle!\n\n");
}

// ============================================================================
// SECTION 7: EXERCICE PRATIQUE (20 points)
// ============================================================================

void test_section7() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 7: EXERCICE PRATIQUE - Debugging                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Trouve et corrige les 3 ERREURS dans ce code:\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  int sum = 0;\n");
    printf("  int n = 100;\n");
    printf("  #pragma omp parallel for\n");
    printf("  for (int i = 0; i < n; i++) {\n");
    printf("      sum += array[i];              // ERREUR 1?\n");
    printf("  }\n\n");
    printf("  int max = array[0];\n");
    printf("  #pragma omp parallel for private(max)\n");
    printf("  for (int i = 1; i < n; i++) {    // ERREUR 2?\n");
    printf("      if (array[i] > max) max = array[i];\n");
    printf("  }\n");
    printf("  printf(\"max = %%d\", max);\n\n");
    printf("  #pragma omp parallel num_threads(4)\n");
    printf("  {\n");
    printf("      int result = compute();\n");
    printf("      results[omp_get_thread_num()] = result;\n");
    printf("  }\n");
    printf("  // Utiliser results[] immédiatement // ERREUR 3?\n");
    printf("  process(results);\n\n");
    printf("Identifie les 3 erreurs:\n");
    printf("1. _______________\n");
    printf("2. _______________\n");
    printf("3. _______________\n\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ RÉPONSES:\n\n");
    printf("ERREUR 1: Race condition sur sum\n");
    printf("  Problème: Tous les threads modifient sum sans synchronisation\n");
    printf("  Solution: #pragma omp parallel for reduction(+:sum)\n\n");
    
    printf("ERREUR 2: max est private!\n");
    printf("  Problème: Chaque thread a sa copre de max, la valeur finale\n");
    printf("            dans le master thread reste array[0]\n");
    printf("  Solution: Enlever private(max) ET ajouter protection:\n");
    printf("            #pragma omp critical ou utiliser reduction manuelle\n\n");
    
    printf("ERREUR 3: Pas de barrière!\n");
    printf("  Problème: process(results) peut s'exécuter avant que tous\n");
    printf("            les threads aient écrit dans results[]\n");
    printf("  Solution: La fin de la région parallel a une barrière implicite,\n");
    printf("            donc OK si process() est APRÈS la région.\n");
    printf("            Mais si c'était dans la région, il faudrait:\n");
    printf("            #pragma omp barrier\n\n");
}

// ============================================================================
// SECTION 8: NOWAIT ET COLLAPSE (10 points)
// ============================================================================

void test_section8() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║  SECTION 8: NOWAIT ET COLLAPSE                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // QUESTION 8.1 (5 points)
    printf("QUESTION 8.1: Clause NOWAIT\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  #pragma omp parallel\n");
    printf("  {\n");
    printf("      #pragma omp for nowait\n");
    printf("      for (int i = 0; i < n; i++) compute1(i);\n\n");
    printf("      #pragma omp for\n");
    printf("      for (int i = 0; i < n; i++) compute2(i);\n");
    printf("  }\n\n");
    printf("Que fait NOWAIT?\n");
    printf("a) Supprime toutes les barrières\n");
    printf("b) Supprime la barrière après la première boucle\n");
    printf("c) Accélère l'exécution toujours\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: b) Supprime la barrière après la première boucle\n");
    printf("  Effet:\n");
    printf("  - Threads qui finissent compute1 tôt peuvent commencer compute2\n");
    printf("  - MAIS: seulement si compute2 n'a PAS besoin des résultats de compute1!\n");
    printf("  - Toujours une barrière implicite après la deuxième boucle\n\n");
    
    // QUESTION 8.2 (5 points)
    printf("QUESTION 8.2: COLLAPSE avec boucles imbriquées\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("Code:\n");
    printf("  #pragma omp parallel for collapse(2) num_threads(4)\n");
    printf("  for (int i = 0; i < 10; i++) {\n");
    printf("      for (int j = 0; j < 20; j++) {\n");
    printf("          work(i, j);\n");
    printf("      }\n");
    printf("  }\n\n");
    printf("Combien d'itérations au total?\n");
    printf("Réponse: ____ itérations\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    
    printf("✓ Réponse: 200 itérations (10 × 20)\n");
    printf("  Explication:\n");
    printf("  - COLLAPSE(2) fusionne les 2 boucles\n");
    printf("  - Crée une seule boucle de 200 itérations\n");
    printf("  - Chaque thread reçoit ~50 itérations (200/4)\n");
    printf("  - Meilleur équilibrage qu'avec seulement la boucle externe\n\n");
}

// ============================================================================
// RÉSULTATS ET CORRECTION
// ============================================================================

void afficher_resultats() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    BARÈME DE NOTATION                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("SECTION 1: Modèle Fork-Join .................. /10 points\n");
    printf("SECTION 2: Clauses de portée ................. /20 points\n");
    printf("SECTION 3: Scheduling ........................ /15 points\n");
    printf("SECTION 4: Réductions ........................ /15 points\n");
    printf("SECTION 5: Synchronisation ................... /20 points\n");
    printf("SECTION 6: Règles des boucles ................ /10 points\n");
    printf("SECTION 7: Exercice pratique ................. /20 points\n");
    printf("SECTION 8: Nowait et Collapse ................ /10 points\n");
    printf("                                               ─────────\n");
    printf("TOTAL ......................................... /120 points\n\n");
    
    printf("ÉCHELLE DE NOTATION:\n");
    printf("  108-120 : Excellent (A+) - Maîtrise parfaite 🏆\n");
    printf("   96-107 : Très bien (A)  - Excellente compréhension ⭐\n");
    printf("   84-95  : Bien (B)       - Bonne maîtrise ✓\n");
    printf("   72-83  : Assez bien (C) - Compréhension correcte\n");
    printf("   60-71  : Passable (D)   - Notions de base acquises\n");
    printf("   < 60   : Insuffisant    - Révision nécessaire 📚\n\n");
    
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    POINTS CLÉS À RETENIR                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("1. MODÈLE FORK-JOIN:\n");
    printf("   - Master thread = toujours thread 0\n");
    printf("   - Ordre d'exécution NON déterministe\n");
    printf("   - Fork au début, Join à la fin (barrière implicite)\n\n");
    
    printf("2. CLAUSES DE PORTÉE:\n");
    printf("   - PRIVATE: copie, valeur INDÉFINIE, ne sort pas\n");
    printf("   - FIRSTPRIVATE: copie, INITIALISÉE, ne sort pas\n");
    printf("   - LASTPRIVATE: copie, récupère DERNIÈRE valeur\n");
    printf("   - SHARED: une seule copie, ATTENTION race conditions!\n");
    printf("   - DEFAULT(NONE): mode strict RECOMMANDÉ\n\n");
    
    printf("3. SCHEDULING:\n");
    printf("   - STATIC: rapide, prévisible, charge équilibrée\n");
    printf("   - DYNAMIC: flexible, charge variable, overhead\n");
    printf("   - GUIDED: compromis, chunks décroissants\n");
    printf("   - Chunk size = taille des blocs d'itérations\n\n");
    
    printf("4. RÉDUCTIONS:\n");
    printf("   - Évite les race conditions sur accumulateurs\n");
    printf("   - Opérateurs: +,*,-,&,|,^,&&,||\n");
    printf("   - Valeur initiale selon opérateur\n");
    printf("   - Préserve la valeur d'origine\n\n");
    
    printf("5. SYNCHRONISATION:\n");
    printf("   - CRITICAL: section protégée, un thread à la fois\n");
    printf("   - ATOMIC: opération simple, plus rapide\n");
    printf("   - BARRIER: tous attendent, point de sync\n");
    printf("   - MASTER vs SINGLE: thread 0 vs n'importe lequel\n\n");
    
    printf("6. RÈGLES DES BOUCLES:\n");
    printf("   - Indice = int ou pointeur\n");
    printf("   - Pas de modification de l'indice dans la boucle\n");
    printf("   - Pas de break/goto\n");
    printf("   - Pas de dépendances entre itérations\n");
    printf("   - Seulement for, pas while\n\n");
    
    printf("7. OPTIMISATIONS:\n");
    printf("   - NOWAIT: supprime barrière si possible\n");
    printf("   - COLLAPSE: fusionne boucles imbriquées\n");
    printf("   - REDUCTION > CRITICAL > ATOMIC pour accumulations\n");
    printf("   - Minimiser la synchronisation\n\n");
}

// ============================================================================
// SECTION BONUS: Questions avancées (optionnelles)
// ============================================================================

void test_bonus() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║             SECTION BONUS: Questions avancées (+10)              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("BONUS 1: Nested Parallelism\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("Si OMP_NESTED=TRUE et:\n");
    printf("  #pragma omp parallel num_threads(2)\n");
    printf("  {\n");
    printf("      #pragma omp parallel num_threads(3)\n");
    printf("      { work(); }\n");
    printf("  }\n\n");
    printf("Combien de threads au niveau 2?\n");
    printf("Réponse: ____ threads\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    printf("✓ Réponse: 6 threads (2 × 3)\n");
    printf("  Chaque thread du niveau 1 crée 3 threads\n\n");
    
    printf("BONUS 2: Performance\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("Pour mesurer le temps d'exécution en OpenMP:\n");
    printf("Quelle fonction utiliser?\n");
    printf("a) clock()\n");
    printf("b) time()\n");
    printf("c) omp_get_wtime()\n\n");
    printf("Réponse: ____\n");
    printf("APPUIE SUR ENTRÉE...");
    getchar();
    printf("✓ Réponse: c) omp_get_wtime()\n");
    printf("  Temps en secondes (wall-clock time)\n");
    printf("  Usage:\n");
    printf("    double start = omp_get_wtime();\n");
    printf("    // code\n");
    printf("    double end = omp_get_wtime();\n");
    printf("    printf(\"Time: %%f\\n\", end - start);\n\n");
}

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

int main() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                   ║\n");
    printf("║           TEST DE CONNAISSANCES - OpenMP                          ║\n");
    printf("║              Basé sur le cours AAPP 2025-2026                     ║\n");
    printf("║                                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("INSTRUCTIONS:\n");
    printf("  • Réponds mentalement à chaque question\n");
    printf("  • Appuie sur ENTRÉE pour voir la réponse\n");
    printf("  • Note tes points au fur et à mesure\n");
    printf("  • Total: 120 points (+ 10 bonus)\n");
    printf("  • Durée estimée: 30-45 minutes\n\n");
    printf("CONSEIL: Prends un papier pour noter tes réponses!\n\n");
    printf("Prêt(e)? Appuie sur ENTRÉE pour commencer...");
    getchar();
    
    // Exécuter les sections
    test_section1();
    printf("Score Section 1: ___/10\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section2();
    printf("Score Section 2: ___/20\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section3();
    printf("Score Section 3: ___/15\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section4();
    printf("Score Section 4: ___/15\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section5();
    printf("Score Section 5: ___/20\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section6();
    printf("Score Section 6: ___/10\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section7();
    printf("Score Section 7: ___/20\n");
    printf("Appuie sur ENTRÉE pour continuer...");
    getchar();
    
    test_section8();
    printf("Score Section 8: ___/10\n");
    printf("Appuie sur ENTRÉE pour voir le bonus...");
    getchar();
    
    test_bonus();
    printf("Score Bonus: ___/10\n");
    printf("Appuie sur ENTRÉE pour les résultats finaux...");
    getchar();
    
    // Afficher les résultats
    afficher_resultats();
    
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                      FIN DU TEST                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("CALCULE TON SCORE TOTAL:\n");
    printf("  Section 1-8: ___/120 points\n");
    printf("  Bonus:       ___/10 points\n");
    printf("               ─────────────\n");
    printf("  TOTAL:       ___/130 points\n\n");
    
    printf("PROCHAINES ÉTAPES:\n");
    printf("  ✓ Si > 100/120: Tu maîtrises bien! Passe aux Labs 2 et 3\n");
    printf("  ✓ Si 80-100:    Bonne base, révise les points faibles\n");
    printf("  ✓ Si < 80:      Relis le cours et refais ce test\n\n");
    
    printf("RESSOURCES:\n");
    printf("  • Documentation officielle: www.openmp.org\n");
    printf("  • Exemples OpenMP: www.openmp.org/resources/openmp-examples/\n");
    printf("  • Tutoriels LLNL: hpc-tutorials.llnl.gov/openmp/\n\n");
    
    printf("Bon courage pour les prochains labs! 🚀\n\n");
    
    return 0;
}