# wrench

A set of modding tools for the Ratchet & Clank PS2 games.

## Screenshots

![Level editor](screenshots/editor.png)
![Texture browser](screenshots/texture-browser.png)

## Features

- View levels from Ratchet & Clank 2 and 3.
- Store mods as .wrench files that double as project files and compression-aware patch files.
- Extract and replace certain textures.
- Extract racpak (*.WAD) archives.
- Decompress and recompress WAD segments (not to be confused with the *.WAD files on the game's filesystem).
- A number of command-line tools for testing.

## Compatibility

This project is probably not very usable yet, however the following game
releases are currently somewhat supported:

| ELF Identifier | Title                              | Edition     | Region | Status                     |
|----------------|------------------------------------|-------------|--------|----------------------------|
| SCES_516.07    | Ratchet & Clank: Locked and Loaded | Black Label | Europe | Skins, HUD images, levels. |
| SCES_524.56    | Ratchet & Clank 3                  | Black Label | Europe | Skins, levels.             |
| SCES_532.85    | Ratchet: Gladiator                 | Black Label | Europe | Skins only.                |

## Building

### Ubuntu 18.04

1.	Install dependencies and tools:
	> sudo apt install git cmake g++ libglew-dev libboost-all-dev libglfw3-dev libglm-dev python3 python3-pydot graphviz
2.	Download the source code using Git:
	> git clone https://github.com/chaoticgd/wrench

3.	Download the remaining dependencies using Git:
	> git submodule update --init --recursive
	
2.	Build it with cmake:
	> cmake . && cmake --build .

### Windows

Not yet.
