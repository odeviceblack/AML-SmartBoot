# FastBoot

> An AML mod for **GTA: SA Android** that dramatically reduces the time between launching the game and actually playing it.

Originally started as a simple Social Club skip utility, FastBoot evolved into a complete boot management system — allowing you to automatically resume progress, start a new game, open the load menu, and fully customize the game's startup behavior.

---

## Features

- Skip Social Club and EULA screens
- Automatic fullscreen mode *(optional)*
- Configurable startup behavior
- Automatic save slot resolution
- Resume the latest valid save instantly
- Start a new game automatically
- Open the Load Game menu automatically
- Advanced logging system with timestamps
- Lightweight and optimized implementation
- Built for AML (Android Mod Loader)

---

## Requirements

- [GTA: SA Android (AML)](https://discord.gg/aml-android-mod-loader-877187821511794730)

---

## Installation

1. Copy `libFastBoot.so` to the AML mods directory.
2. Launch the game.
3. Configure FastBoot through its generated configuration file.

---

## Configuration

FastBoot generates a configuration file on first launch. Example:

```ini
[Preferences]

; Enables log output to a file.
Log Output = 0

; Skips Social Club and EULA screens.
Eula Skip = 1

; Forces immersive fullscreen mode.
Force Full Screen = 0

; Startup behavior.
Start Mode = auto

; Save slots checked from left to right.
Slot List = GTASAsf9.b GTASAsf10.b
```

---

## Startup Modes

FastBoot supports multiple startup modes via the `Start Mode` key.

| Mode | Behavior |
|------|----------|
| `none` | Disables all startup automation. |
| `newgame` | Starts a new game automatically. |
| `loadgame` | Opens the **Load Game** menu automatically. |
| `loadslot` | Loads the first valid save slot found in the configured slot list. |
| `auto` | Attempts to resume a valid save. If none is found, starts a new game. |
| `auto2` | Attempts to resume a valid save. If none is found, opens the Load Game menu. |

---

## Save Slot Resolution

FastBoot evaluates the configured save slots **left to right**. The first valid slot found determines the action to perform.

```ini
Slot List = GTASAsf9.b GTASAsf10.b
Slot List = GTASAsf3.b GTASAsf5.b GTASAsf10.b
```

---

## Logging

When `Log Output = 1`, FastBoot writes diagnostic information to:

```
log_FastBoot.txt
```

Log entries include:

- Timestamps
- Log priorities
- Startup information
- Error reporting
- Internal state useful for debugging

---

## Version History

FastBoot is the third generation of this project:

| Generation | Name |
|------------|------|
| 1st | SCAndSkip |
| 2nd | SmartBoot |
| 3rd | **FastBoot** |

---

## License

This project is provided **as-is** for educational and modding purposes.

Use at your own responsibility.
