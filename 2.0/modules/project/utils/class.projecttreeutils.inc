<?php

class projecttreeutils
{

  /**
   * Original node type
   *
   * @var atkNodeType
   */
  protected $originalType;

  /**
   * New node type
   *
   * @var atkNodeType
   */
  protected $newType;

  /**
   * Original project or package id
   *
   * @var integer
   */
  protected $originalId;

  /**
   * New  project or package id
   *
   * @var integer
   */
  protected $newId;


  /**
   * Constructor.
   *
   * @param atkNodeType $type
   * @param integer $original
   * @param integer $new
   */
  public function __construct( $originalType = 'project.package', $originalId = 0, $newType = 'project.package', $newId = 0 )
  {
    $this->originalType = $originalType;
    $this->newType = $newType;
    $this->originalId = $originalId;
    $this->newId = $newId;
  }


  /**
   * Update the tree through the bottom-up design.
   *
   */
  public function updateTree($projectid)
  {
    $db = atkGetDb();
    $securitymanager = &atkGetSecurityManager();
    $initialplanningallowed = $securitymanager->allowed( "project.project", "initialplanning" );

    //update project
    $this->updateProject($projectid, $db, $initialplanningallowed);

    // Recalculate original tree
    $this->calculateHours( $this->originalType, $this->originalId, $db, $initialplanningallowed );

    // Recalculate new tree
    if( $this->newId != 0 )
    {
      $this->calculateHours( $this->newType, $this->newId, $db, $initialplanningallowed );
    }
  }


  /**
   * Calculate the spend hours for packages and projects
   *
   * @param array $record
   */
  protected function calculateHours( $type, $id, $db, $initialplanningallowed)
  {
    if ($type == 'project.package')
    {
      $this->updatePackage($id, $db, $initialplanningallowed);

      list($res) = $db->getrows("SELECT master, master_id FROM package WHERE id='$id'");
      $this->calculateHours($res['master'], $res['master_id'], $db, $initialplanningallowed);
    }
  }

  /**
   * Update the information in packages.
   *
   * Calculate the total spend minutes/hours, current planning hours
   * and remaining effort time hours from underlying packages and phases and
   * update the information in the parent package.
   *
   * @param integer $package_id
   * @param atkDb $db
   */
  protected function updatePackage( $package_id, $db, $initialplanningallowed )
  {
    $select = "
        SELECT"
          .($initialplanningallowed ?
            " (IFNULL((SELECT SUM(initial_planning) FROM phase WHERE master='project.package' AND master_id='$package_id'),0)
             + IFNULL((SELECT SUM(initial_planning) FROM package WHERE master='project.package' AND master_id='$package_id'),0)) AS initial_planning," : "")."

          (IFNULL((SELECT SUM(time) AS spend_minutes FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE master='project.package' AND master_id='$package_id')),0)
           + IFNULL(SUM(p.spend_hours),0)) AS hours,

          (IFNULL((SELECT SUM(current_planning) FROM phase WHERE master='project.package' AND master_id='$package_id'),0)
          +IFNULL((SELECT SUM(current_planning) FROM package WHERE master='project.package' AND master_id='$package_id'),0)) AS current_planning,

          (IFNULL((SELECT SUM(remaining_effort_time) FROM phase WHERE master='project.package' AND master_id='$package_id'),0)
          +IFNULL((SELECT SUM(remaining_effort_time) FROM package WHERE master='project.package' AND master_id='$package_id'),0)) AS remaining_effort_time
        FROM
          package p
        WHERE
          (p.master='project.package' AND p.master_id='$package_id')";

    list($h) = $db->getrows( $select );

    $completed = $this->calculateCompleted( $h['hours'], $h['current_planning'], $h['remaining_effort_time'] );
    //get project trafficlight_updatestyle
    list($style) = $db->getrows("SELECT trafficlight_updatestyle FROM package WHERE id='$package_id'");

    $update = " UPDATE
                  package
                SET"
                .($initialplanningallowed ? " initial_planning= '".$h['initial_planning']."'," : "")."
                  spend_hours='".$h['hours']."',
                  current_planning='".$h['current_planning']."',
                  completed='$completed',
                  remaining_effort_time='".$h['remaining_effort_time']."'"
                 .(($style['trafficlight_updatestyle'] == 'A') ? ",trafficlight='$completed'" : "")."
                WHERE
                  id='$package_id'";

    $db->query( $update );
  }


  /**
   * Update the information in projects
   *
   * Calculate the total spend minutes/hours, current planning hours
   * and remaining effort time hours from underlying packages and phases and
   * update the information in the parent project.
   *
   * @param integer $project_id
   * @param atkDb $db
   */
  protected function updateProject( $project_id, $db, $initialplanningallowed )
  {
    list($h) = $db->getrows("
        SELECT"
          .($initialplanningallowed ? " SUM(initial_planning) AS initial_planning," : "")."
          SUM(current_planning) AS current_planning,
          SUM(remaining_effort_time) AS remaining_effort_time,
          (SELECT SUM(time) FROM hours WHERE phaseid IN (SELECT id FROM phase WHERE projectid='$project_id')) AS hours
        FROM
          phase
        WHERE
          projectid='$project_id'");

    $completed = $this->calculateCompleted( $h['hours'], $h['current_planning'], $h['remaining_effort_time'] );
    //get project trafficlight_updatestyle
    list($style) = $db->getrows("SELECT trafficlight_updatestyle FROM project WHERE id='$project_id'");
    
    $db->query("UPDATE
                  project
                SET"
                  .($initialplanningallowed ? " initial_planning= '".$h['initial_planning']."'," : "")."
                  spend_hours='".$h['hours']."',
                  current_planning='".$h['current_planning']."',
                  completed='$completed',
                  remaining_effort_time='".$h['remaining_effort_time']."'"
                  .(($style['trafficlight_updatestyle'] == 'A') ? ",trafficlight='$completed'" : "")."
                WHERE id='$project_id'");
  }

  /**
   * Calculate the completed value.
   *
   * @param integer $hours
   * @param integer $planning
   * @param integer $left
   * @return integer
   */
  protected function calculateCompleted( $hours, $planning, $left )
  {
    //If we don't start register time for project, completed =0
    if(empty($hours)) return 0;
    elseif( $left <= 0 ) return 100;

    return (round( (( $hours / ($left +$hours)) * 100), 0 ));
  }

  public function updateRemainTime($phase_id, $time)
  {
    //Is this a support task?
    $node = atkGetNode("project.phase"); 
    if($node->is_support_task($phase_id))
    {
      //remaining effort time = current planning - booked time
      atkGetDb()->query("
        UPDATE phase 
        SET 
          remaining_effort_time=current_planning - (SELECT sum(time) FROM hours WHERE phaseid='$phase_id'),
          completed = (SELECT sum(time) FROM hours WHERE phaseid='$phase_id')/current_planning*100
        WHERE id='$phase_id'");
    }
    else 
    {
      atkGetDb()->query("
        UPDATE phase 
        SET 
          remaining_effort_time='$time',
          current_planning=
            (SELECT sum(time) FROM hours WHERE phaseid='$phase_id')+'$time',
          completed = (SELECT sum(time) FROM hours WHERE phaseid='$phase_id')/((SELECT sum(time) FROM hours WHERE phaseid='$phase_id')+'$time')*100
        WHERE id='$phase_id'");
    }
  }    
  
  public function updateTrafficLight($phase_id)
  {
    atkGetDb()->query("UPDATE phase SET trafficlight=completed WHERE id='$phase_id' AND trafficlight_updatestyle='A'");
  }
  

}