<html>
<head><title>Add Director/Movie Relation</title></head>
<body>
<p>
Search a movie: <br/>
<form method="POST">
<input type="text" name="smovie"/>
<input type="hidden" name="but" value="mov"/>
<input type="submit" value="Search Movies"/>
</form> 
</p>
<p>
Search a director: <br/>
<form method="POST">
<input type="text" name="sdirector"/>
<input type="hidden" name="but" value="director"/>
<input type="submit" value="Search Director"/>
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
	echo "Selected Director: ".$first. " " .$last;
	echo "<br>";
}

if ($_GET["mid"] && $_GET["did"]) { 

	?>

<form method = "POST">
<p>
<input type="hidden" name="select" value="addDirectorToMovie"/>
<input type="submit" value="Add Director to Movie!"/></p>

</form>
<?php }  ?>
<?php
if ($_POST["select"] == "addDirectorToMovie"){
	$mid = $_GET["mid"];
	$did = $_GET["did"];

	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}
	
	$relationQuery = "INSERT IGNORE INTO MovieDirector VALUES ($mid, $did)";

	if (mysql_query($relationQuery, $db_connection)){
		echo "<p>Success: Added Director to Movie!";
		echo "<br/><a href='./movie.php?id=$mid'>View new relation</a></p>";
	}
	else { 
		echo "Failed Adding Director to Movie.";
	}

	mysql_close($db_connection);
}
?>


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
			
			if ($_GET["did"]){
				$did = $_GET["did"];
				$first = $_GET["first"];
				$last = $_GET["last"];
				echo "<a href = './addDirectorMovie.php?did=$did&mid=$r[1]&title=$r[0]&first=$first&last=$last'>";
			} else 
				echo "<a href = 'addDirectorMovie.php?mid=$r[1]&title=$r[0]'>";
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
<!-- Handle when User searches for Director -->
<?php
if($_POST["sdirector"] && ($_POST["but"] == "direct")){
	
	$searchDirectorRaw = $_POST["sdirector"];
	$formattedSearch = preg_replace('/\s+/', ' ', $searchDirectorRaw); 
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
	
	$directorQuery= "SELECT DISTINCT first, last, id FROM Director WHERE first LIKE '%$searchInput[0]%'";

	foreach($searchInput as $t){
		$directorQuery .= " OR first LIKE '%$t%' OR last LIKE '%$t%'";
	}
	$directorQuery .= " ORDER BY first, last";

	$result = mysql_query($directorQuery, $db_connection);

	if($result && mysql_num_rows($result) > 0){
		echo "<br/><b>List of Directors: </b><p>";
		echo "<div style=\"border:5px;width:750px;height:500px;overflow:auto;background-color:#FCFADD;overflow-y:scroll;text-align:left\" ><p>";
				
		while($r = mysql_fetch_row($result)){
			
			if ($_GET["mid"]) {
				$mid = $_GET["mid"];
				$title = $_GET["title"];
				echo "<a href = 'addDirectorMovie.php?mid=$mid&did=$r[2]&first=$r[0]&last=$r[1]&title=$title'>";
			}
			else
				echo "<a href = 'addDirectorMovie.php?did=$r[2]&first=$r[0]&last=$r[1]'>";
			echo "$r[0] $r[1]</a><br/>";
		}
		echo "</p></div>";
	}
	else {
		echo "<br/><b>Search term: \"$searchDirectorRaw\" did not find anything.</b>";
	}
	mysql_close($db_connection);
}
?>


</body>

</html>

</body>
</html>