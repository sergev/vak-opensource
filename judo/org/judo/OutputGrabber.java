/***************************************************************************
 * JUDO - A Java IDE for Children and Beginning Programmers
 * Copyright (C) 2001  Thomas J. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

package org.judo;
import java.io.*;
import java.util.Vector;

/**
 * Grabs the output of a Command instance and optionally parses it with CompilerError
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class OutputGrabber implements Runnable {
  BufferedReader reader;
  String aLine = "";
  Vector all;
  String newline = System.getProperty("line.separator");
  JUDOIDE judoIDE;
  boolean parseForErrors;

  OutputGrabber(BufferedReader br, JUDOIDE ide, boolean parse) {
    reader = br;
    judoIDE = ide;
    parseForErrors = parse;
    all = new Vector();
  }

  public void run() {
    try {
      while((aLine = reader.readLine()) != null) {
        all.addElement(aLine);
      }

      if (parseForErrors) {
        CompilerError ce = new CompilerError(all, judoIDE);
        ce.parseErrors();
      }
    } catch (IOException ioe) {}
  }
}
