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
    show_source("actions.php");
    return;
}

function getfile($file, $ARGS)
{
    if(!is_file("actions/" . $file))
        return "";
    $toret = implode("", file("actions/" . $file));
    $toret = str_replace("\"", "\\\"", $toret);
    eval("\$toret = \"$toret\";");
    return $toret;
}

function chgpwd( $cfg, $session, $mysql, $args)
{
    if(empty($args))
        return false;
    if($mysql->query("UPDATE `" . $cfg["registry"]["table"]["registry"] . "`
                      SET `" . $cfg["registry"]["column"]["registry"]["password"] . "`='" . md5($args) . "'
                      WHERE `" . $cfg["registry"]["column"]["registry"]["name"] . "`='" . $user . "'
                      LIMIT 1"))
        return true;
    return false;
} */

function logout($cfg, $session, $mysql, $args)
{
    if($session->session_destroy())
        senddata("<b>Successfully logged out!</b><br><a href=\"index.php\">Return to log in site</a>");
    else
        senddata("lol.");
}
?>