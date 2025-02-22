A JavaScriptCore is silently present on almost all Linux installations.
Try this on your machine:

    $ ls -l /usr/lib/*/libjavascriptcoregtk*.so*
    lrwxrwxrwx 1 root root       29 Feb 11 15:28 /usr/lib/aarch64-linux-gnu/libjavascriptcoregtk-6.0.so -> libjavascriptcoregtk-6.0.so.1
    lrwxrwxrwx 1 root root       34 Feb 11 15:28 /usr/lib/aarch64-linux-gnu/libjavascriptcoregtk-6.0.so.1 -> libjavascriptcoregtk-6.0.so.1.3.15
    -rw-r--r-- 1 root root 27856544 Feb 11 15:28 /usr/lib/aarch64-linux-gnu/libjavascriptcoregtk-6.0.so.1.3.15

Most probably you will see some versions of this shared libraries installed.
Because web browsers need it.

This Javascript can be used from a C program. You will need the
corresponding include files, for this you should install the
package of the required version. For example, on the latest
Debian it is 6.0:

    sudo apt install libjavascriptcoregtk-6.0-dev

Let's take an example of calculating a factorial, in file js-linux.cpp.

    $ c++ js-linux.cpp -o js-linux `pkg-config --cflags --libs javascriptcoregtk-6.0`
    $ ./js-linux
    The result is: 3628800.000000

The same on MacOS:

    $ c++ js-macos.cpp -o js-macos -framework JavaScriptCore
    $ ./js-macos
    The result is: 3628800.000000
