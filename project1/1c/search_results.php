<?php 
	include 'connect.php';

	$query = $_GET['query'];

	// Split the search query into individual words
	$words = explode(" ", $query);

	$actor_results = array();

	// For each word in the query select the actor that contains the name in either
	// his/her first or last name
	foreach ($words as $word) {
		$actor_sql_query = "SELECT * FROM Actor WHERE (first LIKE \"%$word%\")
		OR (last LIKE \"%$word%\")";

		$actor_query_result = mysql_query($actor_sql_query) or die(mysql_error());

		while ($result = mysql_fetch_assoc($actor_query_result)) {
			$actor_results[] = $result;
		}

	}
 ?>


<html>
<head>
	<title>Search Results</title>
</head>
<body>

	<h1>Search Results For: <?php echo "\"".$query."\"" ?> </h1>
	<?php 
		if ($query != "") {
		
			foreach ($actor_results as $actor) {
				echo "<p>".$actor['first']." ".$actor['last']."</p>";
			}
		}
	?>

</body>
</html>

