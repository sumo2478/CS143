<?php 
	include "connect.php";

	// Retrieve Movie information
	$movie_id = mysql_real_escape_string(trim($_GET['id']));

	$movie_query = "SELECT * FROM Movie WHERE id = $movie_id";
	$movie_query_result = mysql_query($movie_query); // Need to check

	// Retrieve Genre Information
	$genre_query = "SELECT genre
					FROM MovieGenre
					WHERE mid = $movie_id";
	$genre_query_result = mysql_query($genre_query); // Need to check

	// Retrieve Director Information
	$director_query = "SELECT D.first, D.last
					   FROM MovieDirector M, Director D
					   WHERE M.mid = $movie_id AND D.id=M.did";
	$director_query_result = mysql_query($director_query); // Need to check

	// Retrieve Actor Information
	$actor_query = "SELECT A.id, A.last, A.first, MovieActor.role
					FROM Movie M, Actor A, MovieActor
					WHERE M.id=$movie_id AND M.id=MovieActor.mid AND A.id=MovieActor.aid";
	$actor_query_result = mysql_query($actor_query); // Need to check

	// Retrieve Score Information
	$score_query = "SELECT AVG(R.rating) AS score
					FROM Movie M, Review R
					WHERE M.id=$movie_id AND R.mid=$movie_id";

	$score_query_result = mysql_query($score_query); // Need to check

	// Retrieve Reviews
	$review_query = "SELECT * FROM Review WHERE mid=$movie_id";
	$review_query_result = mysql_query($review_query); // Need to check

	if (!$movie_query_result    || !$genre_query_result || 
		!$director_query_result || !$actor_query_result || !$review_query_result) {
		die("Failed to retrieve movie information");
	}


	// Retrieve Data From Queries
	$movie = mysql_fetch_assoc($movie_query_result);

	while ($genre = mysql_fetch_assoc($genre_query_result)) {
		$genres[] = $genre['genre'];
	}

	while ($director = mysql_fetch_assoc($director_query_result)) {
		$directors[] = $director['first']." ".$director['last'];
	}

	$score = mysql_fetch_assoc($score_query_result);
 ?>

 <html>
 <head>
 	<title><?php echo $movie['title']?></title>
 </head>
 <body>
 	<a href="./home.html">Home</a>
 	<h1><?php echo "Movie: ".$movie['title'] ?> </h1>

 	<h3>-- Basic Info --</h3>
 	<p>Year: <?php echo $movie['year'] ?> </p>
 	<p>Rating: <?php echo $movie['rating'] ?></p>
 	<p>Production Company: <?php echo $movie['company'] ?></p>
 	<p>Genres: <?php if ($genres) echo implode(', ', $genres) ?></p>
 	<p>Directors: <?php if ($directors)echo implode(', ', $directors) ?></p>
 	<p>Average User Score: <?php
 								if ($score['score'])
 									echo $score['score'];
 								else 							
 									echo "Not Available";
 							?></p>

 	<h3>-- Actor Info --</h3>
 	<?php 
 		if ($movie_id) {
	 		while ($actor = mysql_fetch_assoc($actor_query_result)) {
	 			echo "<p><a href=./actor.php?id=".$actor['id'].">".$actor['first']." ".$actor['last']."</a> as ".$actor['role']."</p>";
	 		}
	 	}
 	?>

 	<h3>-- Reviews --</h3>
 	<a href="./addComment.php?movie_id=<?php echo $movie_id?>">Add Comment</a>
 	<?php 
 		if ($movie_id) {
	 		while ($review = mysql_fetch_assoc($review_query_result)) {
	 			echo "<p>--------------------------------------------------</p>";
	 			echo "<p>".$review['time']."	".$review['name']."</p>";
	 			echo "<p>Rating: ".$review['rating']."</p>";
	 			echo "<p>Comment: ".$review['comment']."</p>";
	 		}
 		}
 	?>

 </body>
 </html>