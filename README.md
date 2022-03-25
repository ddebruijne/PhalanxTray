# PhalanxTray
Cross-Platform tray application for interfacing with Phalanx-based VFD and Nixie devices.

Features:
- Works on Windows, Mac and Linux with feature parity
- Saves settings to binary data format. Model selection and basic settings
- Content modes: user can switch between apps that can display data on the serial device

See it in action here:

[![](https://res.cloudinary.com/marcomontalbano/image/upload/v1647561020/video_to_markdown/images/video--a7d67cbef50c048a257de8aa0d997dca-c05b58ac6eb4c4700831b2b3070cd403.jpg)](https://i.imgur.com/aV1rzAj.mp4 "")

## Content Modes
- Time: Self-contained, default content mode displaying the current time
- FinalFantasyXIV: Simple forwarder, [danskidb/Dalamud-Klonk](https://github.com/danskidb/Dalamud-Klonk) is the sender plugin

## UDP Packets
Packets are formatted like `COMMAND|ContentModeId|parameter|parameter`

- The max length for a packet is 65536 characters.
- Uses port 11001
- If firewall the allows, can technically receive from across the network, but UDP does not guarantee whether the packet arrives, or the order. It's meant to run on localhost, where these things are not really a concern (but still allows for cross-platform)

### HELLO
Starts a new ContentMode:
```
HELLO|ContentModeId
```

### KEEPALIVE
Has to be called every 10 seconds by the content mode to make sure the content mode stays alive.
```
KEEPALIVE|ContentModeId
```

### SENDDATA
Sends new text to be displayed on the attached serial device 
```
SENDDATA|ContentModeId|TextToBeDisplayed
```

### GOODBYE
Sends new text to be displayed on the attached serial device 
```
GOODBYE|ContentModeId
```

## Libraries used
Cmake is used to modularly include all dependencies.
- [wjwwood/serial](https://github.com/wjwwood/serial) for communicating with serial device
- [chriskohlhoff/asio](https://github.com/chriskohlhoff/asio), wrapped for cmake by [OlivierLDff](https://github.com/OlivierLDff/asio.cmake) - to receive UDP packets from external apps on localhost
- [fmtlib/fmt](https://github.com/fmtlib/fmt) - formatting strings for the serial device, and logs.
- [emilk/loguru](https://github.com/emilk/loguru) - logging
- [danskidb/Tray](https://github.com/danskidb/Tray) - tray icon for configuration and lifetime management.

## Idea list
- if creating content mode with Keepalive packet, don't bring it to the foreground
- Figure out a way to handle dots.
- Improve time display mode by scrolling across the width of the display if not using full width.
- Add more content modes!
- Handle invalid packets

### Mac
- Create DMG
- Intel support - universal

### Linux
- Save paths https://stackoverflow.com/questions/1510104/where-to-store-application-data-non-user-specific-on-linux
- Flatpak?

## Bookmarks
- http://www.cpp-home.com/tutorials/225_1.htm
- https://gist.github.com/minorsecond/cd23af812afffb656c8d7d413ed9792a 
- https://github.com/DFHack/clsocket
- https://stackoverflow.com/questions/29604580/boostasio-write-lock
- https://github.com/cpm-cmake/CPM.cmake