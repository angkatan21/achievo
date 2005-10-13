<?php
  /**
   * Unittest and webtest file
   *
   * Runs all unittests and webtests within the achievo folder and it's
   * subfolders. In advance to using this file, the config variables
   * whithin this file must be adjusted.
   *
   * @package achievo
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html  GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */  
  
  // Test configuration
  $config_atkroot = "./";                                              // Path in which achievo exists
  $config_achievourl = "http://nikita/zandbak/guido/achievo_current/"; // URL wherein achievo can be reached through http from the php interpretor
  $config_achievousername = "webtest";                                 // User account used for webtesting
  $config_achievopassword = "webtest";                                 // Password for the webtesting account
  
  // Include ATK library and Achievo extended web tester (features loginAchievo function)
  include_once($config_atkroot . "atk.inc");
  atkimport("test.achievowebtestcase");

  // Require ATK authentication if not running in text mode
  if(!$_SERVER['PWD'])
  {
    atksession();
    atksecure();
  }

  // Let the atktestsuite run all test files having a name starting with "class.test_"
  $suite = &atknew("atk.test.atktestsuite");
  $suite->run((!$_SERVER['PWD']) ? "html" : "text");
?>