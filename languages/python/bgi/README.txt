The following functions provide a more Pytonic syntax that only uses standard Python types:

    # void detectgraph (int *graphdriver, int *graphmode);
    graphdriver, graphmode = detectgraph ()
    print ("graphdriver, graphmode: ", graphdriver, graphmode);

    # void getarccoords (struct arccoordstype *arccoords);
    ac = arccoordstype ()
    ac = getarccoords ()
    print ("x, y, xstart, ystart, xend, yend: ", ac.x, ac.y,
            ac.xstart, ac.ystart, ac.xend, ac.yend)

    # void getaspectratio (int *xasp, int *yasp);
    xasp, yasp = getaspectratio ()
    print ("xasp, yasp: ", xasp, yasp)

    # void getfillsettings (struct fillsettingstype *fillinfo);
    fs = fillsettingstype ()
    fs = getfillsettings ()
    print ("pattern, color: ", fs.pattern, fs.color)

    # void getlinesettings (struct linesettingstype *lineinfo);
    ls = linesettingstype ()
    ls = getlinesettings ()
    print ("linestyle, thickness: ", ls.linestyle, ls.thickness)

    # void getmoderange (int graphdriver, int *lomode, int *himode);
    lomode, himode = getmoderange ()
    print ("lomode, himode: ", lomode, lomode)

    # void getmouseclick (int btn, int *x, int *y);
    x, y = getmouseclick (WM_LBUTTONDOWN)
    print ("mouse x, mouse y: ", x, y)

    # void getscreensize (int x, int y);
    x, y = getscreensize ()
    print ("size x, size y: ", x, y)

    # void initgraph (int *graphdriver, int *graphmode, char *pathtodriver)
    initgraph ()
