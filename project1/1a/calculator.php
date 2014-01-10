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

		// Check for invalid characters
		$error_expr = '/[^0-9\*\-\+\/]/';
		if (preg_match($error_expr, $expr)) {
			print("Error\n");
			return;
		}

		// Handle all the multiplications and divisions
		eval("\$result = $expr;");

		print("Answer: ".$result."\n");
	}
?>