<?php
/*
  Copyright (C) 2003 Tobias Bahls, Thomas Yao

  This file is part of YaCE 3

  YaCE 3 is free software, you can redistribute it and/or
  modify it under the terms of the Affero General Public License   published by 
  Affero, Inc., either version 1 of the License, or
  (at your option) any later version.

  YaCE 3 is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY, without even the implied warr  of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See th
  Affero General Public License for more details.

  You should have received a copy of the Affero General Public
  License in the COPYING file that comes with YaCE3. If
  not, write to Affero, Inc., 510 Third Street, Suite 225, San
  Francisco, CA 94107 USA.
*/

if($_SERVER["QUERY_STRING"] == "source")
{
    show_source("smilies.php");
    return;
}

require_once("config.inc.php");
require_once("sqlcon.php");

function senddata($data)
{
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="style.css">
<title>YaCE 3 - Smilie list</title>
</head>
<body>
<?php
    echo($data);
?>
</body>
</html>
<?php
    flush();
}

$mysql = &new mysql($cfg["mysql"]["host"], $cfg["mysql"]["user"], $cfg["mysql"]["pass"], $cfg["mysql"]["db"]);
if(!$mysql->connect())
{
    senddata("<b>Fatal error: Could not connect to database! Please tell the webmaster about this problem.</b>");
    exit();
}

$res = $mysql->query("SELECT * from " . $cfg["table"]["replaces"]);
if($res === false)
{
    senddata("<p><b>There are no replaces.</b></p>");
    exit();
}

$data = "<center><h3>YaCE 3 - Smilie list</h3>
<TABLE style=\"FONT-SIZE: 10pt; FONT-FAMILY: Verdana; BORDER-COLLAPSE: collapse\" borderColor=\"#AAAAAA\" cellSpacing=\"0\" cellPadding=\"0\" width=\"98%\" border=\"1\">
<tr><td><b>Pattern</b></td><td><b>Replace</b></td></tr>";
while($row = mysql_fetch_array($res))
{
    $data .= "<tr><td width=\"400\">" . $row[0] . "</td><td width=\"400\">" . $row[1] . "</td></tr>\r\n";
}

$data .= "</table></center>";

senddata($data);
?>