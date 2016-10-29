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
import java.util.*;

/**
 * For parsing compiler errors and reporting them back to the ide
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class CompilerError {

  Vector errorVector;
  JUDOIDE judoIDE;

  Properties errorMap;

  // which element in the errorVector we are currently at
  int errorLineCount = 0;

  String pathSeparator = System.getProperty("file.separator");
  String newline = System.getProperty("line.separator");

  // this string indicates we are at the first line of an error
//  String errorStartLine = "org" + pathSeparator + "judo" + pathSeparator + "JUDOApp.java";
  String errorStartLine = "JUDOApp.java";

  // number of lines above user inputted code in JUDOApp.java
  int headerLines = 0;

  public CompilerError(Vector errors, JUDOIDE ide) {
    judoIDE = ide;
    headerLines = judoIDE.getHeaderLines();
    errorVector = errors;
    initErrorMap();
  }

  private void initErrorMap() {
    errorMap = new Properties();
    errorMap.setProperty("unclosed string literal", judoIDE.lz.CER_STRING_MISSING_QUOTE);
    errorMap.setProperty("empty character literal", judoIDE.ju.getString(judoIDE.lz.CER_EMPTY_CHAR, newline));
    errorMap.setProperty("unclosed character literal", judoIDE.ju.getString(judoIDE.lz.CER_UNCLOSED_CHAR, newline));
    errorMap.setProperty("not a statement", judoIDE.lz.CER_INVALID_STATEMENT);
    errorMap.setProperty("'class' or 'interface' expected", judoIDE.lz.CER_CURLY_BRACE_NO_MATCH);
    errorMap.setProperty("illegal start of expression", judoIDE.lz.CER_CURLY_BRACE_NO_MATCH);
  }

  private String symbolError(String errorLine) {
    String symbolErrorString = "";
    int typeLeftIndex = -1;
    int typeRightIndex = -1;
    int symbolLeftIndex = -1;
    int symbolRightIndex = -1;
    String symbolType = "";
    String symbolName = "";

    // goto next line
    errorLineCount++;
    errorLine = (String) errorVector.elementAt(errorLineCount);

    typeLeftIndex = errorLine.indexOf(":") + 2;
    typeRightIndex = errorLine.indexOf(" ", typeLeftIndex);
    symbolType = errorLine.substring(typeLeftIndex, typeRightIndex);
    symbolLeftIndex = typeRightIndex + 1;
    symbolName = errorLine.substring(symbolLeftIndex);
    if ((symbolRightIndex = symbolName.indexOf(" ")) != -1) {
      symbolName = symbolName.substring(0, symbolRightIndex);
    }

    if (symbolType.equals("variable")) {
      symbolErrorString = judoIDE.lz.CER_UNDEF_VAR + symbolName;
    }
    else if (symbolType.equals("method")) {
      symbolErrorString = judoIDE.lz.CER_UNDEF_FUNC + symbolName;
    }
    else {
      symbolErrorString = judoIDE.lz.CER_UNDEF_SYMBOL + symbolName;
    }

    // discard location line
    errorLineCount += 2;

    // the line causing error
    symbolErrorString += newline + ((String) errorVector.elementAt(errorLineCount));
    errorLineCount++;
    // the carat line to show where error is
    symbolErrorString += newline + ((String) errorVector.elementAt(errorLineCount)) + newline;

    return symbolErrorString;
  }

  private String normalError(String errorLine) {
    String normalErrorString = "";
    errorLineCount++;
    // the line causing error
    normalErrorString += newline + ((String) errorVector.elementAt(errorLineCount));
    errorLineCount++;
    // the carat line to show where error is
    normalErrorString += newline + ((String) errorVector.elementAt(errorLineCount)) + newline;
    return normalErrorString;
  }

  public void parseErrors() {
    String errorMessage = "";
    String errorLine = "";
    String descriptiveMessage;

    while (errorLineCount < errorVector.size()) {
      errorLine = (String) errorVector.elementAt(errorLineCount);
      if (errorStart(errorLine)) {
        int lineNum = getErrorLineNumber(errorLine);
        String errorText = getErrorText(errorLine);
//        if (errorText.equals("main(java.lang.String[]) in org.judo.JUDOApp cannot be applied to ()")) {
        if (errorText.equals("main(java.lang.String[]) in JUDOApp cannot be applied to ()")) {
          errorMessage += judoIDE.lz.CER_NO_VOID_MAIN + newline;
        }
        else if (lineNum < 1 || lineNum > judoIDE.codeTextArea.getLineCount()) {
          errorMessage += judoIDE.lz.CER_CURLY_BRACE_NO_MATCH + newline;
        }
        else if (errorText.equals("cannot resolve symbol")) {
          errorMessage += judoIDE.lz.CER_ERR_ON_LINE + lineNum + ": " + symbolError(errorLine);
        }
        else {
          descriptiveMessage = errorMap.getProperty(errorText);
          if (descriptiveMessage != null) {
            errorMessage += judoIDE.lz.CER_ERR_ON_LINE + lineNum + ": " + descriptiveMessage +
                            normalError(errorLine);
          }
          else {
            errorMessage += judoIDE.lz.CER_ERR_ON_LINE + lineNum + ": " + errorText +
                            normalError(errorLine);
          }
        }
        errorLineCount++;
        nextError();
      }
      else {
        errorLineCount++;
      }
    }

    if (errorVector.size() == 0) {
      judoIDE.displayOutput(judoIDE.lz.CER_COMP_SUCCESS);
      judoIDE.setError(false);
    }
    else {
      // add the number of errors at the end
      errorMessage += (String) errorVector.elementAt(errorVector.size() - 1);
      judoIDE.displayOutput(errorMessage);
      judoIDE.setError(true);
    }
  }

  // skips to the next error
  private void nextError() {
    String errorLine = "";
    while (errorLineCount < errorVector.size()) {
      errorLine = (String) errorVector.elementAt(errorLineCount);
      if (errorStart(errorLine)) {
        break;
      }
      else {
        errorLineCount++;
      }
    }
  }

  private boolean errorStart(String errorLine) {
    return (errorLine.indexOf(errorStartLine) != -1) ? true : false;
  }

  private String getErrorText(String errorLine) {
    int leftIndex = errorLine.indexOf(errorStartLine) + errorStartLine.length() + 1;
    return errorLine.substring(errorLine.indexOf(":", leftIndex) + 2);
  }

  // Assumes this line matches errorStartLine
  private int getErrorLineNumber(String errorLine) {
    String lineNumberString = "-999999";
    int lineNumber = -999999;

    int leftIndex = errorLine.indexOf(errorStartLine) + errorStartLine.length() + 1;
    int rightIndex = errorLine.indexOf(":", leftIndex) - 1;
    try {
      lineNumberString = errorLine.substring(leftIndex, rightIndex + 1);
    } catch (StringIndexOutOfBoundsException sioobe) {
      lineNumberString = "-999999";
    }
    try {
      lineNumber = (new Integer(lineNumberString)).intValue();
    } catch (NumberFormatException nfe) {
      lineNumber = -999999;
    }

    return lineNumber - headerLines - 1;
  }
}
