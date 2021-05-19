Watcom C++ compiler can be installed on Ubuntu by command:

    sudo snap install open-watcom --beta

To setup the environment, add to your .bashrc:

    # Open Watcom C/C++
    if [ -x /snap/open-watcom/current/binl64/wmake ]; then
        export WATCOM="/snap/open-watcom/current"
        export PATH=$PATH:$WATCOM/binl64
        export LINUX_INCLUDE=$WATCOM/lh
    fi
