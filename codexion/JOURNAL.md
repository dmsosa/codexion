*Bonjour, mon nom est Durian, et je fais de mon mieux pour vous aider à bien faire la Codexion.*

> Ce journal, c'est **mes mots, mes doutes et mes erreurs**. J'ai seulement corrigé
> l'orthographe et rangé le contenu en chapitres avec des tableaux, pour qu'un débutant
> puisse suivre dans l'ordre. Tout ce qui est écrit ici, je l'ai compris en le faisant.

---

# Chapitre 0 — Les sources pour commencer

Pour commencer, je recommande ces bonnes sources pour lire une introduction au problème
et comprendre ce qu'il est nécessaire pour le faire :

| # | Source | Ce qu'on y trouve |
|---|---|---|
| 1 | [Dining Philosophers — OpenCSF](https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/DiningPhil.html) | le problème, les 4 conditions de Coffman, 3 solutions |
| 2 | [es.subject.pdf](https://cdn.intra.42.fr/pdf/pdf/198298/es.subject.pdf) | le sujet officiel (à télécharger depuis intra) |
| 3 | [es.subject.pdf (2)](https://cdn.intra.42.fr/pdf/pdf/203872/es.subject.pdf) | une autre version du sujet |
| 4 | [VicProgram/42 — Codexion](https://github.com/VicProgram/42/blob/master/Codexion/coders/tools.c) | une implémentation complète à lire (pas à recopier) |

---

# Chapitre 1 — Où on en est

## Madrid, 30 octobre

On doit faire le projet de Codexion, pour ça on a commencé par bien comprendre le sujet
sur les threads. Si tu es nouveau, je recommande :

1. Lire le sujet (`es.subject.pdf`).
2. Te demander comment tu créerais les coders et les dongles : qu'est-ce qu'ils sont, et
   quelle est la relation entre eux ?
3. Comprendre ce que veut dire « prendre un dongle » (chapitre 3).

## Où on en est, le 2 novembre

| Brique | État | Détail |
|---|---|---|
| `Makefile` | ✅ | compile avec `-Wall -Wextra -Werror -pthread`, ne relinke pas |
| Structures (`t_monitor`, `t_coder`, `t_dongle`) | ✅ | les bonnes idées sont là |
| Parser | ❌ | refuse les entrées **valides** (bug de polarité) |
| Initialisation | ⚠️ | appelée, mais une condition variable n'est pas initialisée |
| Threads coder | ❌ | jamais atteints : le parser fait sortir avant |
| Prise / rendu des dongles | ❌ | le même dongle est pris deux fois |
| Tas (heap) | ⚠️ | 4 bugs dans `sift_down` et les clés |
| Monitor / burnout | ❌ | la deadline est calculée avec un `start_time` jamais rempli |
| Arrêt et nettoyage | ⚠️ | quelques chemins oublient de libérer |
| Tests | ✅ | `tests/run_tests.sh` (voir chapitre 6) |

---

# Chapitre 2 — Le problème de Dijkstra

## 2.1 Le problème d'origine

5 philosophes, 5 fourchettes, un plat. Pour manger il faut **deux** fourchettes. Chacun
prend la gauche, puis la droite. Si les 5 sont affamés en même temps : chacun tient sa
gauche et attend une droite que son voisin tient → personne ne mange, jamais. C'est un
**interblocage (deadlock)**.

La théorie (Coffman) dit qu'un deadlock exige **les 4 conditions en même temps** :

| Condition | Sens | Codexion |
|---|---|---|
| Exclusion mutuelle | un dongle = un seul coder | ✅ inévitable |
| Pas de préemption | seul le propriétaire libère | ✅ inévitable |
| **Hold and wait** | je garde A en attendant B | ⚠️ c'est ça qu'il faut casser |
| **Attente circulaire** | A attend B, B attend A… en rond | ⚠️ c'est ça qu'il faut casser |

**Une seule condition cassée suffit** : plus de deadlock. Les trois solutions du cours :

| Solution | Principe | Pourquoi pas ici |
|---|---|---|
| Limiter les accès | un sémaphore « multiplexeur » | impossible : tout le monde doit pouvoir compiler |
| Casser hold-and-wait | `trylock` + relâcher | ça peut **affamer** (starvation), et le sujet impose FIFO/EDF |
| **Ordre total** | toujours prendre le dongle de plus petit index d'abord | ✅ simple, correct, c'est la base de Codexion |

## 2.2 Ce que Codexion ajoute (et qui change tout)

| Philosophe | Codexion |
|---|---|
| fourchette | **dongle** |
| manger | **compiler** (2 dongles) |
| penser | **debug** puis **refactor** |
| mourir de faim | **burnout** (deadline) |

Trois contraintes en plus du problème de 1965 :

| Contrainte | Ce que ça veut dire | Piège |
|---|---|---|
| **Cooldown** | un dongle relâché est inutilisable pendant `dongle_cooldown` ms | il faut regarder l'heure, pas juste « libre / occupé » |
| **Deadline** | un coder doit **commencer** une compilation au plus tard à `last_compile_start + time_to_burnout` | le monitor doit l'écrire en **moins de 10 ms** |
| **Arbitrage** | quand plusieurs attendent, il faut décider dans un ordre (`fifo` = arrivée, `edf` = deadline la plus proche) | sans ordre → **starvation** : un coder est doublé à l'infini et brûle alors que les paramètres étaient viables |

👉 Conséquence importante : en Codexion, **un deadlock se voit dans les logs** (tout le
monde brûle). « Ça ne segfault pas » ne veut pas dire « c'est correct ».

---

# Chapitre 3 — Qu'est-ce que c'est, prendre un dongle ?

Pour prendre un dongle, on fait un changement dans l'état du dongle, avec un changement
dans l'attribut `used`, qui doit être vrai.

Pour faire ça, il faut **verrouiller le mutex du dongle** pour s'assurer que seulement un
coder à la fois lit l'état actuel du dongle :

| Champ du dongle | Pourquoi il doit être protégé |
|---|---|
| `is_used` / `owner` | savoir s'il est libre, et **à qui** il est |
| `queue` | l'ordre d'attente (le tas) |
| `last_used` / `cooldown_until` | savoir quand il redeviendra disponible |
| `cond` | réveiller ceux qui attendent |

## 3.1 Et les coders ?

Similairement, quand on vérifie l'état des coders, il faut s'assurer que les propriétés
suivantes ne sont vérifiées que par **un coder à la fois** :

| Propriété | Qui l'écrit | Qui la lit |
|---|---|---|
| `compile_count` | le coder | le monitor |
| `last_compile + burnout_time` | le coder | le monitor (deadline) et le tas (EDF) |
| `is_burned_out` | le monitor | tout le monde |

De cette façon, on s'assure qu'un autre fil n'a pas changé les données d'un autre coder
pendant qu'on lisait les propriétés mentionnées avant.

> ⚠️ Leçon du 2 novembre : dans mon code, `last_compile` et `compile_count` sont lus par
> le monitor **sans mutex**. Ça « marche » sur x86, mais c'est une **data race** (comportement
> indéfini). C'est le genre de bug que ThreadSanitizer attrape et pas le compilateur.

## 3.2 Comment on peut organiser ça ?

Le monitor a les responsabilités suivantes :

| Responsabilité | Détail |
|---|---|
| Démarrer / arrêter la simulation | `running` sous `run_mutex` |
| Détecter le burnout | comparer `now` à la deadline de chaque coder |
| Détecter la fin normale | tous les coders ont `compile_count == req_compile` |
| Réveiller les dormeurs | `pthread_cond_broadcast` à l'arrêt |

Et la règle d'or, celle qui m'a coûté le plus de temps :

> Le mutex ne protège pas le dongle, il protège **les variables** qui décrivent le dongle.
> On ne tient **jamais** le mutex pendant `usleep`, et on ne « possède » pas un dongle avec
> un verrou : on le possède avec un champ (`owner`).

---

# Chapitre 4 — Le tas (heap) par dongle

On peut organiser le heap de chaque dongle, car il faut savoir l'ordre dans lequel les
dongles arrivent.

Donc, quand on n'est pas le premier coder à essayer de prendre un dongle, on est poussé
dans le heap, et pour rendre le tri plus simple, on réordonne l'arbre avec `sift_up` à
partir du nœud qui vient d'être poussé. De cette manière, on s'assure que les parents de
ce nœud sont plus petits que lui (tas min).

| Opération | Ce qu'elle fait | Coût |
|---|---|---|
| `queue_push` | ajoute à la fin puis `sift_up` | O(log n) |
| `queue_pop` | prend la racine puis `sift_down` | O(log n) |
| `sch_compare` | compare deux requêtes (FIFO = `seq`, EDF = deadline) | O(1) |

**Ce qui doit être vrai tout le temps :**

| Invariant | Pourquoi |
|---|---|
| `heap[0]` est toujours le prochain servi | c'est ça qui donne FIFO/EDF et évite la famine |
| la clé est **figée au `push`** | si elle change, l'invariant du tas est cassé |
| `heap_size ≤ n_coders` | un coder a au plus une requête par dongle |
| on ne lit jamais `heap[0]` si `heap_size == 0` | sinon on lit de la mémoire au hasard |

---

# Chapitre 5 — La solution de VicProgram

En comparaison, la solution que j'ai vue de la part de VicProgram, que je recommande
beaucoup : [tools.c](https://github.com/VicProgram/42/blob/master/Codexion/coders/tools.c)

Il utilise un mutex et une cond pour chaque dongle, qui permettent de savoir que tous les
autres coders sont en attente pour un dongle, s'il est libre. Aussi, il faut vérifier si
le cooldown est prêt, donc il y a du sens à avoir aussi une condition pour chaque mutex.
De cette façon, tous les threads qui sont en attente pour ce dongle peuvent continuer avec
leurs tâches.

## 5.1 Mes trois questions

| Question | Réponse courte |
|---|---|
| Pourquoi vérifier la simulation **dans** l'attente ? | Parce qu'un `cond_wait` peut dormir pour toujours, et parce que `stop` doit faire partie du **prédicat** d'attente. Sinon on perd un réveil (« lost wakeup »). La version propre : un seul verrou + une seule cond pour toute la simulation, et `while (!stop && !prêt) wait`. |
| Est-ce que pousser une requête dans le tas marche sans allouer une nouvelle requête ? | Oui : `heap_push` copie la requête **par valeur**. Préallouer `n` places par dongle est même mieux (pas de `malloc` sous verrou). Mais il faut : capacité suffisante, ne jamais lire un tas vide, refuser un `push` plein, et **figer la clé**. |
| Une façon plus élégante de vérifier l'arrêt après avoir pris un dongle ? | Ne pas créer cet état : rendre l'acquisition **explicite** (`take_dongle` renvoie 1/0, `acquire_dongles` renvoie 0/1/2) et ne relâcher que ce qu'on possède. À l'arrêt, on ne relâche rien : la simulation est finie, `cleanup` s'occupe de tout après les `join`. |

## 5.2 Ce qu'on garde, ce qu'on corrige

| On garde (c'est juste) | On corrige (bug réel) |
|---|---|
| mutex + cond par dongle | `stop` hors du domaine du prédicat → lost wakeup |
| acquisition ordonnée | `release_both` après arrêt libère des dongles non pris |
| requête par valeur, clé figée | **double unlock** dans les chemins d'arrêt |
| tas préalloué, pas de `malloc` sous verrou | `heap[0]` lu sans vérifier `size > 0` |
| `heap[0]` = prochain servi | `heap_push` silencieux quand c'est plein |
| monitor qui poll à 1 ms | champs du coder lus sans mutex (data race) |

---

# Chapitre 6 — Les tests (3 novembre)

J'ai écrit une suite de tests dans `tests/` :

```sh
./tests/run_tests.sh            # tout
./tests/run_tests.sh --static   # compilation + norme + fonctions
```

Elle vérifie : la structure (`coders/`, `Makefile`), la compilation, la norme, les
fonctions autorisées, 5 configurations viables, la précision du burnout, 12 arguments
invalides, 5 répétitions, `valgrind` et ThreadSanitizer.

## 6.1 Résultat du dernier passage

```
12 PASS   16 FAIL   1 WARN   0 SKIP
```

Le parsing passe enfin (`check_scheduler` renvoie `1`) et `init_monitor` est appelée :
le SIGSEGV systématique a disparu. Mais le programme **ne finit jamais normalement**.
Sur 20 exécutions de `./codexion 4 1000 60 60 60 2 10 fifo` :

| Mode | Fréquence | Ce qu'on voit |
|---|---|---|
| burnout prématuré | 12/20 | un seul coder tourne, `1000 X burned out`, code 0 |
| blocage | 8/20 | aucune sortie, `timeout` : le `join` ne rend jamais la main |
| fin normale | 0/20 | — |

## 6.2 Pourquoi les tests fonctionnels échouent encore

| Test | Cause |
|---|---|
| 5 configs viables | blocage : un coder attend un dongle que personne ne relâche |
| burnout | blocage : la simulation ne s'arrête pas |
| 5 répétitions | 0/5 exécutions correctes |
| sources à la racine | `AGENTS.md §1` : la livraison doit être dans `coders/` |
| `EXAMPLE.c` | vestige de VicProgram : ne compile pas (et 5 erreurs de norme) |
| norminette : 19 erreurs | `queue.c` 7, `EXAMPLE.c` 5, `main.c` 3, `codexion.h` 2, `print_log.c` 1, `coder.c` 1 |
| fonctions interdites | `exit()` n'est pas dans la liste blanche (3 occurrences) |
| `coders = 0` | accepté : aucune validation de plage |
| 9 arguments / temps négatif | acceptés puis bloqués |

## 6.3 La leçon du jour : un seul run ne prouve rien

Le **même binaire** alterne entre burnout et blocage selon l'ordonnancement :

| Ce que je croyais | Ce que le test répété montre |
|---|---|
| « ça marche, j'ai vu les logs » | 1 run sur 20 seulement va au bout, et encore : au mauvais endroit |
| « ça crashe » | le crash a disparu quand `queue_max` a été rempli ; c'était donc **une** cause parmi d'autres |

C'est pour ça que la suite borne chaque exécution par un `timeout` et répète 5 fois.

## 6.4 Les bugs bloquants restants

| # | Fichier:ligne | Problème |
|---|---|---|
| 1 | `coder.c:25-28` | **il manque un `;`** : le corps du `while (!running)` est le `while (running)` qui suit → la plupart des coders ne travaillent jamais, et ceux qui entrent bouclent après l'arrêt |
| 2 | `coder.c:32,36` | `unlock(&run_mutex)` sans l'avoir verrouillé (UB) |
| 3 | `coder.c` | les dongles ne sont jamais relâchés après `refactor` |
| 4 | `dongle.c:71-72` | `take_dongle(coder, d0)` deux fois ; `d1` jamais pris |
| 5 | `dongle.c:66-70` | l'ordre est inversé : on prend le plus grand id d'abord |
| 6 | `init.c:84-94` | `pthread_cond_init` jamais appelé pour `d->cond` |
| 7 | `monitor.c:67` | à l'arrêt, on ne réveille pas les `d->cond` → **hang** |
| 8 | `dongle.c:17,26,77` | `queue[0].coder->id` sans vérifier `queue_idx > 0` |
| 9 | `dongle.c:21/31` + `53` | double unlock |
| 10 | `monitor.c:43` | `last_compile + start_time` : deux dates absolues additionnées |
| 11 | `binary_tree.c:103,116` | `r_child = i * 2 + 1` au lieu de `i * 2 + 2` |
| 12 | `binary_tree.c:112-114` | `temp` n'est pas réécrit après l'échange |
| 13 | `binary_tree.c:78` | clé EDF recalculée en direct → tas incohérent |
| 14 | `time.c:23-34` | `pthread_cond_timedwait` attend une date absolue, on lui donne une durée |

Le détail complet (avec `file:ligne`) est dans `tests/README.md`.

---

# Chapitre 7 — La prochaine étape

L'ordre que je vais suivre :

1. **Le `;` manquant** (`coder.c:25`) : un caractère, et les threads se mettent à tourner.
2. **Prendre `d1`** et dans le bon ordre, puis **relâcher** après `refactor`.
3. **`pthread_cond_init`** des dongles, et **broadcast** des `d->cond` à l'arrêt : c'est
   le blocage du `join`.
4. **Un seul `unlock` par chemin**, et `queue_idx > 0` avant `queue[0]`.
5. Corriger la deadline du monitor et le tas (`r_child`, la copie de `temp`, la clé figée,
   un compteur `seq`, une date absolue pour le cooldown).
6. Norme, `EXAMPLE.c`, `coders/`, `.gitignore`.

## Les 5 règles que je garde

| # | Règle |
|---|---|
| 1 | Un seul domaine de verrou pour `stop` et les prédicats d'attente |
| 2 | Toujours `while (!stop && !prêt) wait`, jamais `if` |
| 3 | La clé d'une requête est figée au `push` |
| 4 | Un mutex protège des données, jamais un `usleep` |
| 5 | Seul le propriétaire (`owner == moi`) libère un dongle |

---

*Journal tenu par Durian. Les mots sont les miens ; les tableaux sont là pour ranger.*
