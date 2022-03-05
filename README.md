# PhalanxTray
Cross-Platform tray application for interfacing with Phalanx-based VFD and Nixie devices.
Receives UDP packets on port 11001

## UDP Packets
Are formatted like
`COMMAND|parameter|parameter|parameter`

The max length for a packet is 65536 characters.

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

## Content Modes
- Time: Self-contained, default content mode displaying the current time
- FinalFantasyXIV: Simple forwarder of SendData to the serial device, [danskidb/Dalamud-Klonk](https://github.com/danskidb/Dalamud-Klonk) is the sender plugin

## To Do
- Add Goodbye command
- Figure out a way to handle dots.
- Improve time display mode by scrolling across the width of the display if not using full width.
- Display date between 28-32s in a minute. (option)
- Add more content modes!
- Handle invalid packets
- Clean up main.cpp

## Bookmarks
- http://www.cpp-home.com/tutorials/225_1.htm
- https://gist.github.com/minorsecond/cd23af812afffb656c8d7d413ed9792a 
- https://github.com/DFHack/clsocket
- https://stackoverflow.com/questions/29604580/boostasio-write-lock
- https://github.com/cpm-cmake/CPM.cmake