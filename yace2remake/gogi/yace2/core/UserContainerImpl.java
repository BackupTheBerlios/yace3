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

package gogi.yace2.core;

import gogi.yace2.interfaces.Server;
import gogi.yace2.interfaces.UserContainer;
import gogi.yace2.interfaces.User;
import java.net.Socket;
import java.util.Hashtable;
import java.util.Enumeration;

public class UserContainerImpl implements UserContainer
{
	Hashtable m_users;
	Server m_yace;

	public UserContainerImpl(Server yace)
	{
		m_users = new Hashtable();
		m_yace = yace;
	}

	public Enumeration allUsers()
	{
		// return all users in this container
		return m_users.elements();
	}

	public User user(String name)
	{
		// returns the user with the name in the variable name
		User to_ret;
		try
		{
			// exception handling is necessary because
			// get() and put() will throw a
			// NullPointerException if the key is null and
			// the cast could fail and throw a
			// ClassCastException

			// toLowerCase() is necessary for incasesensitivity
			to_ret = (User)m_users.get(name.toLowerCase());
		}
		catch(NullPointerException e)
		{
			return null;
		}
		catch(ClassCastException e)
		{
			return null;
		}

		return to_ret;
	}

	public boolean add(String name, String room, Socket sock, java.io.PrintStream pw)
	{
		// create a new user
		try
		{
			// exception handling is necessary because
			// put() will throw a NullPointerException
			// if the key is null.

			User user = new UserImpl(name, room, sock, pw, m_yace);

			// make the username incasesensitive
			m_users.put(name.toLowerCase(), user);
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public boolean del(String name)
	{
		// delete a user specified by its name
		try
		{
			// exception handling is necessary because
			// remove() will throw a NullPointerException
			// if the key is null.

			// toLowerCase() for incasesensitivity
			Object o = m_users.remove(name.toLowerCase());
			// check if there was a user named with name
			return (o == null) ? false : true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}
}