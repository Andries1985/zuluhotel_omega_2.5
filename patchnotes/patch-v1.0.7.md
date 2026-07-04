# Patch Notes - v1.0.7
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: July 4, 2026**

---

Welcome to **Patch 1.0.7**. This update focuses on a major **Townstone upgrades expansion**, **Globe of Sosaria travel improvements**, **Soul Whisperer summon-threshold reliability**, and new **staff visual testing tools** for animated graphics and hue previews.

---

## What Changed

## 🏛️ Townstones - Massive Upgrade Catalog Expansion

### Player Impact

- The Townstone upgrades catalog has been expanded from a small list into a large data-driven set covering many more options.
- Upgrade entries now support upgrade categories and animation flags, which are now surfaced in the upgrades interface.
- The upgrades UI now includes:
  - category selection,
  - richer row previews,
  - paging improvements,
  - direct page jump,
  - cleaner Buy flow.

---

## 🗳️ Townstone Elections - Production Duration Restored

### Player Impact

- Townstone election duration was changed from short testing timing back to a full live cadence.
- Election window is now **1 week**.

---

## 🧾 Player-Run Town Tracking and Controls

### Player Impact

- A new player-run towns configuration has been introduced with purchase-price definitions for configured regions.
- A new player command, **.playerruntowns**, shows player-run town status including purchase state, availability, donations, population, mayor, and feature toggles.
- Staff townbank status tools now include runtime toggles for:
  - player-town available state,
  - player-town purchased state,
  - alongside existing upgrades/donations controls.

---

## 🌍 Globe of Sosaria - Smarter Destination Logic

### Player Impact

- Globe cooldown was reduced from **24 hours** to **8 hours**.
- Globe now tries to return you to **your own townstone** first (if found), before falling back to a random eligible stone.
- Random destination eligibility now excludes **Randorin** for normal players.
- Globe search bounds were widened to cover full intended map range, improving destination discovery reliability.
- Success messaging now tells you whether it returned you to your townstone or sent you to a random one.

---

## 👻 Soul Whisperer - Summon Threshold Stability

### Player Impact

- Soul Whisperer threshold summons (75/50/25%) now use object properties instead of temporary script vars.
- This prevents repeated threshold-trigger behavior caused by runtime state loss.
- Summon thresholds now enforce a strict max of three threshold summons.

---

## 🏷️ Artifact Tooltip Labeling Follow-Up

### Player Impact

- Artifact-tagged items now display an artifact label and expiry status text in tooltip property flow where applicable.
- Expired artifacts show a clear "pending sweep" status message.

---

## 🧪 Staff/Dev Tooling - Animated Graphics Package

### Player Impact

- No direct gameplay change expected for regular players.
- New test commands were added for staff/developer workflows:
  - **.animatedgraphics** (spawn configured animated tile groups),
  - **.animationtest** (spawn graphic ranges),
  - **.colortest** (spawn hue ranges on a graphic).
- These tools were moved into a new `alryc` package, with generated animated-tile datasets and command synopses wired in.

---

## 🛠️ Internal - Townstone Upgrade Authoring Automation

### Player Impact

- No direct gameplay change expected.
- Added script/batch tooling to sync townstone upgrades between cfg and xlsx formats to support large upgrade-set maintenance.

---

## 📋 Summary

✅ Townstone upgrades expanded to a large categorized data set with animation metadata  
✅ Townstone upgrades gump reworked with category select, preview, and page jump  
✅ Election duration set to 1 week for live behavior  
✅ Player-run town pricing/config and status surfaces added  
✅ New `.playerruntowns` status command added  
✅ Globe of Sosaria cooldown reduced to 8 hours  
✅ Globe now prefers your own townstone, with smarter random fallback  
✅ Randorin excluded from random globe destinations for players  
✅ Globe search bounds expanded for full-world coverage  
✅ Soul Whisperer threshold summons now persist reliably via object properties  
✅ Artifact tooltip flow now shows artifact/expiry status where applicable  
✅ New `alryc` staff test commands for animated graphics and hue testing  
✅ Added townstone upgrades cfg/xlsx synchronization tooling

---

Thanks for playing Zuluhotel Omega 2.5.
