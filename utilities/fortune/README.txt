To use these quotes:

 1. Copy these files to your ~/.local/share/fortune/ directory.

 2. Create index for each source file:

        cd ~/.local/share/fortune
        strfile Alfred-North-Whitehead-Quotes

 3. Put in your ~/.bashrc:

        fortune .local/share/fortune | cowsay | lolcat
