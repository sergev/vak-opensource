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

/**
 * Runs an externale executable, giving means of displaying output in ide, with option of parsing the output (as errors of compiler)
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class Command implements Runnable {

  String command;
  JUDOIDE judoIDE;
  boolean parseForErrors;
  boolean successfulCompletion = false;

  Command(String cmd, JUDOIDE ide, boolean parse) {
    command = cmd;
    judoIDE = ide;
    parseForErrors = parse;
  }

  public void run() {
    Runtime r = Runtime.getRuntime();
    Process p = null;
    try {
      p = r.exec(command);

      BufferedReader brSTDOUT = new BufferedReader(new InputStreamReader(p.getInputStream()));
      BufferedReader brSTDERR = new BufferedReader(new InputStreamReader(p.getErrorStream()));

      Thread t1 = new Thread(new OutputGrabber(brSTDOUT, judoIDE, false));
      t1.start();
      Thread t2 = new Thread(new OutputGrabber(brSTDERR, judoIDE, parseForErrors));
      t2.start();

      try {
        int returnValue;
        returnValue = p.waitFor();
        if (returnValue == 0) {
          successfulCompletion = true;
        }
        else {
          successfulCompletion = false;
        }
        t1.join();
        t2.join();
      } catch (InterruptedException ie) {
        System.out.println(ie.toString());
      }

    } catch (java.io.IOException ioe) {
      System.out.println("Inside IOException, Command.java");
      System.out.println(ioe.toString());
      ioe.printStackTrace();
    }
  }

  public boolean completedSuccessfully() {
    return successfulCompletion;
  }
}
