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

public class JUDOMouseEvent {

  public int x;
  public int y;
  public boolean button1;
  public boolean button2;
  public boolean button3;
  public boolean newEvent;
  public int dragStartX1;
  public int dragStartY1;
  public int dragEndX1;
  public int dragEndY1;
  public int dragStartX2;
  public int dragStartY2;
  public int dragEndX2;
  public int dragEndY2;
  public int dragStartX3;
  public int dragStartY3;
  public int dragEndX3;
  public int dragEndY3;
  public JUDOMouseEvent() {
  }
}