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
import javax.swing.BorderFactory;
import javax.swing.border.Border;
import javax.swing.border.TitledBorder;

/**
 * Dialog for setting JUDO program properties (text, graphics, or text/graphics)
 *
 * @author Thomas Dunn
 * @version 1.0
 */
public class JUDOProgramPropertiesDialog extends JDialog implements ActionListener {
    JRadioButton graphicsButton;
    JRadioButton textButton;
    JRadioButton bothButton;

    private JOptionPane optionPane;

    String typeString = JUDOBase.jud0_TYPE_TEXT;
    String initialSelection = typeString;

    boolean canceled = false;

    public String getWindowType() {
        return typeString;
    }

    public void reset() {
      textButton.setSelected(true);
      typeString = JUDOBase.jud0_TYPE_TEXT;
    }

    public void setWindowType(String type) {
      if (type.equals(JUDOBase.jud0_TYPE_BOTH)) {
        bothButton.setSelected(true);
        typeString = JUDOBase.jud0_TYPE_BOTH;
      }
      else if (type.equals(JUDOBase.jud0_TYPE_GRAPHICS)) {
        graphicsButton.setSelected(true);
        typeString = JUDOBase.jud0_TYPE_GRAPHICS;
      }
      else {
        textButton.setSelected(true);
        typeString = JUDOBase.jud0_TYPE_TEXT;
      }
    }

    public boolean canceled() {
      return canceled;
    }

    public void setVisible() {
      setVisible(true);
      initialSelection = getWindowType();
      if (bothButton.isSelected()) {
        bothButton.requestFocus();
      }
      else if (graphicsButton.isSelected()) {
        graphicsButton.requestFocus();
      }
      else {
        textButton.requestFocus();
      }
    }

    public void actionPerformed(ActionEvent e) {
      Object source = e.getSource();
      typeString = e.getActionCommand();
    }

    public JUDOProgramPropertiesDialog(JUDOIDE parent) {

        super(parent, true);
        final JUDOIDE judoIDE = parent;

        setTitle(judoIDE.lz.PRP_CHOOSE_PROG_PROP);

        graphicsButton = new JRadioButton(judoIDE.lz.PRP_GRAPHICS);
        graphicsButton.setActionCommand(JUDOBase.jud0_TYPE_GRAPHICS);

        textButton = new JRadioButton(judoIDE.lz.PRP_TEXT);
        textButton.setActionCommand(JUDOBase.jud0_TYPE_TEXT);
        textButton.setSelected(true);

        bothButton = new JRadioButton(judoIDE.lz.PRP_TEXT_AND_GRAPHICS);
        bothButton.setActionCommand(JUDOBase.jud0_TYPE_BOTH);

        TitledBorder border;
        border = BorderFactory.createTitledBorder(judoIDE.lz.PRP_WIN_STYLE);

        ButtonGroup buttonGroup = new ButtonGroup();
        buttonGroup.add(textButton);
        buttonGroup.add(graphicsButton);
        buttonGroup.add(bothButton);

        graphicsButton.addActionListener(this);
        textButton.addActionListener(this);
        bothButton.addActionListener(this);

        GridLayout gridLayout = new GridLayout(3, 1);
        GridLayout mainGridLayout = new GridLayout(1, 1);
        JPanel gridPanel = new JPanel(gridLayout);
        gridPanel.setBorder(border);
        JPanel mainGridPanel = new JPanel(mainGridLayout);

        gridPanel.add(textButton);
        gridPanel.add(graphicsButton);
        gridPanel.add(bothButton);

        mainGridPanel.add(gridPanel);

        Object[] array = {mainGridPanel};
        final String btnString1;
        btnString1 = judoIDE.lz.PRP_OK;

        final String btnString2 = judoIDE.lz.PRP_CANCEL;
        Object[] options = {btnString1, btnString2};

        optionPane = new JOptionPane(array,
                                    JOptionPane.PLAIN_MESSAGE,
                                    JOptionPane.OK_CANCEL_OPTION,
                                    null,
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
                      setWindowType(initialSelection);
                      canceled = true;
                      setVisible(false);
                    }
                    // user hit OK
                    else {
                        canceled = false;
                        setVisible(false);
                    }
                }
            }
        });
    }
}
