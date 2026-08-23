#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TARGET_LINK="${SCRIPT_DIR}/clang_tidy"

REAL_CLANG_TIDY=""

if [ -e "$TARGET_LINK" ] || [ -L "$TARGET_LINK" ]; then
    REAL_CLANG_TIDY="$TARGET_LINK"
else
    REAL_CLANG_TIDY=$(find "${SCRIPT_DIR}/.." -name "clang_tidy" 2>/dev/null | head -n 1)
fi

if [ -z "$REAL_CLANG_TIDY" ]; then
    echo "ERROR: Não foi possível localizar o link/binário do clang_tidy." >&2
    echo "       Diretório atual de busca: ${SCRIPT_DIR}" >&2
    exit 1
fi

TMP_LOG=$(mktemp)
trap 'rm -f "$TMP_LOG"' EXIT

set +e
"$REAL_CLANG_TIDY" "$@" > "$TMP_LOG" 2>&1
EXIT_CODE=$?
set -e

if [ $EXIT_CODE -ne 0 ]; then
    echo "================================================================================" >&2
    echo " [clang-tidy] Static analysis failed: " >&2
    echo "================================================================================" >&2
    cat "$TMP_LOG" >&2
    echo "================================================================================" >&2
    exit $EXIT_CODE
fi

exit 0
