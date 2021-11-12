Install OneScript as described in readme: https://github.com/EvilBeaver/OneScript

For Linux:
    sudo apt install mono-runtime libmono-i18n4.0-all
    wget https://github.com/EvilBeaver/OneScript/releases/download/v1.7.0/onescript-engine_1.7.0_all.deb
    sudo dpkg -i onescript-engine_1.7.0_all.deb
    sudo apt-get --fix-broken --yes install

For MacOS:
    brew install mono
    wget https://github.com/oscript-library/ovm/releases/download/v1.0.0-RC15/ovm.exe
    mono ovm.exe install stable
    mono ovm.exe use stable
    # source your ~/.bashrc or restart session
