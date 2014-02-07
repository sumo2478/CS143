<?php 
	include "connect.php";

	$actor_id = mysql_real_escape_string(trim($_GET['id']));

	$actor_query = "SELECT * FROM Actor WHERE id = $actor_id";
	$actor_query_result = mysql_query($actor_query) or die(mysql_error());

	$actor = mysql_fetch_assoc($actor_query_result);

	$movie_query = "SELECT M.id, M.title, MovieActor.role FROM Actor A, Movie M, MovieActor
	WHERE A.id = $actor_id AND MovieActor.aid = $actor_id AND MovieActor.mid = M.id";
	$movie_query_result = mysql_query($movie_query) or die(mysql_error());

 ?>

<html>
<head>
	<title><?php echo $actor['first']." ".$actor['last'] ?></title>
</head>
<body>
	<h1><?php echo $actor['first']." ".$actor['last'] ?></h1>
	<h3>-- Basic Info --</h3>
	<p>Sex: <?php echo $actor['sex'] ?> </p>
	<p>Birth: <?php echo $actor['dob'] ?> </p>

	<?php if ($actor['dod']) {
		echo "<p>Death: ".$actor['dod']."</p>";
	} ?>

	<h3>-- Movie Info --</h3>
	<?php 
		while ($movie = mysql_fetch_assoc($movie_query_result)) {
			echo "<p>Acted as ".$movie['role']." in <a href=./movie.php?id=".$movie['id'].">".$movie['title']."</a></p>";
		}
	 ?>

</body>
</html>