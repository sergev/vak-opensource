Application example from: /opt/esp-idf/examples/get-started/hello_world

(1) Setup esp32 toolchain:

    . /opt/esp-idf/export.sh

(2) Configure and build the application:

    cp defconfig sdkconfig
    idf.py set-target esp32c3
    idf.py build

(3) Upload the application to the board:

    idf.py flash
