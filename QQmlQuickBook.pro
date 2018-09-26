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

computeshaderaddbuffer.file = $$PWD/chapter02/computeshaderaddbuffer/computeshaderaddbuffer.pro
computeshaderaddbuffer.depends = buildinstall
SUBDIRS += computeshaderaddbuffer

computeshaderfilterimage.file = $$PWD/chapter02/computeshaderfilterimage/computeshaderfilterimage.pro
computeshaderfilterimage.depends = buildinstall
SUBDIRS += computeshaderfilterimage

invertimage.file = $$PWD/chapter02/invertimage/invertimage.pro
invertimage.depends = buildinstall
SUBDIRS += invertimage

chat.file = $$PWD/chapter03/chat/chat.pro
chat.depends = buildinstall
SUBDIRS += chat

qsgdrawpoint.file = $$PWD/chapter02/qsgdrawpoint/qsgdrawpoint.pro
qsgdrawpoint.depends = buildinstall
SUBDIRS += qsgdrawpoint

qsgdrawtexturepoint.file = $$PWD/chapter02/qsgdrawtexturepoint/qsgdrawtexturepoint.pro
qsgdrawtexturepoint.depends = buildinstall
SUBDIRS += qsgdrawtexturepoint

qquickframebufferobjectdrawroundpoint.file = $$PWD/chapter02/qquickframebufferobjectdrawroundpoint/qquickframebufferobjectdrawroundpoint.pro
qquickframebufferobjectdrawroundpoint.depends = buildinstall
SUBDIRS += qquickframebufferobjectdrawroundpoint

qsgdrawuserender.file = $$PWD/chapter02/qsgdrawuserender/qsgdrawuserender.pro
qsgdrawuserender.depends = buildinstall
SUBDIRS += qsgdrawuserender





