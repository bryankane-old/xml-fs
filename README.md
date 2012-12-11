xml-fs
======

An XML-backed filesystem in FUSE

To compile a fuse file:
======================
`make`

To run:
=======
`fusermount -u /tmp/xml`
`./xml -d /tmp/xml sample.xml`