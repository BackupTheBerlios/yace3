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
// import gogi.yace2.base.*;
import bsh.Interpreter;
// import java.util.*;

public class YaCE implements Server
{
	private Interpreter m_bsh;

	public YaCE()
	{
		// Main program
		System.out.println();
		System.out.println(" +----- YaCE 2 - Remake ----------------------------");

		// Set up the BeanShell environment
		System.out.println(" +-- Setting up BeanShell...");
		try
		{
			m_bsh = new Interpreter();
		}
		catch(java.lang.NoClassDefFoundError e)
		{
			System.out.println(" +-- Fatal error: BeanShell could not be set up!");
			System.out.println(" +-- Please check if BeanShell is installed.");
			System.out.println(" +-- Get BeanShell at http://www.beanshell.org");
			System.out.println(" +--");
			System.out.println(" +-- Exiting...");
			System.exit(1);
		}

		try
		{
			// Set the "yace" variable to be used in the environment
			m_bsh.eval("gogi.yace2.interfaces.Server yace = null;");
			m_bsh.set("yace", this);
		}
		catch(Exception e)
		{
			System.out.println(" +-- Fatal error: Error while setting up \"yace\"");
			System.out.println(" +--              environment variable. Please");
			System.out.println(" +--              check if you have installed Bean-");
			System.out.println(" +--              Shell and YaCE 2 correctly.");
			System.out.println(" +--");
			System.out.println(" +-- Exiting...");
			System.exit(1);
		}


	}
	public Interpreter bsh()
	{
		return m_bsh;
	}
}

/* public class YaCE implements Server
{
    private RoomContainer rc;
    private UserContainer uc;
    private Util util;
    private Interpreter bsh_env;
    private RegistryUserContainer ruc;

    private LogHandler logh;
    private String loghandlerclass;

    public YaCE(String lhc)
    {

    System.out.println("");
    System.out.println(" +---- YaCE 2 ------------------ ");

    Hashtable config = null;

    uc = new UserContainerImpl(this);
    rc = new RoomContainerImpl(this);
    ruc = new FileRegistry("registry");
    util = new Util(this);

        loghandlerclass = lhc;
        try {
        logh = (LogHandler)Class.forName(loghandlerclass).newInstance();
        } catch(Exception e) {
        System.out.println(" +-- Error: LogHandler.class");
        System.exit(1);
        }
    logh.init(this);

        try {
        bsh_env = new Interpreter();
        bsh_env.eval("gogi.yace2.interfaces.Server yace = null;");
        bsh_env.set("yace", this);
    } catch(Exception e) {
        writeLog(" +-- Error: BeanShell Error");
        System.exit(1);
    }

    ruc.load();

    try {
        bsh_env.source("scripts/core/init");
    } catch(EvalError p) {
        writeLog(" +-- Critical Error: <init>\r\n" + p.getMessage());
        System.exit(1);
    } catch(Exception e) {
        writeLog(" +-- Critical Error: init script not found");
        System.exit(1);
    }
    }

    public RoomContainer rooms()
    {
    return rc;
    }

    public UserContainer users()
    {
    return uc;
    }

    public RegistryUserContainer reg()
    {
    return ruc;
    }

    public Util util()
    {
    return util;
    }

    public Interpreter getBsh()
    {
    return bsh_env;
    }

    public void writeLog(String towrite)
    {
        System.out.println(towrite);
        logh.putTimeln(towrite);
    }
}
*/