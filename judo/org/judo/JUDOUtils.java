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

import java.util.Vector;

/**
 * Has utility methods.  Includes getString used in localization
 */
public class JUDOUtils {

  public String getString(String source, String var1) {
    int varPos = source.indexOf("%s");
    if (varPos == -1) {
      return source;
    }
    else {
      return source.substring(0, varPos) + var1 + source.substring(varPos + 2);
    }
  }
  public String getString(String source, String var1, String var2) {
    int varPos = source.indexOf("%s");
    String interpedString = "";
    if (varPos == -1) {
      return source;
    }
    else {
      interpedString = source.substring(0, varPos) + var1 + source.substring(varPos + 2);
      varPos = interpedString.indexOf("%s");
      if (varPos == -1) {
        return source;
      }
      else {
        return interpedString.substring(0, varPos) + var2 + interpedString.substring(varPos + 2);
      }
    }
  }
  //  public String getString(String source, String var1, String var2, String var3) {return "";}
  //  public String getString(String source, String var1, String var2, String var3, String var4) {return "";}
  //  public String getString(String source, String var1, String var2, String var3, String var4, String var5) {return "";}
  //  public String getString(String source, String var1, String var2, String var3, String var4, String var5, String var6) {return "";}

  public static void main(String args[]) {
    JUDOUtils ju = new JUDOUtils();
    //System.out.println(ju.getString("You have spoken to me %s time(s)", "bob is cute"));
    System.out.println(ju.getString("Someone named %s thinks  are good", "Lewita", "200 Puppies"));
  }
}