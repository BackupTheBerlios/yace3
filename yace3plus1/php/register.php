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
include("head.inc.php");

require_once("config.inc.php");
require_once("sqlcon.php");
require_once("registry.php");

function random()
{
    $toret = "";
    $rand = rand(4, 16);
    for($i = 0; $i < $rand; $i++)
    {
        $toret .= md5(rand(rand(0, getrandmax()), rand(0, getrandmax())));
    }
    $toret = md5($toret);
    $toret = substr($toret, rand(0, 20), 12);
    return $toret;
}

function actaddress()
{
    $dirname = isset($_SERVER["PHP_SELF"]) ? $_SERVER["PHP_SELF"] : $PHP_SELF;
    if(!($i = strrpos($dirname, "/")))
        $dirname = "/";
    else
        $dirname = substr($dirname, 0, $i + 1);
    
    $toret = "http://" . (isset($_SERVER["HTTP_HOST"]) ? $_SERVER["HTTP_HOST"] : $HTTP_HOST) . $dirname . "activate.php";
    return $toret;
}

function senddata($data, $regform = false)
{
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="style.css">
<title>YaCE 3 - Registration</title>
</head>
<body>
<div id="login">
<p><a href="<?php echo(actaddress()); ?>">Click here to activate your nickname</a></p>
<?php
    echo($data);
?>
</div></body></html>
<?

function mailtext($name, $key, $pwd)
{
    $actaddress = actaddress();
    $mailtext = implode("", file("mail.txt"));
    eval("\$mailtext = \"$mailtext\";");
    return $mailtext;
}

function validnick($name)
{
    if(!preg_match("/[A-Za-z0-9\-]{3,}/", $name))
        return false;
    return true;
}

if(empty($name) OR empty($mail) OR empty($mail2) OR empty($password) OR empty($password2))
{
    senddata("", true);
    exit();
}
if(!validnick($name))
{
    senddata("<p><b>Das ist kein g&uuml;ltiger Username. Du kannst nur A-Z, a-z, 0-9 und \"-\" benutzen.</b></p>");
    exit();
}
if($password != $password2)
{
    senddata("<p><b>Die Passw&ouml;rter sind nicht gleich!</b></p>", true);
    exit();
}
if($mail != $mail2)
{
    senddata("<p><b>Die E-Mail Adressen sind nicht gleich!</b></p>", true);
    exit();
}
if(strpos($mail, "@") === false OR strpos($mail, ".") === false)
{
    senddata("<p><b>Du hast eine ung&uuml;ltige E-Mail Adressse eingegeben!</b></p>", true);
    exit();
}

$mysql = &new mysql($cfg["mysql"]["host"], $cfg["mysql"]["user"], $cfg["mysql"]["pass"], $cfg["mysql"]["db"]);
if(!$mysql->connect())
{
    senddata("<b>Fatal error: Konnte nicht zur Datenbank verbinden. Bitte dem Webmaster melden.</b>");
    exit();
}

$registry = &new registry(&$mysql, $cfg["registry"]);

if($registry->is_registered($name))
{
    senddata("<p><b>Der Username ist bereits registriert. Bitte w&auml;hle einen anderen.</b></p>");
    exit();
}

$key = random();
$registry->addUser($name, $password, $mail, $key);

mail($mail, "Aktivierungs Key", mailtext($name, $key, $password), "From: noreply@" . (isset($_SERVER["HTTP_HOST"]) ? $_SERVER["HTTP_HOST"] : $HTTP_HOST));
senddata("<p><b>Registrierung hinzugef&uuml;gt!</b>
<br>Du musst deinen Nickname aktivieren, um ihn im Chat benutzen zu können. 
Du bekommst den Aktivierungs Key per E-Mail. 
<a href=\"" . actaddress() . "\">Aktivierte deinen Nickname</a></p>");
?>
