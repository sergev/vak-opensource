#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys, string

for id in sys.argv[1:]:
	id = string.lower (id[:-4])
	print "#include <" +id + ".h>"
