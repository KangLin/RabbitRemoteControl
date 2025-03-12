# Wiki

## Q: When run AppImage, The following error occurs:

```
[root@8bbd00d2d0eb home]# ./RabbitRemoteControl_v0.0.32_Linux_x86_64.AppImage 
qt.qpa.xcb: could not connect to display 
qt.qpa.plugin: From 6.5.0, xcb-cursor0 or libxcb-cursor0 is needed to load the Qt xcb platform plugin.
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem.

Available platform plugins are: xcb.

Aborted (core dumped)
```

### A: This is because there is no desktop environment set up to run the program from the console.
Set up the desktop environment. eg:

```
# Install virtual desktop (virtual framebuffer X server for X Version 11)
sudo apt install -y -q xvfb xpra
# Set desktop environment
sudo Xvfb :91.0 -ac -screen 0 1200x900x24 &
export DISPLAY=:91.0
```
