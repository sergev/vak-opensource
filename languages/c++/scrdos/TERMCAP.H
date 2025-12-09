//
// Defines for termcap parse routines.
//
// Copyright (C) 1994 Cronyx Ltd.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: termcap.h,v 1.5 2008/04/22 15:01:38 ly Exp $
//

const int CAPNUM = 1;
const int CAPFLG = 2;
const int CAPSTR = 3;

struct Captab {
    char tname[3];
    char ttype;
    char *tc;
    int *ti;
    const char **ts;
};
