<html>
<head><title>Add Movie/Actor Relation</title></head>
<body>
<a href="./home.html">Home</a>
<p>
Search a movie: <br/>
<form method="POST">
<input type="text" name="smovie"/>
<input type="hidden" name="but" value="mov"/>
<input type="submit" value="Search Movies"/>
</form> 
</p>
<p>
Search an actor: <br/>
<form method="POST">
<input type="text" name="sactor"/>
<input type="hidden" name="but" value="act"/>
<input type="submit" value="Search Actors"/>
</form>
</p>

<?php
if($_GET["title"]) {
	$title = $_GET["title"];
	echo "Selected Movie: ". $title;
	echo "<br>";
}
if($_GET["first"] && $_GET["last"]) {
	$first = $_GET["first"];
	$last = $_GET["last"];
	echo "Selected Actor: ".$first. " " .$last;
	echo "<br>";
}
	
// Handle case of both Movie and Actor Selected
if ($_GET["mid"] && $_GET["aid"]) { 

	?>

<form method = "POST">
<p>Role: <input type="text" name="role" maxlength="50"/></p>

<p>
<input type="hidden" name="select" value="addActorToMovie"/>
<input type="submit" value="Add Movie to Actor!"/></p>

</form>
<?php }  ?>
<?php
if ($_POST["select"] == "addActorToMovie" && $_POST["role"]){
	$mid = $_GET["mid"];
	$aid = $_GET["aid"];
	$rawRole = $_POST["role"];

	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}
	
	$role = mysql_real_escape_string($rawRole);

	$relationQuery = "INSERT IGNORE INTO MovieActor VALUES ($mid, $aid, '$role')";

	if (mysql_query($relationQuery, $db_connection)){
		echo "<p>Success: Added Actor to Movie!";
		echo "<br/><a href='./movie.php?id=$mid'>View new relation</a></p>";
	}
	else { 
		echo "Failed Adding Actor to Movie.";
	}

	mysql_close($db_connection);
}
?>
<!-- Handle Case where user clicks search Movie -->

<?php
if($_POST["smovie"] && ($_POST["but"] == "mov")) {
	$searchMovieRaw = $_POST["smovie"];
	$formattedSearch = preg_replace('/\s+/', ' ', $searchMovieRaw); 
	$words = explode(" ", $formattedSearch);
	
	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}


	$searchInput = array();
	foreach ($words as $w) {
		array_push($searchInput, mysql_real_escape_string($w));
	}
	
	$movieQuery = "SELECT title, id FROM Movie WHERE title LIKE '%$searchInput[0]%'";
	foreach($searchInput as $t){
		$movieQuery  .= " OR title LIKE '%$t%'";
	}
	
	$movieQuery .= " ORDER BY title";

	$result = mysql_query($movieQuery , $db_connection);

	if($result && mysql_num_rows($result) > 0){
		echo "<br/><p>List of Movies: </p><p>";
		echo "<div style=\"border:5px;width:750px;height:500px;overflow:auto;background-color:#FCFADD;overflow-y:scroll;text-align:left\" ><p>";
		while($r = mysql_fetch_row($result)) {
			
			if ($_GET["aid"]){
				$aid = $_GET["aid"];
				$first = $_GET["first"];
				$last = $_GET["last"];
				echo "<a href = './addMovieActor.php?aid=$aid&mid=$r[1]&title=$r[0]&first=$first&last=$last'>";
			} elseif ($_GET["did"]){
				$did = $_GET["did"];
				$first = $_GET["first"];
				$last = $_GET["last"];
				echo "<a href = './addMovieActor.php?did=$did&mid=$r[1]&title=$r[0]&first=$first&last=$last'>";
			} else 
				echo "<a href = 'addMovieActor.php?mid=$r[1]&title=$r[0]'>";
			echo "" .$r[0]. "</a><br/>";
		}
		echo "</p></div>";
	}
	else {
		echo "<br/><b>Search Term: \"$searchMovieRaw\" did not find anything.</b>";
	}
	mysql_close($db_connection);
}
?>
<!-- Handle when User searches for Actor -->
<?php
if($_POST["sactor"] && ($_POST["but"] == "act")){
	
	$searchActorRaw = $_POST["sactor"];
	$formattedSearch = preg_replace('/\s+/', ' ', $searchActorRaw); 
	$words = explode(" ", $formattedSearch);

	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}
	$searchInput = array();
	foreach ($words as $w){
		array_push($searchInput, mysql_real_escape_string($w));
	}
	
	$actorQuery= "SELECT DISTINCT first, last, id FROM Actor WHERE first LIKE '%$searchInput[0]%'";

	foreach($searchInput as $t){
		$actorQuery .= " OR first LIKE '%$t%' OR last LIKE '%$t%'";
	}
	$actorQuery .= " ORDER BY first, last";

	$result = mysql_query($actorQuery, $db_connection);

	if($result && mysql_num_rows($result) > 0){
		echo "<br/><b>List of Actors: </b><p>";
		echo "<div style=\"border:5px;width:750px;height:500px;overflow:auto;background-color:#FCFADD;overflow-y:scroll;text-align:left\" ><p>";
				
		while($r = mysql_fetch_row($result)){
			
			if ($_GET["mid"]) {
				$mid = $_GET["mid"];
				$title = $_GET["title"];
				echo "<a href = 'addMovieActor.php?mid=$mid&aid=$r[2]&first=$r[0]&last=$r[1]&title=$title'>";
			}
			else
				echo "<a href = 'addMovieActor.php?aid=$r[2]&first=$r[0]&last=$r[1]'>";
			echo "$r[0] $r[1]</a><br/>";
		}
		echo "</p></div>";
	}
	else {
		echo "<br/><b>Search term: \"$searchActorRaw\" did not find anything.</b>";
	}
	mysql_close($db_connection);
}
?>


</body>

</html>