#!/usr/bin/env bash
# =============================================================================
# tests/run_tests.sh — suite de tests de Codexion
#
# Usage :
#   tests/run_tests.sh              # tout
#   tests/run_tests.sh --static     # seulement compilation / norme / fonctions
#   CC=clang tests/run_tests.sh     # choisir le compilateur
#
# La suite ne modifie aucun fichier du projet : tout ce qui est temporaire va
# dans /tmp/opencode/codexion-tests (surchargeable avec CODEXION_TEST_TMP).
# =============================================================================

set -o pipefail

# Pas de fichier core quand un test provoque volontairement un crash.
ulimit -c 0 2>/dev/null || true

HERE=$(cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(cd -- "$HERE/.." && pwd)
# shellcheck source=tests/lib.sh
. "$HERE/lib.sh"

CC=${CC:-cc}
WFLAGS="-Wall -Wextra -Werror -pthread"
TMP=${CODEXION_TEST_TMP:-/tmp/opencode/codexion-tests}
STATIC_ONLY=0

if [ "${1:-}" = "--static" ]; then
	STATIC_ONLY=1
fi

rm -rf -- "$TMP"
mkdir -p -- "$TMP"

# --- Où sont les sources ? ---------------------------------------------------
# AGENTS.md impose coders/ ; en attendant, on accepte aussi la racine.
if [ -d "$ROOT/coders" ]; then
	SRC="$ROOT/coders"
else
	SRC="$ROOT"
fi

INCS=""
for dir in "$SRC" "$SRC/include" "$ROOT/include"; do
	if [ -d "$dir" ]; then
		INCS="$INCS -I$dir"
	fi
done

SRCS=()
while IFS= read -r f; do
	SRCS+=("$f")
done < <(find "$SRC" -name '*.c' -not -path '*/tests/*' | sort)

printf '%s\n' "${C_BOLD}Codexion — suite de tests${C_RESET}"
printf '  racine  : %s\n' "$ROOT"
printf '  sources : %s (%d fichier(s) .c)\n' "$SRC" "${#SRCS[@]}"
printf '  compil. : %s %s\n' "$CC" "$WFLAGS"

# =============================================================================
# 1. STRUCTURE ET COMPILATION
# =============================================================================
section "1. Structure et compilation"

# 1.0 Emplacement de la livraison (AGENTS.md §1 : tout va dans coders/).
if [ "$SRC" = "$ROOT" ]; then
	fail "sources à la racine : la livraison doit être dans coders/ (AGENTS.md §1)" \
		"attendu : coders/Makefile, coders/*.c, coders/*.h"
else
	pass "sources dans coders/"
fi

# 1.1 Makefile (AGENTS.md §2 : livraison dans coders/ avec Makefile).
if [ -f "$SRC/Makefile" ]; then
	pass "Makefile présent ($SRC/Makefile)"
else
	fail "Makefile absent (AGENTS.md §2 : Makefile obligatoire dans coders/)" \
		"règles attendues : \$(NAME), all, clean, fclean, re"
fi

BIN=""

# 1.2 make : c'est lui qui définit ce qu'on livre.
if [ -f "$SRC/Makefile" ]; then
	if (cd "$SRC" && make -s) >"$TMP/make.log" 2>&1; then
		pass "make"
		[ -x "$SRC/codexion" ] && BIN="$SRC/codexion"
	else
		fail "make" "$(tail -12 "$TMP/make.log")"
	fi

	# 1.2 bis : pas de relink (AGENTS.md §2).
	if [ -x "$SRC/codexion" ]; then
		before=$(stat -c %Y "$SRC/codexion" 2>/dev/null)
		sleep 1.1
		(cd "$SRC" && make -s) >/dev/null 2>&1
		after=$(stat -c %Y "$SRC/codexion" 2>/dev/null)
		if [ "$before" = "$after" ]; then
			pass "make ne relinke pas"
		else
			fail "make relinke (AGENTS.md §2)" \
				"le binaire a été reconstruit alors que rien n'avait changé"
		fi
	else
		skip "make ne relinke pas" "make n'a pas produit codexion"
	fi
else
	# Sans Makefile, on tente un lien direct : ça permet quand même de tester.
	if [ "${#SRCS[@]}" -gt 0 ] && $CC $WFLAGS $INCS "${SRCS[@]}" \
		-o "$TMP/codexion" >"$TMP/build.log" 2>&1; then
		pass "compilation directe (sans Makefile)"
		BIN="$TMP/codexion"
	else
		skip "make" "pas de Makefile"
		skip "make ne relinke pas" "pas de Makefile"
	fi
fi

# 1.3 Chaque fichier .c doit compiler seul (les fichiers « extra » comptent).
if [ "${#SRCS[@]}" -eq 0 ]; then
	fail "aucun fichier .c trouvé dans $SRC"
else
	bad_files=()
	for f in "${SRCS[@]}"; do
		# shellcheck disable=SC2086
		if ! $CC $WFLAGS $INCS -fsyntax-only "$f" >"$TMP/syn.log" 2>&1; then
			bad_files+=("${f#"$SRC"/}")
		fi
	done
	if [ "${#bad_files[@]}" -eq 0 ]; then
		pass "chaque .c compile seul (${#SRCS[@]} fichier(s))"
	else
		fail "${#bad_files[@]} fichier(s) .c ne compilent pas" \
			"$(printf '%s\n' "${bad_files[@]}")"
	fi
fi

# 1.4 Fichiers .c présents mais absents du Makefile (fichiers parasites).
if [ -f "$SRC/Makefile" ] && [ "${#SRCS[@]}" -gt 0 ]; then
	built=$(sed -n '/^CFILES/,/^[[:space:]]*$/p' "$SRC/Makefile" \
		| grep -oE '[^[:space:]\\]+\.c' | sed 's|^\./||')
	stray=()
	for f in "${SRCS[@]}"; do
		rel=${f#"$SRC"/}
		if ! printf '%s\n' "$built" | grep -qxF "$rel"; then
			stray+=("$rel")
		fi
	done
	if [ "${#stray[@]}" -eq 0 ]; then
		pass "aucun fichier .c parasite"
	else
		warn "fichier(s) .c absent(s) du Makefile : $(printf '%s ' "${stray[@]}")"
	fi
fi

# =============================================================================
# 2. NORME
# =============================================================================
section "2. Norme"

if command -v norminette >/dev/null 2>&1; then
	norminette "$SRC" >"$TMP/norm.log" 2>&1
	norm_errors=$(grep -cE '^Error:' "$TMP/norm.log" 2>/dev/null || true)
	if [ "${norm_errors:-0}" -eq 0 ]; then
		pass "norminette"
	else
		fail "norminette : $norm_errors erreur(s)" \
			"$(grep -E '^Error:' "$TMP/norm.log" | head -8)"
	fi
else
	skip "norminette" "outil absent"
fi

# =============================================================================
# 3. FONCTIONS AUTORISÉES
# =============================================================================
section "3. Fonctions autorisées (AGENTS.md §3)"

# Liste noire : fonctions classiques qui ne sont PAS dans la liste blanche.
# (whitelist : pthread_create/join/mutex_*/cond_*, gettimeofday, usleep, write,
#  malloc, free, printf, fprintf, strcmp, strlen, atoi, memset)
FORBIDDEN='\b(fork|vfork|system|exit|_exit|pthread_cond_signal|pthread_mutex_trylock|pthread_detach|sem_init|sem_wait|sem_post|nanosleep|clock_gettime|clock_nanosleep|sched_yield|strcpy|strncpy|strdup|memcpy|calloc|realloc|bzero|putchar|puts|fflush|sprintf|snprintf|fopen|fclose|read|close|waitpid|sleep)\s*\('

if [ "${#SRCS[@]}" -eq 0 ]; then
	skip "fonctions autorisées" "aucune source"
else
	offenses=$(grep -nHE "$FORBIDDEN" "${SRCS[@]}" 2>/dev/null || true)
	if [ -z "$offenses" ]; then
		pass "aucune fonction interdite détectée"
	else
		fail "fonctions hors liste blanche" \
			"$(printf '%s\n' "$offenses" | sed "s|$ROOT/||" | head -10)"
	fi
fi

if [ "$STATIC_ONLY" -eq 1 ]; then
	summary
	exit $?
fi

# =============================================================================
# 4. TESTS FONCTIONNELS
# =============================================================================
section "4. Tests fonctionnels"

if [ -z "$BIN" ] || [ ! -x "$BIN" ]; then
	skip "tests fonctionnels" "aucun binaire : la compilation a échoué"
	summary
	exit $?
fi

LOG="$TMP/out.log"
ERR="$TMP/err.log"
RC=0
MS=0
ARGS=""

# run_sim <timeout_s> <args...> : remplit RC, MS, LOG et ERR.
run_sim()
{
	local tmo=$1
	local t0 t1

	shift
	t0=$(date +%s%N)
	( timeout "$tmo" "$BIN" "$@" >"$LOG" 2>"$ERR"; exit $? ) 2>/dev/null
	RC=$?
	t1=$(date +%s%N)
	MS=$(( (t1 - t0) / 1000000 ))
}

# Trace courte d'un crash, pour que l'échec soit actionnable.
crash_trace()
{
	gdb -q -batch -nx -ex run -ex 'bt 4' --args "$BIN" "$@" 2>/dev/null \
		| grep -E '^#[0-9]+ ' | head -3
}

# Le format exact du sujet (AGENTS.md §6), rien d'autre.
log_format_ok()
{
	local bad

	bad=$(grep -vE '^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring)$|^[0-9]+ [0-9]+ burned out$' "$LOG")
	if [ -n "$bad" ]; then
		printf '%s\n' "$bad" >"$TMP/format-bad.txt"
		return 1
	fi
	return 0
}

# Vérifie qu'un run « normal » se termine correctement.
# t_normal <nom> <n_coders> <compiles_requis> <timeout_s> <args...>
t_normal()
{
	local name=$1
	local n=$2
	local req=$3
	local tmo=$4
	local burns err takes comps bt

	shift 4
	run_sim "$tmo" "$@"

	if [ "$RC" -eq 124 ]; then
		fail "$name" "timeout ${tmo}s (deadlock, famine ou boucle infinie)"
		return
	fi
	if [ "$RC" -ge 128 ]; then
		mapfile -t bt < <(crash_trace "$@")
		fail "$name" "crash : signal $((RC - 128))" "${bt[@]}"
		return
	fi
	if [ "$RC" -ne 0 ]; then
		fail "$name" "code de sortie $RC (attendu 0)" \
			"$(head -2 "$ERR")"
		return
	fi
	if ! log_format_ok; then
		fail "$name — format des logs" "$(head -3 "$TMP/format-bad.txt")"
		return
	fi

	burns=$(grep -c ' burned out$' "$LOG")
	if [ "$burns" -ne 0 ]; then
		fail "$name — burnout inattendu" \
			"$burns ligne(s) 'burned out' alors que la config est viable"
		return
	fi

	err=$(awk -v n="$n" -v req="$req" '
		/^[0-9]+ [0-9]+ is compiling$/ { c[$2]++; ids[$2] = 1; next }
		END {
			bad = 0
			for (i in c)
				if (c[i] != req) {
					printf "coder %s : %d compilation(s), attendu %d\n", i, c[i], req
					bad = 1
				}
			if (length(ids) != n) {
				printf "%d coder(s) distinct(s), attendu %d\n", length(ids), n
				bad = 1
			}
			exit bad
		}' "$LOG")
	if [ -n "$err" ]; then
		fail "$name — nombre de compilations" "$err"
		return
	fi

	# Contrôles non bloquants (peuvent varier selon l'implémentation).
	comps=$(grep -c ' is compiling$' "$LOG")
	takes=$(grep -c ' has taken a dongle$' "$LOG")
	if [ "$n" -gt 1 ] && [ "$takes" -ne $((2 * comps)) ]; then
		warn "$name — $takes 'has taken' pour $comps compiles (attendu $((2 * comps)))"
	fi
	if ! awk 'NR > 1 && $1 < prev { exit 1 } { prev = $1 }' "$LOG"; then
		warn "$name — horodatages non monotones"
	fi
	if [ -s "$ERR" ]; then
		warn "$name — écrit sur stderr : $(head -1 "$ERR")"
	fi

	pass "$name (${MS}ms)"
}

# t_burnout <nom> <instant_attendu_ms> <timeout_s> <args...>
# Vérifie l'exigence « log du burnout dans les 10 ms » (AGENTS.md §6).
t_burnout()
{
	local name=$1
	local expected=$2
	local tmo=$3
	local fire lo hi bt

	shift 3
	run_sim "$tmo" "$@"

	if [ "$RC" -eq 124 ]; then
		fail "$name" "timeout ${tmo}s : le burnout ne stoppe pas la simulation"
		return
	fi
	if [ "$RC" -ge 128 ]; then
		mapfile -t bt < <(crash_trace "$@")
		fail "$name" "crash : signal $((RC - 128))" "${bt[@]}"
		return
	fi
	if ! log_format_ok; then
		fail "$name — format des logs" "$(head -3 "$TMP/format-bad.txt")"
		return
	fi

	fire=$(awk '/ burned out$/ { print $1; exit }' "$LOG")
	if [ -z "$fire" ]; then
		fail "$name" "aucune ligne 'burned out' (le coder aurait dû brûler)"
		return
	fi

	lo=$((expected - 10))
	hi=$((expected + 20))
	if [ "$fire" -ge "$lo" ] && [ "$fire" -le "$hi" ]; then
		pass "$name (burnout à ${fire}ms, attendu ~${expected}ms)"
	else
		fail "$name — précision du burnout" \
			"burnout loggé à ${fire}ms, attendu entre ${lo} et ${hi}ms" \
			"(la deadline part du début de la dernière compilation)"
	fi
}

# t_reject <nom> <args...> : doit refuser avec un code != 0, sans crash.
t_reject()
{
	local name=$1

	shift
	run_sim 5 "$@"
	if [ "$RC" -eq 124 ]; then
		fail "$name" "timeout : l'argument invalide bloque le programme"
		return
	fi
	if [ "$RC" -eq 0 ]; then
		fail "$name" "accepté (code 0) alors que l'entrée est invalide"
		return
	fi
	if [ "$RC" -ge 128 ]; then
		fail "$name" "plantage : signal $((RC - 128)) au lieu d'un refus propre"
		return
	fi
	pass "$name (code $RC)"
}

# t_soak <nom> <passes> <n> <req> <timeout> <args...> : répète pour débusquer
# les courses intermittentes.
t_soak()
{
	local name=$1
	local runs=$2
	local n=$3
	local req=$4
	local tmo=$5
	local i ok=0

	shift 5
	for ((i = 1; i <= runs; i++)); do
		run_sim "$tmo" "$@"
		if [ "$RC" -eq 0 ] && [ "$(grep -c ' burned out$' "$LOG")" -eq 0 ]; then
			ok=$((ok + 1))
		fi
	done
	if [ "$ok" -eq "$runs" ]; then
		pass "$name ($ok/$runs)"
	else
		fail "$name" "$ok/$runs exécutions correctes seulement"
	fi
}

# --- 4.1 Configurations viables : tout le monde doit compiler ---------------
note "configs viables (aucun burnout attendu)"
t_normal "fifo, 4 coders, 2 compiles" 4 2 10 4 1000 60 60 60 2 10 fifo
t_normal "edf,  4 coders, 2 compiles" 4 2 10 4 1000 60 60 60 2 10 edf
t_normal "n = 1 (un seul dongle)" 1 2 10 1 1000 50 10 10 2 10 fifo
t_normal "edf, 12 coders (stress)" 12 2 10 12 5000 10 10 10 2 0 edf
t_normal "fifo, cooldown = 100ms" 5 3 10 5 3000 50 10 10 3 100 fifo

# --- 4.2 Burnout : détection et précision -----------------------------------
# 2 coders se partagent les 2 dongles ; un cycle dure 120 ms > 100 ms.
note "burnout (config volontairement non viable)"
t_burnout "burnout détecté à ~100ms" 100 10 2 100 80 20 20 5 0 fifo

# --- 4.3 Arguments : tout doit être rejeté proprement -----------------------
note "arguments invalides"
t_reject "aucun argument"
t_reject "7 arguments (il en manque un)" 4 1000 100 50 50 2 10
t_reject "9 arguments (un en trop)" 4 1000 100 50 50 2 10 fifo extra
t_reject "scheduler inconnu" 4 1000 100 50 50 2 10 sjf
t_reject "scheduler en majuscules" 4 1000 100 50 50 2 10 FIFO
t_reject "coders = 0" 0 1000 100 50 50 2 10 fifo
t_reject "coders négatif" -1 1000 100 50 50 2 10 fifo
t_reject "temps négatif" 4 -1000 100 50 50 2 10 fifo
t_reject "nombre non entier" 4 1000 100x 50 50 2 10 fifo
t_reject "texte à la place d'un nombre" abc 1000 100 50 50 2 10 fifo
t_reject "chaîne vide" "" 1000 100 50 50 2 10 fifo
t_reject "dépassement d'entier" 999999999999999999999 1000 100 50 50 2 10 fifo

# --- 4.4 Répétition (courses intermittentes) --------------------------------
note "répétition"
t_soak "edf x5 (stabilité)" 5 4 2 10 4 1000 60 60 60 2 10 edf

# =============================================================================
# 5. MÉMOIRE ET COURSES DE DONNÉES
# =============================================================================
section "5. Mémoire et courses de données"

if command -v valgrind >/dev/null 2>&1; then
	( timeout 30 valgrind -q --leak-check=full --error-exitcode=42 \
		"$BIN" 4 1000 50 20 20 2 10 edf >"$TMP/vg.log" 2>&1; exit $? ) 2>/dev/null
	vg_rc=$?
	if [ "$vg_rc" -eq 124 ]; then
		fail "valgrind : timeout 30s" \
			"le programme se fige (deadlock) — rien à analyser"
	elif [ "$vg_rc" -ge 128 ]; then
		fail "valgrind : crash" \
			"$(grep -E 'Invalid|Process terminating|Access not within' "$TMP/vg.log" | head -4)"
	elif [ "$vg_rc" -eq 42 ] || grep -qE 'definitely lost: [1-9]' "$TMP/vg.log"; then
		fail "valgrind : fuites / erreurs" \
			"$(grep -E 'definitely lost|Invalid|uninitialised' "$TMP/vg.log" | head -5)"
	else
		pass "valgrind : pas de fuite ni d'accès invalide"
	fi
else
	skip "valgrind" "outil absent"
fi

# ThreadSanitizer : détecte les vraies data races (accès non synchronisés).
TSAN_SRCS=("${SRCS[@]}")
if [ -f "$SRC/Makefile" ]; then
	TSAN_SRCS=()
	while IFS= read -r rel; do
		TSAN_SRCS+=("$SRC/$rel")
	done < <(sed -n '/^CFILES/,/^[[:space:]]*$/p' "$SRC/Makefile" \
		| grep -oE '[^[:space:]\\]+\.c')
fi

if [ "${#TSAN_SRCS[@]}" -gt 0 ]; then
	# shellcheck disable=SC2086
	if $CC -fsanitize=thread -g -O1 -pthread $INCS "${TSAN_SRCS[@]}" \
		-o "$TMP/tsan" >"$TMP/tsan-build.log" 2>&1; then
		timeout 20 "$TMP/tsan" 4 1000 50 20 20 2 10 edf \
			>"$TMP/tsan.log" 2>&1
		tsan_rc=$?
		if grep -q 'FATAL: ThreadSanitizer' "$TMP/tsan.log"; then
			skip "ThreadSanitizer" "indisponible dans cet environnement"
		elif grep -q 'WARNING: ThreadSanitizer' "$TMP/tsan.log"; then
			fail "ThreadSanitizer : course de données" \
				"$(grep -A3 'WARNING: ThreadSanitizer' "$TMP/tsan.log" | head -6)"
		elif [ "$tsan_rc" -eq 124 ]; then
			fail "ThreadSanitizer : timeout" "la simulation ne se termine pas"
		else
			pass "ThreadSanitizer : aucune course détectée"
		fi
	else
		skip "ThreadSanitizer" "compilation instrumentée impossible"
	fi
else
	skip "ThreadSanitizer" "aucune source"
fi

summary
exit $?
