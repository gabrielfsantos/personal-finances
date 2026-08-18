#!/usr/bin/env bash
set -euo pipefail

cd "${BUILD_WORKSPACE_DIRECTORY:-$(pwd)}"

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <target> [<target>...]" >&2
  exit 1
fi

targets=("$@")
bazel_exit=0

bazel build \
  --aspects=//tools/lint:linters.bzl%clang_tidy \
  --@aspect_rules_lint//lint:fail_on_violation \
  --keep_going \
  "${targets[@]}" || bazel_exit=$?

bazel_bin=$(bazel info bazel-bin)

for target in "${targets[@]}"; do
  label="${target#//}"
  label="${label/:/\/}"
  report="${bazel_bin}/${label}.AspectRulesLintClangTidy.out"

  if [[ -f "$report" ]]; then
    if [[ -s "$report" ]]; then
      echo "=== clang-tidy report for ${target} ==="
      cat "$report"
      echo
    fi
  else
    echo "Warning: report not found: $report" >&2
  fi
done

exit "$bazel_exit"
