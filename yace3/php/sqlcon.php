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
    show_source("sqlcon.php");
    return;
}

class mysql
{
    var $con;

    var $host;
    var $user;
    var $pass;
    var $db;
    
    var $cache = array();

    function mysql($host, $user, $pass, $db)
    {
        $this->host = $host;
        $this->user = $user;
        $this->pass = $pass;
        $this->db   = $db;
    }

    function connect()
    {
        $try = 1;
        while(!($this->con = @mysql_connect($this->host, $this->user, $this->pass)))
        {
            if(++$try >= 3)
                return false;
        }
        if(!mysql_select_db($this->db, $this->con))
            return false;
        return true;
    }
    
    function disconnect()
    {
        if($this->con)
            mysql_close($this->con);
        else
            unset($this->con);
        return true;
    }

    function query($query)
    {
        $res = mysql_query($query, $this->con);
        if($res === false)
            return false;
        if(substr($query, 0, 7) == "SELECT ")
        {
            if(mysql_num_rows($res) === 0)
            {
                mysql_free_result($res);
                return false;
            }
        }
        elseif(substr($query, 0, 7) == "DELETE " OR substr($query, 0, 7) == "INSERT " OR substr($query, 0, 8) == "REPLACE " OR substr($query, 0, 7) == "UPDATE")
        {
            if(mysql_affected_rows($this->con) === 0)
                return false;
        }
        $this->cache[] = $res;
        return $res;
    }
    
    function unbuffered_query($query)
    {
        $res = mysql_unbuffered_query($query, $this->con);
        if($res === false)
            return false;
        $this->cache[] = $res;
        return $res;
    }
    
    function free()
    {
        foreach($this->cache as $res)
        {
            if(is_resource($res))
                mysql_free_result($res);
            else
                unset($res);
        }
        $this->cache = array();
        return true;
    }
    
    function select($query)
    {
        if(substr($query, 0, 7) != "SELECT " AND substr($query, 0, 8) != "EXPLAIN " AND substr($query, 0, 5) != "SHOW " AND substr($query, 0, 9) != "DESCRIBE ")
            return false;

        $res = $this->query($query);
        if($res === false)
            return false;

        $toret = array();
        $numrow = 0;
        while($row = mysql_fetch_array($res, MYSQL_ASSOC))
        {
            foreach($row as $key => $value)
            {
                $toret[$numrow][$key] = $value;
            }
            ++$numrow;
        }
        
        return $toret;
    }

    function selectone($query)
    {
        if(substr($query, 0, 7) != "SELECT " AND substr($query, 0, 8) != "EXPLAIN " AND substr($query, 0, 5) != "SHOW " AND substr($query, 0, 9) != "DESCRIBE ")
            return false;
        
        $res = $this->query($query);
        if($res === false)
            return false;

        return mysql_fetch_array($res, MYSQL_ASSOC);
    }
}
?>