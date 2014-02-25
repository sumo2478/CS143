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

void test_insertion()
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
	
	// Test insert out of order
	RecordId rid3;
	rid3.pid = 233;
	rid3.sid = 111;
	b2.insert(15, rid3);

	b2.insert(16,rid3);
	b2.insert(17,rid3);

	rid3.pid = 777;
	rid3.sid = 333;
	b2.insert(7, rid3);
	eid = -1;
	b2.locate(7, eid);

	assert(eid == 1);

	rid3.pid = 5;
	rid3.sid = 8;
	b2.insert(4, rid3);
	eid = -1;
	b2.locate(2, eid);
	
	assert(eid == 0);


	// Continuously appending values to the end
	BTLeafNode b3;
	b3.create();

	RecordId rid4;
	rid4.sid = 4;
	rid4.pid = 7;

	int counter = 0;

	while (b.insert(counter, rid4) == 0)
		counter++;

 // [4,5,7,12,15,16,17]
	cout << "Insertion tests passed...\n";
}

void test_sibling_node()
{
	BTLeafNode b;
	b.create();

	// Testing insert Leaf Node
	RecordId rid;
	rid.pid = 10;
	rid.sid = 13;

	PageId pid = 132;

	b.insert(5, rid);
	b.setNextNodePtr(pid);

	assert(b.getNextNodePtr() == pid);

	// Inserting to the end to see if pid overwritten

	while (b.insert(5,rid) == 0)
		continue;

	assert(b.getNextNodePtr() == pid);

	cout << "Sibling node tests passed...\n";
}

void test_insert_and_split()
{
	BTLeafNode b;
	b.create();
	b.setNextNodePtr(777);

	int counter = 0;
	RecordId rid;
	rid.pid = 10;
	rid.sid = 20;

	while (b.insert(counter, rid) == 0)
		counter+=2;

	BTLeafNode sibling;
	sibling.create();
	int sibling_key = -1;

	RecordId rid2;
	rid2.pid = 33;
	rid2.sid = 11;
	b.insertAndSplit(7, rid2, sibling, sibling_key);

	// Check the sibling_key
	assert(sibling_key == 84);

	int key = -1;
	b.readEntry(0, key, rid);
	assert(key == 0 && rid.pid == 10 && rid.sid == 20);
	b.readEntry(42, key, rid);
	assert(key == 82);
	b.readEntry(4, key, rid);
	assert(key == 7);

	// Assert that the next pointer was set
	assert(sibling.getNextNodePtr() == 777);

	cout << "Insert and split test passed...\n";
}

void test_substitute_insertion()
{
	BTLeafNode b;
	b.create();

	RecordId rid;
	rid.pid = 3;
	rid.sid = 4;

	for (int i = 0; i < 10; i+=2)
		b.insert(i, rid);

	b.insert(7, rid);

	int eid = -1;
	b.locate(7, eid);
	assert(eid == 4);
	b.locate(8, eid);
	assert(eid == 5);

	cout << "Substitution insertion passed...\n";
}

void test_function()
{
	cout << endl;

	test_insertion();

	test_substitute_insertion();

	test_sibling_node();

	test_insert_and_split();

	 

	cout << "\nAll tests passed successfully!\n\n";
}

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  // SqlEngine::run(stdin);


	test_function();
  return 0;
}
