<html>
<head><title>Query</title></head>
<body>

<h1> Queries </h1>

<form method="GET">
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
		$db_connection = mysql_connect("localhost", "cs143", "");

		mysql_select_db("CS143", $db_connection);
		
		$sanitized_name = $_GET['query'];

		$rs = mysql_query($sanitized_name, $db_connection);
		
		$queryResult = mysql_fetch_assoc($rs);

		if ($queryResult)
		{
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

		mysql_close($db_connection);
	}
?>		
</body>
</html>


