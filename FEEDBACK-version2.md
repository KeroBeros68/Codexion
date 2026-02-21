# Feedback et Notation - Projet Codexion (VERSION 2)

**Date**: 21 février 2026  
**Projet**: Codexion - Simulation de coders avec synchronisation multi-thread  
**Notation**: 48/100 (+6 points par rapport à V1)

---

## 📋 Résumé Exécutif

La version 2 montre des améliorations significatives avec l'ajout d'un thread monitor et la structuration du système de burnout. Cependant, plusieurs implémentations restent incomplètes ou contiennent des erreurs critiques qui empêchent le projet de fonctionner correctement.

**Note importante**: Ce feedback respecte **strictement** les fonctions autorisées du sujet:
```
pthread_create, pthread_join, pthread_mutex_init, pthread_mutex_lock,
pthread_mutex_unlock, pthread_mutex_destroy, pthread_cond_init,
pthread_cond_wait, pthread_cond_timedwait, pthread_cond_broadcast,
pthread_cond_destroy, gettimeofday, usleep, write, malloc, free,
printf, fprintf, strcmp, strlen, atoi, memset
```
**Remarque**: `pthread_mutex_timedlock()` n'est PAS autorisé. Voir section "Implémentation du Timeout".

---

## ✅ Points Positifs (Améliorations)

### 1. **Introduction du thread monitor** ⭐ NOUVEAU
- Nouveau fichier `burn_monitor.c` dédié à la surveillance
- Thread monitor séparé pour détecter les burnouts
- Concept correct : boucle de vérification périodique
- Flag `start_simulation` ajouté pour contrôler l'arrêt

### 2. **Tracking du timestamp de compilation** ⭐ NOUVEAU
- Champ `last_compile` ajouté à `t_coder`
- Base pour la détection de burnout
- Permet l'implémentation de EDF

### 3. **Cooldown des dongles déclaré** ⭐ NOUVEAU
- Champ `cooldown_end` ajouté à `t_dongle`
- Infrastructure prête pour l'implémentation

### 4. **Architecture améliorée**
- Séparation plus claire des responsabilités
- Meilleure organisation avec commentaires dans coders.h

### 5. **Prise en compte de `start_simulation`**
- Condition d'arrêt ajoutée dans `coder_routine()`
- Communication entre le monitor et les threads coders

---

## ❌ Problèmes Critiques (Restants et Nouveaux)

### 1. **ERREUR CRITIQUE: Monitor utilise `scheduler` au lieu de `start_simulation`** ⚠️⚠️⚠️
**Sévérité: CRITIQUE**

**Code actuel** (burn_monitor.c, ligne 31):
```c
if (timestamp_ms > (data->coder_list[i].last_compile + data->time_burnout))
{
    data->scheduler = false;  // ❌ ERREUR! scheduler est un enum!
    log_message(&data->coder_list[i], "burnout");
    break;
}
```

**Problèmes**:
- `data->scheduler` est de type `enum e_scheduler_type` (FIFO ou EDF)
- Vous le mettez à `false` (booléen) → incompatible de type!
- Devrait être `data->start_simulation = false`
- Le compilateur devrait rejeter ce code avec -Werror

**Impact**: Le projet ne compile probablement pas en ce moment.

**Piste de correction**:
```c
data->start_simulation = false;  // Utiliser le bon flag
```

---

### 2. **Monitor ne calcule PAS le timestamp correctement** ⚠️⚠️
**Sévérité: CRITIQUE**

**Code actuel** (ligne 23):
```c
timestamp_ms = (tv.tv_sec);  // ❌ C'est en SECONDES, pas ms!
```

**Problème**:
- `tv.tv_sec` = secondes depuis epoch
- Devrait être milliseconds depuis le `start_time`
- La comparaison avec `time_burnout` (en ms) est donc fausse
- Burnout se déclenchera après ~1000 secondes (17 minutes) peu importe les paramètres!

**Piste de correction**:
```c
uint64_t current_ms = ((uint64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
uint64_t start_ms = ((uint64_t)data->start_time.tv_sec * 1000) + (data->start_time.tv_usec / 1000);

if (current_ms - start_ms > (data->coder_list[i].last_compile + data->time_burnout))
{
    // Burnout!
}
```

Ou mieux, calculer le temps écoulé depuis le **dernier compile**:
```c
uint64_t time_since_last_compile = current_ms - data->coder_list[i].last_compile;
if (time_since_last_compile > data->time_burnout)
{
    // Burnout!
}
```

---

### 3. **`last_compile` n'est JAMAIS modifié!** ⚠️⚠️
**Sévérité: CRITIQUE**

**Problème**:
- Champ `last_compile` déclaré dans `t_coder` mais jamais mis à jour
- Aucun appel à `gettimeofday()` pour enregistrer le temps
- Reste à 0 depuis l'initialisation
- Monitor compare toujours `0 + time_burnout` vs `current_time`
- Burnout détecté immédiatement!

**Piste de correction**:
- Ajouter dans `coder_routine()` juste avant de compiler:
```c
struct timeval tv;
gettimeofday(&tv, NULL);
coder->last_compile = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
```

---

### 4. **Fonctions `compile()`, `take_dongle()`, `left_or_right_handed()` sont des stubs** ⚠️⚠️
**Sévérité: CRITIQUE**

**Code actuel**:
```c
void left_or_right_handed(t_coder *coder, pthread_mutex_t *first,
    pthread_mutex_t *second)
{
    // Reçoit les pointeurs mais ne les modifie pas!
    // En C, les modifications locales n'affectent pas l'appelant
    if (coder->id % 2 == 0) {
        first = &coder->left_dongle->dongle_lock;  // ❌ Modifie la copie locale
        second = &coder->right_dongle->dongle_lock;
    }
}

void take_dongle(t_coder *coder, pthread_mutex_t *first,
    pthread_mutex_t *second)
{
    return ;  // ❌ Ne fait absolument rien
}

bool compile(void)
{
    return (true);  // ❌ Retourne toujours true sans rien faire
}
```

**Impact**:
- Pas d'acquisition de dongles
- Pas de vérification des délais
- Pas de logs "has taken a dongle"
- Pas de compilations réelles

**Pistes de correction**:

**Approche 1: Retourner une structure**
```c
typedef struct s_dongle_pair
{
    pthread_mutex_t *first;
    pthread_mutex_t *second;
} t_dongle_pair;

t_dongle_pair get_dongles(t_coder *coder)
{
    t_dongle_pair pair;
    
    if (coder->id % 2 == 0) {
        pair.first = &coder->left_dongle->dongle_lock;
        pair.second = &coder->right_dongle->dongle_lock;
    } else {
        pair.first = &coder->right_dongle->dongle_lock;
        pair.second = &coder->left_dongle->dongle_lock;
    }
    return (pair);
}
```

**Approche 2: Utiliser la mémoire à travers un pointeur vers struct**
```c
// Dans la structure, directement
t_dongle_pair dongle_pair;  // Dans t_coder ou passer une struct

// Ou passer par double pointeur
void get_dongles(t_coder *coder, pthread_mutex_t **first, pthread_mutex_t **second)
{
    if (coder->id % 2 == 0) {
        *first = &coder->left_dongle->dongle_lock;
        *second = &coder->right_dongle->dongle_lock;
    } else {
        *first = &coder->right_dongle->dongle_lock;
        *second = &coder->left_dongle->dongle_lock;
    }
}
```

**Implémenter `compile()` avec condition variables** (puisque `pthread_mutex_timedlock()` n'est pas autorisé):
```c
// Nouvelle structure pour chaque dongle
typedef struct s_dongle
{
    int                 id;
    pthread_mutex_t     dongle_lock;
    pthread_cond_t      cond;           // ← Ajouter
    uint64_t            cooldown_until;
    int                 holder_id;      // Quel coder le tient
} t_dongle;

bool acquire_dongle_with_timeout(t_coder *coder, t_dongle *dongle, uint64_t deadline_ms)
{
    struct timespec deadline;
    struct timeval tv_now;
    
    // Convertir ms absolu en timespec absolu
    gettimeofday(&tv_now, NULL);
    deadline.tv_sec = (time_t)(deadline_ms / 1000);
    deadline.tv_nsec = (long)((deadline_ms % 1000) * 1000000);
    
    // Attendre avec timeout
    pthread_mutex_lock(&dongle->dongle_lock);
    
    // Attendre que le dongle soit disponible ET que cooldown soit passé
    while (dongle->holder_id != -1 || 
           ((uint64_t)tv_now.tv_sec * 1000 + tv_now.tv_usec / 1000) < dongle->cooldown_until)
    {
        if (pthread_cond_timedwait(&dongle->cond, &dongle->dongle_lock, &deadline) == ETIMEDOUT)
        {
            pthread_mutex_unlock(&dongle->dongle_lock);
            return (false);  // Deadline dépassée
        }
        gettimeofday(&tv_now, NULL);
    }
    
    // Dongle acquis!
    dongle->holder_id = coder->id;
    log_message(coder, "has taken a dongle");
    
    pthread_mutex_unlock(&dongle->dongle_lock);
    return (true);
}

bool compile(t_coder *coder, pthread_mutex_t *first, t_dongle *first_dongle,
             pthread_mutex_t *second, t_dongle *second_dongle)
{
    struct timeval tv_start;
    uint64_t deadline_ms;
    
    // Calculer deadline = maintenant + time_to_burnout
    gettimeofday(&tv_start, NULL);
    deadline_ms = ((uint64_t)tv_start.tv_sec * 1000) + (tv_start.tv_usec / 1000) 
                  + coder->data->time_burnout;
    
    // Essayer d'acquérir les deux dongles
    if (!acquire_dongle_with_timeout(coder, first_dongle, deadline_ms))
        return (false);
    
    if (!acquire_dongle_with_timeout(coder, second_dongle, deadline_ms))
    {
        // Libérer le premier si le second échoue
        pthread_mutex_lock(&first_dongle->dongle_lock);
        first_dongle->holder_id = -1;
        first_dongle->cooldown_until = ((uint64_t)tv_start.tv_sec * 1000) 
                                       + (tv_start.tv_usec / 1000) 
                                       + coder->data->dongle_cooldown;
        pthread_cond_broadcast(&first_dongle->cond);
        pthread_mutex_unlock(&first_dongle->dongle_lock);
        
        return (false);
    }
    
    log_message(coder, "is compiling");
    mysleep(coder->data->time_compile);
    
    // Libérer les deux dongles
    release_dongle(first_dongle, coder->data);
    release_dongle(second_dongle, coder->data);
    
    // Mettre à jour le timestamp de compilation
    gettimeofday(&tv_start, NULL);
    coder->last_compile = ((uint64_t)tv_start.tv_sec * 1000) + (tv_start.tv_usec / 1000);
    
    return (true);
}

void release_dongle(t_dongle *dongle, t_data *data)
{
    struct timeval tv;
    
    pthread_mutex_lock(&dongle->dongle_lock);
    
    dongle->holder_id = -1;
    gettimeofday(&tv, NULL);
    dongle->cooldown_until = ((uint64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000) 
                             + data->dongle_cooldown;
    
    pthread_cond_broadcast(&dongle->cond);  // Réveiller tous les attendants
    pthread_mutex_unlock(&dongle->dongle_lock);
}
```

---

### 5. **Monitor s'exécute dans une boucle infinie sans sortie** ⚠️
**Sévérité: HAUTE**

**Code actuel**:
```c
void *monitor(void *arg)
{
    // ...
    while (1)  // Boucle infinie!
    {
        // Vérifier burnout
    }
    return (NULL);
}
```

**Problèmes**:
- Ne sort jamais de la boucle
- `pthread_join()` attendra indéfiniment
- Le programme ne s'arrêtera jamais proprement
- Pas d'usleep → busy loop consommant 100% CPU

**Piste de correction**:
```c
void *monitor(void *arg)
{
    t_data          *data;
    int             i;
    struct timeval  tv;
    uint64_t        current_ms;
    uint64_t        time_since_last;

    data = (t_data *)arg;
    while (data->start_simulation)  // Sort quand un burnout se produit
    {
        usleep(100000);  // Dormir 100ms entre les vérifications (IMPORTANT!)
        
        gettimeofday(&tv, NULL);
        current_ms = ((uint64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
        
        i = 0;
        while (i < data->nb_coders)
        {
            if (data->coder_list[i].last_compile > 0)  // Si a déjà compilé
            {
                time_since_last = current_ms - data->coder_list[i].last_compile;
                
                if (time_since_last > data->time_burnout)
                {
                    // Burnout détecté!
                    data->start_simulation = false;
                    log_message(&data->coder_list[i], "burned out");
                    break;
                }
            }
            else if (current_ms > data->time_burnout)  // Pas encore compilé et deadline dépassée
            {
                data->start_simulation = false;
                log_message(&data->coder_list[i], "burned out");
                break;
            }
            i++;
        }
    }
    return (NULL);
}
```

---

### 6. **Pas de condition variable pour arrêter les threads** ⚠️
**Sévérité: MOYENNE**

**Problème**:
- Les coders vérifient `start_simulation` une seule fois au début de la boucle
- Une fois la condition vraie, ils continuent même si le flag change
- Le monitor change le flag mais les coders ne le voient pas

**Piste de correction**:
```c
while (i < coder->data->nb_compile && coder->data->start_simulation)
{
    // ... cycle compile/debug/refactor
    
    // Revérifier après chaque cycle
}
```

Ce point est correct dans votre code! ✓

---

### 7. **Implémentation du Timeout avec Condition Variables** ⭐ IMPORTANT
**Sévérité: CRITIQUE - Fonction autorisée vs non-autorisée**

**Important**: Le sujet n'autorise PAS `pthread_mutex_timedlock()`. 
Il faut utiliser `pthread_cond_timedwait()` à la place.

**La bonne approche**:
- Chaque dongle a un `pthread_cond_t`
- Chaque dongle a un état: disponible ou occupé
- Chaque dongle a `cooldown_until` (timestamp ms)
- Les coders font:
  1. Lock le mutex du dongle
  2. Attendre avec `pthread_cond_timedwait()` jusqu'au deadline
  3. Vérifier disponibilité + cooldown
  4. Si OK: marquer comme occupé
  5. Unlock et continuer
  6. À la libération: marquer disponible, mettre cooldown_until, broadcast

Cette approche est robuste et respecte les contraintes du sujet.

---

### 8. **Threads.c ne crée pas le monitor!** ⚠️⚠️
**Sévérité: HAUTE**

**Problème**:
- `threads.c` crée les threads coders mais **pas** le thread monitor
- Vous avez créé `monitor()` dans `burn_monitor.c` mais il n'est jamais appelé
- La surveillance de burnout ne fonctionne donc pas

**Piste de correction**:
```c
bool start_thread(t_data *data, pthread_t *threads)
{
    int i;
    pthread_t monitor_thread;
    
    // Créer le monitor EN PREMIER
    if (pthread_create(&monitor_thread, NULL, monitor, data) != 0)
    {
        return (cod_error(data, "Failed to create monitor thread"));
    }
    
    // Puis créer les coders
    i = 0;
    while (i < data->nb_coders)
    {
        if (pthread_create(&threads[i], NULL, coder_routine, 
                &data->coder_list[i]) != 0)
        {
            // ...
        }
        i++;
    }
    return (true);
}

void stop_thread(t_data *data, pthread_t *threads)
{
    int i;
    
    i = 0;
    while (i < data->nb_coders)
    {
        pthread_join(threads[i], NULL);
        i++;
    }
    
    pthread_join(monitor_thread, NULL);  // Joindre le monitor aussi
    // ...
}
```

---

### 9. **Pas d'initialisation de `last_compile` et `start_simulation`** ⚠️
**Sévérité: MOYENNE**

**Problème**:
- `init.c` ne semble pas initialiser `last_compile` à 0
- `start_simulation` initialisé à true (bon) mais pas de trace du reste

**Piste de correction** (dans init.c):
```c
while (i < data->nb_coders)
{
    data->coder_list[i].id = i + 1;
    data->coder_list[i].last_compile = 0;  // Ajouter
    data->coder_list[i].left_dongle = &data->dongle_list[i];
    // ...
}
```

---

### 10. **Makefile: burn_monitor.c non inclus?** ⚠️
**Sévérité: HAUTE**

**Problème probable**:
- `burn_monitor.c` a été créé mais le Makefile ne l'inclut peut-être pas
- Le fichier ne sera pas compilé → linker error

**À vérifier dans Makefile**:
```makefile
COD = \
    coders.c \
    parsing.c \
    init.c \
    clean.c \
    error.c \
    utils.c \
    threads.c \
    logs.c \
    cod_routine.c \
    my_sleep.c \
    burn_monitor.c  # ← Ajouter cette ligne
```

---

### 11. **Format du burnout log incorrect** ⚠️
**Sévérité: BASSE**

**Code actuel**:
```c
log_message(&data->coder_list[i], "burnout");  // Pas "burned out"
```

**Sujet demande**:
```
X burned out  // Pas "burnout"
```

**Piste de correction**:
```c
log_message(&data->coder_list[i], "burned out");
```

---

## 🔄 Plan d'Amélioration pour V3

### ⭐ IMPORTANT: Fonctions Autorisées vs Non-Autorisées

**Fonctions AUTORISÉES** (à utiliser):
```
✓ pthread_create, pthread_join
✓ pthread_mutex_init, pthread_mutex_lock, pthread_mutex_unlock, pthread_mutex_destroy
✓ pthread_cond_init, pthread_cond_wait, pthread_cond_timedwait, pthread_cond_broadcast, pthread_cond_destroy
✓ gettimeofday, usleep
✓ write, malloc, free, printf, fprintf
✓ strcmp, strlen, atoi, memset
```

**Fonctions INTERDITES** (à éviter absolument):
```
✗ pthread_mutex_timedlock() ← N'EXISTE PAS dans la liste autorisée!
✗ sleep(), nanosleep() ← Utiliser usleep() à la place
✗ Libft ← Non autorisée
```

**Impact sur l'architecture**:
- Pas de timeout direct sur mutex
- Utiliser `pthread_cond_timedwait()` + `pthread_cond_broadcast()` pour les délais
- Boucle: lock → wait_cond → check → unlock
- Réveil: lock → change state → broadcast → unlock

---

### Phase 1: Corrections Critiques (1h)
1. ✓ Corriger `data->scheduler = false` → `data->start_simulation = false`
2. ✓ Fixer le calcul de timestamp dans le monitor (secondes → ms)
3. ✓ Ajouter la mise à jour de `last_compile` avant chaque compile
4. ✓ Ajouter `burn_monitor.c` au Makefile
5. ✓ Corriger le format du log "burned out"

### Phase 2: Implémentation des Stubs (2-3h)
1. ✓ Implémenter `left_or_right_handed()` correctement
2. ✓ Implémenter l'acquisition de dongle avec `pthread_cond_timedwait()`
3. ✓ Implémenter `compile()` complètement
4. ✓ Créer le thread monitor dans `threads.c`
5. ✓ Arrêter correctement la boucle du monitor

### Phase 3: Cooldown et Planification (3h)
1. ✓ Utiliser le champ `cooldown_until` des dongles
2. ✓ Vérifier disponibilité avant d'accorder un dongle
3. ✓ Implémenter FIFO simple d'abord
4. ✓ Queue/heap pour EDF

### Phase 4: Tests et Polissage (1h)
1. ✓ Compiler sans warnings
2. ✓ Tester avec des paramètres simples
3. ✓ Valgrind pour les fuites mémoire

---

## 📊 Comparaison V1 vs V2

| Critère | V1 | V2 | Progression |
|---------|----|----|-------------|
| Architecture | 15/15 | 16/15 | ⭐ +1 |
| Parsing | 8/8 | 8/8 | → |
| Gestion mémoire | 8/8 | 8/8 | → |
| Logging sincronisé | 6/8 | 6/8 | → |
| Topologie dongles | 5/5 | 5/5 | → |
| **Gestion burnout** | **0/15** | **3/15** | ⭐⭐⭐ +3 |
| **Planification** | **0/15** | **0/15** | → |
| **Cooldown** | **0/8** | **1/8** | ⭐ +1 |
| **Thread safety** | **2/10** | **4/10** | ⭐ +2 |
| Tests/robustesse | 0/5 | 0/5 | → |
| **TOTAL** | **42/100** | **48/100** | ⭐⭐⭐⭐⭐⭐ +6 |

---

## 📊 Notation Détaillée V2

| Critère | Points | Raison |
|---------|--------|--------|
| Architecture et organisation | 16/15 | Bien structuré + séparation monitor |
| Parsing et validation | 8/8 | Correct |
| Gestion mémoire | 8/8 | Pas de fuites |
| Logging synchronisé | 6/8 | Toujours incomplet |
| Topologie dongles | 5/5 | Correct |
| Gestion burnout | 3/15 | Monitor exists but broken |
| Planification FIFO/EDF | 0/15 | Pas implémenté |
| Cooldown dongles | 1/8 | Infrastructure, pas d'utilisation |
| Thread safety global | 4/10 | Meilleur que V1 |
| Tests et robustesse | 0/5 | Non testé |
| Bonus/Polish | -2/5 | Code incomplet |
| **TOTAL** | **48/100** | |

---

## 🎯 Checklist pour V3

- [ ] Compiler sans erreurs
- [ ] `start_simulation` utilisé correctement
- [ ] Timestamps en millisecondes (pas secondes)
- [ ] `last_compile` mis à jour avant chaque compile
- [ ] Thread monitor créé et joiné
- [ ] Fonction `compile()` acquiert les dongles
- [ ] Logs "X has taken a dongle" présents
- [ ] Logs "X burned out" au bon moment
- [ ] Pas de boucle infinie dans le monitor
- [ ] Pas de busy-waiting

---

---

## 💡 Architecture Correcte avec Condition Variables

Puisque `pthread_mutex_timedlock()` n'est pas autorisé, voici la bonne approche:

### Structure modifiée de dongle:
```c
typedef struct s_dongle
{
    int                 id;
    pthread_mutex_t     lock;           // Mutex principal
    pthread_cond_t      available;      // Condition variable (autorisée!)
    uint64_t            cooldown_until; // Quand il sera vraiment libre
    int                 holder_id;      // Quel coder le tient (-1 = libre)
} t_dongle;
```

### Acquisition avec timeout:
```c
bool acquire_dongle(t_coder *coder, t_dongle *dongle, uint64_t deadline_ms)
{
    struct timespec ts_deadline;
    struct timeval tv_now;
    uint64_t now_ms;
    
    // Convertir deadline_ms en struct timespec absolu
    ts_deadline.tv_sec = deadline_ms / 1000;
    ts_deadline.tv_nsec = (deadline_ms % 1000) * 1000000;
    
    pthread_mutex_lock(&dongle->lock);
    
    // Boucle d'attente jusqu'au deadline
    while (1)
    {
        gettimeofday(&tv_now, NULL);
        now_ms = ((uint64_t)tv_now.tv_sec * 1000) + (tv_now.tv_usec / 1000);
        
        // Vérifier: libre ET cooldown écoulé
        if (dongle->holder_id == -1 && now_ms >= dongle->cooldown_until)
        {
            dongle->holder_id = coder->id;
            log_message(coder, "has taken a dongle");
            pthread_mutex_unlock(&dongle->lock);
            return (true);
        }
        
        // Attendre signal ou timeout
        if (pthread_cond_timedwait(&dongle->available, 
            &dongle->lock, &ts_deadline) == ETIMEDOUT)
        {
            pthread_mutex_unlock(&dongle->lock);
            return (false);  // Deadline dépassée
        }
    }
}
```

### Libération:
```c
void release_dongle(t_dongle *dongle, uint64_t cooldown_ms)
{
    struct timeval tv;
    uint64_t now_ms;
    
    pthread_mutex_lock(&dongle->lock);
    
    gettimeofday(&tv, NULL);
    now_ms = ((uint64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    dongle->cooldown_until = now_ms + cooldown_ms;
    dongle->holder_id = -1;
    
    pthread_cond_broadcast(&dongle->available);  // Réveiller tous les attendants
    
    pthread_mutex_unlock(&dongle->lock);
}
```

### Cycle complet dans coder_routine():
```c
void *coder_routine(void *arg)
{
    t_coder *coder = (t_coder *)arg;
    struct timeval tv_last_compile;
    uint64_t deadline_ms;
    int i = 0;
    
    while (i < coder->data->nb_compile && coder->data->start_simulation)
    {
        // Calculer deadline = maintenant + time_to_burnout
        gettimeofday(&tv_last_compile, NULL);
        uint64_t now_ms = ((uint64_t)tv_last_compile.tv_sec * 1000) + 
                          (tv_last_compile.tv_usec / 1000);
        deadline_ms = now_ms + coder->data->time_burnout;
        
        // Acquérir dongle gauche
        if (!acquire_dongle(coder, coder->left_dongle, deadline_ms))
        {
            log_message(coder, "burned out");
            break;
        }
        
        // Acquérir dongle droit
        if (!acquire_dongle(coder, coder->right_dongle, deadline_ms))
        {
            release_dongle(coder->left_dongle, coder->data->dongle_cooldown);
            log_message(coder, "burned out");
            break;
        }
        
        // Compilation
        log_message(coder, "is compiling");
        mysleep(coder->data->time_compile);
        
        // Libérer les dongles
        release_dongle(coder->left_dongle, coder->data->dongle_cooldown);
        release_dongle(coder->right_dongle, coder->data->dongle_cooldown);
        
        // Mettre à jour last_compile
        gettimeofday(&tv_last_compile, NULL);
        coder->last_compile = ((uint64_t)tv_last_compile.tv_sec * 1000) + 
                              (tv_last_compile.tv_usec / 1000);
        
        // Debug et refactor
        log_message(coder, "is debugging");
        mysleep(coder->data->time_debug);
        
        log_message(coder, "is refactoring");
        mysleep(coder->data->time_refactor);
        
        i++;
    }
    
    return (NULL);
}
```

Cette approche:
- ✅ Utilise UNIQUEMENT les fonctions autorisées
- ✅ Gère le timeout correctement avec `pthread_cond_timedwait()`
- ✅ Permet le cooldown sans busy-waiting
- ✅ Supporte FIFO (broadcast = ordre FIFO par défaut)
- ✅ Peut être étendu pour EDF avec une queue prioritaire

---

## 💡 Observations Générales

**Tendance positive**: Vous explorez les bonnes directions (monitor thread, tracking du temps, stubs structurés).

**À accélérer**: Il faut maintenant implémenter les fonctions qui sont actuellement des stubs. C'est la phase de vraie implémentation qui commence.

**Point critique**: Comprendre que sans `pthread_mutex_timedlock()`, il faut utiliser condition variables avec un pattern spécifique (lock → wait_cond → check → unlock).

**Effort estimé**: 4-5h de travail sérieux pour arriver à ~75-80/100.

---

**Vous êtes sur la bonne voie! 🚀 Continuez l'implémentation!**
