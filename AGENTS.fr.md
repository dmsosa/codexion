# AGENTS.md — Règles du projet Codexion (42)

> **Source :** `es.subject.pdf` (v1.0).
> Ces contraintes sont **obligatoires et permanentes**.
> Elles doivent être respectées dans chaque modification et dans chaque fichier du projet. En cas de conflit avec une autre instruction, ces règles sont prioritaires.

## 0. Mode de travail : apprentissage

- L'utilisateur est en phase d'apprentissage. Il faut expliquer le *pourquoi* des décisions techniques, guider avec des questions et le laisser écrire lui-même le code.
- Compétences pédagogiques de référence : `teaching-style`, `socratic-tutor`, `socrates`.
- Ne pas fournir de code de production que l'utilisateur ne comprend pas.
- Avant de proposer une solution, expliquer le raisonnement qui la justifie.
- Ces règles sont prioritaires sur toute considération de rapidité ou de commodité.

## 1. Norme de 42 (Norminette)

- Langage obligatoire : **C**.
- Le code doit respecter la **Norminette** (`norminette`).
- Une seule erreur de norme dans n'importe quel fichier, y compris les fichiers supplémentaires, entraîne une note de 0.
- Aucun comportement anormal ne doit se produire : segmentation fault, bus error, double free, etc. (hors comportement indéfini).
- Aucune fuite mémoire n'est autorisée : chaque allocation avec `malloc` doit être suivie d'une libération appropriée avec `free`.
- Les fichiers à rendre dans le dossier `coders/` sont :
  - `Makefile`
  - fichiers `*.c`
  - fichiers `*.h`
- Nom de l'exécutable : `codexion`.

Référence de la Norminette :  
https://github.com/42School/norminette/blob/master/pdf/en.norm.pdf

## 2. Compilation et Makefile

- La compilation doit utiliser :

```bash
cc -Wall -Wextra -Werror -pthread
```

- Le Makefile ne doit pas effectuer de relink inutile.
- Les règles minimales obligatoires sont :
  - `$(NAME)`
  - `all`
  - `clean`
  - `fclean`
  - `re`

## 3. Fonctions autorisées et interdictions

- **Variables globales interdites.**
- **Libft interdite.**
- Seules les fonctions suivantes sont autorisées :

```c
pthread_create
pthread_join
pthread_mutex_init
pthread_mutex_lock
pthread_mutex_unlock
pthread_mutex_destroy
pthread_cond_init
pthread_cond_wait
pthread_cond_timedwait
pthread_cond_broadcast
pthread_cond_destroy
gettimeofday
usleep
write
malloc
free
printf
fprintf
strcmp
strlen
atoi
memset
```

Aucune autre fonction ne doit être utilisée sans vérification préalable des exigences du sujet.

## 4. Arguments du programme

Tous les arguments suivants sont obligatoires :

```text
number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Le programme doit valider les arguments et rejeter les entrées invalides, notamment :

- Les valeurs négatives.
- Les valeurs qui ne sont pas des entiers valides.
- Une valeur de `scheduler` différente de `fifo` ou `edf`.

## 5. Modèle de simulation

- Un coder correspond à un thread (`pthread_create`).
- Pour `n` coders, il faut `n` dongles.
- Cas particulier : s'il n'y a qu'un seul coder, un seul dongle est disponible.
- Chaque coder suit le cycle suivant :

  1. **Compile** : nécessite deux dongles, celui de gauche et celui de droite.
  2. **Debug**.
  3. **Refactor**.
  4. Tente de compiler à nouveau.

### Cooldown des dongles

- Le cooldown est obligatoire.
- Après la libération d'un dongle, celui-ci ne peut pas être récupéré avant l'expiration de `dongle_cooldown` millisecondes.

### Équité de l'ordonnancement

L'arbitrage doit respecter le scheduler sélectionné :

- **FIFO** (*First In, First Out*) : la demande arrivée en premier est traitée en premier.
- **EDF** (*Earliest Deadline First*) : la demande dont le deadline de burnout est le plus proche est prioritaire.

Pour EDF :

```text
deadline = last_compile_start + time_to_burnout
```

Avec EDF, aucun coder ne doit s'épuiser si les paramètres de simulation permettent d'éviter ce résultat.

### Surveillance du burnout

- Un thread monitor dédié doit surveiller les coders et détecter les burnouts avec précision.
- La simulation doit s'arrêter dans l'un des deux cas suivants :
  1. Un coder s'épuise.
  2. Tous les coders ont atteint au moins `number_of_compiles_required` compilations.

### Structure de données pour l'ordonnancement

- Implémenter une file de priorité (*priority queue*) à l'aide d'un **heap** développé dans le projet.
- Cette structure doit permettre de gérer les priorités FIFO et EDF.
- Ne pas supposer qu'une file de priorité est fournie directement par le langage C.

## 6. Logs : format exact et synchronisation

Les messages doivent respecter exactement le format suivant :

```text
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Contraintes :

- Deux messages ne doivent jamais être mélangés sur une même ligne.
- L'accès à la sortie doit être protégé par un mutex.
- Le message de burnout doit apparaître dans les 10 ms suivant le moment réel du burnout.
- La mesure du temps doit utiliser `gettimeofday()` (horloge temps réel, solution acceptable et recommandée pour sa simplicité).

## 7. README.md : à la racine du dépôt

La première ligne doit être en italique et respecter exactement le modèle suivant :

```markdown
*Ce projet a été réalisé dans le cadre du cursus de 42 par <login1>[, <login2>[...]].*
```

### Sections obligatoires

Le README doit contenir les sections suivantes :

- **Description**
- **Instructions**
- **Ressources**

La section **Ressources** doit notamment préciser l'utilisation de l'IA :
- Pour quelles tâches elle a été utilisée.
- Dans quelles parties du projet elle a été utilisée.

### Sections supplémentaires obligatoires

#### Blocking cases handled

Décrire les problèmes de concurrence pris en charge et les solutions mises en œuvre, notamment :

- Les interblocages (*deadlocks*) et les conditions de Coffman.
- La prévention de la famine (*starvation*).
- La gestion du cooldown des dongles.
- La détection précise du burnout.
- La sérialisation des logs.

#### Thread synchronization mechanisms

Décrire les mécanismes de synchronisation utilisés, notamment :

- `pthread_mutex_t`
- `pthread_cond_t`
- Les événements ou mécanismes de synchronisation développés dans le projet.

Expliquer comment ils coordonnent l'accès aux ressources partagées :

- Les dongles.
- Les logs.
- L'état du thread monitor.

Fournir des exemples concrets montrant comment les conditions de course (*race conditions*) sont évitées.

## 8. Vérifications finales

Avant de considérer une modification comme terminée, vérifier systématiquement :

- Le respect de la Norminette.
- L'absence de fuites mémoire.
- L'absence de comportements anormaux.
- La compilation avec `-Wall -Wextra -Werror -pthread`.
- L'utilisation exclusive des fonctions autorisées.
- Le respect exact du format des logs.
- Le respect des règles de synchronisation et d'accès aux ressources partagées.

**En cas d'ambiguïté dans le sujet, demander une clarification à l'utilisateur avant de prendre une décision ou de faire une hypothèse.**