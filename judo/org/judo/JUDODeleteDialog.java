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
import java.util.*;

/**
 * Dialog for deleting JUDO programs
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class JUDODeleteDialog extends JDialog {

    final JList programsCombo;

    private JOptionPane optionPane;

    private ImageIcon deleteIcon;

    boolean canceled = false;

    public String getProgramName() {
        return (String) programsCombo.getSelectedValue();
    }

    public boolean canceled() {
      return canceled;
    }

    public JUDODeleteDialog(Frame aFrame, JUDOIDE parent, String deleteIconFilename, String title, String programs[]) {
        super(aFrame, true);
        final JUDOIDE judoIDE = parent;
        deleteIcon = new ImageIcon(deleteIconFilename);

        setTitle(title);
        ComparerIgnoreCase comparer = new ComparerIgnoreCase();
        Arrays.sort(programs, comparer);
        programsCombo = new JList(programs);
        programsCombo.setVisibleRowCount(6);
        programsCombo.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        JScrollPane scroll = new JScrollPane(programsCombo);

        final String msgString1 = judoIDE.lz.DEL_PROG_TO_DELETE;
        Object[] array = {msgString1, scroll};
        final String btnString1;
        btnString1 = judoIDE.lz.DEL_OK;

        final String btnString2 = judoIDE.lz.DEL_CANCEL;
        Object[] options = {btnString1, btnString2};

        optionPane = new JOptionPane(array,
                                    JOptionPane.QUESTION_MESSAGE,
                                    JOptionPane.OK_CANCEL_OPTION,
                                    deleteIcon,
                                    options,
                                    options[0]);
        setContentPane(optionPane);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);

        MouseListener mouseListener = new MouseAdapter() {
          public void mouseClicked(MouseEvent e) {
            if (e.getClickCount() == 2) {

              if (JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(judoIDE,
                                                                          judoIDE.ju.getString(judoIDE.lz.DEL_CONFIRM_MSG, getProgramName()),
                                                                          judoIDE.lz.DEL_CONFIRM_TIT,
                                                                          JOptionPane.YES_NO_OPTION,
                                                                          JOptionPane.QUESTION_MESSAGE)) {
                setVisible(false);
              }
            }
          }
        };

        programsCombo.addMouseListener(mouseListener);

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
                      setVisible(false);
                    }
                    // user hit OK
                    else {
                        // we're done; confirm and dismiss the dialog
                        canceled = false;
                        if (JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(judoIDE,
                                                                                    judoIDE.ju.getString(judoIDE.lz.DEL_CONFIRM_MSG, getProgramName()),
                                                                                    judoIDE.lz.DEL_CONFIRM_TIT,
                                                                                    JOptionPane.YES_NO_OPTION,
                                                                                    JOptionPane.QUESTION_MESSAGE)) {
                          setVisible(false);
                        }
                    }
                }
            }
        });
    }
}
