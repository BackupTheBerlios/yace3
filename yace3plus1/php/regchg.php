<?
/*
  Copyright (C) 2003 Tobias Bahls

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
  function send_data() {
    global $tosend;
    $tosend .= "</div></body></html>";
    echo $tosend;
    die();
  }

  include("config.php");
  $actform = str_replace("%PHPSELF%", isset($_SERVER["PHP_SELF"]) ? $_SERVER["PHP_SELF"] : $PHP_SELF, implode("", file("chgform.inc.txt")));
  $tosend = "
  <html>
  <head>
  <title>YaCE3 - Registry Ändern</title>
  <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />
  </head>
  <div id=\"login\">
  ";

  if (isset($_POST["name"])) {
    $con = @mysql_connect($mysql_host, $mysql_user, $mysql_pass);
    if(!($con AND mysql_select_db($mysql_db))) {
      echo "Fatal Error. Could not Connect to Database.";
      flush();
      die();
    }
    $regdata = mysql_query("SELECT * FROM `registry` WHERE `handle` = '{$_POST["name"]}'",$con);
    if (@mysql_num_rows($regdata) == 0) {
       $tosend .= "User Existiert nicht.";
       send_data();
    }
    $zoidberg = @mysql_fetch_array($regdata);
    if ($zoidberg["password"] != md5($_POST["pw"])) {
       $tosend .= "Falsches Passwort.";
       send_data();
    }
	    
    // Change password only if the form is filed
    if (isset($_POST["pw2"])) {
      if ($_POST["pw2"] != $_POST["pw3"]) {
        $tosend .= "Passwörter stimmen nicht Ueberein.";
	send_data();
      }
      mysql_query("UPDATE `registry` SET `password`=MD5('{$_POST["pw2"]}') WHERE `handle`='{$_POST["name"]}'",$con);
      $tosend .= "Passwort geändert.<br />";
    }
  }
  else {
    $tosend .= $actform;
  }
  send_data();
?>
