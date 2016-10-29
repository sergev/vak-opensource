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
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.html.*;
import java.awt.event.*;
import java.awt.BorderLayout;
import java.net.*;
import java.io.IOException;
import java.awt.print.*;

/**
 * A help window that will render an html page.  Used in displaying JUDO information,
 * tutorials, and references.
 *
 * @author Thomas Dunn
 * @version 1.0
 */

public class JUDOHelpWindow extends JFrame {
  JEditorPane webPagePane;
  final JUDOIDE judoIDE;

  public JUDOHelpWindow(JUDOIDE judoIDE, String title, int width, int height, boolean clickable) {
    super(title);
    this.judoIDE = judoIDE;
    this.setIconImage(new ImageIcon(this.judoIDE.codeBase + this.judoIDE.pathSeparator + "images" + this.judoIDE.pathSeparator + this.judoIDE.judoIconFilename).getImage());
    webPagePane = new JEditorPane();
    webPagePane.putClientProperty(JEditorPane.HONOR_DISPLAY_PROPERTIES, Boolean.TRUE);
    webPagePane.setFont(new Font("Serif", Font.PLAIN, 16));
    webPagePane.setEditable(false);

    this.getContentPane().add(BorderLayout.CENTER, new JScrollPane(webPagePane));
    this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    this.setSize(width, height);

    // add a hyperlink listener to make links clickable if desired
    if (clickable) {
      webPagePane.addHyperlinkListener(new HyperlinkListener() {
        public void hyperlinkUpdate(HyperlinkEvent hle) {
          if (hle.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
            String url = hle.getURL().toString();
            String httpJudoURL = "http://judo/";
            String exitString = "/exit";
            if (url.indexOf(httpJudoURL) != -1) {
              String progName = url.substring(httpJudoURL.length());
              boolean exit = false;
              if (progName.indexOf(exitString) != -1) {
                exit = true;
                progName = progName.substring(0, progName.length() - exitString.length());
              }
              JUDOHelpWindow.this.judoIDE.openSampleProgram(progName);
              JUDOHelpWindow.this.judoIDE.requestFocus();
              if (exit) {
                JUDOHelpWindow.this.setVisible(false);
                JUDOHelpWindow.this.dispose();
              }
            }
            else {
              if (hle instanceof HTMLFrameHyperlinkEvent) {
                HTMLFrameHyperlinkEvent evt = (HTMLFrameHyperlinkEvent) hle;
                HTMLDocument doc = (HTMLDocument) webPagePane.getDocument();
                doc.processHTMLFrameHyperlinkEvent(evt);
              }
              else {
                displayURL(hle.getURL());
              }
            }
          }
        }
      });
    }
  }

  public void displayURL(URL url) {
    try {
      webPagePane.setPage(url);

      // only display the window after the page is loaded
      this.setVisible(true);
    }
    catch (IOException ioe) {
      System.out.println("IOException caught in JUDOHelpWindow.displayURL(URL)");
      System.out.println(ioe);
      ioe.printStackTrace();
    }
  }

  public void displayURL(String helpHTMLFilename) {
    URL helpURL = null;

    try {
      helpURL = new URL(helpHTMLFilename);
      displayURL(helpURL);
    }
    catch (MalformedURLException murle) {
      System.out.println("MalformedURLException caught in JUDOHelpWindow.displayURL(String)");
      System.out.println(murle);
      murle.printStackTrace();
    }
  }

}
