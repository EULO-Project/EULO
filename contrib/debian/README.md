
Debian
====================
This directory contains files used to package eulod/eulo-qt
for Debian-based Linux systems. If you compile eulod/eulo-qt yourself, there are some useful files here.

## eulo: URI support ##


eulo-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install eulo-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your euloqt binary to `/usr/bin`
and the `../../share/pixmaps/eulo128.png` to `/usr/share/pixmaps`

eulo-qt.protocol (KDE)

