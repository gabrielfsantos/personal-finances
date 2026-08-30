#!/usr/bin/env bash
set -euo pipefail

git config user.name "github-actions[bot]"
git config user.email "github-actions[bot]@users.noreply.github.com"

PROCESSED_PRS=""
PR_QUEUE=""

extract_deps() {
  local body="$1"
  if [ -z "$body" ]; then
    return
  fi

  local extracted=""
  while read -r line; do
    local pr_num
    pr_num=$(echo "$line" | grep -oE '[0-9]+' | head -n 1 || true)
    if [ -n "$pr_num" ]; then
      extracted="$extracted $pr_num"
    fi
  done < <(echo "$body" | grep -iE "depends-on:")

  echo "$extracted"
}

INITIAL_DEPS=$(extract_deps "${PR_BODY:-}")
PR_QUEUE="$INITIAL_DEPS"
RESOLVED_ORDER=""

while [ -n "$PR_QUEUE" ]; do
  read -r -a QUEUE_ARRAY <<< "$PR_QUEUE"
  CURRENT_PR="${QUEUE_ARRAY[0]}"
  PR_QUEUE="${QUEUE_ARRAY[*]:1}"

  if echo "$PROCESSED_PRS" | grep -q -w "$CURRENT_PR"; then
    continue
  fi

  echo "Inspecting dependent PR #$CURRENT_PR..."
  PROCESSED_PRS="$PROCESSED_PRS $CURRENT_PR"

  PR_INFO=$(gh pr view "$CURRENT_PR" --json state,body --jq '{state: .state, body: .body}' 2>/dev/null || true)

  if [ -n "$PR_INFO" ]; then
    PR_STATE=$(echo "$PR_INFO" | jq -r '.state' 2>/dev/null || true)
    DEP_BODY=$(echo "$PR_INFO" | jq -r '.body' 2>/dev/null || true)

    if [ "$PR_STATE" != "OPEN" ]; then
      echo "PR #$CURRENT_PR is $PR_STATE. Skipping merge."
      continue
    fi

    RESOLVED_ORDER="$RESOLVED_ORDER $CURRENT_PR"

    if [ -n "$DEP_BODY" ]; then
      NESTED_DEPS=$(extract_deps "$DEP_BODY")
      if [ -n "$NESTED_DEPS" ]; then
        echo "Found nested dependencies in PR #$CURRENT_PR: $NESTED_DEPS"
        PR_QUEUE="$PR_QUEUE $NESTED_DEPS"
      fi
    fi
  else
    echo "Warning: Could not fetch details for PR #$CURRENT_PR. Proceeding with fallback fetch."
    RESOLVED_ORDER="$RESOLVED_ORDER $CURRENT_PR"
  fi
done

if [ -n "$RESOLVED_ORDER" ]; then
  echo "--------------------------------------------------"
  echo "Final merge order for open dependencies:$RESOLVED_ORDER"
  echo "--------------------------------------------------"

  for PR_NUMBER in $RESOLVED_ORDER; do
    echo "Fetching PR #$PR_NUMBER..."
    git fetch origin "pull/$PR_NUMBER/head:dependent-pr-$PR_NUMBER"

    echo "Merging PR #$PR_NUMBER into workspace..."
    git merge "dependent-pr-$PR_NUMBER" --allow-unrelated-histories --no-edit || {
      echo "::error::Failed to merge dependent PR #$PR_NUMBER due to conflicts."
      git status
      exit 1
    }
  done
  echo "All open recursive dependencies merged successfully!"
else
  echo "No active Depends-On dependencies to merge."
fi
