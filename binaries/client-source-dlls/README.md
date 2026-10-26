# DLLs found in the 2.20 client source tree

Prebuilt DLLs that sat beside the 2.20 client source, with no source of their own. None of them is loaded by the 2.20
client as it is now, and the shipped 2.02 and 2.23 `HelGame.exe` import none of them except `search.dll`. Details
of the rendering helpers are in 2.20-Documentation `research/client/rendering/blitters.md` ("Helper DLLs shipped
with the source"). Exports and imports below were read from the PE headers; purposes are *inferred* from names,
imports and strings, not from a decompile.

| DLL | Built (PE, UTC) | Exports | What it appears to be | Used by 2.20? |
|---|---|---|---|---|
| `DesDll.dll` | 2001-01-17 07:11, VC6 (linker 6.0), release | `?DecryptFunc@@YGHPAD0@Z` = `BOOL __stdcall DecryptFunc(char *cipher_text, char *plain_text)` | A one-function decrypt helper, by its name DES. It is a small MFC application statically linked (most of its 131 KB and nearly all imports are MFC runtime), with no strings of its own beyond the export. `DesDll.lib` is its import library. | **No.** The only reference is a commented-out prototype, `//BOOL WINAPI DecryptFunc(LPTSTR cipher_text, LPTSTR plain_text );` (Client `Wmain.cpp:67`). Nothing calls it, and the shipped executables do not import it. What it once decrypted is unknown. |
| `MMXCPY.DLL` | 2001-04-05 08:26, VC6 debug build | `mmxcpy` | An MMX memory copy: contains `CPUID`, `MOVQ` and `EMMS`, the pattern of detect-the-CPU then copy 8 bytes at a time. Debug CRT only, no other imports. PDB `C:\My dll\dll 창고\MMXCPY\Debug\mmxcpy.pdb`. | **No.** No source references it. |
| `Offs.dll` | 2001-04-06 08:28, VC6 debug build | `OFFS_Init`, `OFFS_Create_Offs`, `OFFS_Get_Offs`, `OFFS_Get_Width`, `OFFS_Get_Height`, `OFFS_Blt_ColorKey`, `OFFS_Blt_ColorKey_Rect`, `OFFS_ReleaseAll` | An off-screen surface manager with colour-keyed blits. PDB `C:\My dll\dll 창고\Offs\Debug\Offs.pdb`. | **No.** The client's own `DXC_ddraw` and `CSprite` do this. |
| `DDRAW7.DLL` | 2001-03-21 01:47, VC6 debug build | `DDRAW7_Init`, `_Flip`, `_Get_pDD`, `_Get_pDDSPrimary`, `_Get_pDDSBack`, `_Release` | A DirectDraw 7 setup and flip wrapper. **Not a Microsoft file**, despite the name. PDB `C:\b-\dll-\DDRAW7\Debug\DDraw7.pdb`. | **No.** |
| `Dib16.dll` | 2001-08-27 11:14, VC6 debug build | `DIB16_Init`, `_Load`, `_Get_Bits`, `_Get_bi`, `_Get_Pos_Add`, `_Set_Pixel_hdc`, `_ReleaseAll` | Loads DIBs into 16-bit buffers. **Not a Microsoft file.** PDB `C:\b\My dll\dll 창고\Dib16\Debug\Dib16.pdb`. | **No.** |
| `ClientLogon.dll` | | COM type library (`#import`, Client `Wmain.cpp:28`) | Korean billing / login object. | Only on the Korean login path; dropped for this project. |
| `search.dll` | | `__FindHackingDll__` | The anti-cheat process check. | Removed from the client; see `decompile/search-dll/`. |

`MMXCPY`, `Offs`, `DDRAW7` and `Dib16` share one origin: debug builds (debug CRT, `dbgheap.c`, `HeapValidate`), no
version resource, PDBs in a personal folder `dll 창고` ("dll storage"), built March to August 2001. They look like
an earlier or experimental rendering layer by the same team that the 2.20 client replaced with its own code.

No full decompile was made of `DesDll`, `MMXCPY` or `Offs`: nothing in 2.20 uses them, and the binaries are kept
here as found.
