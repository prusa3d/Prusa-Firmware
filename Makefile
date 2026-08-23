SHELL := /bin/bash

# -----------------------------------------------------------------------------
# Usage
#
#   make <target> [args]
#
#   c              Stage all and commit with message "clean up"
#   d              Show unstaged diff
#   f              Stage all and create a fixup commit for HEAD
#   n              Stage all and commit with message "new feature"
#   p [msg]        Stage all, commit (default message: "wip"), and push
#                    e.g.  make p "fix login bug"
#   q              Pull the latest changes from the remote
#   r              Re-initialize project (prompts for name), then runs tests
#   s              Show git status
#   squash         Interactive rebase with autosquash against origin/main
#   t              Stage all and commit with message "temporary commit"
#   test           Run the test suite
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Standard commands
# Single-letter targets are intentional — these commands are typed constantly.
# Every keystroke saved compounds. Shift is effort. Brevity is the convention.
# -----------------------------------------------------------------------------

.PHONY: c d f n p q s squash t

c:
	git add .
	git commit -am "clean up"

d:
	git diff

f:
	git add .
	git commit --fixup HEAD

n:
	git add .
	git commit -am "new feature"

# Capture extra words after 'p' as the commit message (e.g. make p "my message").
# .DEFAULT absorbs the extra goal at execution time, avoiding eval which parses
# the string as makefile syntax and breaks on words like "include" or "define".
ifeq ($(firstword $(MAKECMDGOALS)),p)
  _P_MSG := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  ifneq ($(_P_MSG),)
.DEFAULT:
	@:
  endif
endif

p:
	git add .
	git commit -am "$(if $(_P_MSG),$(_P_MSG),wip)"
	git push

q:
	git pull

s:
	git status

squash:
	git rebase -i --autosquash origin/main

t:
	git add .
	git commit -am "temporary commit"


# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Custom user-defined commands
# Add your own targets below this line.
# -----------------------------------------------------------------------------

.PHONY: r

r:
	@echo "This is project specific, you'll have to define this yourself"
