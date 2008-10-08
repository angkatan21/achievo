<table border="0" cellpadding="0" cellspacing="0" class="tabsTabs mainTabs">
  <tr>         
    <td id="tab_1" valign="middle" align="left" nowrap="nowrap" class="{if $activetab == 'add'}activetab{else}passivetab{/if}">  
      <a href="{$taburl.add}" title="{atktext id="timereg_advanced"}">{atktext id="timereg_advanced"}</a>
    </td>          
    <td>&nbsp;</td>
    <td id="tab_1" valign="middle" align="left" nowrap="nowrap" class="{if $activetab == 'favorites'}activetab{else}passivetab{/if}">  
      <a href="{$taburl.favorites}" title="{atktext id="timereg_favorites"}">{atktext id="timereg_favorites"}</a>
    </td>          
    <td>&nbsp;</td>
    <td id="tab_1" valign="middle" align="left" nowrap="nowrap" class="{if $activetab == 'times'}activetab{else}passivetab{/if}">  
      <a href="{$taburl.times}" title="{atktext id="timereg_hours"}">{atktext id="timereg_hours"}</a>
    </td>          
  </tr>
</table>


<table cellspacing="0" cellpadding="0" class="recordListContainer">
  <tr>
    <td>
    {if count($errors)}
      <div style="color: red;">
        <strong>{atktext id="error_formdataerror"}</strong><br/><br/>
		    {section name=i loop=$errors}
		    <u>{$errors[i].name}:</u><br/>
		    {foreach from=$errors[i].err item=err}
		      {$err}<br/>
		    {/foreach}
		    <br/>
        {/section}
		  </div>
		{/if}
    
      {$formstart}
    
      <table class="recordList" cellpadding="0" cellspacing="0">    
          
        <!-- header -->
        <tr>
          <th class="recordListThFirst"><b>{atktext id="project"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="phase"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="enddate"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="time"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="completed"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="comment"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="activity"}</b></th>
          <th class="recordListThFirst"><b>{atktext id="favorite"}</b></th>
        </tr>

        <!-- records -->

        {foreach from=$rows item=row}
         <tr id="r1_{$row.id}" class="row{if $row.rownum % 2 == 0 }1{else}2{/if}" onmouseover="highlightrow(this, '#eeeeee')" onmouseout="resetrow(this)">
           <td class="recordListTd">
             <a href="{$project_url}&atkselector=project.id%3D%27{$row.projectid.id}%27">{$row.projectid.name}</a>
             <input type="hidden" name="timereg[{$row.id}][projectid]" value="{$row.projectid.id}" />
             <input type="hidden" name="timereg[{$row.id}][projectname]" value="{$row.projectid.name}" />
           </td>
           <td class="recordListTd">
             <a href="{$phase_url}&atkselector=phase.id%3D%27{$row.id}%27&atkfilter=projectid%3D{$row.projectid.id}">{$row.name}</a>
             <input type="hidden" name="timereg[{$row.id}][name]" value="{$row.name}" />               
             <input type="hidden" name="timereg[{$row.id}][phaseid]" value="{$row.id}" />
           </td>
           <td class="recordListTd">
             {$row.enddate.day}-{$row.enddate.month}-{$row.enddate.year}
           </td>
           <td class="recordListTd">
             <select id="timereg[{$row.id}][time][hours]" name="timereg[{$row.id}][time][hours]">
             {section name=h loop=$times.hours}
               <option value="{$times.hours[h]}"{if $values.timereg[$row.id].time.hours == $times.hours[h]} selected="selected"{/if}>{$times.hours[h]} {atktext id="hours"}</option>
             {/section}
             </select> : 
             <select id="timereg[{$row.id}][time][minutes]" name="timereg[{$row.id}][time][minutes]">
             {section name=m loop=$times.minutes}
               <option value="{$times.minutes[m]}"{if $values.timereg[$row.id].time.minutes == $times.minutes[m]} selected="selected"{/if}>{$times.minutes[m]} {atktext id="minutes"}</option>
             {/section}
             </select>
             <img src="atk/themes/default/images/required_field.gif">
           </td>
           <td class="recordListTd">
             <select id="timereg[{$row.id}][completed]" name="timereg[{$row.id}][completed]">
              {section name=c loop=$completed}
               {if '' != $values.timereg[$row.id].completed}
                 {assign var="progress" value=$values.timereg[$row.id].completed}
               {else}
                 {assign var="progress" value=$row.completed}
               {/if}
               <option value="{$completed[c]}"{if $completed[c] == $progress} selected="selected"{/if}>{$completed[c]}%</option>
              {/section}
             </select>
           </td>
           <td class="recordListTd">
             <input type="text" size="30" id="timereg[{$row.id}][remark]" name="timereg[{$row.id}][remark]" value="{$values.timereg[$row.id].remark}" />
             {if $row.comment_obligatory == 1} <img src="atk/themes/default/images/required_field.gif">{/if}
           </td>
           <td class="recordListTd">
            {if $row.activities|@count == 0}
             &nbsp;
            {else}
             <select id="timereg[{$row.id}][activity]" name="timereg[{$row.id}][activity]">
             {foreach from=$row.activities item=activity}
               <option value="{$activity.activityid.id}"{if $values.timereg[$row.id].activity == $activity.activityid.id} selected="selected"{/if}>{$activity.activityid.name}</option>
             {/foreach}
             </select>
            {/if}
           </td>
           <td class="recordListTdFirst">
             {if $activetab == 'add'}
               {if $row.is_favorite == 0}
                 <a href="javascript:void(0)" onclick="Timereg_Advanced.addFavorite('{$row.add_url}',{$row.id}); return false;"><img src="{$addIcon}" alt="" style="border:0;" id="addicon_{$row.id}" /></a>
               {else}
                 &nbsp;
               {/if}
             {elseif $activetab == 'favorites'}
               <a href="javascript:void(0)" onclick="Timereg_Advanced.removeFavorite('{$row.delete_url}',{$row.id}); return false;"><img src="{$removeIcon}" alt="" style="border:0;" /></a>
             {/if}
           </td>
        </tr>
        {/foreach}

      </table>
      
      <br/><br/>
      
			<div id="action-buttons">
	      {foreach from=$buttons item=button}
	        &nbsp;{$button}&nbsp;
	      {/foreach}
			</div>
      
      {$formend}
    </td>
  </tr>
  
</table>

