# Latest Changes
Always check Discord announcements for all the patch notes.

## What Changed

## 🔮 Talisman of ID - Reliability Fixes

Talisman identification has been cleaned up so it behaves correctly in edge cases.

### Player Impact

- Failed or canceled ID attempts no longer incorrectly consume usage flow.
- ID cooldown timing is now only applied when an ID action actually succeeds.
- Better feedback is shown when an item cannot be identified.
- Container ID attempts now correctly track whether any item was actually identified.

---

## 🐾 Tamed Pets - Master Death/Revive Behavior

Tamed AI behavior has been improved so pets do not remain in bad state transitions around master death.

### Player Impact

- When a master dies, pets now cleanly drop stale guard/follow combat state.
- Pets stay in self-defense mode instead of keeping invalid queued attack targets.
- On master revive, stale command/combat state is reset so pet commands behave predictably again.
- Follow command target naming edge cases were corrected.

---

## 🐎 Animal Trainer & Stablemaster - Safety Fixes

Trainer and ticket recovery flows were hardened to avoid bad outcomes when pet creation fails.

### Player Impact

- Added safety checks when restoring/creating pets.
- If a ticket-based restore fails, the ticket is returned to your backpack.
- Item return logic in trainer flows was corrected for consistent behavior.
- Canceling stable targeting now exits safely.

---

## 📋 Summary

✅ Talisman ID flow now respects true success/failure  
✅ Tamed AI now handles master death/revive transitions more reliably  
✅ Stable/trainer recovery and return-item logic is safer and more consistent

---

Thanks for playing Zuluhotel Omega 2.5.
