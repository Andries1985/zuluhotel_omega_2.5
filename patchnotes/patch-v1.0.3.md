# Patch Notes — v1.0.3
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: May 15, 2026**

---

Welcome to **Patch 1.0.3**. This patch is mostly about making **player vendors easier to manage** and making bank totals more accurate.

---

## 🛒 Player Vendors — What Changed

### New: `.escrow` (Claim Escrow Packages)

A new command, **`.escrow`**, lets you claim packages saved by your vendor.

When you open `.escrow`, you can choose to send each package:
- **To Bank**
- **To Backpack**

Use this whenever your vendor has sent assets to escrow instead of directly into your bank.

### Vendors Now Have Real Ongoing Costs

Player vendors now charge wages over time.

- **In city regions:** wage is based on **40,000 gold/month**
- **Outside city regions:** wage is based on **10,000 gold/month**

The system applies this automatically in daily increments.

### Current City Regions (40k Wage Tier)

- Moonglow
- Moonglow Keep
- Britain
- Jhelom
- Yew
- Empath Abbey
- Minoc
- Trinsic
- Skara Brae
- Magincia
- Ocllo
- Buccaneer's Den
- Nujelm
- Vesper
- Cove
- Serpent's Hold
- Lord British's Castle
- Lord Blackthorne's Castle
- Elven Town
- Randorin
- Delucia
- Papua

### If a Vendor Runs Out of Gold

If your vendor cannot keep up with wages:
- The vendor starts tracking unpaid wage debt
- You get warning messages from the vendor when you interact
- If the debt remains unpaid long enough (about 60 days), the vendor will **dismiss itself automatically**

This prevents vendors from lingering forever in an unpaid state.

### What Happens to Your Stuff if the Vendor Closes

When a vendor is fired or auto-dismisses, it now performs a cleaner closeout:
- Vendor inventory and held assets are cashed out
- Gold payouts are converted into cheques as needed
- Assets are delivered to your bank when possible
- If direct delivery is not possible, items are stored in **Escrow** for pickup via `.escrow`

In short: if delivery fails in the moment, you still have a recovery path.

---

## 🏦 Banking — Balance Now Includes Checks

When using banker balance checks, your reported total now includes qualifying bank checks in addition to coin stacks.

Result: your displayed bank balance should better match your actual stored value.

---

## ⚔️ Stability and Cleanup

Several command and speech-hook updates were made to reduce noisy behavior and improve day-to-day reliability.

Player-visible effect: fewer odd edge cases around vendor support flows.

---

## 🏟️ PvP Arena — Fixes and Improvements

During development, several PvP arena issues were identified and corrected:

- **Single-Arena (1v1):** Match flow and mechanics stabilized for more reliable gameplay.
- **2v2 Arena:** Comprehensive fixes to team coordination, match logic, and error recovery.
- **Prots Display:** The `.prots` command now correctly shows your **Equipped INT** bonus for mage-class characters, making it easier to see your magic equipment's contribution.

---

## 🛠️ Administrator Highlights (High-Level)

For shard administrators and event staff, Patch 1.0.3 also includes:
- Spawnpoint tool command promotions from test scope into admin scope
- Updates to mob/spawnpoint navigation commands
- `eraseEmptyAccounts` command relocation/update in active command routing
- Expanded player-merchant status and vendor maintenance tooling

---

## 📋 Summary

✅ **Escrow Added** — Use `.escrow` to recover vendor packages  
✅ **Vendor Upkeep Added** — Vendors now cost gold over time (city and non-city wage tiers)  
✅ **Auto-Dismiss Safety** — Unpaid vendors can close themselves; assets are cashed out with escrow fallback  
✅ **PvP Arena Fixes** — 1v1 and 2v2 arena stabilization; prots display correction  
✅ **Economy Clarity** — Banker balance now counts checks too

---

**Thank you for playing Zuluhotel Omega 2.5! Report bugs on Discord.**
