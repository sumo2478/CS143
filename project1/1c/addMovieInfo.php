<html>
<head>
	<title>Add Movie Information</title>
</head>
<a href="./home.html">Home</a>
<form method="post">
<h3>Add a new movie: </h3>
<p><b>Title: </b><input type="text" name="title" maxlength="20"/><br/></p>
<p><b>Year (YYYY): </b><input type="text" name="year" maxlength="20"/><br/></p>
<p><b>Company: </b><input type="text" name="company" maxlength="20"/><br/></p>
<!--<p><b>Director: </b><input type="text" name="director" maxlength="20"/><br/></p> -->
<p>MPAA Rating: 
<select name="MPAA">
<option value="G">G</option>
<option value="PG">PG</option>
<option value="PG-13">PG-13</option>
<option value="R">R</option>
<option value="NC-17">NC-17</option>
</select></p><br/>

<p><b>Genre (Select multiple if applicable): </b><br>
<input type="checkbox" name="genre[]" value="Drama"/> Drama
<input type="checkbox" name="genre[]" value="Comedy"/> Comedy
<input type="checkbox" name="genre[]" value="Romance"/> Romance
<input type="checkbox" name="genre[]" value="Crime"/> Crime
<input type="checkbox" name="genre[]" value="Horror"/> Horror
<br>
<input type="checkbox" name="genre[]" value="Mystery"/> Mystery
<input type="checkbox" name="genre[]" value="Thriller"/> Thriller
<input type="checkbox" name="genre[]" value="Action"/> Action
<input type="checkbox" name="genre[]" value="Adventure"/> Adventure
<input type="checkbox" name="genre[]" value="Fantasy"/> Fantasy
<br>
<input type="checkbox" name="genre[]" value="Documentary"/> Documentary
<input type="checkbox" name="genre[]" value="Family"/> Family
<input type="checkbox" name="genre[]" value="Sci-Fi"/> Sci-Fi
<input type="checkbox" name="genre[]" value="Animation"/> Animation
<input type="checkbox" name="genre[]" value="Musical"/> Musical
<br>
<input type="checkbox" name="genre[]" value="War"/> War
<input type="checkbox" name="genre[]" value="Western"/> Western
<input type="checkbox" name="genre[]" value="Adult"/> Adult
<input type="checkbox" name="genre[]" value="Short"/> Short
</p>
<p> <input type="submit" value="Add Movie" name="clicked"></p>
</form>

<?php
if ($_POST["title"] && $_POST["company"] && $_POST["year"] && $_POST["MPAA"] && is_numeric($_POST["year"])){
	
	$title = $_POST["title"];
	$company = $_POST["company"];
	$year = $_POST["year"];
	$mpaa = $_POST["MPAA"];

	$genres = array();
	$isSuccessful = 0;
	for($x=0;$x<19;$x++) {
		if(isset($_POST["genre"][$x])) {
			array_push($genres, $_POST["genre"][$x]);
		}
	}

	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}

	$midQuery = "SELECT id FROM MaxMovieID";
	$midGet = mysql_query($midQuery, $db_connection);
	$midFinal = mysql_fetch_row($midGet);
	
	$mid = $midFinal[0];
	$insertMovie = "INSERT INTO Movie VALUES($mid, '$title', $year, '$mpaa', '$company')";

	if(mysql_query($insertMovie, $db_connection)) {
		$isSuccessful = 1;
		echo "Successful adding of Movie: $title! ";
		echo "See <a href='./movie.php?id=$mid'>New Movie info</a></br> ";

		foreach($genres as $genreType){
			mysql_query("INSERT INTO MovieGenre VALUES ($mid, '$genreType')", $db_connection);
		}
	}
	else {
		echo "Failed adding of Movie: $title! ";
	}

	$success = "";
	if($isSuccessful == 1) {
		$success = "UPDATE MaxMovieID SET id=id+1";
		mysql_query($success, $db_connection);
	}
	mysql_close($db_connection);
}

	?>

</html>