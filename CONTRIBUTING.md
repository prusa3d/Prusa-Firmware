# Contributing

| Status | Date       | Project Version |
|--------|------------|-----------------|
| Active | 2026-04-11 | —               |

Practical guide for contributors working on this project.

---

## Adding or changing a rule

Rules live in the project's primary rules file (e.g. `CLAUDE.md` or equivalent). Each rule should be:

- **Actionable** — specific enough that an agent or human can follow it without interpretation.
- **System-agnostic** — no language, toolchain, or platform assumptions unless the rule is explicitly scoped to one.
- **Self-contained** — the rule must make sense without reading surrounding rules.

For a significant rule change (new section, removal, or a change in intent), write an ADR in `docs/adr/` first. For minor clarifications (wording, examples), edit the rules file directly.

---

## Adding a new agent or tool integration

Agent- and tool-specific instruction files live alongside their respective tooling. Each integration should be a thin file that summarises the key rules and points to the primary rules file as the source of truth. Do not duplicate rule content — only summarise and link.

---

## Branches and commits

Use short Makefile targets for routine commits where available. For non-trivial work, use a descriptive branch name and a proper commit message.

---

## ADRs

Every decision that affects the rules, scaffold structure, or tool integrations warrants an ADR. See the project rules file for the numbering and format rules.
