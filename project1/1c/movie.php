<?php 
	include "connect.php";

	// Retrieve Movie information
	$movie_id = mysql_real_escape_string(trim($_GET['id']));
	if ($movie_id) {

		$movie_query = "SELECT * FROM Movie WHERE id = $movie_id";
		$movie_query_result = mysql_query($movie_query) or die(mysql_error());

		$movie = mysql_fetch_assoc($movie_query_result);

		// Retrieve Actor Information
		$actor_query = "SELECT A.id, A.last, A.first, MovieActor.role
						FROM Movie M, Actor A, MovieActor
						WHERE M.id=$movie_id AND M.id=MovieActor.mid AND A.id=MovieActor.aid";
		$actor_query_result = mysql_query($actor_query) or die(mysql_error());

		// Retrieve Score Information
		$score_query = "SELECT AVG(R.rating) AS score
						FROM Movie M, Review R
						WHERE M.id=$movie_id AND R.mid=$movie_id";

		$score_query_result = mysql_query($score_query) or die(mysql_error());

		$score = mysql_fetch_assoc($score_query_result);

		// Retrieve Reviews
		$review_query = "SELECT * FROM Review WHERE mid=$movie_id";
		$review_query_result = mysql_query($review_query) or die(mysql_error());
	}
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
 	<p>Average User Score: <?php

 								if ($score['score']) {
 									echo $score['score'];
 								}else
 								{
 									echo "Not Available";
 								}
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
 	<a href="./add_comment.php?movie_id=<?php echo $movie_id?>">Add Comment</a>
 	<?php 
 		if ($movie_id) {
	 		while ($review = mysql_fetch_assoc($review_query_result)) {
	 			echo "<p>".$review['time']."	".$review['name']."</p>";
	 			echo "<p>Rating: ".$review['rating']."</p>";
	 			echo "<p>Comment: ".$review['comment']."</p>";
	 		}
 		}
 	?>

 </body>
 </html>