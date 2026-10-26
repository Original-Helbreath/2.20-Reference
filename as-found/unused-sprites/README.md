# Unused sprites

Sprite paks that shipped in the client (retail 2.02 install, unchanged by the 2.23 patch) but that the 2.20 game
never shows. They were moved here from 2.20-Client-Files so the client data holds only what the game uses. Each
folder has the same layout as Client-Files: `<name>.pak/<name>.pak` as found, and `extracted/` with one PNG and one
frame JSON per sheet, made with `pak/pak_tool.py` from [Tools](https://github.com/Original-Helbreath/Tools).

| Pak | What it is | Why it is unused |
|---|---|---|
| `MHelm2.pak` | Male helm type 2: the Full Helm (`MHelm1`) in copper or bronze. Same shape and shading as the Full Helm (silhouette overlap 0.87, shading correlation 0.81 across all 768 frames). | The client loads it (`Game.cpp:4075`), but a helm type comes only from the equipped item's appearance value, and no item in the 2.03 or 2.20 configs (nor 3.51) has helm type 2. |
| `MHelm3.pak` | Male helm type 3: the Full Helm shape (overlap 0.82) with different surface shading, in a dull grey-green metal. | Loaded (`Game.cpp:4076`), but no 2.03 / 2.20 / 3.51 item has helm type 3. The later 3.82 forum server reused type 3 for two male-only helms (Merien Helm M, Drow Helm M). |
| `MHelm5.pak` | Male helm sheets numbered 5. | The 2.20 client never loads it; type 5 is drawn from `NMHelm1` (`Game.cpp:4078`). |
| `WHelm2.pak`, `WHelm3.pak`, `WHelm5.pak` | Empty files (0 bytes), the female counterparts of the three above. Nothing to extract. | Never loaded (`Game.cpp:4165-4170` loads only `WHelm1`, `WHelm4` and `NWHelm1-4`). No female item ever needed them; they look like placeholders that kept the numbering aligned. |

Removing these from the client is safe: `CGame::MakeSprite` (`Game.cpp:3548`) skips a pak it cannot open.

Helm sprites are picked as `9300 + 15 * type` (male) and `19300 + 15 * type` (female), with the type taken from bits
4-7 of the appearance value (`Game.cpp:8398`, `8489`); the game server sets those bits only from the equipped head
item's `m_cApprValue` (server `Game.cpp:16290-16300`). To see type 2 or 3 in game, a head item would need appearance
value 2 or 3, and it would show on male characters only.
