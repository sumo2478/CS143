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
		
		// Handle all the multiplications and divisions
		$multdiv_pattern = '/\-?\d+\.*\d*(\*|\/)\-?\d+\.*\d*/';

		preg_match($multdiv_pattern, $expr, $matches, PREG_OFFSET_CAPTURE);

		while($matches)
		{
			$equation = $matches[0][0];
			eval("\$result = $equation;");
			$count = 1;
			$expr = str_replace($matches[0][0], $result,$expr, $count);

			preg_match($multdiv_pattern, $expr, $matches, PREG_OFFSET_CAPTURE);			
		}

		// Handle addition and subtraction
		$addsub_pattern = '/\-?\d+\.*\d*(\+|\-)\-?\d+\.*\d*/';

		preg_match($addsub_pattern, $expr, $matches, PREG_OFFSET_CAPTURE);

		while($matches)
		{
			$equation = $matches[0][0];
			eval("\$result = $equation;");
			$count = 1;
			$expr = str_replace($matches[0][0], $result,$expr, $count);

			preg_match($addsub_pattern, $expr, $matches, PREG_OFFSET_CAPTURE);			
		}

		print($expr);

	}
?>