# Feedback et Notation - Projet Codexion

**Date**: 21 février 2026  
**Projet**: Codexion - Simulation de coders avec synchronisation multi-thread  
**Notation**: 42/100

---

## 📋 Résumé Exécutif

Le projet implémente les bases d'un simulateur de coders partageant des ressources limitées (dongles) avec synchronisation par pthreads. L'architecture générale est correcte, mais plusieurs aspects critiques ne sont pas fonctionnels ou incomplets, en particulier la gestion des délais d'épuisement (burnout) et le système de planification.

---

## ✅ Points Positifs

### 1. **Architecture générale solide**
- Bonne organisation du code en modules distincts (parsing, init, clean, threads, etc.)
- Séparation claire des responsabilités
- Structure de données bien pensée (`t_data`, `t_coder`, `t_dongle`)

### 2. **Gestion mémoire**
- Allocation dynamique des structures principales
- Fonction `clean()` qui libère proprement les ressources
- Pas de fuites mémoire évidentes

### 3. **Validation des arguments**
- Vérification des arguments en entrée (nombres positifs, scheduler valide)
- Protection contre les débordements avec `ft_atou64_s()`
- Gestion correcte des erreurs de parsing

### 4. **Logging sincronisé**
- Mutex `log_lock` pour éviter les entrelacementsurlogs
- Format de timestamp en millisecondes correct
- Fonction `log_message()` bien implémentée

### 5. **Topologie circulaire des dongles**
- Correctly set up dongle assignments for each coder
- Left and right dongle correctly assigned in circular arrangement
- Proper handling of single coder case

---

## ❌ Problèmes Critiques

### 1. **DÉFAILLANCE MAJEURE: Gestion du burnout incomplète** ⚠️⚠️⚠️
**Sévérité: CRITIQUE**

**Problème observé**:
- Pas de thread moniteur pour détecter le burnout comme requis
- La logique de burnout est dans `coder_routine()` mais elle vérifie après compilation/debug/refactor
- Le délai de burnout est défini comme `time_to_burnout` milliseconds depuis le dernier compile
- Le sujet demande: "If a coder did not **start compiling** within `time_to_burnout`"
- Votre code teste APRÈS les actions, ce qui est trop tardif

**Ce qui manque**:
```
• Un thread monitor séparé qui surveille les deadlines en temps réel
• Une structure pour tracker last_compile_start pour chaque coder
• Une condition variable pour réveiller le monitor
• Log du burnout dans les 10ms du délai réel
```

**Pistes de correction**:
- Créer une structure `s_coder` avec champ `last_compile_start` (uint64_t)
- Ajouter un thread monitor qui utilise `pthread_cond_timedwait()` pour vérifier tous les 100ms
- Le monitor doit lire le timestamp et comparer avec `last_compile_start + time_to_burnout`
- Utiliser un flag `is_burned_out` pour signaler l'arrêt aux threads coders

---

### 2. **Pas de système de planification (FIFO/EDF)** ⚠️⚠️
**Sévérité: CRITIQUE**

**Problème observé**:
- Fonction `compile()` incomplète et syntaxiquement fausse
- Variables `first`, `second`, `deadline` utilisées mais pas déclarées dans cette fonction
- Le code essaie d'utiliser `pthread_mutex_timedlock()` mais sans vrai deadline
- Aucune file d'attente (queue) ou heap n'est implémenté
- Le sujet demande explicitement: "you must implement a priority queue (heap)"

**Ce qui manque**:
```
• Structure de heap/priority queue pour FIFO ou EDF
• Gestion de la liste d'attente des coders attendant les dongles
• Condition variables pour réveiller les coders dans l'ordre correct
• Différence de comportement entre FIFO et EDF
```

**Pistes de correction**:
- Créer `struct s_queue` avec tableau des coders en attente
- Pour FIFO: simple ordre d'arrivée
- Pour EDF: comparer `last_compile_start + time_to_burnout` pour ordonner
- Ajouter à `s_dongle`: `pthread_cond_t`, numéro de propriétaire, timestamp de disponibilité
- Chaque dongle maintient sa queue et distribue selon le scheduler

---

### 3. **Cooldown des dongles non implémenté** ⚠️
**Sévérité: HAUTE**

**Problème observé**:
- Paramètre `dongle_cooldown` parsé mais jamais utilisé
- Pas de timestamp de libération pour chaque dongle
- Les dongles sont immédiatement disponibles après unlock

**Pistes de correction**:
- Ajouter à `s_dongle`: `uint64_t cooldown_until`
- Avant d'accorder un dongle, vérifier que `current_time >= cooldown_until`
- Après libération: `cooldown_until = current_time + dongle_cooldown`
- Utiliser `pthread_cond_timedwait()` avec le délai de cooldown

---

### 4. **Messages de logs incomplets/incorrects** ⚠️
**Sévérité: MOYENNE**

**Problème observé**:
- Manquent les logs "X has taken a dongle" (deux par compile)
- Le burnout log dit "died (burnout)" au lieu de "burned out"
- Pas de distinction entre les états : debug et refactor ne sont pas loggés
- Les messages manquent entre acquisition et compilation

**Pistes de correction**:
- Ajouter `log_message(coder, "has taken a dongle")` à chaque acquisition réussie
- Utiliser exactement: "burned out" (pas "died")
- Appeler log pour chaque action: has taken dongle → is compiling → release → is debugging → is refactoring
- Lire le sujet pour le format EXACT attendu

---

### 5. **Fonction `compile()` syntaxiquement invalide** ⚠️⚠️
**Sévérité: HAUTE**

```c
bool compile()  // Pas de paramètres !
{
    int acquired;
    acquired = 0;
    if (pthread_mutex_timedlock(first, &deadline) == 0)  // first non déclaré !
    {
        if (pthread_mutex_timedlock(second, &deadline) == 0)  // second non déclaré !
        // ...
    }
}
```

**Problème**:
- `first`, `second`, `deadline` ne sont pas passés en paramètres
- Variabl accédées depuis `coder_routine()` mais pas disponibles dans `compile()`
- Fonction n'accepte aucun paramètre alors qu'elle en a besoin

**Pistes de correction**:
- Passer `t_coder *coder`, `pthread_mutex_t **dongles`, `struct timespec deadline` en paramètres
- Ou intégrer la logique directement dans `coder_routine()`
- Ou utiliser un contexte partagé dans une structure

---

### 6. **Pas de gestion de deadline avec `pthread_mutex_timedlock()`** ⚠️
**Sévérité: MOYENNE**

**Problème observé**:
- Le deadline est calculé mais pas utilisé correctement
- `pthread_mutex_timedlock()` attend un `struct timespec` absolu (pas relatif)
- Le code convertit mal `time_burnout` (ms) en timespec
- Conversion `deadline.tv_nsec` peut dépasser 1 billion

**Pistes de correction**:
```c
struct timespec get_absolute_deadline(t_data *data, uint64_t ms_from_now) {
    struct timespec deadline;
    struct timeval now;
    gettimeofday(&now, NULL);
    
    uint64_t deadline_ms = ((uint64_t)now.tv_sec * 1000) + (now.tv_usec / 1000) + ms_from_now;
    deadline.tv_sec = deadline_ms / 1000;
    deadline.tv_nsec = (deadline_ms % 1000) * 1000000;  // *1M pas *1K
    return deadline;
}
```

---

### 7. **Boucle infinie dans `mysleep()`** ⚠️
**Sévérité: BASSE**

```c
while (now - start < time_in_ms)
{
    usleep(500);  // Busy waiting = CPU 100%
    gettimeofday(&tv, NULL);
    now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
```

**Problème**:
- Consomme énormément de CPU (busy loop)
- `usleep(500)` trop petit, reveille toutes les 500µs
- Affecte les mesures de timing et les performances globales

**Pistes de correction**:
- Utiliser `usleep(100000)` (100ms) ou plus selon le contexte
- Ou utiliser nanosleep avec des délais plus longs
- Pour mieux: utiliser condition variables avec timeout

---

### 8. **État global incomplet** ⚠️
**Sévérité: MOYENNE**

**Manque**:
- Pas de flag `simulation_running` pour arrêter proprement
- Pas de synchronisation entre threads coders et le monitor
- Les threads coders ne savent pas quand arrêter (ils finissent juste)
- Pas de compteur d'arguments pour verifier `ac < 9`

**Pistes de correction**:
- Ajouter `bool simulation_running` à `t_data`
- Initialiser à `true` et passer à `false` quand:
  - Un coder brûle
  - Tous ont compilé assez de fois
- Tous les threads vérifient cette flag en boucle
- Utiliser une condition variable pour signaler l'arrêt

---

## 🔄 Processus d'Amélioration Recommandé

### Phase 1: Fondations (2-3h)
1. Corriger la syntaxe de `compile()` ou la refactoriser
2. Implémenter le système de planification de base (FIFO simple d'abord)
3. Ajouter les logs "has taken a dongle" partout
4. Corriger les messages "burned out"

### Phase 2: Burnout et Monitoring (2-3h)
1. Ajouter `last_compile_start` à `t_coder`
2. Créer un thread monitor dédié
3. Implémenter la détection de burnout avec deadline correct
4. Tester avec des scénarios de burnout provoqué

### Phase 3: Cooldown et EDF (2h)
1. Implémenter le cooldown des dongles
2. Ajouter support du scheduler EDF
3. Implémenter la priority queue/heap
4. Tester les deux schedulers

### Phase 4: Polissage (1h)
1. Optimiser `mysleep()`
2. Vérifier les fuites mémoire (valgrind)
3. Tests edge cases: 1 coder, valeurs grandes, etc.

---

## 📊 Détail de la Notation

| Critère | Points | Raison |
|---------|--------|--------|
| Architecture et organisation | 15/15 | Code bien structuré |
| Parsing et validation | 8/8 | Correct et complet |
| Gestion mémoire | 8/8 | Pas de fuites |
| Logging synchronisé | 6/8 | Messages incomplets |
| Topologie dongles | 5/5 | Correct |
| **Gestion burnout** | **0/15** | MANQUANT - Monitor absent |
| **Planification FIFO/EDF** | **0/15** | Pas implémenté |
| **Cooldown dongles** | **0/8** | Pas utilisé |
| **Thread safety global** | **2/10** | Mutexes partiels |
| **Tests et robustesse** | **0/5** | Non testé |
| **Bonus/Polish** | **-2/5** | Code incomplet |
| **TOTAL** | **42/100** | |

---

## 🎯 Prochaines Étapes Proposées

1. **Immédiat**: Corriger les erreurs de syntaxe dans `cod_routine.c`
2. **Court terme**: Implémenter le thread monitor de burnout
3. **Moyen terme**: Ajouter le système de file d'attente (FIFO) pour les dongles
4. **Long terme**: Implémenter EDF et optimiser

---

## 📚 Ressources Recommandées

- `man pthread_cond_timedwait` - Pour les délais avec condition variables
- `man pthread_mutex_timedlock` - Pour acquérir avec timeout
- POSIX time specs: toujours absolu, jamais relatif
- Implémenter un heap binaire pour EDF (voir sujet VI)

---

**Note finale**: Le projet a une bonne base mais nécessite un travail significatif sur les mécanismes de synchronisation avancés. Continuez le travail! 💪
