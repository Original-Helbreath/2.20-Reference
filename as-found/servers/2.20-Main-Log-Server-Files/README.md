# 2.20-Main-Log-Server-Files

The official Main Log server binary and config used with Helbreath 2.20.

## Contents

`HMLServer.cfg`, unpatched, the config of the official 2002-era `MainServer.exe` (its title reads v3.51, "Extasis fix").
The binary itself is kept for reference in [2.20-Reference](https://github.com/Original-Helbreath/2.20-Reference) until the server is recreated from source.
The Main Log server handles account logins (port 2848). Running the official binary with 2.20 needs the byte patches
documented in 2.20-Documentation (`research/servers/official-compat-220.md`).

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
