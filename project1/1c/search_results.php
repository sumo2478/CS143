<?php 
	include 'connect.php';

	$query = mysql_real_escape_string(trim($_GET['query']));

	// Split the search query into individual words
	$words = explode(" ", $query);

	$actor_results = array(); // List of actors that match query
	$movie_results = array();

	// For each word in the query select the actor that contains the name in either
	// his/her first or last name
	foreach ($words as $word) {

		$actor_sql_query = "SELECT id, first, last FROM Actor WHERE (first = \"$word\")
		OR (last = \"$word\")";

		$movie_sql_query = "SELECT id, title from Movie WHERE title LIKE \"%$word%\"";

		$actor_query_result = mysql_query($actor_sql_query) or die(mysql_error());
		$movie_query_result = mysql_query($movie_sql_query) or die(mysql_error());

		// Retrieve actor results
		while ($result = mysql_fetch_assoc($actor_query_result)) {

			// If the actor doesn't exist in the array yet then add it in
			if (!array_key_exists($result['id'], $actor_results)) {
				$actor_results[$result['id']] = $result;
			}			
		}

		// Retrieve movie results
		while ($result = mysql_fetch_assoc($movie_query_result)) {
			if (!array_key_exists($result['id'], $movie_results)) {
				$movie_results[$result['id']] = $result;
			}
		}
	}

 ?>


<html>
<head>
	<title>Search Results</title>
</head>
<body>

	<h1>Search Results For: <?php echo stripslashes($query) ?> </h1>

	<p>Search Actor/Movie: </p>

	<form method="GET">
		<input type="text" name="query">
		<input type="submit" value="Search">
	</form>

	<h2>Actors/Actresses</h2>
	<?php 
		if ($query != "") {
		
			foreach ($actor_results as $actor) {
				echo "<p><a href=./actor.php?id=".$actor['id'].">".$actor['first']." ".$actor['last']."</a></p>";
			}
		}
	?>

	<h2>Movies</h2>
	<?php 
		if ($query != "") {
			foreach ($movie_results as $movie) {
				echo "<p><a href=./movie.php?id=".$movie['id'].">".$movie['title']."</a></p>";
			}
		}
	?>

</body>
</html>

