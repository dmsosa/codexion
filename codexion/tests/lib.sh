#!/usr/bin/env bash
# =============================================================================
# tests/lib.sh — petits helpers partagés par la suite de tests.
# Ne s'exécute pas seule : elle est chargée par run_tests.sh (. tests/lib.sh).
# =============================================================================

if [ -t 1 ]; then
	C_RESET=$'\033[0m'
	C_RED=$'\033[31m'
	C_GREEN=$'\033[32m'
	C_YELLOW=$'\033[33m'
	C_BLUE=$'\033[34m'
	C_BOLD=$'\033[1m'
else
	C_RESET=''
	C_RED=''
	C_GREEN=''
	C_YELLOW=''
	C_BLUE=''
	C_BOLD=''
fi

TESTS_PASS=0
TESTS_FAIL=0
TESTS_SKIP=0
TESTS_WARN=0
FAILED=()

# PASS : un test dont le résultat attendu est respecté.
pass()
{
	TESTS_PASS=$((TESTS_PASS + 1))
	printf '  %sPASS%s  %s\n' "$C_GREEN" "$C_RESET" "$1"
}

# FAIL : le comportement ne respecte pas le sujet / AGENTS.md.
# Les arguments suivants sont affichés en dessous (extraits d'erreur, valeurs).
fail()
{
	TESTS_FAIL=$((TESTS_FAIL + 1))
	FAILED+=("$1")
	printf '  %sFAIL%s  %s\n' "$C_RED" "$C_RESET" "$1"
	shift
	while [ "$#" -gt 0 ]; do
		printf '          %s\n' "$1"
		shift
	done
}

# WARN : suspect, mais pas une violation certaine (à vérifier à la main).
warn()
{
	TESTS_WARN=$((TESTS_WARN + 1))
	printf '  %sWARN%s  %s\n' "$C_YELLOW" "$C_RESET" "$1"
}

# SKIP : impossible de tester (outil absent, pas de binaire, ...).
skip()
{
	TESTS_SKIP=$((TESTS_SKIP + 1))
	printf '  %sSKIP%s  %s (%s)\n' "$C_YELLOW" "$C_RESET" "$1" "${2:-}"
}

section()
{
	printf '\n%s== %s ==%s\n' "$C_BOLD$C_BLUE" "$1" "$C_RESET"
}

note()
{
	printf '  %s..%s %s\n' "$C_BLUE" "$C_RESET" "$1"
}

# Résumé final. Retourne 1 s'il y a au moins un FAIL.
summary()
{
	printf '\n%s== RÉSUMÉ ==%s\n' "$C_BOLD" "$C_RESET"
	printf '  %s%d PASS%s   %s%d FAIL%s   %s%d WARN%s   %s%d SKIP%s\n' \
		"$C_GREEN" "$TESTS_PASS" "$C_RESET" \
		"$C_RED" "$TESTS_FAIL" "$C_RESET" \
		"$C_YELLOW" "$TESTS_WARN" "$C_RESET" \
		"$C_YELLOW" "$TESTS_SKIP" "$C_RESET"
	if [ "${#FAILED[@]}" -gt 0 ]; then
		printf '\n  Échecs :\n'
		local name
		for name in "${FAILED[@]}"; do
			printf '   - %s\n' "$name"
		done
	fi
	if [ "$TESTS_FAIL" -gt 0 ]; then
		return 1
	fi
	return 0
}
