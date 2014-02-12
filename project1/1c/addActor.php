<html>
<head>
	<title>Add Actor or Director</title>
</head>

<p>
	<form method="post">
	
	<h3>Add a new actor or director: Asterisks means required field </h3>
	<p><b>First Name*: </b><input type="text" name="first" maxlength="20"/><br/></p>
	<p><b>Last Name*: </b><input type="text" name="last" maxlength="20" /><br/></p>
	<p><b>Gender*: </b><input type="radio" name="sex" value="Male" checked/> Male 
	<input type="radio" name="sex" value="Female"/> Female</p>
	<p><b>Actor or Director: </b>
	<input type="radio" name="job" value="actor" checked/> Actor
	<input type="radio" name="job" value="director"/> Director </p>
	<p><b>Date of Birth (Month, Day, Year)*: </b>
	<select name="dobm"><option value="0"></option>
	<?php 
	 	$month = array( '01' => 'January', '02' => 'February', '03' => 'March',
	 					'04' => 'April', '05' => 'May', '06' => 'June',
	 					'07' => 'July', '08' => 'August', '09' => 'September',
 						'10' => 'October', '11' => 'November', '12' => 'December');
		foreach ($month as $num => $m) echo "<option value=\"$num\">$m</option>"; 
	 ?>

	 </select>
	 <select name="dobd"><option value="0"></option>
	 <?php for ($i=01;$i<32;$i++) echo "<option value=\"$i\">$i</option>"; ?></select>
	 <select name="doby"><option value="0"></option>
	 <?php for ($i=2014;$i>1800;$i--) echo "<option value=\"$i\">$i</option>"; ?></select>
	</p>

	<p><b>Date of Death (Month, Day, Year): </b>
	 <select name="dodm"><option value="0"></option>
	 <?php
		foreach ($month as $num => $m) echo "<option value=\"$num\">$m</option>";	?></select>
	 <select name="dodd"><option value="0"></option>
	 <?php for ($i=01;$i<32;$i++) echo "<option value=\"$i\">$i</option>"; ?></select>
	 <select name="dody"><option value="0"></option>
	 <?php for ($i=2014;$i>1800;$i--) echo "<option value=\"$i\">$i</option>"; ?></select>
	Leave blank if still alive. 
	</p>
	<p><input type="submit" value="Add Person" name="submitted"/></p>
	</form>
</p>

<?php
	
	if($_POST["submitted"] && $_POST["first"] && $_POST["last"] && $_POST["doby"] && $_POST["dobm"] && $_POST["dobd"]) {

	$firstraw = $_POST["first"];
	$lastraw = $_POST["last"];
	$sex = $_POST["sex"];
	$job = $_POST["job"];

	$dobm = $_POST["dobm"];
	$dobd = $_POST["dobd"];
	$doby = $_POST["doby"];
	$dob = "$doby-$dobm-$dobd"; 

	$dodm = $_POST["dodm"];
	$dodd = $_POST["dodd"];
	$dody = $_POST["dody"];
	$dod = "$dody-$dodm-$dodd";
	$isDead = 0;

	$isSuccessful = 0;

	if($dodm > 0 && $dodd > 0 && $dody > 0)
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
		$insertDirector = "INSERT INTO Director VALUES ($pid, '$last', '$first', '$dob', '$dod')";
	else
		$insertDirector = "INSERT INTO Director VALUES ($pid, '$last', '$first', '$dob', NULL)";

	if(strcmp($job, "actor") == 0) {
		if(mysql_query($insertActor, $db_connection)) {
			$isSuccessful = 1;
			echo "Successful adding of Actor: $first $last!";
			echo "<a href='./actor.php?id=$pid'>Click to view Actor Page here</a><br/>";
		}
		else
			echo "Unsuccessful addition of Actor.";
		
	}
	if(strcmp($job, "director") == 0) {
		if(mysql_query($insertDirector, $db_connection)) {
			$isSuccessful = 1;
			echo "Successful adding of Director: $first $last!";
		}
		else
			echo "Unsuccessful addition of Director.";
	}
	if($isSuccessful)
			mysql_query("UPDATE MaxPersonID SET id=id+1", $db_connection);

	mysql_close($db_connection);
}
elseif($_POST["submitted"]) {
	echo "All fields with astericks are required! Please fill in the necessary fields.";
}

?>

</html>