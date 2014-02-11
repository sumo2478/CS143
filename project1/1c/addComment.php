<html>
<head><title>Add Comment</title></head>
<body>



<?php

	$db_connection = mysql_connect("localhost", "cs143", "");

	if (!$db_connection) {
		echo "Unable to connect to database" . mysql_error();
		exit;
	}

	if (!mysql_select_db("CS143", $db_connection)) {
		echo "Unable to select database: " . mysql_error();
		exit;
	}
	$movie_id = mysql_real_escape_string(trim($_GET['movie_id']));
	$movie_query = "SELECT title, id FROM Movie WHERE id = $movie_id";
	$movie_query_result = mysql_query($movie_query, $db_connection) or die(mysql_error());
	$review = mysql_fetch_row($movie_query_result);
?>

<form method="POST">
	<p><h2> Add Comment to <?php echo "<a href='./movie.php?id=$review[1]'>" .$review[0]. "</a>";?></h2></p>
<p><b>Your Name: </b><input type="text" name="name" maxlength="20" value="Anonymous"/><br/></p>
<p>Movie Rating: 
<select name="Rating">
<option value=5>5 Stars</option>
<option value=4>4 Stars</option>
<option value=3>3 Stars</option>
<option value=2>2 Stars</option>
<option value=1>1 Star</option>
</select></p><br/>
<textarea name="comment" cols="60" rows="8" maxlength="500">Limit 500 characters</textarea>
<br>
<input type="submit" value="Add Comment" />
</form>

<?php
if($_GET["movie_id"] && $_POST["name"] && $_POST["comment"]) {
	$rawName = $_POST["name"];
	$rawComment = $_POST["comment"];
	$id = $_GET["movie_id"];
	$rating = $_POST["Rating"];
	$timeStamp = time();

	$mysql_time = date('Y-m-d H:i:s', $timeStamp);
	$output_date = date("Y-m-d", $timeStamp);

	$name = mysql_real_escape_string($rawName);
	$comment = mysql_real_escape_string($rawComment);

	$reviewQuery = "INSERT INTO Review VALUES ('$name', '$mysql_time', $id, $rating, '$comment')";

	if(mysql_query($reviewQuery, $db_connection)) {
		echo "<br>Adding your review for $name at: $output_date was successful";
	}
	else {
		echo "<br>Failed to add review";
	}
}
mysql_close($db_connection);
?>

</html>