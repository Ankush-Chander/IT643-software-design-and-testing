#!/usr/bin/env bash
#
# check-lab2_3.sh — run this from the root of your fork, on branch lab2_3/design-first,
# before you open the pull request.
#
#   bash lab2_3/check-lab2_3.sh
#
# It checks that your submission is complete and prints the numbers and the commit
# table you need for sections 5 and 6 of lab2_3/REPORT.md. Everything it checks is
# structure, not content — passing it does not mean the work is good, only that a
# marker will be able to find all of it.

set -u

FAILED=0
fail() { printf '[FAIL] %s\n' "$1"; FAILED=1; }
warn() { printf '[warn] %s\n' "$1"; }
ok()   { printf '[ ok ] %s\n' "$1"; }

git rev-parse --git-dir >/dev/null 2>&1 || {
  echo "[FAIL] not inside a git repository — run this from the root of your fork"
  exit 1
}
cd "$(git rev-parse --show-toplevel)" || exit 1

echo "=== 1. branch ==================================================="

BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$BRANCH" = "lab2_3/design-first" ]; then
  ok "on lab2_3/design-first"
else
  warn "you are on '$BRANCH', not 'lab2_3/design-first' — check you meant to be"
fi

BASE=""
for b in main master; do
  if git show-ref --verify --quiet "refs/heads/$b"; then BASE="$b"; break; fi
done
[ -n "$BASE" ] || fail "no local 'main' or 'master' branch to compare against"

echo
echo "=== 2. files ===================================================="

for f in lab2_3/REPORT.md \
         lab2_3/UBIQUITOUS_LANGUAGE.md \
         lab2_3/audits/main.md \
         lab2_3/audits/lab1-head.md; do
  if [ -f "$f" ]; then ok "$f"; else fail "missing: $f"; fi
done

for f in lab2_3/REPORT.md lab2_3/UBIQUITOUS_LANGUAGE.md; do
  [ -f "$f" ] || continue
  git ls-files --error-unmatch "$f" >/dev/null 2>&1 || fail "$f exists but is not committed"
done

echo
echo "=== 3. report structure ========================================="

R=lab2_3/REPORT.md
if [ -f "$R" ]; then
  for n in 1 2 3 4 5 6 7 8; do
    grep -q "^## $n\." "$R" || fail "REPORT.md has no '## $n.' heading — keep all eight, in order"
  done

  BLANKS=$(grep -c '___' "$R" || true)
  if [ "${BLANKS:-0}" -gt 0 ]; then
    fail "REPORT.md still has $BLANKS unfilled '___' placeholders"
  else
    ok "no unfilled placeholders"
  fi

  # section 4 — count table rows that are neither header, separator, nor blank
  ROWS=$(awk '/^## 4\./{s=1;next} /^## 5\./{s=0} s && /^\|/ && !/^\|[- |:]*\|$/ && !/smell reported/ && !/___/ {n++} END{print n+0}' "$R")
  if [ "$ROWS" -ge 3 ]; then
    ok "section 4: $ROWS rejected candidates"
  else
    fail "section 4: found $ROWS filled rows, need at least 3 rejected candidates"
  fi

  WORDS=$(awk '/^## 7\./{s=1} /^## If you did not finish/{s=0} s' "$R" | wc -w | tr -d ' ')
  if [ "$WORDS" -le 500 ]; then
    ok "analysis: $WORDS words"
  else
    warn "analysis is $WORDS words — the limit is 500, and it is enforced by whoever marks it"
  fi
fi

echo
echo "=== 4. audits ==================================================="

SMELLS='Long Method|Large Class|Primitive Obsession|Long Parameter List|Data Clumps|Switch Statement|Temporary Field|Refused Bequest|Divergent Change|Shotgun Surgery|Parallel Inheritance|Duplicate Code|Lazy Class|Data Class|Dead Code|Speculative Generality|Feature Envy|Inappropriate Intimacy|Message Chain|Middle Man|Incomplete Library'

for f in lab2_3/audits/main.md lab2_3/audits/lab1-head.md; do
  [ -f "$f" ] || continue
  NOCITE=$(grep -E -i "$SMELLS" "$f" | grep -v -E ':[0-9]+' | grep -v -E '^\s*#' | wc -l | tr -d ' ')
  if [ "$NOCITE" -eq 0 ]; then
    ok "$f: every smell line carries a file:line citation"
  else
    warn "$f: $NOCITE line(s) name a smell without a file:line citation — a finding you cannot point at is not a finding"
  fi
done

echo
echo "=== 5. commits =================================================="

if [ -n "$BASE" ]; then
  N=$(git rev-list --count "$BASE"..HEAD)
  if [ "$N" -lt 4 ]; then
    fail "only $N commit(s) on this branch — the assignment asks for four, in order, then your report"
  else
    ok "$N commits on $BASE..HEAD"
  fi

  echo
  echo "  --- paste this into section 5 of REPORT.md ---"
  echo
  echo "  | # | sha | subject | what it is |"
  echo "  |---|---|---|---|"
  git log --reverse --format='%h|%s' "$BASE"..HEAD | head -4 | awk -F'|' '
    BEGIN{split("glossary,smell report,the refactor alone,the feature alone",w,",")}
    {printf "  | %d | %s | %s | %s |\n", NR, $1, $2, w[NR]}'
  echo

  set -- $(git log --reverse --format='%H' "$BASE"..HEAD | head -4)
  if [ "$#" -eq 4 ]; then
    C2=$2; C3=$3; C4=$4
    echo "  --- run 2 numbers for section 6 ---"
    echo
    printf '  commit 4, the feature   -w  : %s\n' "$(git diff --shortstat -w "$C3" "$C4" | sed 's/^ *//')"
    printf '  commit 4, the feature   raw : %s\n' "$(git diff --shortstat    "$C3" "$C4" | sed 's/^ *//')"
    printf '  commit 3, the refactor  -w  : %s\n' "$(git diff --shortstat -w "$C2" "$C3" | sed 's/^ *//')"
    printf '  commit 3, the refactor  raw : %s\n' "$(git diff --shortstat    "$C2" "$C3" | sed 's/^ *//')"
    echo

    # commit 3 must not be the feature, and commit 4 must not be a reformat
    if [ -z "$(git diff --shortstat -w "$C2" "$C3")" ]; then
      warn "commit 3 changes nothing once whitespace is ignored — is your refactor really in there?"
    fi
    OVERLAP=$(comm -12 \
      <(git diff --name-only "$C2" "$C3" | sort) \
      <(git diff --name-only "$C3" "$C4" | sort) 2>/dev/null | wc -l | tr -d ' ')
    [ "${OVERLAP:-0}" -gt 0 ] && warn "commits 3 and 4 touch $OVERLAP file(s) in common — expected, but check the refactor did not leak into the feature commit"
  fi
fi

echo
echo "================================================================"
if [ "$FAILED" -eq 0 ]; then
  echo "No failures. Open the PR against your own fork's $BASE, then fill in the sheet."
else
  echo "Fix the [FAIL] lines above and run this again."
fi
exit "$FAILED"
