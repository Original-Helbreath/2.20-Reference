# 2.20-Gate-Server-Source

Source code of the Helbreath gate server, used with 2.19 and 2.20.

## Contents

Transferred from the librehelbreath gate server (commit `0b994cd`, "Sources for the Gate Server of Helbreath 2.19";
the 2.19 and 2.20 gates are identical). The gate is the message bus between game servers: chat, whispers, crusade,
GM commands, parties, user counts and shutdown. It does no login or map routing.

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
