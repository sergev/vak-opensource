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
import java.util.*;
import java.io.*;

/**
 * The base class for JUDO programs, takes care of all the text and graphics I/O.
 * Provides many JUDO functions for use in user programs, as well as lots
 * of predifined colors with nice names
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class JUDOBase extends Frame implements KeyListener, MouseListener {

  // JUDOApp types
  final static String jud0_TYPE_TEXT = "text";
  final static String jud0_TYPE_GRAPHICS = "graphics";
  final static String jud0_TYPE_BOTH = "both";

  final static int jud0_APP_HEIGHT_GRAPHICS = 600;
  final static int jud0_APP_HEIGHT_TEXT = 600;
  final static int jud0_APP_HEIGHT_BOTH = 700;
  final static int jud0_APP_BOTH_ROWS = 10;
  final static int jud0_APP_TEXT_ROWS = 2;
  final static int jud0_APP_GRAPHICS_ROWS = 0;
  final static int jud0_APP_WIDTH = 800;

  static final String jud0_newline = System.getProperty("line.separator");
  protected String jud0_exceptionMessage = "";
  protected TextArea jud0_exceptionArea;
  private String jud0_appType;
  private boolean jud0_showingException = false;
  private boolean jud0_hasException = false;
  int jud0_headerLines = 0;
  int jud0_taFirstPosition = 0;

  int jud0_mouseX;
  int jud0_mouseY;
  boolean jud0_mouseButton1;
  boolean jud0_mouseButton2;
  boolean jud0_mouseButton3;
  int jud0_mouseDragStartX1;
  int jud0_mouseDragStartY1;
  int jud0_mouseDragEndX1;
  int jud0_mouseDragEndY1;
  int jud0_mouseDragStartX2;
  int jud0_mouseDragStartY2;
  int jud0_mouseDragEndX2;
  int jud0_mouseDragEndY2;
  int jud0_mouseDragStartX3;
  int jud0_mouseDragStartY3;
  int jud0_mouseDragEndX3;
  int jud0_mouseDragEndY3;

  public static Graphics jud0_graphics;
  public static Canvas jud0_drawingCanvas;
  TextArea jud0_ta;
  Random jud0_random = new Random();
  Color jud0_backgroundColor = Color.white;

  JUDOMouseEvent jud0_mouseEvent = new JUDOMouseEvent();
  JUDOKeyEvent jud0_keyEvent = new JUDOKeyEvent();
  public static JUDOUtils jud0_ju = new JUDOUtils();
  // localization
  public static JUDO_ru jud0_lz = new JUDO_ru();

  public JUDOBase(String jud0_appType, int width, int height, int rows, int jud0_headerLines) {
    this.jud0_headerLines = jud0_headerLines;
    initGui(jud0_appType, width, height, rows);
  }

  private void initGui(String jud0_appType, int width, int height, int rows) {
    this.jud0_appType = jud0_appType;
    setLayout(new BorderLayout());
    setBackground(Color.white);
    jud0_drawingCanvas = new Canvas();
    jud0_ta = new TextArea(rows, 1);
    jud0_ta.setEditable(false);
    jud0_exceptionArea = new TextArea(rows, 1);

    // Select monospaced font.
    Font font = new Font("Monospaced", Font.PLAIN, 16);
    jud0_ta.setFont(font);
    //jud0_ta.append("Using '" + font.getFamily() + "'" + jud0_newline);

    addComponentListener(new ComponentAdapter() {
      public void componentResized(ComponentEvent e) {
        jud0_drawingCanvas.setBackground(jud0_backgroundColor);
      }
    });

    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent we) {
        System.exit(0);
      }
    });

    if (jud0_appType.equals(JUDOBase.jud0_TYPE_BOTH)) {
      add(BorderLayout.CENTER, jud0_drawingCanvas);
      jud0_ta.addKeyListener(this);
      jud0_ta.addMouseListener(this);
      add(BorderLayout.SOUTH, jud0_ta);
    }
    else if (jud0_appType.equals(JUDOBase.jud0_TYPE_GRAPHICS)) {
//      add(BorderLayout.CENTER, jud0_ta);
      add(BorderLayout.CENTER, jud0_drawingCanvas);
    }
    else {  // JUDOBase.jud0_TYPE_TEXT
      jud0_ta.addKeyListener(this);
      jud0_ta.addMouseListener(this);  // for checking mouse clicks when text area in edit mode
      add(BorderLayout.CENTER, jud0_ta);
    }

    if (jud0_appType.equals(JUDOBase.jud0_TYPE_BOTH) || jud0_appType.equals(JUDOBase.jud0_TYPE_GRAPHICS)) {
//      jud0_drawingCanvas.requestFocus();

      // setup mouse event handlers

      jud0_drawingCanvas.addMouseListener(new MouseAdapter() {

        public void mouseClicked(MouseEvent e) {
          int buttonNum = 0;
          if ((e.getModifiers() & e.BUTTON1_MASK) == e.BUTTON1_MASK) {
            buttonNum = 1;
          }
          else if ((e.getModifiers() & e.BUTTON2_MASK) == e.BUTTON2_MASK) {
            buttonNum = 2;
          }
          else if ((e.getModifiers() & e.BUTTON3_MASK) == e.BUTTON3_MASK) {
            buttonNum = 3;
          }

          // for JUDOApp mouse event handling
          JUDOBase.this.mouseClicked(e.getX(), e.getY(), buttonNum);
        }

        public void mousePressed(MouseEvent e) {
          jud0_mouseEventReceived = true;
          int buttonNum = 0;

          // for JUDOApp mouse polling
          // mousePressed:
          //   record mouse location
          //   show that the button pressed is down
          //   start of drag

          synchronized (jud0_mouseEvent) {
            jud0_mouseEvent.x = e.getX();
            jud0_mouseEvent.y = e.getY();

            // necessary to do in if ... else if ... else if like this
            // because for some reason, if button 3 has already been pressed (had its own onpress event already),
            // in the onpress of button 1, e.getModifiers says that both
            // button 1 and button 3 is being pressed (only want to report on one here)
            if ((e.getModifiers() & e.BUTTON1_MASK) == e.BUTTON1_MASK) {
              jud0_mouseEvent.button1 = true;              // sets it as button 1 being down

              jud0_mouseEvent.dragStartX1 = jud0_mouseEvent.x;  // starts drag for button 1
              jud0_mouseEvent.dragStartY1 = jud0_mouseEvent.y;
              jud0_mouseEvent.dragEndX1 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragEndY1 = jud0_mouseEvent.y;

              buttonNum = 1;
            }
            else if ((e.getModifiers() & e.BUTTON2_MASK) == e.BUTTON2_MASK) {
              jud0_mouseEvent.button2 = true;

              jud0_mouseEvent.dragStartX2 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragStartY2 = jud0_mouseEvent.y;
              jud0_mouseEvent.dragEndX2 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragEndY2 = jud0_mouseEvent.y;

              buttonNum = 2;
            }
            else if ((e.getModifiers() & e.BUTTON3_MASK) == e.BUTTON3_MASK) {
              jud0_mouseEvent.button3 = true;

              jud0_mouseEvent.dragStartX3 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragStartY3 = jud0_mouseEvent.y;
              jud0_mouseEvent.dragEndX3 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragEndY3 = jud0_mouseEvent.y;

              buttonNum = 3;
            }
            jud0_mouseEvent.newEvent = true;
          }

          // for JUDOApp mouse event handling
          JUDOBase.this.mousePressed(e.getX(), e.getY(), buttonNum);
        }

        public void mouseReleased(MouseEvent e) {
          jud0_mouseEventReceived = true;
          int buttonNum = 0;

          // for JUDOApp mouse polling
          // mouseReleased:
          //   record mouse location
          //   show that the button released is not down

          synchronized (jud0_mouseEvent) {
            jud0_mouseEvent.x = e.getX();
            jud0_mouseEvent.y = e.getY();

            // done in if ... else if ... else if for reason described in onpressed
            if ((e.getModifiers() & e.BUTTON1_MASK) == e.BUTTON1_MASK) {
              jud0_mouseEvent.button1 = false;
              buttonNum = 1;
            }
            else if ((e.getModifiers() & e.BUTTON2_MASK) == e.BUTTON2_MASK) {
              jud0_mouseEvent.button2 = false;
              buttonNum = 2;
            }
            else if ((e.getModifiers() & e.BUTTON3_MASK) == e.BUTTON3_MASK) {
              jud0_mouseEvent.button3 = false;
              buttonNum = 3;
            }

            jud0_mouseEvent.newEvent = true;
          }

          // for JUDOApp mouse event handling
          JUDOBase.this.mouseReleased(e.getX(), e.getY(), buttonNum);
        }
      });
      jud0_drawingCanvas.addMouseMotionListener(new MouseMotionAdapter() {
        public void mouseMoved(MouseEvent e) {
          jud0_mouseEventReceived = true;

          // for JUDOApp mouse polling
          // mouseMoved:
          //   record mouse location
          //   mouseMoved event has no important button information

          synchronized (jud0_mouseEvent) {
            jud0_mouseEvent.x = e.getX();
            jud0_mouseEvent.y = e.getY();
            // button should be false since we are moving, not dragging
            jud0_mouseEvent.button1 = false;
            jud0_mouseEvent.button2 = false;
            jud0_mouseEvent.button3 = false;
            jud0_mouseEvent.newEvent = true;
          }

          // for JUDOApp mouse event handling
          JUDOBase.this.mouseMoved(e.getX(), e.getY());
        }
        public void mouseDragged(MouseEvent e) {
          jud0_mouseEventReceived = true;

          // for JUDOApp mouse polling
          // mouseDragged:
          //   record mouse location
          //   onpressed and onreleased take care of button adjustments, no changes made to them here
          //   update end of drag

          synchronized (jud0_mouseEvent) {
            jud0_mouseEvent.x = e.getX();
            jud0_mouseEvent.y = e.getY();
            // don't alter buttons, they are set before (pressed) and after (released) drag
            if (jud0_mouseEvent.button1) {
              jud0_mouseEvent.dragEndX1 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragEndY1 = jud0_mouseEvent.y;
            }
            if (jud0_mouseEvent.button2) {
              jud0_mouseEvent.dragEndX2 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragEndY2 = jud0_mouseEvent.y;
            }
            if (jud0_mouseEvent.button3) {
              jud0_mouseEvent.dragEndX3 = jud0_mouseEvent.x;
              jud0_mouseEvent.dragEndY3 = jud0_mouseEvent.y;
            }
            jud0_mouseEvent.newEvent = true;

            // for JUDOApp mouse event handling
            JUDOBase.this.mouseDragged(e.getX(), e.getY(), jud0_mouseEvent.button1, jud0_mouseEvent.button2, jud0_mouseEvent.button3);
          }
        }
      });  // end setup mouse handlers

      // setup keyboard handler
      jud0_drawingCanvas.addKeyListener(this);
    }

  }  // end initGui

  protected void exceptionOccurred(Exception e) {
    jud0_exceptionMessage = getExceptionMessage(e);
    jud0_exceptionArea.setText(jud0_exceptionMessage);
    jud0_exceptionArea.setEditable(false);
    ExceptionKeyListener keyListener = new ExceptionKeyListener();

    jud0_ta.addKeyListener(keyListener);
    jud0_drawingCanvas.addKeyListener(keyListener);
    jud0_exceptionArea.addKeyListener(keyListener);

    if (! jud0_appType.equals(JUDOBase.jud0_TYPE_TEXT)) {
      jud0_drawingCanvas.requestFocus();
    }

    jud0_hasException = true;
  }

  private String getExceptionMessage(Exception e) {
    try {
      StringWriter stringWriter = new StringWriter();
      e.printStackTrace(new PrintWriter(stringWriter));
      String stackTrace = stringWriter.toString();
      StringTokenizer st = new StringTokenizer(stackTrace, System.getProperty("line.separator"));
      System.out.println(stackTrace);
      int errorLineNumber = 0;
      int lineNumber = 0;
      String functionName = "";

      if (st.countTokens() > 1) {
        String errorLine = st.nextToken();
        while (st.hasMoreTokens()) {
          errorLine = st.nextToken();
          if (errorLine.indexOf("JUDOApp.") != -1 && (lineNumber = getErrorLineNumber(errorLine)) >= jud0_headerLines) {
            break;
          }
        }

        if (lineNumber > 0) {
          errorLineNumber = lineNumber - jud0_headerLines - 1;
        }

        // to get function name, look between ( and the . before it
        int leftParenIndex = errorLine.indexOf("(");
        functionName = errorLine.substring(errorLine.lastIndexOf(".", leftParenIndex) + 1, leftParenIndex);
      }

      String exceptionObject = "";
      String eString = e.toString();
      String message = "";
      int colonIndex = eString.indexOf(":");
      if (colonIndex != -1) {
        int periodIndex = eString.lastIndexOf(".", colonIndex);
        exceptionObject = eString.substring(periodIndex + 1, colonIndex);
        message = e.getMessage();
      }
      else {
        exceptionObject = eString.substring(eString.lastIndexOf(".") + 1);
      }

      int exceptionIndex = exceptionObject.indexOf("Exception");
      if (exceptionIndex != -1) {
        exceptionObject = exceptionObject.substring(0, exceptionIndex) + " error";
      }
      if (exceptionObject.equals("NullPointer error")) {
        exceptionObject = "Undefined value error";
      }

      String msg = exceptionObject + " in function " + functionName;
      if (errorLineNumber != 0) {
        msg += " on line " + errorLineNumber;
      }
      if (! message.equals("")) {
        msg += ":\n\t" + message;
      }

      return msg;
    }

    catch (Exception e2) {
      return "";
    }
  }

  private int getErrorLineNumber(String errorLine) {
    int colonIndex = errorLine.indexOf(":");
    int parenIndex = errorLine.indexOf(")");
    String errorLineNumberString = errorLine.substring(colonIndex + 1, parenIndex);
    int lineNumber = 0;
    try {
      lineNumber = (new Integer(errorLineNumberString)).intValue();
    }
    catch (NumberFormatException nfe) {
    }
    return lineNumber;
  }

  private void showException() {
    if (jud0_appType.equals(JUDOBase.jud0_TYPE_BOTH)) {
      remove(jud0_ta);
      add(BorderLayout.SOUTH, jud0_exceptionArea);
    }
    else if (jud0_appType.equals(JUDOBase.jud0_TYPE_TEXT)) {
      remove(jud0_ta);
      add(BorderLayout.CENTER, jud0_exceptionArea);
    }
    else {
      remove(jud0_drawingCanvas);
      add(BorderLayout.CENTER, jud0_exceptionArea);
    }

    jud0_exceptionArea.requestFocus();

    validate();
    repaint();
    jud0_showingException = true;
  }

  private void hideException() {
    if (jud0_appType.equals(JUDOBase.jud0_TYPE_BOTH)) {
      remove(jud0_exceptionArea);
      add(BorderLayout.SOUTH, jud0_ta);
      jud0_ta.requestFocus();
    }
    else if (jud0_appType.equals(JUDOBase.jud0_TYPE_TEXT)) {
      remove(jud0_exceptionArea);
      add(BorderLayout.CENTER, jud0_ta);
      jud0_ta.requestFocus();
    }
    else {
      remove(jud0_exceptionArea);
      add(BorderLayout.CENTER, jud0_drawingCanvas);
      jud0_drawingCanvas.requestFocus();
    }

    validate();
    repaint();
    jud0_showingException = false;
  }

  //////////////////////////////////////////////////////////////////////////////
  //                       USER OVERRIDABLE METHODS                           //
  //////////////////////////////////////////////////////////////////////////////

  void mouseClicked(int x, int y, int buttonNumber) {

  }
  void mousePressed(int x, int y, int buttonNumber) {

  }
  void mouseReleased(int x, int y, int buttonNumber) {

  }
  void mouseMoved(int x, int y) {

  }
  void mouseDragged(int x, int y, boolean button1, boolean button2, boolean button3) {

  }

  //////////////////////////////////////////////////////////////////////////////
  //                      TEXTAREA PROCESSING METHODS                         //
  //////////////////////////////////////////////////////////////////////////////
  public void keyTyped(KeyEvent e) {
    //processKey(e);
  }
  private String getKeyString(char keyChar, int keyCode) {
    switch (keyCode) {
      case KeyEvent.VK_ESCAPE:
        return "escape";
      case KeyEvent.VK_F1:
        return "f1";
      case KeyEvent.VK_F2:
        return "f2";
      case KeyEvent.VK_F3:
        return "f3";
      case KeyEvent.VK_F4:
        return "f4";
      case KeyEvent.VK_F5:
        return "f5";
      case KeyEvent.VK_F6:
        return "f6";
      case KeyEvent.VK_F7:
        return "f7";
      case KeyEvent.VK_F8:
        return "f8";
      case KeyEvent.VK_F9:
        return "f9";
      case KeyEvent.VK_F10:
        return "f10";
      case KeyEvent.VK_F11:
        return "f11";
      case KeyEvent.VK_F12:
        return "f12";
      case KeyEvent.VK_PRINTSCREEN:
        return "print screen";
      case KeyEvent.VK_SCROLL_LOCK:
        return "scroll lock";
      case KeyEvent.VK_PAUSE:
        return "pause";
      case KeyEvent.VK_INSERT:
        return "insert";
      case KeyEvent.VK_HOME:
        return "home";
      case KeyEvent.VK_DELETE:
        return "delete";
      case KeyEvent.VK_END:
        return "end";
      case KeyEvent.VK_PAGE_UP:
        return "page up";
      case KeyEvent.VK_PAGE_DOWN:
        return "page down";
      case KeyEvent.VK_CAPS_LOCK:
        return "caps lock";
      case KeyEvent.VK_NUM_LOCK:
        return "num lock";
      case KeyEvent.VK_BACK_SPACE:
        return "backspace";
      case KeyEvent.VK_ENTER:
        return "enter";
      case KeyEvent.VK_TAB:
        return "tab";
      case KeyEvent.VK_META:
        return "meta";
      case KeyEvent.VK_CONTROL:
        return "control";
      case KeyEvent.VK_SHIFT:
        return "shift";
      case KeyEvent.VK_ALT:
        return "alt";
      case KeyEvent.VK_UP:
        return "up";
      case KeyEvent.VK_DOWN:
        return "down";
      case KeyEvent.VK_LEFT:
        return "left";
      case KeyEvent.VK_RIGHT:
        return "right";
      default:
        return "" + keyChar;
    }
  }
  public synchronized void keyPressed(KeyEvent e) {
    jud0_keyEventReceived = true;
    if (! jud0_ta.isEditable()) {  // if its not editable, keep track of which keys are down
      synchronized(jud0_keyEvent) {
        String key = getKeyString(e.getKeyChar(), e.getKeyCode());
        if (jud0_keyEvent.waitingForKey) {
          jud0_keyEvent.keyRead = key;
          jud0_keyEvent.waitingForKey = false;
          jud0_keyEvent.setKeyDown(key); // for getKeyStatus()
          this.notify();
        }
        else {
          jud0_keyEvent.setKeyDown(key);
        }
      }
    }
    else {  // it is editable, so filter out key strokes you don't want
      processKey(e);
    }
  }
  public void keyReleased(KeyEvent e) {
    jud0_keyEventReceived = true;
    synchronized(jud0_keyEvent) {
      String key = getKeyString(e.getKeyChar(), e.getKeyCode());
      jud0_keyEvent.setKeyUp(key);
    }
  }
  synchronized protected void processKey(KeyEvent e) {
    int keyCode = e.getKeyCode();
    if (keyCode == KeyEvent.VK_ENTER) {
      e.consume();
      jud0_ta.setEditable(false);
      this.notify();
    }
    if (keyCode == KeyEvent.VK_PAGE_UP ||
        keyCode == KeyEvent.VK_PAGE_DOWN ||
        keyCode == KeyEvent.VK_UP ||
        keyCode == KeyEvent.VK_DOWN) {
      e.consume();
    }
    if ((keyCode == KeyEvent.VK_END || keyCode == KeyEvent.VK_HOME) && ((e.getModifiers() & e.CTRL_MASK) == e.CTRL_MASK)) {
      e.consume();
    }
    if (keyCode == KeyEvent.VK_LEFT || keyCode == KeyEvent.VK_BACK_SPACE) {
      int taCaretPos = jud0_ta.getCaretPosition();
      if (taCaretPos == jud0_taFirstPosition) {
        e.consume();
      }
    }
  }

  class ExceptionKeyListener implements KeyListener {
    public synchronized void keyPressed(KeyEvent e) {
      int keyCode = e.getKeyCode();
      if (jud0_hasException && keyCode == KeyEvent.VK_E && e.isControlDown()) {
        if (jud0_showingException) {
          hideException();
        }
        else {
          showException();
        }
        e.consume();
      }
    }
    public void keyReleased(KeyEvent e) {}
    public void keyTyped(KeyEvent e) {}
  }

  private String getLine() {
    String text = jud0_ta.getText();
    int jud0_newlineIndex = text.lastIndexOf(jud0_newline);
    if (jud0_newlineIndex >= 0) {
      String tmp = text.substring(jud0_newlineIndex + jud0_newline.length(), text.length());
      return tmp;
    }
    else {
      return text;
    }
  }
  private String getEnteredString() {
    String enteredString = "";
    enteredString = getLine();
    jud0_ta.append(jud0_newline);
    jud0_ta.setCaretPosition(jud0_ta.getText().length());
    return enteredString;
  }

  public void mousePressed(MouseEvent me) {
    jud0_ta.requestFocus();
    if (jud0_ta.isEditable()) {
      me.consume();
    }
  }
  public void mouseReleased(MouseEvent me) {
    jud0_ta.requestFocus();
    if (jud0_ta.isEditable()) {
      me.consume();
    }
  }
  public void mouseClicked(MouseEvent me) {
    jud0_ta.requestFocus();
    if (jud0_ta.isEditable()) {
      me.consume();
    }
  }
  public void mouseEntered(MouseEvent me) {}
  public void mouseExited(MouseEvent me) {}

  //////////////////////////////////////////////////////////////////////////////
  //                            OUTPUT METHODS                                //
  //////////////////////////////////////////////////////////////////////////////
  public void printLine(String str) {
    jud0_ta.append(str + jud0_newline);
    jud0_drawingCanvas.requestFocus();
  }

  public void printLine(int num) {
    jud0_ta.append(num + jud0_newline);
    jud0_drawingCanvas.requestFocus();
  }

  public void printLine(double num) {
    jud0_ta.append(num + jud0_newline);
    jud0_drawingCanvas.requestFocus();
  }

  public void printLine(boolean bool) {
    jud0_ta.append(bool + jud0_newline);
    jud0_drawingCanvas.requestFocus();
  }

  public void printLine(Color color) {
    jud0_ta.append("red: " + color.getRed() + " green: " + color.getGreen() + " blue: " + color.getBlue() + jud0_newline);
    jud0_drawingCanvas.requestFocus();
  }

  public void print(String str) {
    jud0_ta.append(str);
    jud0_drawingCanvas.requestFocus();
  }

  public void print(int num) {
    jud0_ta.append("" + num);
    jud0_drawingCanvas.requestFocus();
  }

  public void print(double num) {
    jud0_ta.append("" + num);
    jud0_drawingCanvas.requestFocus();
  }

  public void print(boolean bool) {
    jud0_ta.append("" + bool);
    jud0_drawingCanvas.requestFocus();
  }

  public void print(Color color) {
    jud0_ta.append("red: " + color.getRed() + " green: " + color.getGreen() + " blue: " + color.getBlue());
    jud0_drawingCanvas.requestFocus();
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            INPUT METHODS                                 //
  //////////////////////////////////////////////////////////////////////////////
  public synchronized String readString() {
    Color backDisabledColor = jud0_ta.getBackground();
    String enteredString = "";
    String text = jud0_ta.getText();

    if (text.lastIndexOf(jud0_newline) != text.length() - jud0_newline.length()) {
      jud0_ta.append(jud0_newline);
    }
    jud0_taFirstPosition = jud0_ta.getText().length();
    jud0_ta.setEditable(true);
    jud0_ta.setBackground(Color.white);
    jud0_ta.requestFocus();
    jud0_ta.setCaretPosition(jud0_ta.getText().length()); // fixes MacOSX caret positioning problem
    try {
      this.wait();
    } catch (InterruptedException ie) {
      System.out.println("Interrupted Exception caught in JUDOBase.java readString");
      System.out.println(ie.toString());
      ie.printStackTrace();
    }
    jud0_ta.setBackground(backDisabledColor);
    enteredString = getEnteredString();
    //delay(.5);
    return enteredString;
  }

  public int readInt() {
    int enteredInt = MIN_INT;
    try {
      enteredInt = (new Integer(readString())).intValue();
    } catch (NumberFormatException nfe) {
      enteredInt = MIN_INT;
    }
    return enteredInt;
  }

  public double readDouble() {
    double enteredDouble = MIN_INT;
    try {
      String doubleString = readString();
      enteredDouble = (new Double(doubleString)).doubleValue();
    } catch (NumberFormatException nfe) {
      enteredDouble = MIN_INT;
    }
    return enteredDouble;
  }

  public boolean readBoolean() {
    boolean enteredBoolean = false;
    String booleanString = readString();
    enteredBoolean = (new Boolean(booleanString)).booleanValue();
    return enteredBoolean;
  }

  public Color readColor() {
    String colorString = readString();
    return getColor(colorString);
  }

  public Color getColor(String colorString) {
    char colorCharacters[] = colorString.toCharArray();
    String newColorString = "";
    for (int i = 0; i < colorCharacters.length; i++) {
      if (colorCharacters[i] != ' ') {
        newColorString += colorCharacters[i];
      }
    }
    newColorString = newColorString.toLowerCase();

    if (newColorString.equals("aliceblue")) {
      return aliceBlue;
    }
    else if (newColorString.equals("antiquewhite")) {
      return antiqueWhite;
    }
    else if (newColorString.equals("aqua")) {
      return aqua;
    }
    else if (newColorString.equals("aquamarine")) {
      return aquamarine;
    }
    else if (newColorString.equals("azure")) {
      return azure;
    }
    else if (newColorString.equals("beige")) {
      return beige;
    }
    else if (newColorString.equals("bisque")) {
      return bisque;
    }
    else if (newColorString.equals("black")) {
      return black;
    }
    else if (newColorString.equals("blanchedalmond")) {
      return blanchedAlmond;
    }
    else if (newColorString.equals("blue")) {
      return blue;
    }
    else if (newColorString.equals("blueviolet")) {
      return blueViolet;
    }
    else if (newColorString.equals("brown")) {
      return brown;
    }
    else if (newColorString.equals("burlywood")) {
      return burlyWood;
    }
    else if (newColorString.equals("cadetblue")) {
      return cadetBlue;
    }
    else if (newColorString.equals("chartreuse")) {
      return chartreuse;
    }
    else if (newColorString.equals("chocolate")) {
      return chocolate;
    }
    else if (newColorString.equals("coral")) {
      return coral;
    }
    else if (newColorString.equals("cornflowerblue")) {
      return cornflowerBlue;
    }
    else if (newColorString.equals("cornsilk")) {
      return cornsilk;
    }
    else if (newColorString.equals("crimson")) {
      return crimson;
    }
    else if (newColorString.equals("cyan")) {
      return cyan;
    }
    else if (newColorString.equals("darkblue")) {
      return darkBlue;
    }
    else if (newColorString.equals("darkcyan")) {
      return darkCyan;
    }
    else if (newColorString.equals("darkgoldenrod")) {
      return darkGoldenrod;
    }
    else if (newColorString.equals("darkgray")) {
      return darkGray;
    }
    else if (newColorString.equals("darkgreen")) {
      return darkGreen;
    }
    else if (newColorString.equals("darkkhaki")) {
      return darkKhaki;
    }
    else if (newColorString.equals("darkmagenta")) {
      return darkMagenta;
    }
    else if (newColorString.equals("darkolivegreen")) {
      return darkOliveGreen;
    }
    else if (newColorString.equals("darkorange")) {
      return darkOrange;
    }
    else if (newColorString.equals("darkorchid")) {
      return darkOrchid;
    }
    else if (newColorString.equals("darkred")) {
      return darkRed;
    }
    else if (newColorString.equals("darksalmon")) {
      return darkSalmon;
    }
    else if (newColorString.equals("darkseagreen")) {
      return darkSeaGreen;
    }
    else if (newColorString.equals("darkslateblue")) {
      return darkSlateBlue;
    }
    else if (newColorString.equals("darkslategray")) {
      return darkSlateGray;
    }
    else if (newColorString.equals("darkturquoise")) {
      return darkTurquoise;
    }
    else if (newColorString.equals("darkviolet")) {
      return darkViolet;
    }
    else if (newColorString.equals("deeppink")) {
      return deepPink;
    }
    else if (newColorString.equals("deepskyblue")) {
      return deepSkyBlue;
    }
    else if (newColorString.equals("dimgray")) {
      return dimGray;
    }
    else if (newColorString.equals("dodgerblue")) {
      return dodgerBlue;
    }
    else if (newColorString.equals("firebrick")) {
      return fireBrick;
    }
    else if (newColorString.equals("floralwhite")) {
      return floralWhite;
    }
    else if (newColorString.equals("forestgreen")) {
      return forestGreen;
    }
    else if (newColorString.equals("fuchsia")) {
      return fuchsia;
    }
    else if (newColorString.equals("gainsboro")) {
      return gainsboro;
    }
    else if (newColorString.equals("ghostwhite")) {
      return ghostWhite;
    }
    else if (newColorString.equals("gold")) {
      return gold;
    }
    else if (newColorString.equals("goldenrod")) {
      return goldenrod;
    }
    else if (newColorString.equals("gray")) {
      return gray;
    }
    else if (newColorString.equals("green")) {
      return green;
    }
    else if (newColorString.equals("greenyellow")) {
      return greenYellow;
    }
    else if (newColorString.equals("honeydew")) {
      return honeydew;
    }
    else if (newColorString.equals("hotpink")) {
      return hotPink;
    }
    else if (newColorString.equals("indianred")) {
      return indianRed;
    }
    else if (newColorString.equals("indigo")) {
      return indigo;
    }
    else if (newColorString.equals("ivory")) {
      return ivory;
    }
    else if (newColorString.equals("khaki")) {
      return khaki;
    }
    else if (newColorString.equals("lavender")) {
      return lavender;
    }
    else if (newColorString.equals("lavenderblush")) {
      return lavenderBlush;
    }
    else if (newColorString.equals("lawngreen")) {
      return lawnGreen;
    }
    else if (newColorString.equals("lemonchiffon")) {
      return lemonChiffon;
    }
    else if (newColorString.equals("lightblue")) {
      return lightBlue;
    }
    else if (newColorString.equals("lightcoral")) {
      return lightCoral;
    }
    else if (newColorString.equals("lightcyan")) {
      return lightCyan;
    }
    else if (newColorString.equals("lightgoldenrodyellow")) {
      return lightGoldenrodYellow;
    }
    else if (newColorString.equals("lightgreen")) {
      return lightGreen;
    }
    else if (newColorString.equals("lightgrey")) {
      return lightGrey;
    }
    else if (newColorString.equals("lightpink")) {
      return lightPink;
    }
    else if (newColorString.equals("lightsalmon")) {
      return lightSalmon;
    }
    else if (newColorString.equals("lightseagreen")) {
      return lightSeaGreen;
    }
    else if (newColorString.equals("lightskyblue")) {
      return lightSkyBlue;
    }
    else if (newColorString.equals("lightslategray")) {
      return lightSlateGray;
    }
    else if (newColorString.equals("lightsteelblue")) {
      return lightSteelBlue;
    }
    else if (newColorString.equals("lightyellow")) {
      return lightYellow;
    }
    else if (newColorString.equals("lime")) {
      return lime;
    }
    else if (newColorString.equals("limegreen")) {
      return limeGreen;
    }
    else if (newColorString.equals("linen")) {
      return linen;
    }
    else if (newColorString.equals("magenta")) {
      return magenta;
    }
    else if (newColorString.equals("maroon")) {
      return maroon;
    }
    else if (newColorString.equals("mediumaquamarine")) {
      return mediumAquamarine;
    }
    else if (newColorString.equals("mediumblue")) {
      return mediumBlue;
    }
    else if (newColorString.equals("mediumorchid")) {
      return mediumOrchid;
    }
    else if (newColorString.equals("mediumpurple")) {
      return mediumPurple;
    }
    else if (newColorString.equals("mediumseagreen")) {
      return mediumSeaGreen;
    }
    else if (newColorString.equals("mediumslateblue")) {
      return mediumSlateBlue;
    }
    else if (newColorString.equals("mediumspringgreen")) {
      return mediumSpringGreen;
    }
    else if (newColorString.equals("mediumturquoise")) {
      return mediumTurquoise;
    }
    else if (newColorString.equals("mediumvioletred")) {
      return mediumVioletRed;
    }
    else if (newColorString.equals("midnightblue")) {
      return midnightBlue;
    }
    else if (newColorString.equals("mintcream")) {
      return mintCream;
    }
    else if (newColorString.equals("mistyrose")) {
      return mistyRose;
    }
    else if (newColorString.equals("moccasin")) {
      return moccasin;
    }
    else if (newColorString.equals("navajowhite")) {
      return navajoWhite;
    }
    else if (newColorString.equals("navy")) {
      return navy;
    }
    else if (newColorString.equals("oldlace")) {
      return oldLace;
    }
    else if (newColorString.equals("olive")) {
      return olive;
    }
    else if (newColorString.equals("olivedrab")) {
      return oliveDrab;
    }
    else if (newColorString.equals("orange")) {
      return orange;
    }
    else if (newColorString.equals("orangered")) {
      return orangeRed;
    }
    else if (newColorString.equals("orchid")) {
      return orchid;
    }
    else if (newColorString.equals("palegoldenrod")) {
      return paleGoldenrod;
    }
    else if (newColorString.equals("palegreen")) {
      return paleGreen;
    }
    else if (newColorString.equals("paleturquoise")) {
      return paleTurquoise;
    }
    else if (newColorString.equals("palevioletred")) {
      return paleVioletRed;
    }
    else if (newColorString.equals("papayawhip")) {
      return papayaWhip;
    }
    else if (newColorString.equals("peachpuff")) {
      return peachPuff;
    }
    else if (newColorString.equals("peru")) {
      return peru;
    }
    else if (newColorString.equals("pink")) {
      return pink;
    }
    else if (newColorString.equals("plum")) {
      return plum;
    }
    else if (newColorString.equals("powderblue")) {
      return powderBlue;
    }
    else if (newColorString.equals("purple")) {
      return purple;
    }
    else if (newColorString.equals("red")) {
      return red;
    }
    else if (newColorString.equals("rosybrown")) {
      return rosyBrown;
    }
    else if (newColorString.equals("royalblue")) {
      return royalBlue;
    }
    else if (newColorString.equals("saddlebrown")) {
      return saddleBrown;
    }
    else if (newColorString.equals("salmon")) {
      return salmon;
    }
    else if (newColorString.equals("sandybrown")) {
      return sandyBrown;
    }
    else if (newColorString.equals("seagreen")) {
      return seaGreen;
    }
    else if (newColorString.equals("seashell")) {
      return seashell;
    }
    else if (newColorString.equals("sienna")) {
      return sienna;
    }
    else if (newColorString.equals("silver")) {
      return silver;
    }
    else if (newColorString.equals("skyblue")) {
      return skyBlue;
    }
    else if (newColorString.equals("slateblue")) {
      return slateBlue;
    }
    else if (newColorString.equals("slategray")) {
      return slateGray;
    }
    else if (newColorString.equals("snow")) {
      return snow;
    }
    else if (newColorString.equals("springgreen")) {
      return springGreen;
    }
    else if (newColorString.equals("steelblue")) {
      return steelBlue;
    }
    else if (newColorString.equals("tan")) {
      return tan;
    }
    else if (newColorString.equals("teal")) {
      return teal;
    }
    else if (newColorString.equals("thistle")) {
      return thistle;
    }
    else if (newColorString.equals("tomato")) {
      return tomato;
    }
    else if (newColorString.equals("turquoise")) {
      return turquoise;
    }
    else if (newColorString.equals("violet")) {
      return violet;
    }
    else if (newColorString.equals("wheat")) {
      return wheat;
    }
    else if (newColorString.equals("white")) {
      return white;
    }
    else if (newColorString.equals("whitesmoke")) {
      return whiteSmoke;
    }
    else if (newColorString.equals("yellow")) {
      return yellow;
    }
    else if (newColorString.equals("yellowgreen")) {
      return yellowGreen;
    }
    else if (newColorString.equals("lightgray")) {
      return lightGray;
    }
    else if (newColorString.equals("darkgrey")) {
      return darkGrey;
    }
    else if (newColorString.equals("darkslategrey")) {
      return darkSlateGrey;
    }
    else if (newColorString.equals("dimgrey")) {
      return dimGrey;
    }
    else if (newColorString.equals("grey")) {
      return grey;
    }
    else if (newColorString.equals("lightslategrey")) {
      return lightSlateGrey;
    }
    else if (newColorString.equals("slategrey")) {
      return slateGrey;
    }
    else {
      return black;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //                        KEYBOARD INPUT METHODS                            //
  //////////////////////////////////////////////////////////////////////////////
  public boolean getKeyState(String key) {
    jud0_keyStateListen = true;
    jud0_keyEventReceived = false;

    if (key.equals("space")) {
      key = " ";
    }
    synchronized(jud0_keyEvent) {
      return jud0_keyEvent.keyDown(key);
    }
  }

  public synchronized String readKey() {
    jud0_drawingCanvas.requestFocus();
    synchronized(jud0_keyEvent) {
      jud0_keyEvent.waitingForKey = true;
    }

    try {
      this.wait();
    } catch (InterruptedException ie) {
      System.out.println("Interrupted Exception caught in JUDOBase.java readKey");
      System.out.println(ie.toString());
      ie.printStackTrace();
    }

    synchronized(jud0_keyEvent) {
      return jud0_keyEvent.keyRead;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //                          MOUSE INPUT METHODS                             //
  //////////////////////////////////////////////////////////////////////////////
  public boolean getMouseEvent() {
    jud0_mouseEventListen = true;
    jud0_mouseEventReceived = false;

    synchronized (jud0_mouseEvent) {
      if (jud0_mouseEvent.newEvent) {
        jud0_mouseX = jud0_mouseEvent.x;
        jud0_mouseY = jud0_mouseEvent.y;
        jud0_mouseButton1 = jud0_mouseEvent.button1;
        jud0_mouseButton2 = jud0_mouseEvent.button2;
        jud0_mouseButton3 = jud0_mouseEvent.button3;
        jud0_mouseDragStartX1 = jud0_mouseEvent.dragStartX1;
        jud0_mouseDragStartY1 = jud0_mouseEvent.dragStartY1;
        jud0_mouseDragEndX1 = jud0_mouseEvent.dragEndX1;
        jud0_mouseDragEndY1 = jud0_mouseEvent.dragEndY1;
        jud0_mouseDragStartX2 = jud0_mouseEvent.dragStartX2;
        jud0_mouseDragStartY2 = jud0_mouseEvent.dragStartY2;
        jud0_mouseDragEndX2 = jud0_mouseEvent.dragEndX2;
        jud0_mouseDragEndY2 = jud0_mouseEvent.dragEndY2;
        jud0_mouseDragStartX3 = jud0_mouseEvent.dragStartX3;
        jud0_mouseDragStartY3 = jud0_mouseEvent.dragStartY3;
        jud0_mouseDragEndX3 = jud0_mouseEvent.dragEndX3;
        jud0_mouseDragEndY3 = jud0_mouseEvent.dragEndY3;
        jud0_mouseEvent.newEvent = false;

        return true;
      }
      else {
        return false;
      }
    }
  }
  public int getMouseX() {
    return jud0_mouseX;
  }
  public int getMouseY() {
    return jud0_mouseY;
  }
  public int getDragStartX(int buttonNumber) {
    if (buttonNumber == 1) {
      return jud0_mouseDragStartX1;
    }
    else if (buttonNumber == 2) {
      return jud0_mouseDragStartX2;
    }
    else if (buttonNumber == 3) {
      return jud0_mouseDragStartX3;
    }
    else {
      return 0;
    }
  }
  public int getDragStartY(int buttonNumber) {
    if (buttonNumber == 1) {
      return jud0_mouseDragStartY1;
    }
    else if (buttonNumber == 2) {
      return jud0_mouseDragStartY2;
    }
    else if (buttonNumber == 3) {
      return jud0_mouseDragStartY3;
    }
    else {
      return 0;
    }
  }
  public int getDragEndX(int buttonNumber) {
    if (buttonNumber == 1) {
      return jud0_mouseDragEndX1;
    }
    else if (buttonNumber == 2) {
      return jud0_mouseDragEndX2;
    }
    else if (buttonNumber == 3) {
      return jud0_mouseDragEndX3;
    }
    else {
      return 0;
    }
  }
  public int getDragEndY(int buttonNumber) {
    if (buttonNumber == 1) {
      return jud0_mouseDragEndY1;
    }
    else if (buttonNumber == 2) {
      return jud0_mouseDragEndY2;
    }
    else if (buttonNumber == 3) {
      return jud0_mouseDragEndY3;
    }
    else {
      return 0;
    }
  }

  public boolean getMouseButton(int buttonNumber) {
    if (buttonNumber == 1) {
      return jud0_mouseButton1;
    }
    else if (buttonNumber == 2) {
      return jud0_mouseButton2;
    }
    else if (buttonNumber == 3) {
      return jud0_mouseButton3;
    }
    else {
      return false;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            COLOR METHODS                                 //
  //////////////////////////////////////////////////////////////////////////////
  public Color darker(Color color) {
    return color.darker();
  }

  public Color brighter(Color color) {
    if (color.equals(black)) {
      return new Color(3, 3, 3);
    }
    else {
      return color.brighter();
    }
  }

  public Color getColor(int red, int green, int blue) {
    int newRed = Math.abs(red);
    int newGreen = Math.abs(green);
    int newBlue = Math.abs(blue);
    newRed = (newRed > 255) ? newRed % 255 : newRed;
    newGreen = (newGreen > 255) ? newGreen % 255 : newGreen;
    newBlue = (newBlue > 255) ? newBlue % 255 : newBlue;
    return new Color(newRed, newGreen, newBlue);
  }

  public int getRed(Color color) {
    return color.getRed();
  }

  public int getGreen(Color color) {
    return color.getGreen();
  }

  public int getBlue(Color color) {
    return color.getBlue();
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            TEST METHODS                                  //
  //////////////////////////////////////////////////////////////////////////////
  public boolean equal(String firstString, String secondString) {
    return firstString.equals(secondString);
  }

  public boolean equal(Color firstColor, Color secondColor) {
    return firstColor.equals(secondColor);
  }

  public boolean equal(int firstInt, int secondInt) {
    return firstInt == secondInt;
  }

  public boolean equal(boolean firstBoolean, boolean secondBoolean) {
    return firstBoolean == secondBoolean;
  }

  public boolean equal(double firstDouble, double secondDouble) {
    return firstDouble == secondDouble;
  }

  //////////////////////////////////////////////////////////////////////////////
  //                           RANDOM METHODS                                 //
  //////////////////////////////////////////////////////////////////////////////
//  public int randomInt() {
//    return jud0_random.nextInt();
//  }

  public int randomInt(int max) {
    return (int) (jud0_random.nextDouble() * max);
  }

  public double randomDouble() {
    return jud0_random.nextDouble();
  }

  public boolean randomBoolean() {
    return jud0_random.nextBoolean();
  }

  public Color randomColor() {
    return new Color(absoluteValue(jud0_random.nextInt() % 255),
                     absoluteValue(jud0_random.nextInt() % 255),
                     absoluteValue(jud0_random.nextInt() % 255));
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            MATH METHODS                                  //
  //////////////////////////////////////////////////////////////////////////////

  public int absoluteValue(int num) {
    return Math.abs(num);
  }

  public double absoluteValue(double num) {
    return Math.abs(num);
  }

  public double E = Math.E;
  public double PI = Math.PI;
  public int MIN_INT = Integer.MIN_VALUE;
  public int MAX_INT = Integer.MAX_VALUE;

  public double sin(double radians) {
    return Math.sin(radians);
  }

  public double cos(double radians) {
    return Math.cos(radians);
  }

  public double tan(double radians) {
    return Math.tan(radians);
  }

  public double squareRoot(double num) {
    return Math.sqrt(num);
  }

  public double log(double num) {
    return Math.log(num);
  }

  public double ceiling(double num) {
    return Math.ceil(num);
  }

  public double floor(double num) {
    return Math.floor(num);
  }

  public double round(double num) {
    return Math.round(num);
  }

  public double degreesToRadians(double degrees) {
    return Math.toRadians(degrees);
  }

  public double radiansToDegrees(double radians) {
    return Math.toDegrees(radians);
  }

  public double power(double a, double b) {
    return Math.pow(a, b);
  }

  public double arctan(double angle) {
    return Math.atan(angle);
  }

  public double arcsin(double angle) {
    return Math.asin(angle);
  }

  public double arccos(double angle) {
    return Math.acos(angle);
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            MISC METHODS                                  //
  //////////////////////////////////////////////////////////////////////////////
  public static void delay(double seconds) {
    try {
      Thread.sleep(Math.round(seconds * 1000));
    } catch (Exception e) {}
  }

  public int arrayLength(int[] intArray) {
    return intArray.length;
  }

  public int arrayLength(double[] doubleArray) {
    return doubleArray.length;
  }

  public int arrayLength(String[] stringArray) {
    return stringArray.length;
  }

  public int arrayLength(boolean[] booleanArray) {
    return booleanArray.length;
  }

  public int arrayLength(Color[] colorArray) {
    return colorArray.length;
  }

  public int doubleToInt(double num) {
    return (int) num;
  }

  public double intToDouble(int num) {
    return (double) num;
  }

  //////////////////////////////////////////////////////////////////////////////
  //                              TIME METHODS                                //
  //////////////////////////////////////////////////////////////////////////////
  private long jud0_startTime = System.currentTimeMillis();
  public void startTimer() {
    jud0_startTime = System.currentTimeMillis();
  }
  public double stopTimer() {
    long startTime = jud0_startTime;
    jud0_startTime = System.currentTimeMillis();  // becomes stop time and new start time

    return (1.0 * (jud0_startTime - startTime)) / 1000;
  }

  // getTimerEvent(double timo) - returns true, if timo seconds passed
  // since last call. Delays execution by timo seconds, if there are no
  // events from mouse and keyboard.
  //
  // In case of mouse or keyboard events, false is returned.
  // But only when getMouseEvent() orРё getKeyState() were
  // called before getTimerEvent().

  // Flags:
  // mouseEventReceived
  //   - set by mousePressed(), mouseReleased(), mouseMoved(), mouseDragged()
  //   - reset by getMouseEvent()
  // mouseEventListen
  //   - set by getMouseEvent()
  //   - reset by getTimerEvent()
  // keyEventReceived
  //   - set by keyPressed(), keyReleased()
  //   - reset by getKeyState()
  // keyStateListen
  //   - set by getKeyState()
  //   - reset by getTimerEvent()
  private boolean jud0_mouseEventReceived = false;
  private boolean jud0_mouseEventListen = false;
  private boolean jud0_keyEventReceived = false;
  private boolean jud0_keyStateListen = false;
  private long jud0_nextEventTime = 0;
  private javax.swing.Timer jud0_timer;

  public synchronized void timerFired() {
    this.notify();
  }
  public synchronized boolean getTimerEvent(double timo) {
    long msec = (long) (timo * 1000);
    long now = System.currentTimeMillis();
    if (jud0_nextEventTime == 0) {
      jud0_nextEventTime = now;
      jud0_timer = new javax.swing.Timer (10, new ActionListener() {
        public void actionPerformed(ActionEvent actionEvent) {
          timerFired();
        }
      });
    }

    jud0_timer.start();
    for (;;) {
      if (now >= jud0_nextEventTime) {
        jud0_nextEventTime += msec;
        jud0_timer.stop();
        return true;
      }
      if ((jud0_mouseEventListen && jud0_mouseEventReceived) ||
        (jud0_keyStateListen && jud0_keyEventReceived))
      {
        jud0_mouseEventListen = false;
        jud0_keyStateListen = false;
        jud0_timer.stop();
        return false;
      }

      try {
        this.wait();
      } catch (InterruptedException ie) {
        System.out.println("Interrupted Exception caught in JUDOBase.java getTimerEvent");
        System.out.println(ie.toString());
        ie.printStackTrace();
        jud0_timer.stop();
        return false;
      }
      now = System.currentTimeMillis();
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            GRAPHICS METHODS                              //
  //////////////////////////////////////////////////////////////////////////////
  public void setColor(Color color) {
    jud0_graphics.setColor(color);
  }

  public void drawLine(int x1, int y1, int x2, int y2) {
    jud0_graphics.drawLine(x1, y1, x2, y2);
  }

  public void drawRectangle(int x, int y, int width, int height) {
    jud0_graphics.drawRect(x, y, width, height);
    jud0_ta.append("");
  }

  public void fillRectangle(int x, int y, int width, int height) {
    jud0_graphics.fillRect(x, y, width, height);
    jud0_ta.append("");
  }

  public void clearRectangle(int x, int y, int width, int height) {
    Color tempColor = jud0_graphics.getColor();
    jud0_graphics.setColor(jud0_backgroundColor);
    jud0_graphics.fillRect(x, y, width, height);
    jud0_graphics.setColor(tempColor);
    // somehow fixes a clearRectangle right before a delay
    jud0_ta.append("");
  }

  public void clearDrawing() {
    clearRectangle(0, 0, getDrawingWidth(), getDrawingHeight());
  }

  public void drawOval(int x, int y, int width, int height) {
    jud0_graphics.drawOval(x, y, width, height);
  }

  public void fillOval(int x, int y, int width, int height) {
    jud0_graphics.fillOval(x, y, width, height);
  }

  public void drawCircle(int x, int y, int radius) {
    jud0_graphics.drawOval(x, y, radius * 2, radius * 2);
  }

  public void fillCircle(int x, int y, int radius) {
    jud0_graphics.fillOval(x, y, radius * 2, radius * 2);
  }

  public void drawString(String str, int x, int y) {
    jud0_graphics.drawString(str, x, y);
  }

  public void drawString(String str, int size, int x, int y) {
    Font oldFont = jud0_graphics.getFont();
    jud0_graphics.setFont(new Font(null, Font.PLAIN, size));
    jud0_graphics.drawString(str, x, y);
    jud0_graphics.setFont(oldFont);
  }

  public void drawString(String str, boolean bold, boolean italics,
                  int size, int x, int y) {
    drawString(str, null, bold, italics, size, x, y);
  }

  public void drawString(String str, String fontName,
                  boolean bold, boolean italics,
                  int size, int x, int y) {
    Font oldFont = jud0_graphics.getFont();
    int style = Font.PLAIN;
    if (bold) {
      style = Font.BOLD;
    }
    if (italics) {
      style |= Font.ITALIC;
    }
    jud0_graphics.setFont(new Font(fontName, style, size));
    jud0_graphics.drawString(str, x, y);
    jud0_graphics.setFont(oldFont);
  }

  public int getDrawingWidth() {
    return jud0_drawingCanvas.getSize().width;
  }

  public int getDrawingHeight() {
    return jud0_drawingCanvas.getSize().height;
  }

  public void setBackgroundColor(Color color) {
    jud0_backgroundColor = color;
    Color tempColor = jud0_graphics.getColor();
    jud0_graphics.setColor(color);
    jud0_graphics.fillRect(0, 0, getDrawingWidth(), getDrawingHeight());
    jud0_ta.append("");
    jud0_graphics.setColor(tempColor);
  }

  public void drawPolygon(int[] xPoints, int[] yPoints, int numPoints) {
    jud0_graphics.drawPolygon(xPoints, yPoints, numPoints);
  }

  public void fillPolygon(int[] xPoints, int[] yPoints, int numPoints) {
    jud0_graphics.fillPolygon(xPoints, yPoints, numPoints);
  }

  //////////////////////////////////////////////////////////////////////////////
  //                             STRING METHODS                               //
  //////////////////////////////////////////////////////////////////////////////
  public int indexOf(String haystack, String needle) {
    return haystack.indexOf(needle);
  }

  public int lastIndexOf(String haystack, String needle) {
    return haystack.lastIndexOf(needle);
  }

  public String substring(String str, int beginIndex, int endIndex) {
    return str.substring(beginIndex, endIndex);
  }

  public String substring(String str, int beginIndex) {
    return str.substring(beginIndex);
  }

  public int stringLength(String str) {
    return str.length();
  }

  public String characterAt(String str, int index) {
    return new Character(str.charAt(index)).toString();
  }

  public int compareStrings(String str1, String str2) {
    return str1.compareTo(str2);
  }

  public String toUpperCase(String str) {
    return str.toUpperCase();
  }

  public String toLowerCase(String str) {
    return str.toLowerCase();
  }

  //////////////////////////////////////////////////////////////////////////////
  //                           COLOR DEFINITIONS                              //
  //////////////////////////////////////////////////////////////////////////////
  public static Color lightGray = new Color(211, 211, 211);
  public static Color darkGrey = new Color(169, 169, 169);
  public static Color darkSlateGrey = new Color(47, 79, 79);
  public static Color dimGrey = new Color(105, 105, 105);
  public static Color grey = new Color(128, 128, 128);
  public static Color lightSlateGrey = new Color(119, 136, 153);
  public static Color slateGrey = new Color(112, 128, 144);

  public static Color aliceBlue = new Color(240, 248, 255);
  public static Color antiqueWhite = new Color(250, 235, 215);
  public static Color aqua = new Color(0, 255, 255);
  public static Color aquamarine = new Color(127, 255, 212);
  public static Color azure = new Color(240, 255, 255);
  public static Color beige = new Color(245, 245, 220);
  public static Color bisque = new Color(255, 228, 196);
  public static Color black = new Color(0, 0, 0);
  public static Color blanchedAlmond = new Color(255, 235, 205);
  public static Color blue = new Color(0, 0, 255);
  public static Color blueViolet = new Color(138, 43, 226);
  public static Color brown = new Color(165, 42, 42);
  public static Color burlyWood = new Color(222, 184, 135);
  public static Color cadetBlue = new Color(95, 158, 160);
  public static Color chartreuse = new Color(127, 255, 0);
  public static Color chocolate = new Color(210, 105, 30);
  public static Color coral = new Color(255, 127, 80);
  public static Color cornflowerBlue = new Color(100, 149, 237);
  public static Color cornsilk = new Color(255, 248, 220);
  public static Color crimson = new Color(220, 20, 60);
  public static Color cyan = new Color(0, 255, 255);
  public static Color darkBlue = new Color(0, 0, 139);
  public static Color darkCyan = new Color(0, 139, 139);
  public static Color darkGoldenrod = new Color(184, 134, 11);
  public static Color darkGray = new Color(169, 169, 169);
  public static Color darkGreen = new Color(0, 100, 0);
  public static Color darkKhaki = new Color(189, 183, 107);
  public static Color darkMagenta = new Color(139, 0, 139);
  public static Color darkOliveGreen = new Color(85, 107, 47);
  public static Color darkOrange = new Color(255, 140, 0);
  public static Color darkOrchid = new Color(153, 50, 204);
  public static Color darkRed = new Color(139, 0, 0);
  public static Color darkSalmon = new Color(233, 150, 122);
  public static Color darkSeaGreen = new Color(143, 188, 143);
  public static Color darkSlateBlue = new Color(72, 61, 139);
  public static Color darkSlateGray = new Color(47, 79, 79);
  public static Color darkTurquoise = new Color(0, 206, 209);
  public static Color darkViolet = new Color(148, 0, 211);
  public static Color deepPink = new Color(255, 20, 147);
  public static Color deepSkyBlue = new Color(0, 191, 255);
  public static Color dimGray = new Color(105, 105, 105);
  public static Color dodgerBlue = new Color(30, 144, 255);
  public static Color fireBrick = new Color(178, 34, 34);
  public static Color floralWhite = new Color(255, 250, 240);
  public static Color forestGreen = new Color(34, 139, 34);
  public static Color fuchsia = new Color(255, 0, 255);
  public static Color gainsboro = new Color(220, 220, 220);
  public static Color ghostWhite = new Color(248, 248, 255);
  public static Color gold = new Color(255, 215, 0);
  public static Color goldenrod = new Color(218, 165, 32);
  public static Color gray = new Color(128, 128, 128);
  public static Color green = new Color(0, 128, 0);
  public static Color greenYellow = new Color(173, 255, 47);
  public static Color honeydew = new Color(240, 255, 240);
  public static Color hotPink = new Color(255, 105, 180);
  public static Color indianRed = new Color(205, 92, 92);
  public static Color indigo = new Color(75, 0, 130);
  public static Color ivory = new Color(255, 255, 240);
  public static Color khaki = new Color(240, 230, 140);
  public static Color lavender = new Color(230, 230, 250);
  public static Color lavenderBlush = new Color(255, 240, 245);
  public static Color lawnGreen = new Color(124, 252, 0);
  public static Color lemonChiffon = new Color(255, 250, 205);
  public static Color lightBlue = new Color(173, 216, 230);
  public static Color lightCoral = new Color(240, 128, 128);
  public static Color lightCyan = new Color(224, 255, 255);
  public static Color lightGoldenrodYellow = new Color(250, 250, 210);
  public static Color lightGreen = new Color(144, 238, 144);
  public static Color lightGrey = new Color(211, 211, 211);
  public static Color lightPink = new Color(255, 182, 193);
  public static Color lightSalmon = new Color(255, 160, 122);
  public static Color lightSeaGreen = new Color(32, 178, 170);
  public static Color lightSkyBlue = new Color(135, 206, 250);
  public static Color lightSlateGray = new Color(119, 136, 153);
  public static Color lightSteelBlue = new Color(176, 196, 222);
  public static Color lightYellow = new Color(255, 255, 224);
  public static Color lime = new Color(0, 255, 0);
  public static Color limeGreen = new Color(50, 205, 50);
  public static Color linen = new Color(250, 240, 230);
  public static Color magenta = new Color(255, 0, 255);
  public static Color maroon = new Color(128, 0, 0);
  public static Color mediumAquamarine = new Color(102, 205, 170);
  public static Color mediumBlue = new Color(0, 0, 205);
  public static Color mediumOrchid = new Color(186, 85, 211);
  public static Color mediumPurple = new Color(147, 112, 219);
  public static Color mediumSeaGreen = new Color(60, 179, 113);
  public static Color mediumSlateBlue = new Color(123, 104, 238);
  public static Color mediumSpringGreen = new Color(0, 250, 154);
  public static Color mediumTurquoise = new Color(72, 209, 204);
  public static Color mediumVioletRed = new Color(199, 21, 133);
  public static Color midnightBlue = new Color(25, 25, 112);
  public static Color mintCream = new Color(245, 255, 250);
  public static Color mistyRose = new Color(255, 228, 225);
  public static Color moccasin = new Color(255, 228, 181);
  public static Color navajoWhite = new Color(255, 222, 173);
  public static Color navy = new Color(0, 0, 128);
  public static Color oldLace = new Color(253, 245, 230);
  public static Color olive = new Color(128, 128, 0);
  public static Color oliveDrab = new Color(107, 142, 35);
  public static Color orange = new Color(255, 165, 0);
  public static Color orangeRed = new Color(255, 69, 0);
  public static Color orchid = new Color(218, 112, 214);
  public static Color paleGoldenrod = new Color(238, 232, 170);
  public static Color paleGreen = new Color(152, 251, 152);
  public static Color paleTurquoise = new Color(175, 238, 238);
  public static Color paleVioletRed = new Color(219, 112, 147);
  public static Color papayaWhip = new Color(255, 239, 213);
  public static Color peachPuff = new Color(255, 218, 185);
  public static Color peru = new Color(205, 133, 63);
  public static Color pink = new Color(255, 192, 203);
  public static Color plum = new Color(221, 160, 221);
  public static Color powderBlue = new Color(176, 224, 230);
  public static Color purple = new Color(128, 0, 128);
  public static Color red = new Color(255, 0, 0);
  public static Color rosyBrown = new Color(188, 143, 143);
  public static Color royalBlue = new Color(65, 105, 225);
  public static Color saddleBrown = new Color(139, 69, 19);
  public static Color salmon = new Color(250, 128, 114);
  public static Color sandyBrown = new Color(244, 164, 96);
  public static Color seaGreen = new Color(46, 139, 87);
  public static Color seashell = new Color(255, 245, 238);
  public static Color sienna = new Color(160, 82, 45);
  public static Color silver = new Color(192, 192, 192);
  public static Color skyBlue = new Color(135, 206, 235);
  public static Color slateBlue = new Color(106, 90, 205);
  public static Color slateGray = new Color(112, 128, 144);
  public static Color snow = new Color(255, 250, 250);
  public static Color springGreen = new Color(0, 255, 127);
  public static Color steelBlue = new Color(70, 130, 180);
  public static Color tan = new Color(210, 180, 140);
  public static Color teal = new Color(0, 128, 128);
  public static Color thistle = new Color(216, 191, 216);
  public static Color tomato = new Color(255, 99, 71);
  public static Color turquoise = new Color(64, 224, 208);
  public static Color violet = new Color(238, 130, 238);
  public static Color wheat = new Color(245, 222, 179);
  public static Color white = new Color(255, 255, 255);
  public static Color whiteSmoke = new Color(245, 245, 245);
  public static Color yellow = new Color(255, 255, 0);
  public static Color yellowGreen = new Color(154, 205, 50);
}
