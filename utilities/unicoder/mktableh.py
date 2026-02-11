#
# Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
#
# This file is part of Unicoder utility, which is distributed
# under the terms of the GNU General Public License (GPL).
# See the accompanying file "COPYING" for more details.
#
import sys

for source in sys.argv[1:]:
	encoding_name = source[:-4].lower()
	print(f"#include <{encoding_name}.h>")
