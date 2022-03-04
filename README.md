# PhalanxTray
Cross-Platform tray application for interfacing with Phalanx-based VFD and Nixie devices.

## Commands
Are formatted like
`COMMAND|parameter|parameter|parameter`

The max length for a command is 65536 characters.

### HELLO
Starts a new ContentMode:
```
HELLO|FinalFantasyXIV
```

### KEEPALIVE
Has to be called every X seconds by the content mode to make sure the content mode stays alive.
```
KEEPALIVE|FinalFantasyXIV
```

### SENDDATA
Sends new text to be displayed on the attached serial device 
```
SENDDATA|FinalFantasyXIV|TextToBeDisplayed
```

## Bookmarks
- http://www.cpp-home.com/tutorials/225_1.htm
- https://gist.github.com/minorsecond/cd23af812afffb656c8d7d413ed9792a 
- https://github.com/DFHack/clsocket
- https://stackoverflow.com/questions/29604580/boostasio-write-lock
- https://github.com/cpm-cmake/CPM.cmake