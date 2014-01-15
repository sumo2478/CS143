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

            // Remove all spaces
            $expr = str_replace(" ", "", $expr);

            // Replace instances of -- with +
            $expr = str_replace("--", "+", $expr);

            // Check for invalid characters
            $error_expr = '/[^0-9\*\-\+\/\.]/';
            if (preg_match($error_expr, $expr)) {
                    print("Invalid input: ".$expr."\n");
                    return;
            }

            $zero = FALSE;

    		function error_z($errno, $errstr, $errfile, $errline) {
    		    if ($errstr == "Division by zero") {
    		        global $zero;
    		        $zero = TRUE;
    		    }
    		}

    		set_error_handler('error_z');


            // Handle all the multiplications and divisions
            $multdiv_pattern = '/\-?\d+\.*\d*(\*|\/)\-?\d+\.*\d*/';

            preg_match($multdiv_pattern, $expr, $matches, PREG_OFFSET_CAPTURE);
            while($matches)
            {
                    $equation = $matches[0][0];
                    eval("\$result = $equation;");
                    if ($zero == TRUE) { 
    				    echo "Invalid Input: Division by zero error!";
    				    return;
    				}
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

            print("Answer: ".$expr."\n");
        }
?>