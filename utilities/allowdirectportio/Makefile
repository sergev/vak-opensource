# $Id: Makefile,v 1.4 2003/06/19 10:23:40 das Exp $  Makefile for allowdirectportio.
#
# Copyright (C) 2003 Dwight A. Schauer
#
#    This file is part of allowdirectportio.
#
#    allowdirectportio is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    allowdirectportio is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with allowdirectporio; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

INSTALL_DIR	= /usr/local/bin
TARGET		= allowdirectportio
TARGET_SOURCE	= $(TARGET).c

TARGET_BIN	= $(INSTALL_DIR)/$(TARGET).lpt

$(TARGET):	$(TARGET_SOURCE)
		$(CC) -O2 -o $@ -Werror -pedantic -Wall $(TARGET_SOURCE)
		strip $@

clean:
		rm -f $(TARGET) *~

install:	$(TARGET)
		rm -f $(TARGET_BIN)
		install -D $(TARGET) $(TARGET_BIN)
		chown root $(TARGET_BIN)
		chmod u+s $(TARGET_BIN)
