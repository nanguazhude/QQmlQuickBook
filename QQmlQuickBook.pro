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

simpledraw.file = $$PWD/chapter01/simpledraw/simpledraw.pro
simpledraw.depends = buildinstall
SUBDIRS += simpledraw

opengldrawtriangle.file = $$PWD/chapter02/opengldrawtriangle/opengldrawtriangle.pro
opengldrawtriangle.depends = buildinstall
SUBDIRS += opengldrawtriangle




