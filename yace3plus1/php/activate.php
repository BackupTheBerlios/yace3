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
{
    show_source("activate.php");
    return;
}

require_once("config.inc.php");
require_once("sqlcon.php");
require_once("registry.php");

function senddata($data, $actform = false)
{
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="style.css">
<title>YaCE 3 - Activation</title>
</head>
<body>
<div id="login">
<?php
    echo($data);
    if($actform)
        echo(actform());
?>
</div></body></html>
<?php
    flush();
}

function actform()
{
    $PHPSELF = isset($_SERVER["PHP_SELF"]) ? $_SERVER["PHP_SELF"] : $PHP_SELF;
    $actform = implode("", file("actform.inc.txt"));
    eval("\$actform = \"$actform\";");
    return $actform;
}

if(isset($_REQUEST["name"]))
    $name = $_REQUEST["name"];
if(isset($_REQUEST["key"]))
    $key = $_REQUEST["key"];

if(empty($name) OR empty($key))
{
    senddata("", true);
    exit();
}

$mysql = &new mysql($cfg["mysql"]["host"], $cfg["mysql"]["user"], $cfg["mysql"]["pass"], $cfg["mysql"]["db"]);
if(!$mysql->connect())
{
    senddata("<b>Fatal error: Could not connect to database! Please tell the webmaster about this problem.</b>");
    exit();
}

$registry = &new registry(&$mysql, $cfg["registry"]);

if(!$registry->activateUser($name, $key))
{
    senddata("<p><b>The key is wrong or the user does not exist!</b></p>", true);
    exit();
}

senddata("<p>The username \"" . $name . "\" has been successfully activated! You can use it now in the chat.</p>");
?>