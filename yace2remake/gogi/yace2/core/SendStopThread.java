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

import java.net.Socket;

public class SendStopThread extends Thread
{
    private Socket m_sock;
    private int m_timeout;
    private boolean m_sent;

    public SendStopThread(Socket sock, int timeout)
    {
    	// initialize all variables
		m_sock = sock;
		m_timeout = timeout;
		m_sent = false;
    }

    public synchronized void done()
    {
    	// if message was sent, set sent to true
		m_sent = true;
    }

    public void run()
    {
    	// this class is implemented in gogi.yace2.UserImpl
    	// to check if the timeout is arrived when the message
    	// was sent.
		Thread.currentThread().setName("SendStopper");

		int slept = 0;
		while(slept < m_timeout)
		{
		    try
		    {
				Thread.sleep(500);
		    }
		    catch(Exception e)
		    {
		    }

		    slept += 500;

		    if(m_sent)
		    {
				return;
		    }
		}

		if(!m_sent)
		{
		    try
		    {
				m_sock.close();
		    }
		    catch(Exception e)
		    {
		    }
		}
    }
}

