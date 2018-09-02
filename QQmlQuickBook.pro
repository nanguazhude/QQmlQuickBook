TEMPLATE = subdirs

buildinstall.file = $$PWD/buildinstall/buildinstall.pro
SUBDIRS += buildinstall

hellowworld.file = $$PWD/chapter01/hellowworld/hellowworld.pro
hellowworld.depends = buildinstall
SUBDIRS += hellowworld

checkfileformat.file = $$PWD/chapter01/checkfileformat/checkfileformat.pro
checkfileformat.depends = buildinstall
SUBDIRS += checkfileformat

memory.file = $$PWD/chapter01/memory/memory.pro
memory.depends = buildinstall
SUBDIRS += memory

qquickwindowopengldraw.file = $$PWD/chapter02/qquickwindowopengldraw/qquickwindowopengldraw.pro
qquickwindowopengldraw.depends = buildinstall
SUBDIRS += qquickwindowopengldraw

qwidgetopengldraw.file = $$PWD/chapter02/qwidgetopengldraw/qwidgetopengldraw.pro
qwidgetopengldraw.depends = buildinstall
SUBDIRS += qwidgetopengldraw

qwindowopengldraw.file = $$PWD/chapter02/qwindowopengldraw/qwindowopengldraw.pro
qwindowopengldraw.depends = buildinstall
SUBDIRS += qwindowopengldraw


