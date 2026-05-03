#!/usr/bin/env bash
# tests/run_tests.sh — runs every tests/sample*.ml against the compiled
# binary and compares stdout to the matching .expected file.
# Exits 0 on full pass, 1 on any failure.

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BIN="${REPO_ROOT}/ml_translator"

if [[ ! -x "${BIN}" ]]; then
    echo "error: ${BIN} not found or not executable. Run 'make' first." >&2
    exit 1
fi

pass=0
fail=0

for ml in "${SCRIPT_DIR}"/sample*.ml; do
    name="$(basename "${ml}" .ml)"
    expected="${SCRIPT_DIR}/${name}.expected"
    [[ -f "${expected}" ]] || { echo "SKIP  ${name}"; continue; }

    actual="$("${BIN}" "${ml}" 2>&1 || true)"
    want="$(cat "${expected}")"

    if [[ "${actual}" == "${want}" ]]; then
        printf "PASS  %s\n" "${name}"
        pass=$((pass + 1))
    else
        printf "FAIL  %s\n" "${name}"
        printf "      expected: %q\n" "${want}"
        printf "      got:      %q\n" "${actual}"
        fail=$((fail + 1))
    fi
done

echo
echo "Results: ${pass} passed, ${fail} failed"
[[ ${fail} -eq 0 ]]