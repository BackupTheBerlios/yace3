<?php
/*
  Copyright (C) 2003 Thomas Yao

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
{
    show_source("index.php");
    return;
}

require_once("../config.inc.php");
require_once("../sqlcon.php");
require_once("session.php");

function senddata($data, $loginform = false)
{
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="../style.css">
<title>YaCE 3 - User Control Panel</title>
</head>
<body>
<div id="login">
<?php
    echo($data);
    if($loginform)
        echo(loginform());
?>
</div></body></html>
<?php
    flush();
}

function loginform()
{
    $ACTION = "cp.php";
    $loginform = implode("", file("loginform.inc.txt"));
    $loginform = str_replace("\"", "\\\"", $loginform);
    eval("\$loginform = \"$loginform\";");
    return $loginform;
}

$mysql = &new mysql($cfg["mysql"]["host"], $cfg["mysql"]["user"], $cfg["mysql"]["pass"], $cfg["mysql"]["db"]);
if(!$mysql->connect())
{
    senddata("<b>Fatal error: Could not connect to database! Please tell the webmaster about this problem.</b>");
    exit();
}

$session = &new session(&$mysql, $cfg["usercp"]);

if($session->session_start())
{
    header("Location: cp.php");
    flush();
    exit();
}

senddata("", true);
?>