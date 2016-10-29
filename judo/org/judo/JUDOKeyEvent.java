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

import java.util.Hashtable;
public class JUDOKeyEvent {

  private Hashtable keysPressed = new Hashtable();
  public String keyRead = "";
  public boolean waitingForKey = false;

  public JUDOKeyEvent() {
  }

  public boolean keyDown(String key) {
    if (keysPressed.get(key) == null) {
      return false;
    }
    return ((Boolean) keysPressed.get(key)).booleanValue();
  }

  public void setKeyDown(String key) {
    keysPressed.put(key, new Boolean(true));
  }
  public void setKeyUp(String key) {
    keysPressed.put(key, new Boolean(false));
  }
}