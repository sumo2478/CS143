<h1>Calculator</h1>
<p>
<form method="GET">
<input type="text" name="expr"><input type="submit" value="Calculate">
</form>
</p>

<?php 
	// Retrieve the expression from the form if available
	if ($_GET['expr'])
	{
		$expr = $_GET['expr'];
		
		// Error checking here...

		// Handle all the multiplications and divisions
		eval("\$result = $expr;");

		print("Answer: ".$result."\n");
	}
?>