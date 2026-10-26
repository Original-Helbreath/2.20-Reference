# SQL Server scripts (3.51 era)

SQL scripts for the database-backed log servers, kept byte for byte as found. They come from a 3.51-era fan
server pack (2003-2004, a Spanish-language operator), not from a 2.20 install: no 2.20-era database script is
known. What they mean for 2.20 is written up in 2.20-Documentation `research/servers/database.md`.

| File | Found in | What it is |
|---|---|---|
| `mainlogin.txt` | `351SqlFiles.zip` (the outpost download) | Full `CREATE` script for the account database `mainlogin`, used by the Main Log server: `ACCOUNT_T`, `WS1_T`, `WS2_T`, one index. |
| `RebuildIdx.sql` | `351configsandsqldump.rar` | Index rebuild for the character database `ws1`, used by the World Log server; it names its tables: `character_t`, `skill_t`, `item_t`, `bankitem_t`, `guild_t`, `guildmember_t`. |
| `CrusadeScript.SQL` | `351configsandsqldump.rar` | Crusade result tables and stored procedures. Korean comments, cp949. |
| `HeldenianScript.SQL` | `351configsandsqldump.rar` | The same for Heldenian, with a battle `type` column. Korean comments, cp949. |

The dialect is Microsoft SQL Server T-SQL (`GO`, `dbcc dbreindex`, `CREATE PROC`, `SMALLDATETIME`, `NCHAR`).
No script defines the character tables themselves (`CHARACTER_T` and the rest), and there is no data.
