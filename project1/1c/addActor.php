<!--
	<form action="./addActor.php" method="GET">
			Identity:	<input type="radio" name="identity" value="Actor" checked="true">Actor
						<input type="radio" name="identity" value="Director">Director<br/>
			
			First Name:	<input type="text" name="first" maxlength="20"><br/>
			Last Name:	<input type="text" name="last" maxlength="20"><br/>
			Sex:		<input type="radio" name="sex" value="Male" checked="true">Male
						<input type="radio" name="sex" value="Female">Female<br/>
						
			Date of Birth:	<input type="text" name="dob"><br/>
			Date of Death:	<input type="text" name="dod"> (Leave empty if still alive now)<br/>
			<input type="submit" value="Add Person"/>
		</form>
-->
<html>
<head>
	<title>Add Actor or Director</title>
</head>

<p>
	<form method="post">
	
	<h3>Add a new actor or director: </h3>
	<p><b>First Name: </b><input type="text" name="first" maxlength="20"/><br/></p>
	<p><b>Last Name: </b><input type="text" name="last" maxlength="20" /><br/></p>
	<p><b>Gender: </b><input type="radio" name="sex" value="Male" checked/> Male 
	<input type="radio" name="sex" value="Female"/> Female</p>
	<p><b>Actor or Director: </b>
	<input type="radio" name="job" value="actor" checked/> Actor
	<input type="radio" name="job" value="director"/> Director </p>
	<p><b>Date of Birth (Month, Day, Year): </b>
	<select name="dobm"><option value="0"></option>
	<?php 
	 	$month = array( '01' => 'January', '02' => 'February', '03' => 'March', '04' => 'April',
	 					'05' => 'May', '06' => 'June', '07' => 'July', '08' => 'August', 
	 					'09' => 'September', '10' => 'October', '11' => 'November', '12' => 'December');

		// for each month, add to the option list
		foreach ($month as $k => $m) echo "<option value=\"$k\">$m</option>"; 
	 
	 ?>

	 </select>
	 <select name="dobd"><option value="0"></option>
	 <?php for ($x=01;$x<32;$x++) echo "<option value=\"$x\">$x</option>"; ?></select>
	 <select name="doby"><option value="0"></option>
	 <?php for ($x=2012;$x>1900;$x--) echo "<option value=\"$x\">$x</option>"; ?></select>
	</p>

	<p><b>Date of Death (Month, Day, Year): </b>
	 <select name="dodm"><option value="0"></option>
	 <?php 	// for each month, add to the option list
		foreach ($month as $k => $m) echo "<option value=\"$k\">$m</option>";	?></select>
	 <select name="dodd"><option value="0"></option>
	 <?php for ($x=01;$x<32;$x++) echo "<option value=\"$x\">$x</option>"; ?></select>
	 <select name="dody"><option value="0"></option>
	 <?php for ($x=2012;$x>1900;$x--) echo "<option value=\"$x\">$x</option>"; ?></select>
	Leave blank if still alive. 
	</p>
	<p><input type="submit" value="Add Person" name="clicked"/></p>
	</form>
</p>

<?php
	
	if($_POST["first"] && $_POST["last"] && $_POST["doby"] && $_POST["dobm"] && $_POST["dobd"]) {

	$firstraw = $_POST["first"];
	$lastraw = $_POST["last"];
	$sex = $_POST["sex"];
	$job = $_POST["job"];

	//DOB
	$dobm = $_POST["dobm"];
	$dobd = $_POST["dobd"];
	$doby = $_POST["doby"];
	$dob = "$doby-$dobm-$dobd"; 

	//DOD
	$dodm = $_POST["dodm"];
	$dodd = $_POST["dodd"];
	$dody = $_POST["dody"];
	$dod = "$dody-$dodm-$dodd";
	$isDead = 0;

	$isSuccessful = 0;

	if($dodm > 0 && $dodd > 0 && $dody > 1899)
		$isDead = 1;

	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}

	$first = mysql_real_escape_string($firstraw);
	$last = mysql_real_escape_string($lastraw);

	$pidquery = "SELECT id FROM MaxPersonID";
	$pidsearch = mysql_query($pidquery, $db_connection);
	$pidfinished = mysql_fetch_row($pidsearch);
	$pid = $pidfinished[0];

	$insertActor = "";
	$insertDirector = "";
	if ($isDead) 
		$insertActor = "INSERT INTO Actor VALUES ($pid, '$last', '$first', '$sex', '$dob', '$dod')";
	else
		$insertActor =  "INSERT INTO Actor VALUES ($pid, '$last', '$first', '$sex', '$dob', NULL)";

	if($isDead)
		$insertDirector = "INSERT INTO Director VALUES ($pid, '$last', '$first', '$sex', '$dob', '$dod')";
	else
		$insertDirector = "INSERT INTO Director VALUES ($pid, '$last', '$first', '$sex', '$dob', NULL)";

	if(strcmp($job, "actor") == 0) {
		if(mysql_query($insertActor, $db_connection)) {
			$isSuccessful = 1;
			echo "successful adding of Actor: $first $last!";
			echo "View the profile at <a href='./actor.php?id=$pid'>here</a><br/>";
		}
		else
			echo "unsuccessful addition of Actor";
	}
	if(strcmp($job, "director") == 0) {
		if(mysql_query($insertDirector, $db_connection)) {
			$isSuccessful = 1;
			echo "successful adding of Director: $first $last!";
			//echo "View the profile at <a href='./actor.php?id=$pid'>here</a><br/>";
		}
		else
			echo "unsuccessful addition of director";
	}
	if($isSuccessful)
			mysql_query("UPDATE MaxPersonID SET id=id+1", $db_connection);

	mysql_close($db_connection);
}

?>

</html>