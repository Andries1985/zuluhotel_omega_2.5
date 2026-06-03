# Latest Changes
Always check Discord announcements for all the patch notes.

## What Changed

## 🔮 Talisman of ID Improvements

Using a Talisman of ID is now more reliable and fair:

- Canceling or failing an ID attempt no longer incorrectly advances talisman usage.
- The ID cooldown timestamp is only applied when an ID action actually succeeds.
- Clearer feedback is now shown when identification is canceled, blocked, or fails.
- Container identification now tracks whether at least one item was successfully identified.

---

## 🐾 Tamed Pet AI Fixes

Tamed pets now behave more consistently when their master dies and revives:

- Pets now cleanly stop active guard/follow combat state when their master dies.
- Pets remain in self-defense mode instead of getting stuck in old attack queues.
- On master revive, pets reset stale combat state so commands work predictably again.
- Follow command speech handling was cleaned up to avoid target-name mismatch edge cases.

---

## 🐎 Stablemaster and Animal Trainer Safeguards

Several safety and quality fixes were applied to trainer/stable flows:

- Added null-creation checks when spawning pets from trainer/ticket workflows.
- If pet recreation fails from a ticket, the ticket is safely returned to your backpack.
- Fixed item-return container calls in trainer flows to prevent misplaced return items.
- Added cancel handling when a stable target selection is aborted.

---

Thanks for playing Zuluhotel Omega 2.5.
