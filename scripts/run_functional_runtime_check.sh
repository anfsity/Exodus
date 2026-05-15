#!/usr/bin/env bash

set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FUNCTIONAL_DIR="${1:-$ROOT_DIR/tests/functional}"
RUN_CMD_TEMPLATE="${RUN_CMD_TEMPLATE:-$ROOT_DIR/build/compiler {test}}"

if [[ ! -d "$FUNCTIONAL_DIR" ]]; then
  echo "functional directory not found: $FUNCTIONAL_DIR" >&2
  exit 1
fi

mapfile -t TEST_FILES < <(find "$FUNCTIONAL_DIR" -maxdepth 1 -type f -name '*.sy' | sort)
if [[ ${#TEST_FILES[@]} -eq 0 ]]; then
  echo "no .sy files found under: $FUNCTIONAL_DIR" >&2
  exit 1
fi

total=0
runtime_error_count=0

for test_file in "${TEST_FILES[@]}"; do
  total=$((total + 1))

  cmd="${RUN_CMD_TEMPLATE//\{test\}/$test_file}"
  output="$(bash -lc "$cmd" 2>&1)"
  status=$?

  lower_output="$(printf '%s' "$output" | tr '[:upper:]' '[:lower:]')"
  has_runtime_error=0

  if [[ "$lower_output" == *"runtime error"* ]] ||
    [[ "$lower_output" == *"segmentation fault"* ]] ||
    [[ "$lower_output" == *"floating point exception"* ]] ||
    [[ "$lower_output" == *"core dumped"* ]] ||
    [[ "$lower_output" == *"aborted"* ]]; then
    has_runtime_error=1
  fi

  if ((status >= 128)); then
    has_runtime_error=1
  fi

  if ((has_runtime_error)); then
    runtime_error_count=$((runtime_error_count + 1))
    echo "$(basename "$test_file"): runtime error"
  else
    echo "$(basename "$test_file"): ok"
  fi
done

echo "runtime error: $runtime_error_count/$total"

if ((runtime_error_count > 0)); then
  exit 2
fi
