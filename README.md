# 2.20-Reference

Reference material for Helbreath 2.20: precompiled binaries, kept as found and never run from here.

## Contents

- `binaries/client-2.02/` - `Helbreath.exe`, `HelGame.exe`, `Uninstal.exe`, `search.dll`, `zlib.dll` from the retail 2.02 install.
- `binaries/client-2.23/` - `HelGame.exe` from the official 2.23 patch.
- `binaries/main-log-server/`, `binaries/world-log-server/` - the official 2002-era `MainServer.exe` and `WorldServer.exe`,
  unpatched. No source exists for these; they are what the decompile in the log-server Source repos was made from.
- `binaries/client-source-dlls/` - prebuilt DLLs that shipped inside the 2.20 client source tree (`ClientLogon.dll`,
  `DDRAW7.DLL`, `DesDll.dll`, `Dib16.dll`, `MMXCPY.DLL`, `Offs.dll`, `search.dll`), with no source of their own.
  Its `README.md` says what each appears to be and that the 2.20 client uses none of them.

- `libraries/client-source/`, `libraries/gate-source/` - prebuilt link libraries (`.lib`) that sat in the source trees.
  Most are Windows / DirectX SDK import libraries (`DDRAW`, `DINPUT`, `DSOUND`, `DXGUID`, `WINMM`, `WS2_32`, `WSOCK32`,
  `IMM32`) that a build should take from a downloadable SDK instead; `Cint.lib`, `DesDll.lib`, `search.lib` and
  `StrTok.lib` are the game's own and need their sources found or recreated (see the project board).
- `decompile/launcher/` - which 2.02 exe the client source builds (`HelGame.exe`), and a decompile of the
  `Helbreath.exe` FTP auto-update launcher (protocol, files, how it starts the game) with cleaned pseudo-C.
- `decompile/search-dll/` - search.dll, the client's anti-cheat process check: what both builds do, with Ghidra decompiles. The client no longer loads it.
- `decompile/cint/` - Cint.lib (`CInt`, the memory-obfuscated int): what it did, with the disassembly. The client now uses plain `int`; real protection is the server owning and validating values.
- `as-found/client-2.02/Readme.htm` - the readme from the retail 2.02 install.
- `as-found/unused-sprites/` - client sprite paks the 2.20 game never shows (`MHelm2`, `MHelm3`, `MHelm5`, and the
  empty `WHelm2`, `WHelm3`, `WHelm5`, and `MPTWK`), with their sheets extracted and a note on what each is and why it is unused.
- `as-found/sql-3.51/` - SQL Server scripts of the database-backed log servers (3.51 era): the account database
  schema, the character database's index script, and the crusade / Heldenian tables and procedures.
- `as-found/game-server-import/` - files from the 2.20 server import that no program reads: runtime logs from 2000
  (`Events.log`, `ItemEvents.log`, `Level-Chart.log`, `LogEvents.log`, `TEST.LOG`, `ascII.txt`) and a 2.03 `GateServer.cfg`
  that sat in the game server folder.

The Files repos hold only configuration and data each program reads; anything compiled, or kept only as evidence, lives here.

This repo is a plain reference store with a single `master` branch, unlike the buildable repos.

## Project

Part of [Original Helbreath](https://github.com/Original-Helbreath): keeping the original 1999-2005 Siementech Helbreath alive.
Repos are named `<version>-<Part>`; other versions follow the same pattern.
Issues are tracked on the [project board](https://github.com/orgs/Original-Helbreath/projects/1). The analysis behind everything is in
[2.20-Documentation](https://github.com/Original-Helbreath/2.20-Documentation).
