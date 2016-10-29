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

import java.awt.event.*;
import java.awt.*;
import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import java.net.URL;
import javax.swing.undo.*;
import javax.swing.text.Document;

/**
 * The main class of the IDE.  Everything is controlled via the action of the controls in this window.
 *
 * @author Thomas Dunn
 * @version 1.3
 */
public class JUDOIDE extends JFrame implements ActionListener, WindowListener, DocumentListener, FilenameFilter {

  /////////////////////////////////////////////////////////////////////////
  // CLASS AND MEMBER DATA

  // a newline
  static String newline = System.getProperty("line.separator");
  static String pathSeparator = System.getProperty("file.separator");

  // for interpolating vars in String for localization
  public static JUDOUtils ju = new JUDOUtils();

  // the localization language
  static String judoLanguage;
  static String judoProgramDir;
  static String userHomeDirKey = "$HOME";

  static String programDirectory = "programs";
  String helpSamplesDir = "samples";

  public static JUDO_localization lz;
  static {
    readConfiguration();
  }

  final static String JUDO_VERSION = "1.3.0";
  final static String JUDOAPP_TITLE = "TITLE";
  final static String JUDO_TYPE = "TYPE";
  final static String JUDO_KEY_PREFIX = "$";
  final static String JUDO_PROP_COMMENT = "//~JUDOPROP~//";
  final static String JUDO_MAIN_STRING = "//<judomain>";

//  final static int HELP_WINDOW_WIDTH = 590;
//  final static int HELP_WINDOW_HEIGHT = 375;
  final static int HELP_WINDOW_WIDTH = 800;
  final static int HELP_WINDOW_HEIGHT = 700;
  final static int JUDO_IDE_WIDTH = 800;
  final static int JUDO_IDE_HEIGHT = 800;

  final static int JUDO_APP = 0;
  final static int JUDO_APPLET = 1;

  int appHeight = JUDOBase.jud0_APP_HEIGHT_TEXT;
  int appWidth = JUDOBase.jud0_APP_WIDTH;
  int appRows = JUDOBase.jud0_APP_TEXT_ROWS;

  // instead of reading dealing with properties dialog, this var is used
  String appType = JUDOBase.jud0_TYPE_TEXT;

  // to help determine program type
  boolean hasText = false;
  boolean hasGraphics = false;

  // dialogs
  JUDOSaveDialog saveDialog;
  JUDOOpenDialog openDialog;
  JUDODeleteDialog deleteDialog;

  String saveIconFilename = "Save24.gif";
  String openIconFilename = "Open24.gif";
  String deleteIconFilename = "Delete24.gif";
  String judoIconFilename = "judo16.gif";
  String judoAppTemplateFilename = "JUDOApp.template";
  String judoAppletTemplateFilename = "JUDOApplet.template";
  String judoAppJavaFilename = "JUDOApp.java";
  String judoAppletJavaFilename = "JUDOApplet.java";
  String floppyPath = "A:" + pathSeparator;
  String extension = "judo";
  String titlePrefix = " JUDO - ";

  // the name of a program after it has been saved.
  protected String defaultProgramName = lz.IDE_DEFAULT_PROG_NAME;
  String programName = defaultProgramName;
  String lastProgramName = "";
  boolean lastSaveDefault = true;

  // Headers to put at the top of users program
  HashMap headerMap;

  // key/value pairs for interpolating in template
  HashMap templateMap;
  String templateKeyPrefix = "[[";
  String templateKeySuffix = "]]";

  // the program name, lowercased, with spaces removed
  String programFilename;

  // if program text has been changed since a save
  boolean isDirty = false;

  boolean isWindows = false;
  boolean isMacOSX = false;

  // location of source code
  String codeBase;
  String packagePath;

  // binary locations
  String jdkBase;
  String jdkBin;
  String javacString;
  String javaString;

  String classpath;

  // number of lines in the header code - for offset in error messages
  int headerLines;

  // if there was an error in compilation
  boolean error = true;

  ///////////////////
  // GUI COMPONENTS

  JMenuBar menuBar;

  JMenu fileMenu;
  JMenuItem newMenuItem;
  JMenuItem saveMenuItem;
  JMenuItem saveAsMenuItem;
  JMenuItem saveToFloppyMenuItem;
  JMenuItem saveAsToFloppyMenuItem;
  JMenuItem openMenuItem;
  JMenuItem openFromFloppyMenuItem;
  JMenuItem openSampleMenuItem;
  JMenuItem deleteMenuItem;
  JMenuItem deleteFromFloppyMenuItem;
  JMenuItem exitMenuItem;

  JMenu editMenu;
  JMenuItem copyMenuItem;
  JMenuItem cutMenuItem;
  JMenuItem pasteMenuItem;
  JMenuItem undoMenuItem;
  JMenuItem gotoLineMenuItem;
  JMenuItem redoMenuItem;
  JMenuItem selectAllMenuItem;

  JMenu programMenu;
  JMenuItem appletWizardMenuItem;

  JMenu helpMenu;
  JMenuItem judoHelpMenuItem;
  JMenuItem tutorialsMenuItem;
  JMenuItem functionReferenceMenuItem;
  JMenuItem colorReferenceMenuItem;
  JMenuItem aboutMenuItem;
  JMenuItem judoWebsiteMenuItem;

  JButton runButton;
  JTextArea codeTextArea;

  static JTextArea outputTextArea;

  JSplitPane splitPane;

  /**
   * For copy/cut/paste
   */
  Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();

  /**
   * For undo/redo
   */
  final UndoManager undoManager = new UndoManager();

  // end - CLASS AND MEMBER DATA
  /////////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////////
  // THE FOLLOWING METHODS ARE FOR JUDO INITIALIZATION
  public JUDOIDE() {
    super("JUDO");
    setTitle(titlePrefix + defaultProgramName);
    setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    templateMap = new HashMap();
    headerMap = new HashMap();
    headerMap.put("VERSION", JUDO_VERSION);

    if (((String) System.getProperty("os.name")).startsWith("Windows")) {
      isWindows = true;
    }
    else if (((String) System.getProperty("os.name")).startsWith("Mac OS X")) {
      isMacOSX = true;
    }

    buildGUI();

    if (! initJava()) {
      displayErrorPane();
    }
    // initialize the save dialog
    saveDialog = new JUDOSaveDialog(this, this, codeBase + pathSeparator + "images" + pathSeparator + saveIconFilename, lz.IDE_SAVE_PROG);
    saveDialog.pack();

    // where the help sample programs are located
    helpSamplesDir = codeBase + pathSeparator + helpSamplesDir;

    this.setIconImage(new ImageIcon(codeBase + pathSeparator + "images" + pathSeparator + judoIconFilename).getImage());

    // install undo manager instance as listener for undoable edits
    codeTextArea.getDocument().addUndoableEditListener(undoManager);
  }

  void displayErrorPane() {
    JPanel errorPane = new JPanel();
    JLabel errorLabel = new JLabel();
    errorLabel.setText(lz.IDE_ERR_INIT_JAVA);
    errorPane.add(errorLabel);
    getContentPane().add(BorderLayout.CENTER, errorPane);
  }

  /**
   * Initialized java system
   *
   * There are two main configurations for JUDO/Java installations.
   *
   * If it is a Windows system with the full installer, JUDO would
   * have come both with copies of a JRE1.3 for running JUDO, and a copy
   * of jdk1.3 under the codebase.  If this is the configuration, then
   * use the supplied jdk1.3
   *
   * If it is a Linux, Solaris, MacOS, other, or Windows without the full
   * installer system, then the user must have previously installed a
   * Java 2 JDK.  There would be no JRE or JDK with JUDO.
   * If this is the configuration, then use the jdk found from the java.home path
   */
  private boolean initJava() {
    String javaHomeString = System.getProperty("java.home");
    String javaVersionString = System.getProperty("java.version");
    String javaVendorString = System.getProperty("java.vendor");
    codeBase = System.getProperty("user.dir");
    int javaIndex;

    classpath = System.getProperty("java.class.path");

    // a jdk1.3 bundled with JUDO was found
    String jdk13 = codeBase + pathSeparator + "jdk1.3";
    if ((new File(jdk13)).exists()) {
      jdkBase = jdk13;
    }
    // no jdk1.3 bundled with JUDO was found, use the one that should be
    // running JUDO right now.
    else {
      if (isMacOSX) {
        jdkBase = javaHomeString;
      }
      else {
        javaIndex = javaHomeString.toUpperCase().lastIndexOf((pathSeparator + "jre").toUpperCase());
        if (javaIndex == -1) {
            jdkBase = javaHomeString;
//            return false;
        }
        else {
          jdkBase = javaHomeString.substring(0, javaIndex);
        }
      }
    }
    if ((new File(jdkBase + pathSeparator + "jdk" + javaVersionString)).exists()) {
      jdkBase = jdkBase + pathSeparator + "jdk" + javaVersionString;
    }
    jdkBin = jdkBase + pathSeparator + "bin";
    javacString = jdkBin + pathSeparator + "javac";

    // use javaw on windows since it does not pop up an extra annoying box
    if (isWindows) {
      javaString = jdkBin + pathSeparator + "javaw";
    }
    else {
      javaString = jdkBin + pathSeparator + "java";
    }
    packagePath = "org" + pathSeparator + "judo";

    return true;
  }

  private static void readConfiguration() {
    readConfigurationFile("judo.properties"); // the main installation configuration
    readConfigurationFile(System.getProperty("user.home") + pathSeparator + ".judo.properties"); // configuration for local user
  }

  private static void readConfigurationFile(String file) {
    if ((new File(file)).exists()) {
      Properties prop = new Properties();
      try {
        prop.load(new FileInputStream(file));
        judoLanguage = prop.getProperty("judo.language");
        if (judoLanguage == null) {
          judoLanguage = "en";
        }

        judoProgramDir = prop.getProperty("judo.programdir");
        if (judoProgramDir != null) {
          int homeIndex = judoProgramDir.indexOf(userHomeDirKey);
          if (homeIndex != -1) {
            StringBuffer progDirBuffer = new StringBuffer(judoProgramDir);
            progDirBuffer.replace(homeIndex, homeIndex + userHomeDirKey.length(), System.getProperty("user.home"));
            judoProgramDir = progDirBuffer.toString();
          }
        }
        else {
          judoProgramDir = System.getProperty("user.dir") + pathSeparator + programDirectory;
        }
      }
      catch (IOException ioe) {
        // default to english
        judoLanguage = "en";
      }

      // localization
      try {
        Class lzClass = Class.forName("org.judo.JUDO_" + judoLanguage);
        lz = (JUDO_localization) lzClass.newInstance();
      }
      catch (Exception e) {
        e.printStackTrace();
        System.out.println("Reflection error in JUDOIDE when creating localization object: " + e.toString());
      }
    }
  }

  private void buildGUI() {

    getContentPane().setLayout(new BorderLayout());

    menuBar = new JMenuBar();
    fileMenu = new JMenu(lz.IDE_FILE);
    fileMenu.setMnemonic('f');
    newMenuItem = new JMenuItem(lz.IDE_FILE_NEW, KeyEvent.VK_N);
    newMenuItem.setMnemonic('n');
    newMenuItem.addActionListener(this);
    saveMenuItem = new JMenuItem(lz.IDE_FILE_SAVE, KeyEvent.VK_S);
    saveMenuItem.setMnemonic('s');
    saveMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
    saveMenuItem.addActionListener(this);
    saveAsMenuItem = new JMenuItem(lz.IDE_FILE_SAVE_NEW_NAME);
    saveAsMenuItem.setMnemonic('a');
    saveAsMenuItem.addActionListener(this);
    saveToFloppyMenuItem = new JMenuItem(lz.IDE_FILE_SAVE_FLOPPY);
    saveToFloppyMenuItem.setMnemonic('f');
    saveToFloppyMenuItem.addActionListener(this);
    saveAsToFloppyMenuItem = new JMenuItem(lz.IDE_FILE_SAVE_FLOPPY_NEW_NAME);
    saveAsToFloppyMenuItem.addActionListener(this);
    openMenuItem = new JMenuItem(lz.IDE_FILE_OPEN, KeyEvent.VK_O);
    openMenuItem.setMnemonic('o');
    openMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
    openMenuItem.addActionListener(this);
    openFromFloppyMenuItem = new JMenuItem(lz.IDE_FILE_OPEN_FLOPPY);
    openFromFloppyMenuItem.addActionListener(this);
    openSampleMenuItem = new JMenuItem(lz.IDE_FILE_OPEN_SAMPLE);
    openSampleMenuItem.addActionListener(this);
    deleteMenuItem = new JMenuItem(lz.IDE_FILE_DELETE);
    deleteMenuItem.addActionListener(this);
    deleteFromFloppyMenuItem = new JMenuItem(lz.IDE_FILE_DELETE_FLOPPY);
    deleteFromFloppyMenuItem.addActionListener(this);
    exitMenuItem = new JMenuItem(lz.IDE_FILE_EXIT);
    exitMenuItem.setMnemonic('x');
    exitMenuItem.addActionListener(this);

    editMenu = new JMenu(lz.IDE_EDIT);
    editMenu.setMnemonic('e');
    copyMenuItem = new JMenuItem(lz.IDE_EDIT_COPY, KeyEvent.VK_C);
    copyMenuItem.setMnemonic('c');
    copyMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, ActionEvent.CTRL_MASK));
    copyMenuItem.addActionListener(this);
    cutMenuItem = new JMenuItem(lz.IDE_EDIT_CUT, KeyEvent.VK_X);
    cutMenuItem.setMnemonic('t');
    cutMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ActionEvent.CTRL_MASK));
    cutMenuItem.addActionListener(this);
    pasteMenuItem = new JMenuItem(lz.IDE_EDIT_PASTE, KeyEvent.VK_V);
    pasteMenuItem.setMnemonic('p');
    pasteMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, ActionEvent.CTRL_MASK));
    pasteMenuItem.addActionListener(this);
    undoMenuItem = new JMenuItem(lz.IDE_EDIT_UNDO, KeyEvent.VK_Z);
    undoMenuItem.setMnemonic('u');
    undoMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ActionEvent.CTRL_MASK));
    undoMenuItem.addActionListener(this);
    redoMenuItem = new JMenuItem(lz.IDE_EDIT_REDO, KeyEvent.VK_Y);
    redoMenuItem.setMnemonic('r');
    redoMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
    redoMenuItem.addActionListener(this);
    gotoLineMenuItem = new JMenuItem(lz.IDE_EDIT_GOTO_LINE, KeyEvent.VK_G);
    gotoLineMenuItem.setMnemonic('g');
    gotoLineMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
    gotoLineMenuItem.addActionListener(this);
    selectAllMenuItem = new JMenuItem(lz.IDE_EDIT_SELECT_ALL, KeyEvent.VK_A);
    selectAllMenuItem.setMnemonic('a');
    selectAllMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, ActionEvent.CTRL_MASK));
    selectAllMenuItem.addActionListener(this);

//    programMenu = new JMenu(lz.IDE_PROG);
//    programMenu.setMnemonic('p');
//    appletWizardMenuItem = new JMenuItem("Applet Wizard...");
//    appletWizardMenuItem.setMnemonic('a');
//    appletWizardMenuItem.addActionListener(this);

    helpMenu = new JMenu(lz.IDE_HELP);
    helpMenu.setMnemonic('h');
    judoHelpMenuItem = new JMenuItem(lz.IDE_HELP_JUDO_HELP, KeyEvent.VK_H);
    judoHelpMenuItem.setMnemonic('h');
    judoHelpMenuItem.addActionListener(this);
    tutorialsMenuItem = new JMenuItem(lz.IDE_HELP_TUTORIALS, KeyEvent.VK_T);
    tutorialsMenuItem.setMnemonic('t');
    tutorialsMenuItem.addActionListener(this);

    functionReferenceMenuItem = new JMenuItem(lz.IDE_HELP_FUNC_REF, KeyEvent.VK_F);
    functionReferenceMenuItem.setMnemonic('f');
    functionReferenceMenuItem.addActionListener(this);
    colorReferenceMenuItem = new JMenuItem(lz.IDE_HELP_COLOR_REF, KeyEvent.VK_C);
    colorReferenceMenuItem.setMnemonic('c');
    colorReferenceMenuItem.addActionListener(this);
    aboutMenuItem = new JMenuItem(lz.IDE_HELP_ABOUT_JUDO, KeyEvent.VK_A);
    aboutMenuItem.setMnemonic('a');
    aboutMenuItem.addActionListener(this);
    judoWebsiteMenuItem = new JMenuItem(lz.IDE_HELP_JUDO_WEBSITE, KeyEvent.VK_W);
    judoWebsiteMenuItem.setMnemonic('w');
    judoWebsiteMenuItem.addActionListener(this);

    setJMenuBar(menuBar);

    menuBar.add(fileMenu);
    fileMenu.add(newMenuItem);
    fileMenu.addSeparator();
    fileMenu.add(openMenuItem);
    if (isWindows) {
      fileMenu.add(openFromFloppyMenuItem);
    }
    fileMenu.add(openSampleMenuItem);
    fileMenu.addSeparator();

    fileMenu.add(saveMenuItem);
    fileMenu.add(saveAsMenuItem);
    fileMenu.addSeparator();
    if (isWindows) {
      fileMenu.add(saveToFloppyMenuItem);
      fileMenu.add(saveAsToFloppyMenuItem);
      fileMenu.addSeparator();
    }
    fileMenu.add(deleteMenuItem);
    if (isWindows) {
      fileMenu.add(deleteFromFloppyMenuItem);
    }
    fileMenu.addSeparator();
    fileMenu.add(exitMenuItem);

    menuBar.add(editMenu);
    editMenu.add(undoMenuItem);
    editMenu.add(redoMenuItem);
    editMenu.addSeparator();
    editMenu.add(cutMenuItem);
    editMenu.add(copyMenuItem);
    editMenu.add(pasteMenuItem);
    editMenu.addSeparator();
    editMenu.add(selectAllMenuItem);
    editMenu.add(gotoLineMenuItem);

//    menuBar.add(programMenu);

    menuBar.add(helpMenu);
//    helpMenu.add(judoHelpMenuItem);
//    helpMenu.addSeparator();
    helpMenu.add(tutorialsMenuItem);
    helpMenu.add(functionReferenceMenuItem);
    helpMenu.add(colorReferenceMenuItem);
    helpMenu.addSeparator();
    helpMenu.add(judoWebsiteMenuItem);
    helpMenu.add(aboutMenuItem);

    runButton = new JButton(lz.IDE_BUTTON_RUN);
    runButton.addActionListener(this);
    runButton.setMnemonic('r');

    JPanel controlPanel = new JPanel();
    controlPanel.add(runButton);
    getContentPane().add(BorderLayout.NORTH, controlPanel);

    outputTextArea = new JTextArea(4, 1);
    outputTextArea.setEditable(false);
    //outputTextArea.setTabSize(3); // don't use, carets won't point at errors
    ErrorMessageListener errorMessageListener = new ErrorMessageListener();
    outputTextArea.addMouseListener(errorMessageListener);

    JScrollPane outputScrollPane = new JScrollPane(outputTextArea);

    codeTextArea = new JTextArea();
    codeTextArea.setForeground(Color.white);
    codeTextArea.setBackground(Color.black);
    codeTextArea.setCaretColor(Color.white);
    codeTextArea.setTabSize(4);
    codeTextArea.getDocument().addDocumentListener(this);
    JScrollPane codeScrollPane = new JScrollPane(codeTextArea);

    // Select monospaced font.
    Font font = new Font("Lucida Sans Typewriter", Font.PLAIN, 16);
    if (font.getName() != "Lucida Sans Typewriter") {
        font = new Font("Monaco", Font.PLAIN, 16);
        if (font.getName() != "Monaco") {
            font = new Font("Monospaced", Font.PLAIN, 16);
        }
    }
    codeTextArea.setFont(font);
    outputTextArea.setFont(font.deriveFont(Font.PLAIN, 14));

    splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT,
                               codeScrollPane, outputScrollPane);

    splitPane.setOneTouchExpandable(true);
    splitPane.setDividerLocation(630);
    splitPane.setResizeWeight(.75);

    //Provide minimum sizes for the two components in the split pane
    Dimension minimumSize = new Dimension(100, 50);
    codeScrollPane.setMinimumSize(minimumSize);
    outputScrollPane.setMinimumSize(minimumSize);

    getContentPane().add(BorderLayout.CENTER, splitPane);
  }

  public class ErrorMessageListener extends MouseAdapter {
    public void mouseClicked(MouseEvent me) {
      if (me.getClickCount() == 2) {
        String errorText = outputTextArea.getText();
        String errorLineText = "Error on line ";
        if (errorText.indexOf("rror") != -1) {
          int caretIndex = outputTextArea.getCaretPosition();
          int errorIndex = errorText.lastIndexOf(errorLineText, caretIndex);
          if (errorIndex != -1) {
            int colonIndex = errorText.indexOf(":", errorIndex);
            int lineNum = new Integer(errorText.substring(errorIndex + errorLineText.length(), colonIndex)).intValue();
            setLineNumber(lineNum);
          }
        }
      }
    }
  }

  /**
   * This is where it all starts
   */
  public static void main(String args[]) {
    JUDOIDE ide = new JUDOIDE();

    ide.addWindowListener(ide);

    // put it in the top left in case we are on a small screen
    // should fit fine in 640x480
    ide.setLocation(10, 15);
    ide.setSize(JUDO_IDE_WIDTH, JUDO_IDE_HEIGHT);

    ide.setVisible(true);
    ide.codeTextArea.requestFocus();
    ide.codeTextArea.setCaretPosition(0);
  }
  // end - JUDO INITIALIZATION
  ///////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////
  // THE FOLLOWING METHODS DEAL WITH COMPILING THE USERS SOURCE
  // AND RUNNING IT
  /**
   * Runs the compiled source code of a JUDOApp
   */
  void runCode() {

    // they just hit run, dont let them do it again till running is done.
    enableRun(false);

    // only (re)compile if necessary
    if (isError()) {
      compileCode(JUDOIDE.JUDO_APP);
    }

    // only run java if there were no errors (output in output text area)
    if (! isError()) {
      Command java = null;

      if (isWindows) {
        java = new Command(javaString +
                           " -classpath \"" + judoProgramDir + ";" + classpath + "\" " +
                           "JUDOApp",
                           this, false);
      }
      else {
        java = new Command(javaString +
                           " -classpath " + judoProgramDir + ":" + classpath + " " +
                           "JUDOApp",
                            this, false);
      }
      Thread javaThread = new Thread(java);
      javaThread.start();
    }

    enableRun(true);
  }

  /**
   * Compiles the code of the JUDOApp
   */
  void compileCode(int appOrApplet) {

    // let user know we are compiling
    displayOutput("");

    String filename = codeBase + pathSeparator + packagePath + pathSeparator;
    if (appOrApplet == JUDOIDE.JUDO_APP) {
      filename += judoAppTemplateFilename;
    }
    else {
      filename += judoAppletTemplateFilename;
    }

    String programCode = getFileText(filename, true);
    String bodyCode = codeTextArea.getText();

    determineProgramType(bodyCode);

    String type = (String) headerMap.get(JUDO_TYPE);
    if (type == null) {
      type = JUDOBase.jud0_TYPE_TEXT;
    }
    setJUDOAppDimensions(type);
    setTemplateKey("appRows", "" + appRows);
    setTemplateKey("appHeight", "" + appHeight);
    setTemplateKey("appWidth", "" + appWidth);
    setTemplateKey("appType", type);
    setTemplateKey("headerLines", "" + this.getHeaderLines());
    if (! type.equals(JUDOBase.jud0_TYPE_TEXT)) {
      setTemplateKey("initGraphics", "true");
    }
    else {
      setTemplateKey("initGraphics", "false");
    }
    setTemplateKey("programName", programName);
    setTemplateKey("bodyCode", bodyCode);
    programCode = buildCodeFromTemplate(programCode);

//    filename = codeBase + pathSeparator + packagePath + pathSeparator;
    filename = judoProgramDir + pathSeparator;
    if (appOrApplet == JUDOIDE.JUDO_APP) {
      filename += judoAppJavaFilename;
    }
    else {
      filename += judoAppletJavaFilename;
    }

    putFileText(filename, programCode);

    // compile the code with javac
    Command javac = null;

    // if we are on windows, we need double quotes around filenames
    if (isWindows) {
      javac = new Command(javacString +
                                  " -classpath \"" + classpath +
				  "\" -encoding UTF-8 \"" + filename + "\"",
                                  this, true);
    }
    // not windows
    else {
      javac = new Command(javacString +
                                  " -classpath " + classpath +
				  " -encoding UTF-8 " + filename,
                                  this, true);
    }

    Thread javacThread = new Thread(javac);
    javacThread.start();

    try {
      javacThread.join();
    } catch (InterruptedException ie) {
      displayOutput(lz.IDE_COMPILE_ERR);
    }
  }

  void checkForCall(String code, String functionName, boolean graphicsCall) {
    if (code.indexOf(functionName) != -1) {
      if (graphicsCall) {
        hasGraphics = true;
      }
      else {
        hasText = true;
      }
    }
  }

  /**
   * Gets the text of a file (NOTE: max length of file is hardcoded)
   * @param filename the absolute path to the file to read in
   * @param isHeader whether the file is the code header or not
   * @return a string with the contents of the file, or null on error
   */
  String getFileText(String filename, boolean isHeader) {
    BufferedReader codeReader = null;

    // open the file
    try {
      codeReader = new BufferedReader(new FileReader(filename));
    }
    catch (FileNotFoundException fnfe) {
      displayOutput(ju.getString(lz.IDE_404_ERR, filename));
    }

    // get the code
    try {
      String line = "";
      String code = "";

      if (isHeader)
        headerLines = 0;

      while ((line = codeReader.readLine()) != null) {
        if (line.indexOf(JUDO_MAIN_STRING) != -1) {
          isHeader = false;
          headerLines++;
        }
        code += "\n" + line;
        if (isHeader)
          headerLines++;
      }

      if (isHeader)
        setHeaderLines(headerLines);

      codeReader.close();
      return code;
    } catch (IOException ioe) {
      displayOutput(ju.getString(lz.IDE_READ_FILE_ERR, filename));
    }
    return null;
  }

  /**
   * Writes the given text to a file
   * @param filename the absolute path to the file to write out
   * @param text the text to write to the file
   * @return true on success, false on error
   */
  boolean putFileText(String filename, String text) {

    PrintWriter codeWriter = null;

    // open the file
    try {
      codeWriter = new PrintWriter(new BufferedWriter(new FileWriter(new File(filename))));
    }
    catch (IOException ioe) {
      displayOutput(ju.getString(lz.IDE_CREATE_FILE_ERR, filename));
    }

    // write out the code
    codeWriter.print(text);
    codeWriter.close();
    return true;
  }

  String buildCodeFromTemplate(String templateCode) {
    String key = "";
    String val = "";
    Set keys = templateMap.keySet();
    Iterator keyIterator = keys.iterator();
    while (keyIterator.hasNext()) {
      key = (String) keyIterator.next();
      val = (String) templateMap.get(key);
      templateCode = replaceTemplateKey(templateCode, key, val);
    }
    return templateCode;
  }

  void setTemplateKey(String key, String value) {
    templateMap.put(key, value);
  }

  String replaceTemplateKey(String templateCode, String key, String value) {
    key = templateKeyPrefix + key + templateKeySuffix;
    int index;
    while ((index = templateCode.indexOf(key)) != -1) {
      String newCode = templateCode.substring(0, index) + value;
      if (templateCode.length() - 1 >= index + key.length()) {
        newCode += templateCode.substring(index + key.length());
      }
      templateCode = newCode;
    }
    return templateCode;
  }
  // end - COMPILING USERS SOURCE AND RUNNING METHODS
  ///////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////////
  // THE FOLLOWING METHODS MOSTLY DEAL WITH UPDATING THE GUI
  /**
   * Called when error occur
   *
   * @param errorMsg the message of this error to display
   */
  void alert(String errorMsg) {
    codeTextArea.setText(errorMsg);
  }

  void displayOutput(String output) {
    outputTextArea.setEditable(true);
    outputTextArea.setText(output);
    outputTextArea.setEditable(false);
  }

  private boolean isError() {
    return error;
  }

  public void setError(boolean isError) {
    error = isError;
  }

  private void setHeaderLines(int lines) {
    headerLines = lines;
  }

  public int getHeaderLines() {
    return headerLines;
  }

  public void setLineNumber(int num) {
    int lineNum = 1;
    int lastNewline = 0;
    char text[] = codeTextArea.getText().toCharArray();

    if (num < 1)
      return;

    if (num == 1) {
      codeTextArea.setCaretPosition(0);
      return;
    }

    int i;
    for (i = 0; i < text.length; i++) {
      if (text[i] == '\n') {
        lastNewline = i;
        lineNum++;
      }
      if (num == lineNum) {
        codeTextArea.requestFocus();
        codeTextArea.setCaretPosition(lastNewline + 1);
        return;
      }
    }
  }

  private void enableRun(boolean enabled) {
    runButton.setEnabled(enabled);
  }

  void setJUDOAppDimensions(String appType) {
    if (appType.equals(JUDOBase.jud0_TYPE_GRAPHICS)) {
      appHeight = JUDOBase.jud0_APP_HEIGHT_GRAPHICS;
      appRows = JUDOBase.jud0_APP_GRAPHICS_ROWS;
    }
    else if (appType.equals(JUDOBase.jud0_TYPE_BOTH)) {
      appHeight = JUDOBase.jud0_APP_HEIGHT_BOTH;
      appRows = JUDOBase.jud0_APP_BOTH_ROWS;
    }
    else {
      appHeight = JUDOBase.jud0_APP_HEIGHT_TEXT;
      appRows = JUDOBase.jud0_APP_TEXT_ROWS;
    }
  }

  // for implementing FilenameFilter
  public boolean accept(File dir, String name) {
    return name.endsWith("." + extension);
  }

  void exitJUDO(AWTEvent e) {
    if (isDirty) {
      int save;
      save = JOptionPane.showConfirmDialog(this, lz.IDE_EXIT_JUDO_NO_SAVE_MSG,
                                           lz.IDE_EXIT_NO_SAVE_TIT, JOptionPane.YES_NO_OPTION);
      if (save == JOptionPane.YES_OPTION) {
        setVisible(false);
        dispose();
        System.exit(0);
      }
    }
    else {
      setVisible(false);
      dispose();
      System.exit(0);
    }
  }

  public void windowClosing(WindowEvent we) {
    exitJUDO(we);
  }
  public void windowActivated(WindowEvent e) {}
  public void windowClosed(WindowEvent e) {}
  public void windowDeactivated(WindowEvent e) {}
  public void windowDeiconified(WindowEvent e) {}
  public void windowIconified(WindowEvent e) {}
  public void windowOpened(WindowEvent e) {}

  public void changedUpdate(DocumentEvent e) {
    setDirty();
    setError(true);
  }
  public void insertUpdate(DocumentEvent e) {
    setDirty();
    setError(true);
  }
  public void removeUpdate(DocumentEvent e) {
    setDirty();
    setError(true);
  }
  void setDirty() {
    isDirty = true;
    updateTitle();
  }
  void updateTitle() {
    setTitle(titlePrefix + programName + "*");
  }
  // end - UPDATING THE GUI methods
  ///////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////
  // THE FOLLOWING METHODS DEAL MOSTLY WITH JUDO SOURCE FILE OPERATIONS
  boolean save(boolean defaultLocation, boolean saveAs) {
    String filePath = "";

    // if it is the first save, or our last save was to a different save location
    // then make it a save as
    if (programName.equals(defaultProgramName) || defaultLocation != lastSaveDefault) {
      saveAs = true;
    }

    if (saveAs) {
      saveDialog.setTitle(lz.IDE_SAVE_NEW_NAME_TIT);
    }
    else {
      saveDialog.setTitle(lz.IDE_SAVE_TIT);
    }

    // only show save dialog if doing a save as
    if (saveAs) {
      saveDialog.setLocationRelativeTo(this);
      saveDialog.setVisible(true);
      // user hit cancel instead of save
      if (saveDialog.canceled()) {
        return false;
      }
      lastProgramName = programName;
      programName = saveDialog.getProgramName();
      programFilename = saveDialog.getProgramFilename();
    }

    if ((! defaultLocation) && isWindows) {
      filePath = floppyPath + pathSeparator + programDirectory + pathSeparator + programFilename + "." + extension;
    }
    else {
//      filePath = codeBase + pathSeparator + programDirectory + pathSeparator + programFilename + "." + extension;
      filePath = judoProgramDir + pathSeparator + programFilename + "." + extension;
    }

    // if we are doing a save as and the file exists, we need to ask what to
    // do, otherwise, just save the file.
    if (saveAs && fileExists(filePath)) {
      // display exists option pane
      int decision;
      decision = JOptionPane.showConfirmDialog(this, ju.getString(lz.IDE_PROG_EXISTS_MSG, programName),
                                               lz.IDE_PROG_EXISTS_TIT,
                                               JOptionPane.YES_NO_OPTION,
                                               JOptionPane.QUESTION_MESSAGE);
      // if they say yes, then save it, if not, we're done, return
      if (decision == JOptionPane.YES_OPTION) {
        return writeJUDOFile(defaultLocation);
      }
      else {
        programName = lastProgramName;
        return false;
      }
    }
    else {
      return writeJUDOFile(defaultLocation);
    }
  }

  /**
   * sets up output directory (exits? writable?)
   * calls to write the file, or displays error messages where necessary
   * @param defaultLocation if we are saving in the main programs directory
   * @return true if wrote out the JUDO program successfully, false otherwise.
   */

  boolean writeJUDOFile(boolean defaultLocation) {
    File floppyProgramDirectory = new File(floppyPath + pathSeparator + programDirectory);
//    File defaultProgramDirectory = new File(codeBase + pathSeparator + programDirectory);
    File defaultProgramDirectory = new File(judoProgramDir);

    // if we want to save to floppy
    if ((! defaultLocation) && isWindows) {
      if (! checkForFloppy()) {
        // display disk is not in drive message, put floppy in drive and save
        JOptionPane.showMessageDialog(this, lz.IDE_NO_FLOPPY_MSG,
                                      lz.IDE_FLOPPY_ERR_TIT,
                                      JOptionPane.ERROR_MESSAGE);
        return false;
      }
      // floppy is in drive
      else {
        if (! floppyProgramDirectory.exists()) {
          // judo program dir doesnt exist, try and create it
          if (! floppyProgramDirectory.mkdir()) {
            JOptionPane.showMessageDialog(this, lz.IDE_FLOPPY_WRITE_PROT_MSG,
                                          lz.IDE_FLOPPY_ERR_TIT,
                                          JOptionPane.ERROR_MESSAGE);
            return false;
          }
        }
        if (! floppyProgramDirectory.canWrite()) {
          JOptionPane.showMessageDialog(this, lz.IDE_FLOPPY_WRITE_PROT_MSG,
                                        lz.IDE_FLOPPY_ERR_TIT,
                                        JOptionPane.ERROR_MESSAGE);
          return false;
        }
        return writeProgramToDisk(floppyPath + pathSeparator + programDirectory + pathSeparator + programFilename + "." + extension);
      }
    }
    else {
      if (! defaultProgramDirectory.exists()) {
        // judo program dir doesnt exist, try and create it
        if (! defaultProgramDirectory.mkdir()) {
          JOptionPane.showMessageDialog(this, lz.IDE_NO_PROG_DIR_MSG,
                                        lz.IDE_ERR_SAVING_TIT,
                                        JOptionPane.ERROR_MESSAGE);
          return false;
        }
        if (! defaultProgramDirectory.canWrite()) {
          JOptionPane.showMessageDialog(this, lz.IDE_PROG_DIR_NOT_WRIT,
                                        lz.IDE_ERR_SAVING_TIT,
                                        JOptionPane.ERROR_MESSAGE);
          return false;
        }
      }
//      return writeProgramToDisk(codeBase + pathSeparator + programDirectory + pathSeparator + programFilename + "." + extension);
        return writeProgramToDisk(judoProgramDir + pathSeparator + programFilename + "." + extension);
    }
  }

  boolean writeProgramToDisk(String filePath) {
    headerMap.put(JUDOAPP_TITLE, programName);
    headerMap.put(JUDO_TYPE, appType);

    // if filePath ends in .judo.judo, remove one
    // this happens when saving a file that you opened
    if (filePath.indexOf("." + extension + "." + extension) != -1) {
      filePath = filePath.substring(0, filePath.length() - extension.length());
    }

    String headers = generateHeaderComments();
    String code = codeTextArea.getText();
    return putFileText(filePath, headers + code);
  }

  private boolean fileExists(String filePath) {
    File codeFile = new File(filePath);
    return codeFile.exists();
  }

  // if programOrSample == true, it is from the program dir
  // if programOrSample == false, it is from the samples dir
  HashMap getProgramNames(boolean programOrSample, boolean defaultLocation) {
    String basePath = "";
    Vector programVector = new Vector();
    HashMap programs = new HashMap();

    if (programOrSample) {
      // get list of programs off of floppy
      if ((! defaultLocation) && isWindows) {
        basePath = floppyPath + pathSeparator + programDirectory;
      }
      // get list of programs off hard drive
      else {
        basePath = judoProgramDir;
      }
    }
    else {
      basePath = helpSamplesDir;
    }

    File programsDir = new File(basePath);
    File[] programFiles = programsDir.listFiles(this);
    if (programFiles == null) {
      return null;
    }
    String line = "";
    for (int i = 0; i < programFiles.length; i++) {
      programs.put(getProgramName(programFiles[i]), programFiles[i].getName());
    }
    return programs;
  }

  String[] getProgramNamesStringArray(HashMap programsMap) {
    String[] names;
    int count = 0;
    names = new String[programsMap.size()];
    String key = "";
    Set keys = programsMap.keySet();
    Iterator keyIterator = keys.iterator();
    while (keyIterator.hasNext()) {
      key = (String) keyIterator.next();
      names[count] = key;
      count++;
    }
    return names;
  }

  String getProgramName(File f) {
    BufferedReader programReader = null;
    String name = "";

    // open the file
    try {
      programReader = new BufferedReader(new FileReader(f));
    }
    catch (FileNotFoundException fnfe) {
      displayOutput(ju.getString(lz.IDE_404_ERR, f.getName()));
    }

    // get the code
    try {
      String line = "";
      int index = -1;
      String searchString = JUDO_KEY_PREFIX + JUDOAPP_TITLE + "=";
      while ((line = programReader.readLine()) != null) {
        if ((index = line.indexOf(searchString)) != -1) {
          name = line.substring(index + searchString.length());
          break;
        }
      }

      programReader.close();
      return name;
    } catch (IOException ioe) {
      displayOutput(ju.getString(lz.IDE_READ_FILE_ERR, f.getName()));
    }
    return null;
  }

  /**
   * Reads in a .judo file and returns the code
   * Also initializes the headerMap based on the JUDOPROP comments
   * @param filePath the path to the file to read in
   * @return the code of the .judo file
   */
  String getProgramText(String filePath) {
    BufferedReader programReader = null;
    String programText = "";
    int index = -1;

    // open the file
    try {
      programReader = new BufferedReader(new FileReader(filePath));
    }
    catch (FileNotFoundException fnfe) {
      displayOutput(ju.getString(lz.IDE_404_ERR, filePath));
    }

    // get the code
    try {
      String line = "";
      String key = "";
      String val = "";
      String searchString = JUDO_PROP_COMMENT;

      while ((line = programReader.readLine()) != null) {
        if ((index = line.indexOf(searchString)) != -1) {
          // read the comment and store in headerMap
          key = line.substring(line.indexOf(JUDO_KEY_PREFIX, index) + 1, line.indexOf("=", index));
          val = line.substring(line.indexOf("=", index) + 1);
          headerMap.put(key, val);
          continue;
        }
        programText += line + "\n";
      }

      programReader.close();
      return programText;
    } catch (IOException ioe) {
      displayOutput(ju.getString(lz.IDE_READ_FILE_ERR, filePath));
    }
    return null;
  }

  boolean openProgram(boolean defaultLocation) {
    String filePath = "";
    String programText = "";

    // if there are changes, check that they dont want to save before
    // opening a new one
    if (isDirty) {
      int save;
      save = JOptionPane.showConfirmDialog(this, lz.IDE_OPEN_NO_SAVE_MSG,
                                           lz.IDE_OPEN_NO_SAVE_TIT, JOptionPane.YES_NO_OPTION);
      if (save == JOptionPane.NO_OPTION) {
        return false;
      }
    }

    // if we are trying to open off of floppy, check that one is in the drive
    // if it is not, display message
    if ((! defaultLocation) && (! checkForFloppy())) {
      JOptionPane.showMessageDialog(this, lz.IDE_OPEN_NO_FLOPPY_MSG,
                                    lz.IDE_OPEN_ERR_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
    }

    HashMap programsMap = getProgramNames(true, defaultLocation);
    if (programsMap == null || programsMap.size() == 0) {
      JOptionPane.showMessageDialog(this, lz.IDE_NO_PROG_MSG,
                                    lz.IDE_NO_PROG_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
    }

    openDialog = new JUDOOpenDialog(this, this, codeBase + pathSeparator + "images" + pathSeparator + openIconFilename, lz.IDE_OPEN_TIT, getProgramNamesStringArray(programsMap));
    openDialog.pack();
    openDialog.setLocationRelativeTo(this);
    openDialog.setVisible(true);

    if (openDialog.canceled()) {
      return false;
    }
    //lastProgramName = programName;

    // set error because we are opening new program, and if user hits run
    // we want it to recompile
    setError(true);

    programName = openDialog.getProgramName();
    programFilename = (String) programsMap.get(programName);

    // populate text area

    if ((! defaultLocation) && isWindows) {
      filePath = floppyPath + pathSeparator + programDirectory + pathSeparator + programFilename;
    }
    else {
//      filePath = codeBase + pathSeparator + programDirectory + pathSeparator + programFilename;
      filePath = judoProgramDir + pathSeparator + programFilename;
    }
    programText = getProgramText(filePath);
    if (programText == null || programText.equals("")) {
      JOptionPane.showMessageDialog(this, ju.getString(lz.IDE_OPEN_PROG_MSG, programName),
                                    lz.IDE_OPEN_PROG_ERR_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
    }
    outputTextArea.setText("");
    codeTextArea.setText(programText);
    isDirty = false;
    setTitle(titlePrefix + programName);
    appType = (String) headerMap.get(JUDO_TYPE);
    codeTextArea.requestFocus();
    codeTextArea.setCaretPosition(0);
    return true;
  }

  void openSampleProgram(String filename) {
    if (isDirty) {
      if (JOptionPane.NO_OPTION == JOptionPane.showConfirmDialog(this,
                                              lz.IDE_OPEN_SAMPLE_CONFIRM_MSG,
                                              lz.IDE_OPEN_SAMPLE_CONFIRM_TIT,
                                              JOptionPane.YES_NO_OPTION,
                                              JOptionPane.QUESTION_MESSAGE)) {
        return;
      }
    }

    String filePath = helpSamplesDir + pathSeparator + filename;
    if ((new File(helpSamplesDir + "-" + judoLanguage +
      pathSeparator + filename)).exists())
        filePath = helpSamplesDir + "-" + judoLanguage +
          pathSeparator + filename;
    String programText = getProgramText(filePath);
    programName = (String) headerMap.get(JUDOAPP_TITLE);
    HashMap programsMap = getProgramNames(false, true);
    programFilename = (String) programsMap.get(programName);
    if (programText == null || programText.equals("")) {
      JOptionPane.showMessageDialog(this, ju.getString(lz.IDE_OPEN_PROG_MSG, programName),
                                    lz.IDE_OPEN_PROG_ERR_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return;
    }
    outputTextArea.setText("");
    codeTextArea.setText(programText);
    setError(true);
    setTitle(titlePrefix + programName);
    setDirty();
    appType = (String) headerMap.get(JUDO_TYPE);
    codeTextArea.requestFocus();
    codeTextArea.setCaretPosition(0);
  }

  boolean deleteProgram(boolean defaultLocation) {
    String filePath = "";
    String programText = "";

    // if we are trying to open off of floppy, check that one is in the drive
    // if it is not, display message
    if ((! defaultLocation) && (! checkForFloppy())) {
      JOptionPane.showMessageDialog(this, lz.IDE_DEL_NO_FLOPPY_MSG,
                                    lz.IDE_DEL_ERR_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
    }

    HashMap programsMap = getProgramNames(true, defaultLocation);
    if (programsMap == null || programsMap.size() == 0) {
      JOptionPane.showMessageDialog(this, lz.IDE_NO_PROG_DEL_MSG,
                                    lz.IDE_NO_PROG_DEL_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
    }

    deleteDialog = new JUDODeleteDialog(this, this, codeBase + pathSeparator + "images" + pathSeparator + deleteIconFilename, lz.IDE_DEL_TIT, getProgramNamesStringArray(programsMap));
    deleteDialog.pack();
    deleteDialog.setLocationRelativeTo(this);
    deleteDialog.setVisible(true);

    if (deleteDialog.canceled()) {
      return false;
    }

    programName = deleteDialog.getProgramName();
    programFilename = (String) programsMap.get(programName);

    if ((! defaultLocation) && isWindows) {
      filePath = floppyPath + pathSeparator + programDirectory + pathSeparator + programFilename;
    }
    else {
//      filePath = codeBase + pathSeparator + programDirectory + pathSeparator + programFilename;
      filePath = judoProgramDir + pathSeparator + programFilename;
    }

    // delete the file
    File fileToDelete = new File(filePath);
    if (! fileToDelete.delete()) {
      JOptionPane.showMessageDialog(this, ju.getString(lz.IDE_DELETE_PROG_MSG, programName),
                                    lz.IDE_DELETE_PROG_ERR_TIT,
                                    JOptionPane.ERROR_MESSAGE);
      return false;
    }

    codeTextArea.requestFocus();
    return true;
  }

  boolean newProgram() {
    // if there are changes, check that they dont want to save before
    // opening a new one
    if (isDirty) {
      int save;
      save = JOptionPane.showConfirmDialog(this, lz.IDE_NEW_WITHOUT_SAVE_MSG,
                                           lz.IDE_NEW_WITHOUT_SAVE_TIT, JOptionPane.YES_NO_OPTION);
      if (save == JOptionPane.NO_OPTION) {
        return false;
      }
    }

    headerMap.put(JUDO_TYPE, JUDOBase.jud0_TYPE_BOTH);

    // set error because we are opening new program, and if user hits run
    // we want it to recompile
    setError(true);

    programName = defaultProgramName;
    outputTextArea.setText("");
    codeTextArea.setText("");
    codeTextArea.requestFocus();
    codeTextArea.setCaretPosition(0);
    isDirty = false;
    setTitle(titlePrefix + programName);
    return true;
  }

  String getFilenameFromProgramName(String programNameText) {
    String filenameText = "";
    for (int i = 0; i < programNameText.length(); i++) {
      if (programNameText.charAt(i) != ' ') {
        filenameText += programNameText.charAt(i);
      }
    }
    filenameText = filenameText.toLowerCase();
    return filenameText;
  }

  private String generateHeaderComments() {
    String key = "";
    String val = "";
    String headerText = "";
    Set keys = headerMap.keySet();
    Iterator keyIterator = keys.iterator();
    while (keyIterator.hasNext()) {
      key = (String) keyIterator.next();
      val = (String) headerMap.get(key);
      headerText += JUDO_PROP_COMMENT + JUDO_KEY_PREFIX + key + "=" + val + newline;
    }

    return headerText;
  }

  boolean checkForFloppy() {
    File floppyDrive = new File(floppyPath);
    return (floppyDrive.exists() && floppyDrive.canWrite());
  }
  // end JUDO SOURCE FILE OPERATIONS METHODS
  /////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////
  //  ACTIONPERFORMED
  /**
   * Act on all of JUDO's GUI Events
   * This is a huge method because there are many events that can happen
   * from many different sources.  Mostly menus
   */
  public void actionPerformed(ActionEvent ae) {

    if (ae.getSource() == runButton) {
      runCode();
    }
    else if (ae.getSource() == exitMenuItem) {
      exitJUDO(ae);
    }
    else if (ae.getSource() == saveMenuItem) {
      if (save(true, false)) {
        setTitle(titlePrefix + programName);
        isDirty = false;
        lastSaveDefault = true;
      }
    }
    else if (ae.getSource() == saveAsMenuItem) {
      if (save(true, true)) {
        setTitle(titlePrefix + programName);
        isDirty = false;
        lastSaveDefault = true;
      }
    }
    else if (ae.getSource() == saveToFloppyMenuItem) {
      if (save(false, false)) {
        setTitle(titlePrefix + programName);
        isDirty = false;
        lastSaveDefault = false;
      }
    }
    else if (ae.getSource() == saveAsToFloppyMenuItem) {
      if (save(false, true)) {
        setTitle(titlePrefix + programName);
        isDirty = false;
        lastSaveDefault = false;
      }
    }
    else if (ae.getSource() == openMenuItem) {
      openProgram(true);
    }
    else if (ae.getSource() == openFromFloppyMenuItem) {
      openProgram(false);
    }
    else if (ae.getSource() == openSampleMenuItem) {
      openHelpItem(lz.IDE_FILE_OPEN_SAMPLE, "SamplePrograms.html", true);
    }
    else if (ae.getSource() == deleteMenuItem) {
      deleteProgram(true);
    }
    else if (ae.getSource() == deleteFromFloppyMenuItem) {
      deleteProgram(false);
    }
    else if (ae.getSource() == newMenuItem) {
      newProgram();
    }
    else if (ae.getSource() == appletWizardMenuItem) {
      compileCode(JUDOIDE.JUDO_APPLET);
    }
    else if (ae.getSource() == gotoLineMenuItem) {
      String lineNumberString = JOptionPane.showInputDialog(this, lz.IDE_GOTO_LINE_NUM_MSG,
                                                      lz.IDE_GOTO_LINE_NUM_TIT,
                                                      JOptionPane.QUESTION_MESSAGE);
      if (lineNumberString == null) {
        return;
      }

      try {
        int lineNumber = (new Integer(lineNumberString)).intValue();
        int lineCount = codeTextArea.getLineCount();
        if (lineNumber < 1) {
          JOptionPane.showMessageDialog(this, lz.IDE_LINE_NUM_GREATER_MSG,
                                              lz.IDE_INV_LINE_NUM_TIT,
                                              JOptionPane.INFORMATION_MESSAGE);
          return;
        }
        else if (lineNumber > lineCount) {
          JOptionPane.showMessageDialog(this, ju.getString(lz.IDE_LINE_COUNT_MSG, ""+lineCount),
                                              lz.IDE_INV_LINE_NUM_TIT,
                                              JOptionPane.INFORMATION_MESSAGE);
          return;
        }
        setLineNumber(lineNumber);
      } catch (NumberFormatException nfe) {
        if (lineNumberString.equals("")) {
          JOptionPane.showMessageDialog(this, lz.IDE_NO_LINE_NUM_MSG,
                                              lz.IDE_INV_LINE_NUM_TIT,
                                              JOptionPane.ERROR_MESSAGE);
        }
        else {
          JOptionPane.showMessageDialog(this, ju.getString(lz.IDE_LINE_NUM_INV_MSG, lineNumberString),
                                              lz.IDE_INV_LINE_NUM_TIT,
                                              JOptionPane.ERROR_MESSAGE);
        }
      }
    }
    else if (ae.getSource() == undoMenuItem) {
      try {
        undoManager.undo();
      }
      catch (CannotUndoException ex) {
      }
    }
    else if (ae.getSource() == redoMenuItem) {
      try {
        undoManager.redo (); }
      catch (CannotRedoException ex) {
      }
    }
    else if (ae.getSource() == selectAllMenuItem) {
      codeTextArea.requestFocus();
      codeTextArea.selectAll();
    }
    else if (ae.getSource() == copyMenuItem) {
      String selectedText = codeTextArea.getSelectedText();
      if (selectedText == null || selectedText.equals("")) {
        return;
      }
      StringSelection contents = new StringSelection(selectedText);
      clipboard.setContents(contents, null);
    }
    else if (ae.getSource() == cutMenuItem) {
      String selectedText = codeTextArea.getSelectedText();
      if (selectedText == null || selectedText.equals("")) {
        return;
      }
      StringSelection contents = new StringSelection(selectedText);
      clipboard.setContents(contents, null);
      codeTextArea.replaceRange("", codeTextArea.getSelectionStart(), codeTextArea.getSelectionEnd());
    }
    else if (ae.getSource() == pasteMenuItem) {
      Transferable contents = clipboard.getContents(this);
      String val = "";
      try {
        val = (String) contents.getTransferData(DataFlavor.stringFlavor);
      }
      catch(Exception e) {
        System.out.println("Caught Exception in JUDOIDE.actionPerformed() in pasteMenu section.");
        System.out.println("Error reading clipboard?");
        System.out.println(e);
        e.printStackTrace();
      }

      // delete whatever was selected and insert in its place
      codeTextArea.replaceRange("", codeTextArea.getSelectionStart(), codeTextArea.getSelectionEnd());
      codeTextArea.insert(val, codeTextArea.getCaretPosition());
    }

    ///////////////////////////////////////////////
    // THE HELP MENUS
    ///////////////////////////////////////////////
    else if (ae.getSource() == judoHelpMenuItem) {
      openHelpItem(lz.IDE_HELP_JUDO_HELP, "index.html", true);
    }
    else if (ae.getSource() == tutorialsMenuItem) {
      openHelpItem(lz.IDE_HELP_JUDO_HELP, "index.html", true);
    }
    else if (ae.getSource() == functionReferenceMenuItem) {
      openHelpItem(lz.IDE_HELP_FUNC_REF, "JUDOAppAPI.html", false);
    }
    else if (ae.getSource() == colorReferenceMenuItem) {
      openHelpItem(lz.IDE_HELP_COLOR_REF, "JUDOColorValues.html", false);
    }
    else if (ae.getSource() == aboutMenuItem) {
      JOptionPane.showMessageDialog(this, ju.getString(lz.IDE_JUDO_ABOUT_MSG, JUDO_VERSION),
                                    " " + lz.IDE_HELP_ABOUT_JUDO,
                                    JOptionPane.INFORMATION_MESSAGE);
    }
    else if (ae.getSource() == judoWebsiteMenuItem) {
      JUDOHelpWindow helpWindow = new JUDOHelpWindow(this, " " + lz.IDE_HELP_JUDO_WEBSITE,
                                      HELP_WINDOW_WIDTH,
                                      HELP_WINDOW_HEIGHT,
                                      true);
      helpWindow.setLocation(40, 60);

      try {
        helpWindow.displayURL(new URL("http://judo.sourceforge.net"));
      }
      catch (Exception e) {}
    }
  }
  //  ACTIONPERFORMED - end
  /////////////////////////////////////////////////////////////////////////

  // open a help file
  private void openHelpItem(String title, String helpFile, boolean allowLinks) {
    JUDOHelpWindow helpWindow = new JUDOHelpWindow(this, " " + title,
                                    HELP_WINDOW_WIDTH,
                                    HELP_WINDOW_HEIGHT,
                                    allowLinks);
    helpWindow.setLocation(40, 60);

    if ((new File(codeBase + "/docs-" + judoLanguage + "/" + helpFile)).exists())
      helpFile = codeBase + "/docs-" + judoLanguage + "/" + helpFile;
    else
      helpFile = codeBase + "/docs/" + helpFile;

    if (isWindows) {
      helpWindow.displayURL("file:/" + helpFile);
    }
    else {
      helpWindow.displayURL("file://" + helpFile);
    }
  }

  /**
   * Determines the program type by reading the program code and seeing
   * what functions are called.
   */
  void determineProgramType(String code) {
    hasText = false;
    hasGraphics = false;
    appType = JUDOBase.jud0_TYPE_BOTH;

    checkForCall(code, "printLine", false);
    checkForCall(code, "print", false);
    checkForCall(code, "readString", false);
    checkForCall(code, "readInt", false);
    checkForCall(code, "readDouble", false);
    checkForCall(code, "readBoolean", false);
    checkForCall(code, "readColor", false);

    checkForCall(code, "drawLine", true);
    checkForCall(code, "drawRectangle", true);
    checkForCall(code, "fillRectangle", true);
    checkForCall(code, "clearRectange", true);
    checkForCall(code, "clearDrawing", true);
    checkForCall(code, "drawOval", true);
    checkForCall(code, "fillOval", true);
    checkForCall(code, "drawCircle", true);
    checkForCall(code, "fillCircle", true);
    checkForCall(code, "drawString", true);
    checkForCall(code, "setBackgroundColor", true);
    checkForCall(code, "setColor", true);
    checkForCall(code, "getDrawingWidth", true);
    checkForCall(code, "getDrawingHeight", true);
    checkForCall(code, "drawPolygon", true);
    checkForCall(code, "fillPolygon", true);
    checkForCall(code, "getMouseEvent", true);
    checkForCall(code, "getMouseX", true);
    checkForCall(code, "getMouseY", true);
    checkForCall(code, "getDragStartX", true);
    checkForCall(code, "getDragStartY", true);
    checkForCall(code, "getDragEndX", true);
    checkForCall(code, "getDragEndY", true);
    checkForCall(code, "getMouseButton", true);

    if (hasText && ! hasGraphics) {
      appType = JUDOBase.jud0_TYPE_TEXT;
    }
    else if (! hasText && hasGraphics) {
      appType = JUDOBase.jud0_TYPE_GRAPHICS;
    }

    headerMap.put(JUDO_TYPE, appType);
  }
}
