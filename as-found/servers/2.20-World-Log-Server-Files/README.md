# 2.20-World-Log-Server-Files

The official World Log server binary and config used with Helbreath 2.20.

## Contents

`WLserver.cfg`, unpatched, the config of the official 2002-era `WorldServer.exe`. The binary itself is kept for
reference in [2.20-Reference](https://github.com/Original-Helbreath/2.20-Reference) until the server is recreated from source.
The World Log server handles characters, saving and map-to-server routing. Running the official binary with 2.20 needs a
one-byte registration-id patch, documented in 2.20-Documentation (`research/servers/official-compat-220.md`).

`game-server-push/` holds the game content the log server pushes to each game server at start-up (`Item*.cfg`, `Npc.cfg`,
`Magic.cfg`, `Skill.cfg`, `Quest.cfg`, `portion.cfg`, `BuildItem.cfg`, `DupItemID.cfg`, `WLserver.cfg`), plus the 2.03
log server's own `Lserver.cfg`. They are the **2.03** copies found in the 2.20 server import; names inside are Korean (cp949).
The 2.20 game server tests for the keyword `potion`, so the 2.03 `portion.cfg` will not load as-is. The official binary does
not send this push (see the project board).

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
