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
    show_source("session.php");
    return;
}

class session
{
    var $mysql;
    var $cfg;
    var $newversion;
    var $cookiename = "yace3-usercp";
    var $getname    = "s";
    var $started;

    var $vars;
    var $id;
    var $name;

    function session($mysql, $cfg)
    {
        $this->started      = false;
        $this->cfg          = $cfg;
        $this->mysql        = &$mysql;
        $this->newversion   = intval(str_replace(".", "", phpversion())) > 406;
        $this->mysql->query("DELETE FROM `" . $this->cfg["table"]["sessions"] . "`
                             WHERE
                             `" . $this->cfg["column"]["sessions"]["time"] . "` < (" . time(). " - 3600)");
    }

    function session_start($name = "")
    {
        global $HTTP_COOKIE_VARS;
        $id = "";
        if(!$this->newversion)
        {
            if(!empty($HTTP_COOKIE_VARS[$this->cookiename]))
                $id = $HTTP_COOKIE_VARS[$this->cookiename];
            elseif(!empty($HTTP_GET_VARS[$this->getname]))
                $id = $HTTP_GET_VARS[$this->getname];
        }
        else
        {
            if(!empty($_COOKIE[$this->cookiename]))
                $id = $_COOKIE[$this->cookiename];
            elseif(!empty($_GET[$this->getname]))
                $id = $_GET[$this->getname];
        }

        if(empty($id))
        {
            if(empty($name))
                return false;
            $id = $this->new_session($name);
            $this->vars = $this->reactivate_session($id);
        }
        else
            if(!($this->vars = $this->reactivate_session($id)))
            {
                if(empty($name))
                    return false;
                $id = $this->new_session($name);
                $this->vars = $this->reactivate_session($id);
            }

        if(!empty($name))
            $this->name = $name;

        if(!$this->check_vars())
        {
            $this->name = "";
            $this->id   = "";
            $this->vars = false;
            return false;
        }

        $this->started = true;
        return true;
    }
    
    function new_session($name)
    {
        global $HTTP_SERVER_VARS;
        $id = md5(microtime() . chr(rand(32, 128)) . chr(rand(32, 128)));
        $useragent = ($this->newversion) ? $_SERVER["HTTP_USER_AGENT"] : $HTTP_SERVER_VARS["HTTP_USER_AGENT"];
        $ip = ($this->newversion) ? $_SERVER["REMOTE_ADDR"] : $HTTP_SERVER_VARS["REMOTE_ADDR"];
        $vars = array("useragent" => $useragent, "ip" => $ip);
        $this->mysql->query("INSERT INTO `" . $this->cfg["table"]["sessions"] . "`
                             (
                               `" . $this->cfg["column"]["sessions"]["id"] . "`,
                               `" . $this->cfg["column"]["sessions"]["name"] . "`,
                               `" . $this->cfg["column"]["sessions"]["vars"] . "`,
                               `" . $this->cfg["column"]["sessions"]["time"] . "`
                             )
                             VALUES
                             (
                               '" . $id . "',
                               '" . $name . "',
                               '" . serialize($vars) . "',
                               '" . time() . "'
                             )
                             ");
        setcookie($this->cookiename, $id, time() + 3600);
        $this->id = $id;
        return $id;
    }
    
    function reactivate_session($id)
    {
        if($res = $this->mysql->selectone("SELECT * from `" . $this->cfg["table"]["sessions"] . "`
                                        WHERE
                                        `" . $this->cfg["column"]["sessions"]["id"] . "`='" . $id . "'
                                        "))
        {
            $this->id = $id;
            $this->name = $res[$this->cfg["column"]["sessions"]["name"]];
            return unserialize($res[$this->cfg["column"]["sessions"]["vars"]]);
        }
        return false;
    }
    
    function session_destroy($id = "")
    {
        if($this->started AND empty($id))
            $id = $this->id;
        
        if(!$this->started AND empty($id))
            return false;

        if($this->mysql->query("DELETE FROM `" . $this->cfg["table"]["sessions"] . "`
                               WHERE
                               `" . $this->cfg["column"]["sessions"]["id"] . "`='" . $id . "'
                               LIMIT 1
                               "))
        {
            $this->vars = false;
            $this->started = false;
            $this->name = "";
            $this->id   = "";
            setcookie($this->cookiename, "", time() - 3600);
            return true;
        }

        return false;
    }
    
    function check_vars()
    {
        global $HTTP_SERVER_VARS;
        if(!is_array($this->vars))
            return false;

        if(empty($this->name)
           OR
           empty($this->id)
           OR
           $this->vars["useragent"] != (($this->newversion) ? $_SERVER["HTTP_USER_AGENT"]   : $HTTP_SERVER_VARS["HTTP_USER_AGENT"])
           OR
           $this->vars["ip"]        != (($this->newversion) ? $_SERVER["REMOTE_ADDR"]       : $HTTP_SERVER_VARS["REMOTE_ADDR"])
           )
            return false;
        return true;
    }
    
    function getName()
    {
        return $this->name;
    }
    
    function getID()
    {
        return $this->id;
    }
    
    function getVar($key)
    {
        if(!is_array($this->vars))
            return false;

        return $this->vars[$key];
    }

    function addVar($key, $value)
    {
        if(!is_array($this->vars))
            return false;

        $this->vars[$key] = $value;
        return true;
    }
    
    function delVar($key)
    {
        if(!is_array($this->vars))
            return false;

        unset($this->vars[$key]);
        return true;
    }

    function saveVars()
    {
        if(!is_array($this->vars) OR empty($this->id))
            return false;
        return $this->mysql->query("UPDATE `" . $this->cfg["table"]["sessions"] . "`
                                    SET
                                    `" . $this->cfg["column"]["sessions"]["vars"] . "` = '" . serialize($this->vars) . "'
                                    WHERE
                                    `" . $this->cfg["column"]["sessions"]["id"] . "` = '" . $this->id . "'
                                    LIMIT 1
                                    ");
    }
}
?>