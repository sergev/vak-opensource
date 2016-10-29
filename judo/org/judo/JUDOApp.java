
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
import java.awt.*;
import java.awt.event.*;

/**
 * The code that wraps a JUDO program, making it a complete Java program
 * that will pop up in a window
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class JUDOApp extends JUDOBase {

  static int height = 400;
  static int width = 480;
  int textRows = 0;
  static String programTitle = "All Your Base";

  public JUDOApp() {
    super("graphics", 480, 400, 0, 74);
  }

  public static void main(String args[]) {
    JUDOApp ja = new JUDOApp();
    ja.addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent we) {
        System.exit(0);
      }
    });

    ja.pack();
    Insets insets = ja.getInsets();
    ja.setSize(width + insets.left + insets.right,
               height + insets.top + insets.bottom);
    ja.setTitle(" " + programTitle);
    ja.setLocation(40, 85);
    ja.setVisible(true);

    graphics = drawingCanvas.getGraphics();
    delay(1);
    drawingCanvas.requestFocus();

    try {
      ja.main();
    }
    catch (Exception e) {
      ja.setTitle(" " + programTitle + " - " + lz.APP_EXCEPTION_OCCURED + " (Ctrl-e)");
      ja.exceptionOccurred(e);
    }

    if (ja.getTitle().equals(" " + programTitle)) {
      ja.setTitle(" " + programTitle + " - " + lz.APP_PROG_COMPLETE);
    }
  }
  //<judomain>
  void main() {
	setBackgroundColor(red);
	Color color = white;
	boolean getDarker = true;
	String msg1 = "All Your Base";
	String msg2 = "Are Belong To Us";

	while (true) {
		if (getDarker) {
			color = darker(color);
		}
		else {
			color = brighter(color);
		}
		if (equal(color, white)) {
			getDarker = true;
		}
		else if (equal(color, black)) {
			getDarker = false;
		}

		delay(.06);
		setColor(color);
		fillOval(100, 63, 300, 250);

		if (equal(color, white)) {
			setColor(black);
		}
		else {
			setColor(white);
		}
		if (getDarker) {
			drawString(msg1, true, false, 40, 110, 185);
		}
		else {
			drawString(msg2, false, true, 35, 110, 185);
		}
	}
}


  //</judomain>
}