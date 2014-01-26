<html>
<head><title>CLICK ME</title></head>
<body>

<h1> Queries </h1>

<form action="query.php" method="GET">
<textarea name="query" cols="60" rows="8"></textarea>
<input type="submit" value="Submit" />
</form>

</p>
<p><small>Note: tables and fields are case sensitive. Run "show tables" to see the list of
available tables.</small>
</p>

<h3>Results from MySQL:</h3>
<?php
	if ($_GET['query'])
	{
		$testquery = $_GET['query'];
		//echo $testquery;
		$db_connection = mysql_connect("localhost", "cs143", "");
		mysql_select_db("CS143", $db_connection);
		//$testquery = "select * from Actor";
		$sanitized_name = mysql_real_escape_string($_GET['query']);;
		$query_to_issue = sprintf($query, $sanitized_name);
		//echo $query;
		$rs = mysql_query($testquery, $db_connection);
		//$num = mysql_numrows($rs);
		//$numFields = mysql_num_fields($rs);
		?>
		    <!-- <table border=1 cellspacing=1 cellpadding=2>		
		    	<tr align=center>
		    	-->
		<?php
		//echo $rs;
		$queryResult = mysql_fetch_assoc($rs);
		if ($queryResult){
			echo "<table border=1 cellspacing=1 cellpadding=2>";
			echo "<tr align=center>";
			foreach ($queryResult as $columns =>$rows){
				echo "<th align=center> {$columns}</th>";
			}
			echo "</tr>";
			do {
				echo "<tr align=center>";
				foreach($queryResult as $columns => $rows){
					if(is_null($rows))
						echo "<td align=center>N/A</td>";
					else
						echo "<td align=center>$rows</td>";
				}
				echo "</tr>";

			} while ($queryResult = mysql_fetch_assoc($rs));
			echo "</table>";
		}

		/*
		while($row = mysql_fetch_row($rs)) {
		    $i = 0;
		    echo '<table border=1 cellspacing=1 cellpadding=2>';
		    while($i < $num)
		    {
		    	//$fieldName = mysql_fetch_field($rs, $i);
		    	$fieldValue = $row[$i];
		    	?>

		    		<!-- <td><b> <?php print "$fieldvalue"; ?></b></td>
		    	-->
		    	<?php
		    	print "<br />";
		    	$i++;
		    }
		    $sid = $row[0];
		    $name = $row[1];
		    $email = $row[2];

		}
		*/
		?>
		<?php
		mysql_close($db_connection);
	}
?>
</body>
</html>


