/*
  Copyright (C) 2002-2004 Thomas Yao

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

public interface RegistryControl
{
	// get all registered users
	public Enumeration allUsers();

	// get a specific registered user
	public RegistryUser user(String name);

	// add and delete an user
	public boolean add(String name, String password);
	public boolean del(String name);

	// check if user's password is right
	public boolean access(String name, String password);

	// load and save the registry
	public boolean load();
	public boolean save();
}