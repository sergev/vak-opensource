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
import java.applet.Applet;
import java.util.Random;

/**
 * The base class for JUDO Applets, takes care of all the text and graphics I/O.
 * Provides many JUDO functions for use in user programs, as well as lots
 * of predifined colors with nice names
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class JUDOAppletBase extends Applet implements KeyListener, MouseListener {

  static final String newline = System.getProperty("line.separator");

  static Graphics graphics;
  static Canvas drawingCanvas;
  TextArea ta;
  Random random = new Random();

  void initGui(String appType, int width, int height, int rows) {
    setLayout(new BorderLayout());
    setBackground(Color.white);
    drawingCanvas = new Canvas();
    ta = new TextArea(rows, 1);

    if (appType.equals(JUDOBase.TYPE_BOTH)) {
      add(BorderLayout.CENTER, drawingCanvas);
      ta = new TextArea(rows, 1);
      ta.setEditable(false);
      ta.addKeyListener(this);
      ta.addMouseListener(this);
      add(BorderLayout.SOUTH, ta);
    }
    else if (appType.equals(JUDOBase.TYPE_GRAPHICS)) {
      drawingCanvas = new Canvas();
      add(BorderLayout.CENTER, drawingCanvas);
    }
    else {
      drawingCanvas = new Canvas();
      ta = new TextArea(rows, 1);
      ta.setEditable(false);
      ta.addKeyListener(this);
      ta.addMouseListener(this);
      add(BorderLayout.CENTER, ta);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //                      TEXTAREA PROCESSING METHODS                         //
  //////////////////////////////////////////////////////////////////////////////
  public void keyTyped(KeyEvent e) {
    processKey(e);
  }
  public void keyPressed(KeyEvent e) {
    processKey(e);
  }
  public void keyReleased(KeyEvent e) {
    processKey(e);
  }
  synchronized protected void processKey(KeyEvent e) {
    int keyCode = e.getKeyCode();
    if (keyCode == KeyEvent.VK_ENTER) {
      e.consume();
      ta.setEditable(false);
      this.notify();
    }
    if (keyCode == KeyEvent.VK_PAGE_UP ||
        keyCode == KeyEvent.VK_PAGE_DOWN ||
        keyCode == KeyEvent.VK_UP ||
        keyCode == KeyEvent.VK_DOWN) {
      e.consume();
    }
  }
  String getLine() {
    String text = ta.getText();
    int newlineIndex = text.lastIndexOf(newline);
    if (newlineIndex >= 0) {
      String tmp = text.substring(newlineIndex + newline.length(), text.length());
      return tmp;
    }
    else {
      return text;
    }
  }
  public String getEnteredString() {
    String enteredString = "";
    enteredString = getLine();
    ta.append(newline);
    ta.setCaretPosition(ta.getText().length());
    return enteredString;
  }

  public void mousePressed(MouseEvent me) {
    if (ta.isEditable()) {
      me.consume();
    }
  }
  public void mouseReleased(MouseEvent me) {
    if (ta.isEditable()) {
      me.consume();
    }
  }
  public void mouseClicked(MouseEvent me) {
    if (ta.isEditable()) {
      me.consume();
    }
  }
  public void mouseEntered(MouseEvent me) {}
  public void mouseExited(MouseEvent me) {}

  //////////////////////////////////////////////////////////////////////////////
  //                            OUTPUT METHODS                                //
  //////////////////////////////////////////////////////////////////////////////
  void printLine(String str) {
    ta.append(str + newline);
  }

  void printLine(int num) {
    ta.append(num + newline);
  }

  void printLine(double num) {
    ta.append(num + newline);
  }

  void printLine(boolean bool) {
    ta.append(bool + newline);
  }

  void printLine(Color color) {
    ta.append(color.toString() + newline);
  }

  void print(String str) {
    ta.append(str);
  }

  void print(int num) {
    ta.append("" + num);
  }

  void print(double num) {
    ta.append("" + num);
  }

  void print(boolean bool) {
    ta.append("" + bool);
  }

  void print(Color color) {
    ta.append(color.toString());
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            INPUT METHODS                                 //
  //////////////////////////////////////////////////////////////////////////////
  synchronized String readString() {
    String enteredString = "";
    String text = ta.getText();
    if (text.lastIndexOf(newline) != text.length() - newline.length()) {
      ta.append(newline);
    }
    ta.setEditable(true);
    try {
      this.wait();
    } catch (InterruptedException ie) {
      System.out.println("Interrupted Exception caught in JUDOBase.java");
      System.out.println(ie.toString());
      ie.printStackTrace();
    }
    enteredString = getEnteredString();
    delay(.2);
    return enteredString;
  }

  int readInt() {
    int enteredInt = -999999;
    try {
      enteredInt = (new Integer(readString())).intValue();
    } catch (NumberFormatException nfe) {
      enteredInt = -999999;
    }
    return enteredInt;
  }

  double readDouble() {
    double enteredDouble = -999999.999999;
    try {
      String doubleString = readString();
      enteredDouble = (new Double(doubleString)).doubleValue();
    } catch (NumberFormatException nfe) {
      enteredDouble = -999999.999999;
    }
    return enteredDouble;
  }

  boolean readBoolean() {
    boolean enteredBoolean = false;
    String booleanString = readString();
    enteredBoolean = (new Boolean(booleanString)).booleanValue();
    return enteredBoolean;
  }

  Color readColor() {
    String colorString = readString();
    return getColor(colorString);
  }

  Color getColor(String colorString) {
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
    else {
      return black;
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
  public int randomInt() {
    return random.nextInt();
  }

  public double randomDouble() {
    return random.nextDouble();
  }

  public boolean randomBoolean() {
    return random.nextBoolean();
  }

  public Color randomColor() {
    return new Color(absoluteValue(random.nextInt() % 255),
                     absoluteValue(random.nextInt() % 255),
                     absoluteValue(random.nextInt() % 255));
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

  //////////////////////////////////////////////////////////////////////////////
  //                            MISC METHODS                                  //
  //////////////////////////////////////////////////////////////////////////////
  static void delay(double seconds) {
    try {
      Thread.sleep(Math.round(seconds * 1000));
    } catch (Exception e) {}
  }

  //////////////////////////////////////////////////////////////////////////////
  //                            GRAPHICS METHODS                              //
  //////////////////////////////////////////////////////////////////////////////
  void setColor(Color color) {
    graphics.setColor(color);
  }

  void drawLine(int x1, int y1, int x2, int y2) {
    graphics.drawLine(x1, y1, x2, y2);
  }

  void drawRectangle(int x, int y, int width, int height) {
    graphics.drawRect(x, y, width, height);
  }

  void fillRectangle(int x, int y, int width, int height) {
    graphics.fillRect(x, y, width, height);
  }

  void clearRectange(int x, int y, int width, int height) {
    graphics.clearRect(x, y, width, height);
  }

  void drawOval(int x, int y, int width, int height) {
    graphics.drawOval(x, y, width, height);
  }

  void fillOval(int x, int y, int width, int height) {
    graphics.fillOval(x, y, width, height);
  }

  void drawCircle(int x, int y, int radius) {
    graphics.drawOval(x, y, radius, radius);
  }

  void fillCircle(int x, int y, int radius) {
    graphics.fillOval(x, y, radius, radius);
  }

  void drawString(String str, int x, int y) {
    graphics.drawString(str, x, y);
  }

  void drawString(String str, int size, int x, int y) {
    Font oldFont = graphics.getFont();
    graphics.setFont(new Font(null, Font.PLAIN, size));
    graphics.drawString(str, x, y);
    graphics.setFont(oldFont);
  }

  void drawString(String str, boolean bold, boolean italics,
                  int size, int x, int y) {
    drawString(str, null, bold, italics, size, x, y);
  }

  void drawString(String str, String fontName,
                  boolean bold, boolean italics,
                  int size, int x, int y) {
    Font oldFont = graphics.getFont();
    int style = Font.PLAIN;
    if (bold) {
      style = Font.BOLD;
    }
    if (italics) {
      style |= Font.ITALIC;
    }
    graphics.setFont(new Font(fontName, style, size));
    graphics.drawString(str, x, y);
    graphics.setFont(oldFont);
  }

  int getDrawingWidth() {
    return drawingCanvas.getSize().width;
  }

  int getDrawingHeight() {
    return drawingCanvas.getSize().height;
  }

  void setDrawingBackground(Color color) {
    drawingCanvas.setBackground(color);
  }

  //////////////////////////////////////////////////////////////////////////////
  //                           COLOR DEFINITIONS                              //
  //////////////////////////////////////////////////////////////////////////////
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