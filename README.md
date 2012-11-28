xml-fs
======

An XML-backed filesystem in FUSE

To compile a fuse file:
======================
`gcc -o hello -D_FILE_OFFSET_BITS=64 -lfuse -DFUSE_USE_VERSION=22 hello.c`
