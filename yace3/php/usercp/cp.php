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
    show_source("cp.php");
    return;
}

require_once("../config.inc.php");
require_once("../sqlcon.php");
require_once("session.php");
require_once("actions.php");

function senddata($data, $choice = false)
{
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="../style.css">
<title>YaCE 3 - User Control Panel</title>
</head>
<body>
<div id="main">
<p><b><font size="2">YaCE 3 - User Control Panel</font></b></p>
<?php
    echo($data);
    if($choice)
        echo(parsechoice());
?>
</div></body></html>
<?php
    flush();
}

function parsechoice()
{
    global $cfg, $mysql, $session;
    $tosend = "";
    $res = $mysql->select("SELECT * from `" . $cfg["usercp"]["table"]["cat"] . "` ORDER BY `" . $cfg["usercp"]["column"]["cat"]["order"] . "` ASC");
    if($res === false)
        return false;
    for($i = 0; $i < count($res); $i++)
    {
        $cat = getcat($res[$i][$cfg["usercp"]["column"]["cat"]["name"]]);
        $r = $mysql->select("SELECT * from `" . $cfg["usercp"]["table"]["actions"] . "` WHERE " . $cfg["usercp"]["column"]["actions"]["catid"] . "='" . $res[$i][$cfg["usercp"]["column"]["cat"]["id"]] . "'");
        if($r === false)
            return false;
        $acts = "";
        for($j = 0; $j < count($r); $j++)
        {
            $acts .= getaction($r[$j][$cfg["usercp"]["column"]["actions"]["label"]], $r[$j][$cfg["usercp"]["column"]["actions"]["action"]], $session->getname . "=" . $session->getID() );
        }
        $tosend .= getchoice($cat, $acts);
    }
    return $tosend;
}

function getchoice($CAT, $ACTIONS)
{
    $choice = implode("", file("templates/choice.inc.txt"));
    $choice = str_replace("\"", "\\\"", $choice);
    eval("\$choice = \"$choice\";");
    return $choice;
}

function getcat($CATEGORY)
{
    $cat = implode("", file("templates/category.inc.txt"));
    $cat = str_replace("\"", "\\\"", $cat);
    eval("\$cat = \"$cat\";");
    return $cat;
}

function getaction($LABEL, $ACTION, $ID)
{
    $PHPSELF = isset($_SERVER["PHP_SELF"]) ? $_SERVER["PHP_SELF"] : $PHP_SELF;
    $act = implode("", file("templates/action.inc.txt"));
    $act = str_replace("\"", "\\\"", $act);
    eval("\$act = \"$act\";");
    return $act;
}

function checkPW($user, $md5pwd)
{
    global $cfg, $mysql;
    if($res = $mysql->selectone("SELECT `" . $cfg["registry"]["column"]["registry"]["password"] . "` from `" . $cfg["registry"]["table"]["registry"] . "` WHERE `" . $cfg["registry"]["column"]["registry"]["name"] . "`='" . $user . "'"))
    {
        if($res[$cfg["registry"]["column"]["registry"]["password"]] != $md5pwd)
            return 1;
        return 2;
    }
    return 0;
}

function isAction($func)
{
    global $cfg, $mysql;
    if($mysql->selectone("SELECT `" . $cfg["usercp"]["column"]["actions"]["action"] . "` from `" . $cfg["usercp"]["table"]["actions"] . "` WHERE `" . $cfg["usercp"]["column"]["actions"]["action"] . "`='" . $func . "'"))
        return true;
    return false;
}

$mysql = &new mysql($cfg["mysql"]["host"], $cfg["mysql"]["user"], $cfg["mysql"]["pass"], $cfg["mysql"]["db"]);
if(!$mysql->connect())
{
    senddata("<b>Fatal error: Could not connect to database! Please tell the webmaster about this problem.</b>");
    exit();
}

$loginname = "";
if(!empty($_POST["loginname"]))
    $loginname = $_POST["loginname"];
if(!empty($_POST["password"]))
    $loginpwd = $_POST["password"];

if(!empty($loginpwd) AND !empty($loginname))
    switch(checkPW($loginname, md5($loginpwd)))
    {
        case 0:
            senddata("<p><b>This user does not exist!</b> Please use the back button of your browser and check your input values.</p>");
            exit();
            break;
        case 1:
            senddata("<p><b>The password is wrong!</b> Please use the back button of your browser and check your input values.</p>");
            exit();
            break;
        case 2:
            break;
    }

$session = &new session(&$mysql, $cfg["usercp"]);

if(!$session->session_start($loginname))
{
    senddata("<p><b>Your session is invalid!</b> Please use <a href=\"index.php\">this link</a> to log in into the control panel.</p>");
    exit();
}

if(!empty($loginpwd))
    $session->addVar("password", md5($loginpwd));

if(checkPW($session->getName(), $session->getVar("password")) != 2)
{
    senddata("<p><b>Your session is invalid!</b> Please use <a href=\"index.php\">this link</a> to log in into the control panel.</p>");
    exit();
}

$action = "";
if(isset($_REQUEST["action"]))
    $action = $_REQUEST["action"];

switch($action)
{
    case "":
        senddata("", true);
        break;
    default:
        if(!isAction($action))
            senddata("", true);
        else
        {
            $reqmethod = isset($_SERVER["REQUEST_METHOD"]) ? $_SERVER["REQUEST_METHOD"] : $HTTP_SERVER_VARS["REQUEST_METHOD"];
            $args = false;
            switch($regmethod)
            {
                case "GET":
                    $args = isset($_GET) ? $_GET : $HTTP_GET_VARS;
                    break;
                case "POST":
                    $args = isset($_POST) ? $_POST : $HTTP_POST_VARS;
                    break;
                default:
                    $args = false;
                    break;
            }
            $action($cfg, &$session, &$mysql, $args);
        }
        break;
}

$session->saveVars();
$mysql->disconnect();
?>