<?php
/*
  Copyright (C) 2003 Thomas Yao, Tobias Bahls

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

if($_SERVER["QUERY_STRING"] == "source")
    die("This is a file with strict privacy content. You cannot get the source of this file.");

// mysql config
$cfg["mysql"]["host"]   = "localhost";
$cfg["mysql"]["user"]   = "web2";
$cfg["mysql"]["pass"]   = "d311r4715n3?";
$cfg["mysql"]["db"]     = "usr_web2_1";

$cfg["table"]["replaces"]   = "replaces";

// yace3 registry
// table names
$cfg["registry"]["table"]["registry"]   = "stfu";
$cfg["registry"]["table"]["keys"]       = "actkeys";

// column names
$cfg["registry"]["column"]["registry"]["name"]      = "w00t";
$cfg["registry"]["column"]["registry"]["password"]  = "WamP";
$cfg["registry"]["column"]["registry"]["mail"]      = "loool";

$cfg["registry"]["column"]["keys"]["name"]      = "name";
$cfg["registry"]["column"]["keys"]["password"]  = "password";
$cfg["registry"]["column"]["keys"]["key"]       = "actkey";
$cfg["registry"]["column"]["keys"]["mail"]      = "mail";
$cfg["registry"]["column"]["keys"]["regtime"]   = "regtime";

// user cp - session table
$cfg["usercp"]["table"]["sessions"] = "usercp_sessions";
$cfg["usercp"]["table"]["cat"]      = "usercp_categories";
$cfg["usercp"]["table"]["actions"]  = "usercp_actions";

$cfg["usercp"]["column"]["sessions"]["name"]        = "name";
$cfg["usercp"]["column"]["sessions"]["id"]          = "id";
$cfg["usercp"]["column"]["sessions"]["vars"]        = "vars";
$cfg["usercp"]["column"]["sessions"]["time"]        = "time";

$cfg["usercp"]["column"]["cat"]["id"]       = "id";
$cfg["usercp"]["column"]["cat"]["name"]     = "name";
$cfg["usercp"]["column"]["cat"]["order"]    = "order";

$cfg["usercp"]["column"]["actions"]["catid"]    = "catid";
$cfg["usercp"]["column"]["actions"]["label"]    = "label";
$cfg["usercp"]["column"]["actions"]["action"]   = "action";
$cfg["usercp"]["column"]["actions"]["order"]    = "order";
?>
