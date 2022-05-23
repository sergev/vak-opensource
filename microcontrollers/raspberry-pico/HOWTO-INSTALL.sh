#
# Get sources
#
mkdir ~/Project/Pico
cd ~/Project/Pico

git clone https://github.com/raspberrypi/pico-sdk
(cd pico-sdk; git submodule update --init)

git clone https://github.com/raspberrypi/pico-examples

#
# Add these to ~/.bashrc
#
export PICO_SDK_PATH=$HOME/Project/Pico/pico-sdk
export PICO_EXAMPLES_PATH=$HOME/Project/Pico/pico-examples

#
# Build a couple of examples: LED and UART
#
cd pico-examples
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Debug
make -Cblink
make -Chello_world

#
# Picoprobe: firmware for Pico-based debug adapter
#
cd ~/Project/Pico
git clone https://github.com/raspberrypi/picoprobe.git
cd picoprobe
mkdir build
cd build
cmake ../
make

#
# Picotool: command line utility for programming via bootloader
#
cd ~/Project/Pico
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build
cd build
cmake ../
make
sudo cp picotool /usr/local/bin/

#
# OpenOCD: interface for debugger and programming via Picoprobe adapter
#
cd ~/Project/Pico
git clone https://github.com/raspberrypi/openocd.git -b rp2040 --depth=1
cd openocd
./bootstrap
./configure --enable-picoprobe
make
sudo make install

#
# Get extensions for Visual Studio Code
#
code --install-extension marus25.cortex-debug
code --install-extension ms-vscode.cmake-tools
code --install-extension ms-vscode.cpptools
