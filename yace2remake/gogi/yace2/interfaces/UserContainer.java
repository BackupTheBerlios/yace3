/*
  Copyright (C) 2004 Thomas Yao

  YaCE 2 Original File:
  Copyright (C) 2002-2003 Giuliano Gagliardi

  This file is part of YaCE 2

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
import java.net.Socket;
import java.io.PrintStream;

public interface UserContainer
{
	// get all users in this container
	public Enumeration allUsers();

	// get a specific user
	public User get(String name);

	// add and delete an user
	public boolean add(String name, String room, Socket csock, PrintStream pw);
	public boolean del(String name);
}
