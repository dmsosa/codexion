# tests/ — suite de tests de Codexion

Cette suite répond à une seule question : **est-ce que le programme respecte le sujet,
les règles de `AGENTS.md`, et est-ce qu'il survit à la concurrence ?**

Elle ne modifie aucun fichier du projet. Tout ce qu'elle produit va dans
`/tmp/opencode/codexion-tests` (surchargeable avec `CODEXION_TEST_TMP`).

## Lancer

```sh
./tests/run_tests.sh            # tout
./tests/run_tests.sh --static   # compilation + norme + fonctions (rapide)
CC=clang ./tests/run_tests.sh   # autre compilateur
```

Codes de sortie : `0` si tout passe, `1` dès qu'un test échoue.
Chaque ligne est `PASS`, `FAIL`, `WARN` (suspect mais pas certain) ou `SKIP`
(outil absent / pas de binaire).

## Ce que la suite vérifie

| # | Test | Référence |
|---|---|---|
| 1.0 | les sources sont dans `coders/` | AGENTS §1 |
| 1.1 | `Makefile` présent | AGENTS §2 |
| 1.2 | `make` construit `codexion` | AGENTS §2 |
| 1.2b | un 2e `make` ne relinke pas | AGENTS §2 |
| 1.3 | **chaque** `.c` compile seul avec `-Wall -Wextra -Werror -pthread` | AGENTS §1 |
| 1.4 | aucun `.c` parasite hors du `Makefile` | AGENTS §1 |
| 2 | `norminette` sans erreur | AGENTS §1 |
| 3 | aucune fonction hors liste blanche | AGENTS §3 |
| 4.1 | configs viables : tout le monde compile, zéro burnout | sujet |
| 4.2 | burnout loggé dans la fenêtre attendue (±10 ms) | AGENTS §6 |
| 4.3 | arguments invalides rejetés proprement (pas de crash) | AGENTS §4 |
| 4.4 | 5 répétitions identiques (courses intermittentes) | — |
| 5 | `valgrind` : pas de fuite / accès invalide | AGENTS §1 |
| 5b | `ThreadSanitizer` : pas de data race | — |

### Ce que la suite ne peut PAS voir (boîte noire)

- **l'ordre FIFO/EDF** : les logs ne disent pas quel dongle est pris. Pour le vérifier
  il faut instrumenter le binaire (trace interne) ou relire le code.
- **le cooldown** : même raison, on ne sait pas quel dongle est relâché.
- **la famine sur longue durée** : il faudrait des milliers de cycles ; ici on fait 3–5
  passes. `t_soak` est là pour être allongé quand le reste passera.

## Dernier passage — 2026-11-03

> **État à 15:35** : le build repasse et la validation est en place.

```
14 PASS   14 FAIL   1 WARN   0 SKIP
```

Corrigé depuis le 02/11 : parsing, `init_monitor`, `queue_max`, validation des entiers,
`d0`/`d1` dans le bon ordre, `release_both` après le cycle, `cond_init` des dongles,
barrière de démarrage. La suite **va au bout** (plus de blocage du harnais).

Ce qui bloque encore : les 5 configs viables se figent. Le log (relancé avec
`stdbuf -o0`, sinon le buffer stdout est perdu quand le process est tué) montre
**les 4 coders qui compilent en même temps à 0 ms** avec 4 dongles : impossible.

| Cause | Effet |
|---|---|
| `shift_up` (`binary_tree.c:53`) : `if (!sch_compare(...)) break;` alors que `sch_compare` ne renvoie **jamais 0** | tout nouveau nœud monte à la racine → chacun se croit premier et prend le dongle |
| `wait_dongle` ne vérifie que « suis-je la tête ? » | il ne regarde pas `d->owner` : deux coders peuvent « tenir » le même dongle |
| `release_dongle` ne libère que si `queue[0] == c` | si la tête a changé, le dongle reste occupé → famine puis blocage |
| `queue_max = n` et 2 `push` par cycle | la file déborde, `queue_push` ignore en silence → attente éternelle |

Deux échecs viennent d'un **désaccord test/validation** : `check_values` refuse
`cooldown = 0` (`< 1`), alors que les configs « 12 coders » et « burnout » utilisent 0.
À trancher avec le sujet (0 = pas de cooldown est raisonnable).

> 🐛 **Bug du harnais corrigé le 03/11** : `log_format_ok` attendait `^[0-9]+ (has taken…)`
> alors que le format est `timestamp id message` (`^[0-9]+ [0-9]+ …`). Le test de burnout
> échouait donc à tort ; avec la regex corrigée, le burnout est loggé à **100 ms** (attendu
> 100 ±10) et le format est bon. Morale : un test aussi peut mentir — toujours vérifier
> pourquoi il échoue avant de « corriger » le code.

> ⚠️ **Un seul run ne prouve rien** : le même binaire alterne entre burnout, blocage et
> sortie normale selon l'ordonnancement. C'est pour ça que `t_soak` fait 5 passes et que
> la suite borne chaque exécution par un `timeout`.

Le harnais : `valgrind` a un `timeout 30`, et ThreadSanitizer compile la liste `CFILES`
du `Makefile` (au lieu de tous les `.c`, ce qui échouait à cause de `EXAMPLE.c`).

---

# Échecs trouvés (par gravité)

## Corrigés depuis le 02/11

| # | Fichier | Correction |
|---|---|---|
| A1 | `main.c` | `init_monitor()` est appelée **et son retour testé** |
| A2 | `parser.c:18-20` | `check_scheduler` renvoie `1` en cas de succès |
| A3 | `main.c` | le retour de `parse_params` est testé |
| A4 | `init.c:98` | `queue_max` est rempli (le SIGSEGV de `queue_push` a disparu) |
| — | `monitor.c:64` | le `printf("Starting the simulation ...")` a été retiré |

## A. Bloquants — le programme ne peut pas fonctionner

| # | Fichier:ligne | Problème | Conséquence observée |
|---|---|---|---|
| A4 | `coder.c:25-28` | **il manque un `;`** après `while (!monitor_is_running(...))` : le corps de ce `while` est le `while (running)` qui suit | la plupart des coders **ne travaillent jamais** (burnout à `burnout_time`) ; ceux qui entrent par chance bouclent à l'infini après l'arrêt → **hang** |
| A5 | `dongle.c:71-72` | `take_dongle(coder, d0)` est écrit **deux fois** ; `d1` n'est jamais pris | chaque coder ne demande qu'un dongle → attente infinie |
| A5b | `dongle.c:66-70` | le `if (d0->id < d1->id)` **inverse** l'ordre : on prend le plus grand id d'abord | l'ordre total qui évite l'interblocage est cassé |
| A6 | `coder.c` | les dongles ne sont **jamais relâchés** après `refactor` | l'autre coder attend à vie (confirmé par gdb : `cond_wait` sur `d->cond`) |
| A7 | `init.c:84-94` | `pthread_cond_init` **n'est jamais appelé** pour `d->cond` | `pthread_cond_wait` sur une cond non initialisée (UB) ; `free.c` ne la détruit pas non plus |
| A8 | `coder.c:32, 36` | `pthread_mutex_unlock(&run_mutex)` **sans l'avoir verrouillé** | UB : le mutex du monitor peut être « libéré » par un fil qui ne le possède pas |
| A9 | `dongle.c:17, 26, 77` | `d->queue[0].coder->id` sans vérifier `queue_idx > 0` | déréférencement NULL si la file est vide (le crash du 02/11 ; encore possible) |
| A10 | `dongle.c:21/31` puis `53` | `wait_dongle` déverrouille en cas d'arrêt, puis `take_dongle` déverrouille **encore** | **double unlock** |
| A11 | `monitor.c:67` | à l'arrêt, le monitor fait `broadcast(&run_cond)` mais **pas** sur les `d->cond` | les coders endormis dans `wait_dongle` ne sont jamais réveillés → **hang au `join`** |
| A12 | `monitor.c:43` | `last_compile + start_time` : `last_compile` est **absolu** et `start_time` aussi → on additionne deux dates | après la 1re compilation, la deadline part dans le futur lointain → burnout jamais détecté |

## B. Sémantique du sujet

| # | Fichier:ligne | Problème |
|---|---|---|
| B1 | `binary_tree.c:78` | EDF compare `a.coder->last_compile` **en direct** : la clé change quand le coder recompile → invariant du tas cassé. Il faut **figer la clé au `push`**. |
| B2 | `binary_tree.c:23` | `seq = now_ms()` : deux coders arrivés dans la même milliseconde ont le **même `seq`** → comparateur ambigu → FIFO arbitraire. Il faut un **compteur monotone**. |
| B3 | `monitor.c:44` | le burnout ne vérifie pas `compile_count < req_compile` : un coder **déjà terminé** peut être déclaré brûlé. |
| B4 | `monitor.c:70` | `check_burnout()` est évalué **avant** `check_all_done()` : on logue un burnout au lieu d'une fin normale. |
| B5 | `coder.c:45` | `last_compile` est mis à jour **à la fin du refactor**, pas au début de la compilation → deadline et EDF partent du mauvais instant. |
| B6 | `binary_tree.c:103, 116` | `r_child = i * 2 + 1` au lieu de `i * 2 + 2`. |
| B7 | `binary_tree.c:112-114` | dans `shift_down`, `temp` n'est **jamais réécrit** après l'échange → un élément est dupliqué, l'autre perdu. |
| B8 | `binary_tree.c:33-38` | `queue_pop` sur un tas vide laisse un « fantôme » dans `queue[0]`. |
| B9 | `coder.c:30` | `printf("Je suis coder nummer %d\n")` : pas d'horodatage, pas de `log_mutex` → **casse le format des logs**. |
| B10 | `coder.c:44-45` | `compile_count` et `last_compile` sont écrits **sans mutex**, lus par le monitor sans mutex → **data race**. |
| B11 | `time.c:23-34` | `pthread_cond_timedwait` attend une date **absolue** ; `wait_until_ms` construit une **durée** → le cooldown n'est jamais respecté. |
| B12 | `queue.c:27-46` | `insert_queue` (tri par insertion) n'est **jamais appelée** : code mort, et 7 erreurs de norme. |

## C. Gestion des ressources

| # | Fichier:ligne | Problème |
|---|---|---|
| C1 | `dongle.c:86-90` | sur arrêt, `release_both` « libère » des dongles **qui n'ont peut-être pas été pris** |
| C2 | `dongle.c:75-84` | `release_dongle` lit `queue[0]` **avant** de verrouiller `d->mutex` (TOCTOU) |
| C3 | `free.c:20-25` | `pthread_cond_destroy` n'est jamais appelé pour les cond des dongles (jamais initialisées, cf. A7) |
| C4 | `init.c:100-103` | si `init_dongles` échoue, aucun nettoyage : fuite |
| C5 | `main.c:44` | `if (monitor)` est **toujours vrai** (adresse d'une variable locale) ; on `join` un thread qui n'a peut-être pas été créé |
| C6 | `main.c:27` | `mon_thread` n'est pas initialisé et son retour n'est jamais testé |
| C7 | `init.c:26-45` | les deux branches `bonus` sont **identiques**, et `monitor->bonus` n'est jamais affecté |

## D. Parsing / validation

| # | Fichier:ligne | Problème |
|---|---|---|
| D1 | `main.c:47` | `argc < 9` accepte les arguments **en trop** (test « 9 arguments » : accepté puis bloqué) |
| D2 | `parser.c:36-42` | aucune validation de plage : `coders = 0` est accepté (code 0), un **temps négatif** aussi (puis blocage) |
| D3 | `utils/ft_atol_or_exit.c` | aucun contrôle de dépassement (`999999999999999999999` « passe » par chance : le `calloc` échoue ensuite) |
| D4 | `error.c:23`, `ft_atol_or_exit.c:33,37` | `exit()` **n'est pas dans la liste blanche** (AGENTS §3) |
| D5 | `error.c:22` | l'usage mentionne `-b bonus`, qui n'est jamais parsé |

## E. Hygiène / livraison

| # | Fichier | Problème |
|---|---|---|
| E1 | `EXAMPLE.c` | vestige du code de VicProgram : **ne compile pas** et n'est pas dans le `Makefile` (un seul fichier qui ne compile pas = 0 avec la norme) |
| E2 | racine | les sources sont à la racine ; `AGENTS.md` exige `coders/` |
| E3 | norminette | **19 erreurs** : `queue.c` 7, `EXAMPLE.c` 5, `main.c` 3, `codexion.h` 2, `print_log.c` 1, `coder.c` 1 |
| E4 | `Makefile` | aucune dépendance aux `.h` : modifier `codexion.h` ne reconstruit **rien** (objets périmés) |
| E5 | `Makefile` | `AR`, `libft`, `bonus` déclarés mais inutilisés |
| E6 | racine | `codexion` et `obj/` ne sont pas ignorés par git |

---

# Ordre de correction conseillé

1. **A4** (le `;` manquant) : sans ça, la moitié des coders ne démarrent jamais.
2. **A5/A5b** (prendre `d1`, et dans le bon ordre) et **A6** (relâcher après `refactor`).
3. **A7** (`pthread_cond_init` des dongles) et **A11** (réveiller aussi les `d->cond`
   à l'arrêt) : c'est le hang.
4. **A8/A9/A10** (un seul `unlock` par chemin, `queue_idx > 0`, pas de `unlock` sans
   `lock`) : plus de crash ni de double unlock.
5. **A12/B3/B4/B5** (deadline correcte, ignorer les coders finis) → burnout juste.
6. **B1/B2/B6/B7/B8/B11** (clé figée, compteur `seq`, `r_child`, copie de `temp`,
   date absolue) → FIFO/EDF et cooldown corrects.
7. **B9/B10** (logs sérialisés, compteurs sous mutex) → format et data races.
8. **D1…D5 / C1…C7** (validation, `exit`, nettoyage) → entrées et mémoire propres.
9. **E1…E6** (supprimer `EXAMPLE.c`, déplacer dans `coders/`, norme, Makefile, `.gitignore`).

# Ajouter un test

- Un cas « doit réussir » : `t_normal "nom" <n> <compiles> <timeout> <args...>`
  (le nom entre guillemets, puis les 8 arguments du sujet).
- Un cas « doit être refusé » : `t_reject "nom" <args...>`.
- Une précision temporelle : `t_burnout "nom" <instant_ms> <timeout> <args...>`.
- Un test maison : écrire une fonction dans la section 4 et l'appeler ; utiliser
  `run_sim`, puis lire `$LOG`, `$ERR`, `$RC`, `$MS`.
