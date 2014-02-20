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
#include "RecordFile.h"
#include "PageFile.h"
#include "BTreeNode.h"
#include <iostream>
#include <cassert>

using namespace std;

void test_function()
{
	BTLeafNode b;
	b.create();

	// Testing insert Leaf Node
	RecordId rid;
	rid.pid = 10;
	rid.sid = 13;

	b.insert(5, rid);

	int key = 0;
	RecordId n_rid;

	b.readEntry(0, key, n_rid);
	assert(key == 5);
	assert(n_rid.pid == 10);
	assert(n_rid.sid == 13);

	// Test insert first leaf node
	rid.pid = 100;
	rid.sid = 113;
	b.insert(12, rid);
	b.readEntry(0, key, n_rid);
	assert(key == 5);
	assert(n_rid.pid == 10);
	assert(n_rid.sid == 13);

	// Test insert second leaf node
	b.readEntry(1, key, n_rid);
	assert(key == 12);
	assert(n_rid.pid == 100);
	assert(n_rid.sid == 113);

	// Test writing leaf node to file
	PageFile p("test", 'w');
	PageId pid = 1;
	b.write(pid, p);

	BTLeafNode b2;
	b2.read(pid, p);
	b2.readEntry(1, key, n_rid);
	assert(key == 12);
	assert(n_rid.pid == 100);
	assert(n_rid.sid == 113);

	// Test the locate function
	int eid = -1;
	b2.locate(6, eid);
	assert(eid == 1);
	eid = -1;
	b2.locate(1, eid);
	assert(eid == 0);
	eid = -1;
	b2.locate(4, eid);
	assert(eid == 0);
	eid = -1;
	b2.locate(5, eid);
	assert(eid == 0);
	eid = -1;

	int err = b2.locate(21, eid);
	assert(err == RC_NO_SUCH_RECORD);

	eid = -1;
	b2.locate(12, eid);
	assert(eid == 1);
	eid = -1;
	b2.locate(11, eid);
	assert(eid == 1);



	cout << "\nAll tests passed successfully!\n\n";
}

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  // SqlEngine::run(stdin);


	test_function();
  return 0;
}
