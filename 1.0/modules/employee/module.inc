<?php

  class mod_employee extends achievoModule
  {
    function getNodes()
    {
      registerNode("employee.profile", array("admin", "add", "edit", "delete" , "grantall"));
      registerNode("employee.employee", array("admin", "add", "edit", "delete", "stats", "view_all"));
      registerNode("employee.usercontracts", array("admin", "add", "edit", "delete"));
      registerNode("employee.userprefs", array("edit"));
      registerNode("employee.functionlevel", array("admin", "add", "edit", "delete"));
      registerNode("employee.department", array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      menuitem("employeemanagement",dispatch_url("employee.employee", "admin"),"main",array("employee.employee", "admin"),600);
      menuitem("profile_admin", dispatch_url("employee.profile", "admin"), "employeemanagement", array("employee.profile", "admin"));
      menuitem("employee_admin", dispatch_url("employee.employee", "admin"), "employeemanagement", array("employee.employee", "admin"));
      menuitem("employee_stats", dispatch_url("employee.employeestats", "stats"), "employeemanagement", array("employee.employee", "stats"));
      menuitem("functionlevel_admin", dispatch_url("employee.functionlevel", "admin"), "employeemanagement", array("employee.functionlevel", "admin"));
      
      menuitem("-","","main",true,620);
    }
    
    function getDependencies($mode)
    {
      return array("person");
    }
  }

?>