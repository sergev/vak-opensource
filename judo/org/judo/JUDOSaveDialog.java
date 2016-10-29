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

import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

/**
 * Dialog for saving JUDO programs and giving them a name
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class JUDOSaveDialog extends JDialog {
    private String typedText = "";
    String initialName = "";
    String filenameText = "";
    private int saveTextFieldLength = 10;
    final JTextField textField;

    private JOptionPane optionPane;

    private ImageIcon saveIcon;

    boolean canceled = false;

    public String getProgramName() {
        return typedText;
    }

    public String getProgramFilename() {
        return filenameText;
    }

    public void setProgramName(String progName) {
      typedText = progName;
      textField.setText(progName);
    }

    // used when there was a problem saving,
    // should not stay as the last Program Name inputted
    public void resetName() {
      setProgramName(initialName);
    }

    public void setVisible(boolean visible) {
      super.setVisible(visible);
      textField.selectAll();
      textField.requestFocus();
      initialName = getProgramName();
    }

    public boolean canceled() {
      return canceled;
    }

    public JUDOSaveDialog(Frame aFrame, JUDOIDE parent, String saveIconFilename, String title) {
        super(aFrame, true);
        final JUDOIDE judoIDE = parent;
        saveIcon = new ImageIcon(saveIconFilename);

        setTitle(title);

        final String msgString1 = judoIDE.lz.SAV_ENTER_PROG_NAME;
        textField = new JTextField(saveTextFieldLength);
        Object[] array = {msgString1, textField};

        final String btnString1;
        btnString1 = judoIDE.lz.SAV_SAVE;

        final String btnString2 = judoIDE.lz.SAV_CANCEL;
        Object[] options = {btnString1, btnString2};

        optionPane = new JOptionPane(array,
                                    JOptionPane.QUESTION_MESSAGE,
                                    JOptionPane.YES_NO_OPTION,
                                    saveIcon,
                                    options,
                                    options[0]);
        setContentPane(optionPane);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent we) {
                /*
                 * Instead of directly closing the window,
                 * we're going to change the JOptionPane's
                 * value property.
                 */
                    optionPane.setValue(new Integer(
                                        JOptionPane.CLOSED_OPTION));
            }
        });

        textField.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                optionPane.setValue(btnString1);
            }
        });

        optionPane.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                String prop = e.getPropertyName();

                if (isVisible()
                 && (e.getSource() == optionPane)
                 && (prop.equals(JOptionPane.VALUE_PROPERTY) ||
                     prop.equals(JOptionPane.INPUT_VALUE_PROPERTY))) {
                    Object value = optionPane.getValue();

                    if (value == JOptionPane.UNINITIALIZED_VALUE) {
                        //ignore reset
                        return;
                    }

                    // Reset the JOptionPane's value.
                    // If you don't do this, then if the user
                    // presses the same button next time, no
                    // property change event will be fired.
                    optionPane.setValue(
                            JOptionPane.UNINITIALIZED_VALUE);

                    // user hit cancel or exit
                    if (! value.equals(btnString1)) {
                      canceled = true;
                      typedText = "";
                      textField.setText(initialName);
                      filenameText = "";
                      setVisible(false);
                    }
                    // user hit save
                    else {
                        canceled = false;
                        typedText = textField.getText();
                        boolean isValidText = true;

                        // generate the filename
                        filenameText = "";
                        for (int i = 0; i < typedText.length(); i++) {
                          if (typedText.charAt(i) != ' ') {
                            filenameText += typedText.charAt(i);
                          }
                        }
                        filenameText = filenameText.toLowerCase();

                        // if they entered Untitled as the program name
                        if (judoIDE.defaultProgramName.toLowerCase().equals(filenameText)) {
                          textField.requestFocus();
                          textField.selectAll();
                          JOptionPane.showMessageDialog(
                            JUDOSaveDialog.this,
                            judoIDE.lz.SAV_CHOOSE_DIFF_NAME_MSG,
                            judoIDE.lz.SAV_CHOOSE_DIFF_NAME_TIT,
                            JOptionPane.ERROR_MESSAGE);
                          typedText = "";
                          return;
                        }

                        // make sure the name was entered, length > 0
                        if (filenameText.length() == 0) {
                          textField.requestFocus();
                          textField.selectAll();
                          JOptionPane.showMessageDialog(
                            JUDOSaveDialog.this,
                            judoIDE.lz.SAV_TYPE_PROG_NAME_MSG,
                            judoIDE.lz.SAV_TYPE_PROG_NAME_TIT,
                            JOptionPane.ERROR_MESSAGE);
                          typedText = "";
                          return;
                        }

                        // make sure only letters, numbers and spaces are in the program name
                        for (int i = 0; i < typedText.length(); i++) {
                          if (! ((typedText.charAt(i) >= 'A' && typedText.charAt(i) <= 'Z') ||  // uppercase letters
                                 (typedText.charAt(i) >= 'a' && typedText.charAt(i) <= 'z') || // lowercase letters
                                 (typedText.charAt(i) >= '0' && typedText.charAt(i) <= '9') ||  // numbers
                                 (typedText.charAt(i) == ' '))) {                            // space
                            // display wrong chars error
                            textField.requestFocus();
                            textField.selectAll();
                            JOptionPane.showMessageDialog(
                                            JUDOSaveDialog.this,
                                            judoIDE.lz.SAV_ONLY_NUM_LET_SPACE_MSG,
                                            judoIDE.lz.SAV_INV_PROG_NAME,
                                            JOptionPane.ERROR_MESSAGE);
                            typedText = "";
                            return;
                          }
                        }

                        // make sure first character of filename is not a number
                        if (filenameText.length() > 0 && filenameText.charAt(0) >= 48 && filenameText.charAt(0) <= 57) {
                          // display cant have number as first character
                          textField.requestFocus();
                          textField.selectAll();
                          JOptionPane.showMessageDialog(
                                          JUDOSaveDialog.this,
                                          judoIDE.lz.SAV_NO_SAV_PROG_WITH_NUM_MSG,
                                          judoIDE.lz.SAV_INV_PROG_NAME,
                                          JOptionPane.ERROR_MESSAGE);
                          typedText = "";
                          filenameText = "";
                          return;
                        }

                        // we're done; dismiss the dialog
                        setVisible(false);
                    }
                }
            }
        });
    }
}
