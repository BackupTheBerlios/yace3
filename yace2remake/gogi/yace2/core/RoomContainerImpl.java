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

import gogi.yace2.interfaces.RoomContainer;
import gogi.yace2.interfaces.Room;
import java.util.Hashtable;
import java.util.Enumeration;

public class RoomContainerImpl implements RoomContainer
{
	Hashtable m_rooms;

	public RoomContainerImpl()
	{
		m_rooms = new Hashtable();
	}

	public Enumeration allRooms()
	{
		// return all rooms in this container
		return m_rooms.elements();
	}

	public boolean exists(String name)
	{
		// check if a room exists
		// you cannot use "room(name) != null", because
		// room() always returns a Room object, even if
		// it has not been in the m_rooms hashtable.

		try
		{
			// exception handling is necessary because
			// get() will throw a NullPointerException if
			// the key is null.
			return (m_rooms.get(name) != null);
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public Room room(String name)
	{
		// returns the room with the name in the variable name
		// if this room does not exist this container will create
		// a new object, put it into the m_rooms hashtable and
		// return it.
		Room to_ret;
		try
		{
			// exception handling is necessary because
			// get() and put() will throw a
			// NullPointerException if the key is null and
			// the cast could fail and throw a
			// ClassCastException
			to_ret = (Room)m_rooms.get(name);

			// if this room does not exist, create
			// a new room.
			if(to_ret == null)
			{
				to_ret = new RoomImpl(name);
				m_rooms.put(name, to_ret);
			}
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

	public boolean del(String name)
	{
		// delete a room specified by its name
		try
		{
			// exception handling is necessary because
			// remove() will throw a NullPointerException
			// if the key is null.

			Object o = m_rooms.remove(name);
			// check if there was a room named with name
			return (o == null) ? false : true;
		}
		catch(NullPointerException e)
		{
			return false;
		}
	}

	public void clear()
	{
		// clear all empty rooms
		Room next;
		for(Enumeration e = m_rooms.elements(); e.hasMoreElements();)
		{
			try
			{
				// exception handling is necessary because
				// nextElement() will throw a NoSuchElementException
				// if no more elements exist and the cast could fail
				// and throw a ClassCastException
				next = (Room)e.nextElement();
			}
			catch(ClassCastException ex)
			{
				// ignore the error and continue
				continue;
			}
			catch(java.util.NoSuchElementException ex)
			{
				// ignore the error and continue
				continue;
			}

			// check if room is empty or not by using the
			// users() function from Room
			Enumeration to_check = next.users();
			if(!to_check.hasMoreElements())
			{
				// delete the room
				del(next.name());
			}
		}
	}
}
