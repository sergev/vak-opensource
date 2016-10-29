Develop a GUI utility to manage filesystem images and write them to SD card. Use Qt toolkit.

Requirements:
(1) Should work on Linux, Windows and Mac.

(2) Use wizard style interface.

(3) User should be able to select one of three basic actions:
    mount, create or check a filesystem on SD card or in a image file.

(4) Action #1: mount a filesystem using FUSE framefork
    (http://sourceforge.net/apps/mediawiki/fuse/index.php?title=Main_Page).
    See Develop your own filesystem with FUSE article
    (http://www.ibm.com/developerworks/library/l-fuse/).

(5) Action #2: create a filesystem. User can create a partition table
    (or reuse existing one), format root and /u filesystems and fill
    them with contents from a specified .tar.gz archive.

(6) Action #3: check root and/or /u filesystems for errors.
