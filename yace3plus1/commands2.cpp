/*
  Copyright (C) 2003 Giuliano Montecarlo

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

static int hug(CA)
{
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("hug");
  tosend = replaceUser(caller, tosend, "-S");
  tosend = replaceUser(args.arg(0), tosend, "-R");
  sendRoomI(caller, tosend);
  return 0;
}

static int kiss(CA)
{
  if(!exists(args.arg(0), caller)) return 0;
  string tosend = yace->sql().getString("kiss");
  tosend = replaceUser(caller, tosend, "-S");
  tosend = replaceUser(args.arg(0), tosend, "-R");
  sendRoomI(caller, tosend);
  return 0;
}

/* not yet
static int myip(CA)
{
  sendUser(caller, replace(yace->sql().getString("myip"), "%IP%", getIP(caller)));
  return 0;
}
*/
