package org.judo;

import java.util.*;

/**
 * Class FixEnumImpl implements the Enumeration interface
 * given that the number of elements in an enumeration is known
 * at enumeration construction time.
 *
 * The enumeration retrieval operations are synchronized for
 * concurrent access.
 *
 * @see java.util.Enumeration
 *
 * @author Thomas Wang
 * @version 0.55
 */
public class FixEnumImpl
  implements Enumeration
{
  protected int capacity;
  protected int read_index;
  protected int write_index;
  protected Object enum_array[];

  /**
   * Constructor for class SynFixEnumImpl.
   * @param total_num total number of elements in the enumeration
   */
  public FixEnumImpl(int total_num)
  {
    this.capacity = total_num;
    this.read_index = this.write_index = 0;
    if (total_num > 0)
    {
      this.capacity = total_num;
      this.enum_array = new Object[total_num];
    }
    else
    {
      this.capacity = 0;
      this.enum_array = null;
    }
  }

  /**
   * Add an element to the enumeration.  This method is used
   * inside the package to populate the enumeration.
   *
   * @param elem the element to be added to the enumeration
   */
  void addMember(Object elem)
  {
    this.enum_array[this.write_index++] = elem;
  }

  /**
   * Method hasMoreElements() returns true if there are more
   * values to be enumerated, and can be used to determine whether
   * a loop should continue.
   *
   * @see java.util.Enumeration#hasMoreElements
   */
  public synchronized boolean hasMoreElements()
  {
    return this.read_index < this.capacity;
  }

  /**
   * Method nextElement() will return the next element in the
   * enumeration.  If no more elements are available, the exception
   * NoSuchElementException will be thrown.
   *
   * @exception NoSuchElementException thrown when no more items
   * available
   * @see java.util.Enumeration#nextElement
   */
  public synchronized Object nextElement()
  {
    return this.enum_array[this.read_index++];
  }
}