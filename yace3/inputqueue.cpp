/*
  Copyright (C) 2003 Giuliano Gagliardi, Tobias Bahls

  This file is part of YaCE 3

  YaCE 3 is free software, you can redistribute it and/or
  modify it under the terms of the Affero General Public License as
  published by Affero, Inc., either version 1 of the License, or
  (at your option) any later version.

  YaCE 3 is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY, without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  Affero General Public License for more details.

  You should have received a copy of the Affero General Public
  License in the COPYING file that comes with YaCE3. If
  not, write to Affero, Inc., 510 Third Street, Suite 225, San
  Francisco, CA 94107 USA.
*/

#include "inputqueue.h"
#include "commands.h"
#include "functions.h"
#include "hash_map.h"
#include "yace.h"
#include "user.h"
#include "stringutil.h"
#include "ircfunctions.h"
#include <iostream>
#include <sstream>

void
inputqueue::init ()
{
	register_commands (cmds);
}

void
inputqueue::run ()
{
	while (true)
	{
		sem.wait ();

		m_q.enterMutex ();
		inner_input got = q.front ();
		q.pop ();
		m_q.leaveMutex ();

		string command;
		string user = got.user;
		commandargs argz ("");
		commandfunc f;

		yace->log ("(" + got.user + ")" + " " + got.text);
		cout << "(" << got.user << ")" << " " << got.text << endl;

		// POST PROCESS INPUT <-- to put somewhere else

		::user * u = yace->users ().getUser (user);
		if (u == NULL)
			continue;

		int i;
		if ((i = yace->sql ().getConfNum ("maxlines")))
		{
			if (u->igetProp ("said") > i)
			{
				if ((i =
				     yace->sql ().
				     getConfNum ("neverkickrights")))
				{
					if (u->igetProp ("rights") < i)
						u->quit ();
				}
				else
				{
					u->quit ();
				}
			}
		}

		u->incrProp ("said");
		u->isetProp ("silence", 0);

		if (u->sgetProp ("away") != "")
		{
			returns (u);
		}

		if (yace->sql ().getConfNum ("maxlen"))
			if (got.text.length () >
			    unsigned (yace->sql ().getConfNum ("maxlen")))
				got.text = "~";

		u->DecRef ();
		// POST PROCESS END

		string sendirc, gottext;
		gottext = got.text;
		iconv ("ISO-8859-1", "UTF-8", sendirc, gottext);
		if (got.text.find ("/") == 0)
		{
			commandargs tmp (got.text.substr (1));
			if (cmds.count (tolower (tmp.arg (0))))
			{
				command = tolower (tmp.arg (0));
				argz = commandargs (tmp.rest (0));
			}
			else
			{
				command = "m";
				argz = commandargs (got.text.substr (1));
			}
		}
		else
		{
			command = "say";
			sendUserIRC (user, sendirc);
			argz = commandargs (replaceI
					    (replaceAll (got.text),
					     "sebi",
					     "Der &Auml;rmste Stra&szlig;enpenner von Frankfurt"),
					    got.text);
		}


		f = cmds[command];
		f (user, argz);
	}
}

void
inputqueue::final ()
{
	delete this;		//not really needed
}

void
inputqueue::postInput (const string & user, const string & text)
{
	inner_input a;
	a.user = user;
	a.text = text;

	m_q.enterMutex ();
	q.push (a);
	sem.post ();
	m_q.leaveMutex ();
}
