# 2.20-Game-Server-Files

Configuration and data files for the Helbreath 2.20 game server (HGserver).

## Contents

Only the files the game server reads from its own disk, as found in the librehelbreath import of the 2.20 server
(commit `1697e41`):

- `gserver.cfg` - the server's own settings (name, addresses, ports, the maps it hosts).
- `Crusade.cfg` - crusade structure placement.
- `GameData/CrusadeGUID.Txt` - crusade state the server rewrites at run time.

Everything else comes from elsewhere:
- **Maps** (`mapdata/*.amd` and the per-map `.txt` files) come from [2.20-Map-Files](https://github.com/Original-Helbreath/2.20-Map-Files).
- **Game content** (items, NPCs, magic, skills, quests, potions, crafting, duplicate-item ids) is never read from disk by the
  game server: the log server reads those files and pushes them over the network at start-up. They live in
  [2.20-World-Log-Server-Files](https://github.com/Original-Helbreath/2.20-World-Log-Server-Files).
- `WarSchedule.cfg` and `notice.txt` are also read from disk, but no 2.20 copy exists (see the project board).

The configs are **2.03** copies; no official 2.20 copy is known.

## Branches

| Branch | Purpose |
|---|---|
| `original` | The files exactly as found. Read-only by convention: never commit here. (It is not protected yet, because the org is on GitHub Free and its repos are private.) |
| `development` | Integration branch. Work happens in feature branches, which merge into `development`. |
| `stable` | Default branch. `development` is merged here once it is stable. |

## Project

Part of [Original Helbreath](https://github.com/Original-Helbreath): keeping the original 1999-2005 Siementech Helbreath alive.
Repos are named `<version>-<Part>`; other versions follow the same pattern.
Issues are tracked on the [project board](https://github.com/Original-Helbreath/projects/1). The analysis behind everything is in
[2.20-Documentation](https://github.com/Original-Helbreath/2.20-Documentation).
