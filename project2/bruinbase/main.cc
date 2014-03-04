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
#include "BTreeIndex.h"
#include <iostream>
#include <cassert>
 #include <unistd.h>

using namespace std;

void test_nonleaf_insertion()
{
	PageFile p("test", 'w');
	PageId pid1 = 1;
	PageId pid2 = 3;
	PageId pid3 = 199;
	PageId pid4 = 81;
	BTNonLeafNode b;
	
	b.initializeRoot(pid1, 3, pid2);
	b.insert(5, pid3);
	b.insert(4, pid4);
	//b.insert(1, 11);
	b.printBuffer();
	cout << "KeyCount:" << b.getKeyCount() <<endl;
	b.locateChildPtr(6,pid2);
	assert(pid2 == 199);
}

void test_insertion()
{
	BTLeafNode b;

	// Testing insert single entry into Leaf Node
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

	// Test insert to end
	rid.pid = 100;
	rid.sid = 113;
	b.insert(12, rid);

	// Make sure 5 is still first
	b.readEntry(0, key, n_rid);
	assert(key == 5);
	assert(n_rid.pid == 10);
	assert(n_rid.sid == 13);

	// Make sure 12 is last
	b.readEntry(1, key, n_rid);
	assert(key == 12);
	assert(n_rid.pid == 100);
	assert(n_rid.sid == 113);

	// Test inserting to the beginning
	rid.pid = 55;
	rid.sid = 66;
	b.insert(3, rid);

	// Make sure 3 is first
	b.readEntry(0, key, n_rid);
	assert(key == 3);
	assert(n_rid.pid == 55);
	assert(n_rid.sid == 66);

	// Test writing leaf node to file
	PageFile p("test", 'w');
	PageId pid = 1;
	b.write(pid, p);

	BTLeafNode b2;
	b2.read(pid, p);
	b2.readEntry(1, key, n_rid);
	assert(key == 5);
	assert(n_rid.pid == 10);
	assert(n_rid.sid == 13);

	// Test the locate function
	int eid = -1;
	b2.locate(6, eid);
	assert(eid == 2);
	eid = -1;
	b2.locate(1, eid);
	assert(eid == 0);
	eid = -1;
	b2.locate(4, eid);
	assert(eid == 1);
	eid = -1;
	b2.locate(5, eid);
	assert(eid == 1);
	eid = -1;
	// Test end
	b2.locate(100, eid);
	eid = -1;

	int err = b2.locate(21, eid);
	assert(err == RC_NO_SUCH_RECORD);

	eid = -1;
	b2.locate(12, eid);
	assert(eid == 2);
	eid = -1;
	b2.locate(11, eid);
	assert(eid == 2);
	
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
	b2.locate(8, eid);

	assert(eid == 3);

	rid3.pid = 5;
	rid3.sid = 8;
	b2.insert(4, rid3);
	eid = -1;
	b2.locate(2, eid);
	
	assert(eid == 0);


	// Continuously appending values to the end
	BTLeafNode b3;

	RecordId rid4;
	rid4.sid = 4;
	rid4.pid = 7;

	int counter = 0;

	while (b.insert(counter, rid4) == 0)
		counter++;
	b2.printBuffer();
 // [4,5,7,12,15,16,17]
	cout << "Insertion tests passed...\n";
}

void test_sibling_node()
{
	BTLeafNode b;

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

void test_nonleaf_insert_and_split()
{
	BTNonLeafNode b;
	b.initializeRoot(1, 1, 3);
	PageId pid = 10;
	int counter = 2;
	while(b.insert(counter, pid) == 0) {
		counter += 2;
		pid += 1;
	}
	b.printBuffer();

	BTNonLeafNode sibling;

	int sibling_key = -1;

	b.insertAndSplit(127, 7777777, sibling, sibling_key);
	
	assert(sibling_key == 126);

}
void test_insert_and_split()
{
	BTLeafNode b;
	b.setNextNodePtr(777);

	int counter = 0;
	RecordId rid;
	rid.pid = 10;
	rid.sid = 20;

	while (b.insert(counter, rid) == 0)
		counter+=2;

	BTLeafNode sibling;
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

void test_index_insert()
{
	BTreeIndex b;
	RecordId rid;
	rid.pid = 3;
	rid.sid = 4;
	int key;
	RecordId rid2;

	b.open("test", 'w');
	
	//for (int i = 0; i < 1000; i++)
	//{
	//	b.insert(i, rid);	
	//}

	b.printTree();
	IndexCursor ic;
	int searchKey = 945;
	if(b.locate(searchKey, ic) != 0) {
		cout << "Failed to find " << searchKey << endl;
	}
	else
	{
		cout << "Found search key " << searchKey << endl;
		cout << "With PageId " << ic.pid << endl;
		cout << "With Entry# " << ic.eid << endl;
	}
	RC rc = (b.readForward(ic, key, rid2));
	if( rc == 0 ) {
		cout << "With PageId " << ic.pid << endl;
		cout << "With Entry# " << ic.eid << endl;
		cout << "With rid2.pid = " << rid.pid << endl;
		cout << "With rid2.sid = " << rid.sid << endl;
	}
	else if( rc == RC_END_OF_TREE)
	{
		cout << "END OF THE TREE" << endl;
	}
	b.close();
}

void test_random()
{
	BTNonLeafNode b;


	for (int i = 0; i < 10; i++)
	{
		if (b.insert(i, 1) < 0)
		{
			cout << "i: " << i << endl;
			break;
		}
	}
	cout << "Key count: " << b.getKeyCount() << endl;
	BTNonLeafNode sibling;

	int key = -1;
	b.insertAndSplit(20, 1, sibling, key);

	b.printBuffer();
	sibling.printBuffer();

	cout << "End: " << key;
}

void test_function()
{

	//test_nonleaf_insertion();
	//test_nonleaf_insert_and_split();
	
	//test_insertion();

	//test_substitute_insertion();

	//test_sibling_node();

	//test_insert_and_split();

	test_index_insert();

	//test_random();

	cout << "\nAll tests passed successfully!\n\n";
}

int main()
{
	// run the SQL engine taking user commands from standard input (console).
	// SqlEngine::run(stdin);


	test_function();
	return 0;
}
