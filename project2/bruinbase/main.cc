/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
 #include "BTreeNode.h"

#include <iostream>
 using namespace std;

void test_function()
{
	BTLeafNode b1(102);
  cout << "Count: " << b1.getKeyCount() << endl;
}

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  // SqlEngine::run(stdin);
	test_function();

  return 0;
}
