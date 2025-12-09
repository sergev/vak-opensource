//
// An example of using the screen management package.
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
// $Id: hello.cpp,v 1.3 2006/04/19 17:26:54 ly Exp $
//

#include "screen.h"

Screen V;

int main()
{
    V.Put(V.Lines / 2, V.Columns / 2 - 6, "Hello, World!");
    V.Sync();
    V.GetKey();
    V.Move(V.Lines - 1, 0);
    V.Sync();
}
