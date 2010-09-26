<?php

  class mod_todo extends achievoModule
  {
    function getNodes()
    {
      registerNode("todo.todo", array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      $selectedproject = array_key_exists("selectedproject", $_REQUEST) ? $_REQUEST["selectedproject"] : 0;
      if ($selectedproject == 0)
      {
        $this->menuitem("project_todoview", dispatch_url("todo.project_todoview", "admin"), "projectmanagement", array("todo.todo", "admin"));
      }
      else
      {
        $this->menuitem("project_todoview", dispatch_url("todo.project_todoview", "edit", array("atkselector" => "project.id=".$selectedproject)), "projectmanagement", array("todo.todo", "admin"));
      }
    }

    function getDependencies($mode)
    {
      return array("project");
    }


    function getPimItems()
    {
      return array("todos"=>"getTodo");
    }

    function getTodo()
    {
      include_once(moduleDir("todo")."pim.inc");
      return pimTodos();
    }
  }

?>