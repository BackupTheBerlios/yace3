/*
  Copyright (C) 2004 Thomas Yao

  YaCE 2 Original File:
  Copyright (C) 2002-2003 Giuliano Gagliardi

  YaCE 2 is free software, you can redistribute it and/or
  modify it under the terms of the Affero General Public License as
  published by Affero, Inc., either version 1 of the License, or
  (at your option) any later version.

  YaCE 2 is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY, without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  Affero General Public License for more details.

  You should have received a copy of the Affero General Public
  License in the COPYING file that comes with YaCE2. If
  not, write to Affero, Inc., 510 Third Street, Suite 225, San
  Francisco, CA 94107 USA.
*/

package gogi.yace2.interfaces;
import java.util.Enumeration;

public interface User
{
	// send something to an user
	public boolean send(String to_send);

	// add and get a string to the props hashtable
	public String sgetProp(String prop);
	public boolean ssetProp(String prop, String to_set);

	// add and get a long to the props hashtable
	public long igetProp(String prop);
	public boolean isetProp(String prop, long to_set);

	// add and get a boolean to the props hashtable
	public boolean bgetProp(String prop);
	public boolean bsetProp(String prop, boolean to_set);
	public boolean bsetProp(String prop, long to_set);
	public boolean bsetProp(String prop, String to_set);

	// add and get an object to the props hashtable
	public Object getProp(String prop);
	public boolean setProp(String prop, Object toset);

	// get other user data
	public String name();
	public Room room();

	// let an user join another room
	public boolean join(String room);

	// disconnect the user
	public void disconnect();

	// get all props
	public Enumeration allProps();
}
