# Launcher decompile: Helbreath.exe (retail 2.02)

Static analysis only; no binary was executed. Tags: [V] verified (read from the binary or the source),
[I] inferred, [U] unverified. Addresses are virtual (image base 0x400000). Ghidra program names:
`Helbreath_202.exe` (analysed); the two `HelGame.exe` builds were examined with pefile and string dumps only.

## Contents

- Identification verdict
- Purpose
- Start-up flow
- Update/patch protocol
- Files and registry
- How it launches the game
- Open questions
- Pseudo-C and skipped library code

## Identification verdict

**`HelGame.exe` is the game client that `sources/client-2.20` builds [I]. `Helbreath.exe` is a separate
auto-update launcher (internal name `mfcftp`) that has no source in our trees.** The build target is
inferred: the source has no project file and never names `HelGame.exe`; the string and import matches
below are [V].

| Evidence | HelGame.exe 2.02 | Helbreath.exe 2.02 |
|---|---|---|
| Imports | DDRAW, DINPUT, DSOUND, WINMM, WS2_32, IMM32, `search.dll` ord 1 [V] - the DirectX/Winsock set `Wmain.cpp` and `DXC_*.cpp` use | WININET (FTP only), MFC dependencies (OLE, COMCTL32, WINSPOOL, comdlg32), WSOCK32 for `AfxSocketInit` [V] |
| Version resource | none [V] | `mfcftp MFC Application`, `mfcftp.EXE`, 1.0.0.1, "Copyright (C) 2000" [V] |
| Source-unique strings | mutex `0543kjg3j31%`, "Only one Helbreath client program allowed!", class format `Client-I%d`, `login.cfg`, `contents\ItemName.cfg`, "Helbreath V%d.%d" - all in `Wmain.cpp` / `Game.cpp` [V] | none of these; instead `update.helbreath.com`, `update.lst_`, `HelGame.exe`, "Helbeath Auto-Update" [V] |
| PE timestamp | 2001-08-21 06:59:20 UTC [V] | 2001-08-21 07:18:17 UTC (built 19 min later, same VC6 linker 6.0) [V] |

The window title "Helbreath Crusade" from `Wmain.cpp:330` is absent from the 2.02 HelGame.exe [V]: the 2.02
build is older than the 2.20 source in at least that string (also `DEF_LOWERVERSION` is 19/20 in the source).

**2.23 HelGame.exe** (2003-04-15 06:33:22 UTC) is packed [V]: unnamed sections, an empty `.adata` section,
entry point at the start of the first section, and one import per DLL (kernel32 only `LoadLibraryA`/
`GetProcAddress`/`GetModuleHandleA`). A later analysis identified the protector as ASProtect 1.2x
(entry bytes `68 01 20 56 00 E8 01 00 00 00 C3 C3`, the ASPack 2.12 loader reused as the outer layer,
polymorphic decryption at 0x562057), not plain ASPack [I]. Apart from the import names (`search.dll`
among them) no application strings are readable [V]; the mutex, `login.cfg` and every message string are hidden,
so a string comparison with the 2.20 source is not possible without unpacking. It still imports `search.dll`
ordinal 1 and the same DLL set (DDRAW, DINPUT, DSOUND, IMM32, WINMM, WS2_32) [V], so it is the same program
family; how far it diverges from the 2.20 source is [U].

## Purpose

A small MFC dialog ("Helbreath Auto-Update") with a status text, a progress text and a progress bar. Intent:
bring the install up to date from Siementech's FTP server, then start `HelGame.exe` [V]. It is the program the
user starts; the game itself never contacts the update server [I].

## Start-up flow

1. `CMfcftpApp::InitInstance` 0x4010b0: `AfxSocketInit`, on failure "Windows sockets initialization failed."
   and exit; `AfxEnableControlContainer`, `Enable3dControls`, modal `CMfcftpDlg` (IDD 0x66) [V].
2. `CMfcftpDlg::OnInitDialog` 0x4013d0: progress range 0..100, step 2, position 0; then
   `AfxBeginThread(UpdateThread, this)` via 0x4016b0 [V].
3. `UpdateThread` 0x4016d0 does the whole update, launches the game, and ends the dialog with
   `EndDialog(IDOK)` (0x4024f0) [V].

There is no in-app cancel: the cancel flag 0x43b0f0 is only ever read (0x4024e0) [V], so it is always 0.
Closing the window runs the stock wizard OnClose/OnCancel (0x401650/0x401670); `CanExit` 0x401690 returns 1
when `m_pAutoProxy` (+0xa0) is 0, so the dialog ends, the process exits mid-update and the game is not
launched [I].

## Update/patch protocol

All transfers are FTP through WinINet [V]:

- `InternetOpenA("SIEMENTECH FTP ", INTERNET_OPEN_TYPE_DIRECT, ...)` 0x402360 (user agent has a trailing space).
- `InternetConnectA("update.helbreath.com", port 0 = 21, user "helsmt", password "helsmt",
  INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE)` 0x402380. The credentials are one literal at 0x436448 used
  for both [V].
- `FtpSetCurrentDirectoryA("patch_en")` 0x4023c0 - English patch tree [V]. Other regions presumably used
  other directories [I].
- Download `update.lst_` (binary) to the install dir, 0x4023e0; gunzip it to `update.lst` with zlib 1.0.4
  (`gzopen`/`gzread`), 0x4025c0 [V].
- `update.lst` is an array of 228-byte (0xe4) records [V]:

  | Offset | Size | Field |
  |---|---|---|
  | 0x00 | 110 | local path of the game file, relative to the current directory |
  | 0x6e | 110 | name of the gzip'd file on the server (relative to `patch_en`), also its temp local name |
  | 0xdc | 4 | expected modification time (`time_t`) |
  | 0xe0 | 4 | expected file size in bytes |

- Pass 1 (0x401bd0): for each record, `IsFileCurrent` 0x402040 compares the local file with
  `CFile::GetStatus`: year, month, day, hour, minute and size must match; **seconds are not compared** [V].
  Stale or missing entries are copied to `update.tmp`, sizes summed.
- If nothing is stale: done, success [V]. If the total's top bits exceed 0xc800000 (about 200 MB) it shows
  "file size = N" and fails with "You need to download too many files. Please use homepage." [V].
- Pass 2: for each entry, create the gz name's directory, `FtpOpenFileA(name, GENERIC_READ, BINARY)`, stream
  it in 4 KB reads to a local file of the same name, updating "Download : %s(%dK)" and
  "%dK download complete(Full %dM)" and the bar [V]. Then `ExtractEntry` 0x402140: create the target's
  directory, open it (stamped 1980-01-01 first, 0x402230), gunzip the .gz into it (0x402620), set its
  access/modify time to the record's `time_t` with `_futime` (0x402330), delete the .gz [V].
- There is no checksum, signature or version number anywhere: freshness is mtime + size only [V].
- No file deletion list, no self-update of `Helbreath.exe` beyond what the list may overwrite [I] (a list
  entry for `Helbreath.exe` would fail to open while running [I]).

Error results and messages (OK/Cancel boxes; all but the `InternetOpen` one end "Would you like to start
game?") [V]:
-1002 list could not be opened ("making download list"), -1001 too big, -1003 download failed,
-1004 extract failed ("check you have enough disk space"), connect failure, `update.lst_` download failure,
`InternetOpen` failure (string 0x4363cc: an English line, then the question in Korean, CP949 "run the
client?", not "start game?").

Robustness notes [V from disassembly]: a failed `InternetReadFile` writes zero bytes and retries forever
(only TRUE with 0 bytes ends the loop, 0x402494 and the same pattern in 0x401bd0); the return of the
`update.lst` gunzip is ignored; the FTP handle is closed twice on error paths and also on success
(0x402011 inside 0x401bd0, then 0x4019f4); the progress bar uses 32-bit `bytes*100` (0x401f27-0x401f34),
which wraps once the running byte count passes about 42 MB.

## Files and registry

- Reads/writes in the current directory: `update.lst_`, `update.lst`, `update.tmp`, each server gz file
  (temporary), and every game file named in the list, creating directories with `_mkdir` [V]. All three
  `update.lst_` and `update.lst` are deleted at the end [V]. `update.tmp` is deleted only after a pass-2 run
  or an extract failure: it stays (empty) on a normal up-to-date run (0x401cd1 JZ 0x40202c skips the remove)
  and stays on a download failure (0x401fc4) [V].
- No `LOGIN.CFG`, no INI, no `WritePrivateProfileString` call in app code (the import is MFC's) [V for app
  code 0x401000-0x402700].
- No registry use: no `SetRegistryKey` in `InitInstance`; the ADVAPI32 imports are MFC's registration
  helpers [I]. The game client writes `HKCU\Software\Siementech\Helbreath\Settings` itself (`Game.cpp:330`) [V].
- Nothing to do with `search.dll` or `zlib.dll`: it neither imports nor loads them; zlib 1.0.4 is statically
  linked [V]. (The shipped `zlib.dll` is not used by this launcher [I].)

## How it launches the game

After the update, whatever its outcome, 0x401a36: copy `GetCommandLineA()`, take the text after the last
`"` (or, if none, after the first space), trim spaces, and build `"HelGame.exe <args>"` or `"HelGame.exe"`;
if the result was success or the user pressed OK, `WinExec(cmd, SW_SHOW)` [V]. No path is prepended, no
existence or version check, no CRC of HelGame.exe, no handle is kept [V]. The pass-through arguments become
HelGame's `lpCmdLine`, which `Wmain.cpp` passes to `Initialize()` [V for the source; which arguments the
retail shortcut used is U].

## Open questions

- [U] The real contents of `patch_en/update.lst_` (server long gone); the record layout above is from the
  reader only.
- [U] Whether other locales shipped this launcher with a different directory (only `patch_en` is present).
- [U] Which program uses the shipped `zlib.dll` - neither HelGame.exe 2.02 nor this launcher imports it.
- [U] 2.23 HelGame.exe vs 2.20 source: needs unpacking (ASProtect 1.2x [I]) before any string or code comparison.
- [U] Whether a 2.23 launcher existed; only HelGame.exe ships in the 2.23 patch.

## Pseudo-C and skipped library code

`pseudo-c/launcher.c` holds the app's own logic in 0x4010b0-0x402690 (InitInstance, dialog, thread, list
processing, FTP and gzip helpers), cleaned and renamed. Not every function has a body there: the dialog
ctor 0x4011f0, the dtors 0x4012b0-0x4012f0 and the one-line wrappers 0x4023b0, 0x4023c0, 0x4024e0 and
0x4024f0 are only described or inlined at their call sites, and 0x4010b0 is not a defined function in
Ghidra (rebuilt from disassembly).

Skipped, with how they were recognised:

- MFC 4.2 static library (0x41f000 onward, plus 0x401000-0x401190 wizard boilerplate: app ctor 0x401010,
  thread dtor 0x401060, `CDialog::~CDialog` 0x4011b0, scalar-deleting dtors): RTTI names `CWinApp`,
  `CDialog`, `CProgressCtrl`, `COleControlSite` etc., MFC message/error resource strings, Ghidra's own
  `CWinThread::~CWinThread`/`CDialog::~CDialog` labels, and the AppWizard pattern (`AfxSocketInit`,
  `Enable3dControls`, automation proxy at +0xa0, "About mfcftp..." menu).
- MSVC 6 CRT (0x410000-0x41ef00: `sprintf` 0x410827, `strchr`/`strrchr`, `_open` 0x414ae3, `_read` 0x416544,
  `_write` 0x416782, `_close` 0x414a03, `_splitpath` 0x410bd8, `_mkdir` 0x41eae7, `remove` 0x41eadc,
  `_futime` 0x4108af, `mktime` 0x4109e6, `fopen`/`fclose`, `__chkstk` 0x410880): CRT error strings
  ("Runtime Error!", errno texts) and call shapes.
- zlib 1.0.4 (0x4026b0-0x40400x: `gzopen` 0x402690, `gzdopen` 0x4028b0, `gzread` 0x402b40, `gzclose`
  0x402e40, inflate 0x403170...): version string "1.0.4", the deflate/inflate copyright banners and the
  zlib error strings ("incorrect header check", "invalid window size").
