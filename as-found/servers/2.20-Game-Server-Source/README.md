# 2.20-Game-Server-Source

Source code of the Helbreath 2.20 game server (HGserver, Visual C++ 6).

## Contents

Transferred from the librehelbreath import of the leaked 2.20 server source (commit `1697e41`, "Imported HGServer 2.20 sources").
Its configuration and data files are in [2.20-Game-Server-Files](https://github.com/Original-Helbreath/2.20-Game-Server-Files).
The shipped switches build the binary-save (DB) mode and report version 2.19; a 2.20 setup needs a FILE-mode build.

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
