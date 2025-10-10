#!/usr/bin/env bash
set -e

echo "Building tests..."
make tests > /dev/null

echo "Running tests..."
pass=0
fail=0

for test_exe in tests/bin/*; do
  if "$test_exe" > /dev/null; then
    echo "[PASS] $(basename "$test_exe")"
    pass=$((pass+1))
  else
    echo "[FAIL] $(basename "test_exe")"
    fail=$((fail+1))
  fi
done

echo
echo "✅ Passed: $pass"
echo "❌ Failed: $fail"

exit $fail
