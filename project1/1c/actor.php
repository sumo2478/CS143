<?php 
	include "connect.php";

	// Retrieve Actor information
	$actor_id = mysql_real_escape_string(trim($_GET['id']));

	$actor_query = "SELECT * FROM Actor WHERE id = $actor_id";
	$actor_query_result = mysql_query($actor_query);		

	// Retrieve Movies That Actor Was in
	$movie_query = "SELECT M.id, M.title, MovieActor.role FROM Actor A, Movie M, MovieActor
	WHERE A.id = $actor_id AND MovieActor.aid = $actor_id AND MovieActor.mid = M.id";

	$movie_query_result = mysql_query($movie_query);

	// Check to make sure that we retrieved the actor information
	if (!$movie_query_result || !$actor_query_result) 
		die("Failed to retrieve actor information");
	 
	 $actor = mysql_fetch_assoc($actor_query_result);

 ?>

<html>
<head>
	<title><?php echo $actor['first']." ".$actor['last'] ?></title>
</head>
<body>
	<a href="./home.html">Home</a>

	<h1><?php echo "Actor: ".$actor['first']." ".$actor['last'] ?></h1>
	<h3>-- Basic Info --</h3>
	<p>Sex: <?php echo $actor['sex'] ?> </p>
	<p>Birth: <?php echo $actor['dob'] ?> </p>

	<?php if ($actor['dod']) {
		echo "<p>Death: ".$actor['dod']."</p>";
	} ?>

	<h3>-- Movie Info --</h3>
	<?php 
		if ($actor_id) {
			while ($movie = mysql_fetch_assoc($movie_query_result)) {
				echo "<p>Acted as ".$movie['role']." in <a href=./movie.php?id=".$movie['id'].">".$movie['title']."</a></p>";
			}
		}
	 ?>

</body>
</html>