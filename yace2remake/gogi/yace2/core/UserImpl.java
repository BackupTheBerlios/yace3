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

import gogi.yace2.interfaces.Room;
import gogi.yace2.interfaces.Server;
import gogi.yace2.interfaces.User;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Enumeration;
import java.util.Hashtable;

public class UserImpl implements User
{
	// define props hashtable
	private Hashtable m_props;

	// define name and room
	private String m_name;
	private String m_room;

	// define socket and printwriter
	private Socket m_sock;
	private PrintWriter m_pw;

	// define server
	private Server m_yace;

	public UserImpl(String name, String room, Socket sock, java.io.PrintStream pw, Server yace)
	{
		// initialize all variables
		m_props = new Hashtable();

		m_name = name;
		m_room = room;

		m_sock = sock;
		m_pw = new PrintWriter(pw);

		m_yace = yace;

		// user joins a room
		yace.rooms().room(room).joins(this);
	}

	public boolean send(String to_send)
	{
		// set up the send stopper (if 10000 milliseconds have gone,
		// close the socket because of timeout)
		SendStopThread sst = new SendStopThread(m_sock, 10000);
		sst.start();

		try
		{
			// exception handling
			// try to print the text
		    m_pw.print(to_send);
		    m_pw.flush();
		}
		catch(Exception e)
		{
			// if failed, disconnect the user.
		    disconnect();
		}

		sst.done();
		return true;
	}

	public String sgetProp(String prop)
	{
		try
		{
			// exception handling is necessary because
			// a NullPointerException will be thrown if
			// a cast fails.
			// get the value and cast
			String to_ret = (String)m_props.get(prop);
			// throw an exception if the key does not exist
			if(to_ret == null)
				throw new NullPointerException();

			return to_ret;
		}
		catch(NullPointerException e)
		{
			return "";
		}
		catch(ClassCastException e)
		{
			// if cast has failed
			// each object can be made to a string with
			// the function toString(), so we do so.
			// NullPointerException has not to be caught
			// because it would be caught by
			// catch(NullPointerException e) if prop is null.
			return m_props.get(prop).toString();
		}
	}

	public boolean ssetProp(String prop, String to_set)
	{
		try
		{
			// exception handling is necessary because
			// put() will throw a NullPointerException if
			// the key or the value is null.
			m_props.put(prop, to_set);
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public long igetProp(String prop)
	{
		// if you only want to check if a value is
		// 0 or not, you may use bgetProp(), because
		// it returns a boolean value.
		try
		{
			// exception handling is necessary because
			// a NullPointerException will be thrown if
			// a cast fails.
			// get the value and cast
			Long to_ret = (Long)m_props.get(prop);
			// throw an exception if the key does not exist
			if(to_ret == null)
				throw new NullPointerException();

			return to_ret.longValue();
		}
		catch(NullPointerException e)
		{
			return 0;
		}
		catch(ClassCastException e)
		{
			// if cast has failed
			return 0;
		}
	}

	public boolean isetProp(String prop, long to_set)
	{
		try
		{
			// the second parameter in put() must be
			// an object, so you need to write "new Long()"
			// exception handling is necessary because
			// put() will throw a NullPointerException if
			// the key or the value is null.
			m_props.put(prop, new Long(to_set));
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public boolean bgetProp(String prop)
	{
		// this function will check if a value is
		// a Long or a Boolean and will cast it
		// into a primitive boolean type.
		try
		{
			// exception handling is necessary because
			// a NullPointerException will be thrown if
			// a cast fails.
			// get the value as an object
			Object value = m_props.get(prop);
			// throw an exception if the key does not exist
			if(value == null)
				throw new NullPointerException();

			boolean to_ret = false;
			// check if object is a boolean
			if((value.toString() == "true") || (value.toString() == "false"))
			// cast into Boolean and get booleanValue()
				to_ret = ((Boolean)value).booleanValue();
			else
			// cast into Long and check if 0 or not
				to_ret = (((Long)value).longValue() == 0) ? (false) : (true);

			return to_ret;
		}
		catch(NullPointerException e)
		{
			return false;
		}
		catch(ClassCastException e)
		{
			// if cast has failed
			return false;
		}
	}

	public boolean bsetProp(String prop, boolean to_set)
	{
		try
		{
			// the second parameter in put() must be
			// an object, so you need to write "new Boolean()"
			// exception handling is necessary because
			// put() will throw a NullPointerException if
			// the key or the value is null.
			m_props.put(prop, new Boolean(to_set));
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public boolean bsetProp(String prop, long to_set)
	{
		try
		{
			// the second parameter in put() must be
			// an object, so you need to write "new Boolean()".
			// Boolean does not allow a long as a constructor
			// parameter, so to_set must be checked and true or
			// false must be given as a parameter.
			// exception handling is necessary because
			// put() will throw a NullPointerException if
			// the key or the value is null.
			m_props.put(prop, new Boolean((to_set == 0) ? (false) : (true)));
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public boolean bsetProp(String prop, String to_set)
	{
		try
		{
			// Boolean allows a string as a contructor
			// parameter, the boolean value will be true
			// if to_set is a string equals "true".
			// exception handling is necessary because
			// put() will throw a NullPointerException if
			// the key or the value is null.
			m_props.put(prop, new Boolean(to_set));
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public Object getProp(String prop)
	{
		// no need to cast because this function
		// returns null if the object does not exist.
		try
		{
			// exception handling is necessary because
			// get() will throw a NullPointerException if
			// the key is null.
			return m_props.get(prop);
		}
		catch(NullPointerException e)
		{
			return null;
		}
	}

	public boolean setProp(String prop, Object to_set)
	{
		try
		{
			// exception handling is necessary because
			// put() will throw a NullPointerException if
			// the key or the value is null.
			m_props.put(prop, to_set);
			return true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public String name()
	{
		// return user name
		return m_name;
	}

	public Room room()
	{
		// return the room the user is in
		return m_yace.rooms().room(m_room);
	}

	public boolean join(String room)
	{
		// user leaves the old room
		// check if old room is empty and clear if necessary
		if(room().leaves(name()) == 2)
			m_yace.rooms().clear();

		// set the m_room variable to the new room
		m_room = room;

		// join the new room
		room().joins(this);

		return true;
	}

	public void disconnect()
	{
		// disconnect the user
		try
		{
		    m_sock.close();
		}
		catch(Exception e)
		{
		}
	}

	public Enumeration allProps()
	{
		// return all user properties
		return m_props.keys();
	}
}