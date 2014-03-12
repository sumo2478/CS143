/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <iostream>

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
    RC rc = pf.open(indexname, mode);

    // Read in the tree specifications
    // If the tree is empty then just return
    if (pf.endPid() == 0)
  		return rc;

  	char buffer[1024];
  	pf.read(TREE_INFO_PAGE,buffer);

  	TreeInfo* t = (TreeInfo*) buffer;
  	treeHeight = t-> treeHeight;
  	rootPid = t-> rootPid;
 
    return rc;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	char buffer[1024];
	pf.read(TREE_INFO_PAGE, buffer);

	// Write the tree info the the page file
	TreeInfo* t = (TreeInfo*) buffer;
	t->treeHeight = treeHeight;
	t->rootPid = rootPid;

	pf.write(TREE_INFO_PAGE, buffer);

    RC rc = pf.close();
    return rc;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	RC rc = 0;
	// If the tree is empty then place a leaf node as the root
	if (treeHeight == 0)
	{
		BTLeafNode new_node;
		new_node.insert(key, rid);
	
		// Update the new root
		rootPid = 1;
		new_node.write(rootPid, pf); // Write the node to the first page file

		treeHeight = 1;
	}
	else
	{
		int new_key, new_pid;
		rc = insert_recurse(key, rid, 1, rootPid, new_key, new_pid);

		// If the node had to be split then should initialize a new root
		if (rc == RC_NODE_FULL)
		{
			rc = createRoot(rootPid, new_key, new_pid);
		}
	}


    return rc;
}

/**
 * Recursively inserts (key, RecordId) pair to the index
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @param level[IN] the current level we are inserting too
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert_recurse(int key, const RecordId& rid, int level, PageId currNodePid, int &new_key, PageId &new_pid)
{
	RC rc = 0;

	// If we are at the leaf node level then just insert to the leaf node
	if (level == treeHeight)
	{
		BTLeafNode b;
		b.read(currNodePid, pf);

		// Try to insert into leaf node
		rc = b.insert(key, rid);

		if (rc == 0)
		{
			b.write(currNodePid, pf);
			return 0;
		}

		// If the node is full then we need to do an insert and split
		// Initialize the sibling leaf node
		BTLeafNode sibling_node;
		int sibling_key;

		b.insertAndSplit(key, rid, sibling_node, sibling_key);

		// Write the new sibling node to the page file
		PageId sibling_pid = pf.endPid();
		sibling_node.write(sibling_pid, pf);
		
		// Update the leaf nodes next pointer
		b.setNextNodePtr(sibling_pid);
		b.write(currNodePid, pf);



		// If we are at the root level then have to create a nonleaf node root
		new_key = sibling_key;
		new_pid = sibling_pid;

		return rc;
	}

	// Otherwise insert into nonleaf node
	BTNonLeafNode node;
	node.read(currNodePid, pf);

	// Locate the proper node to insert to
	PageId child_pid = -1;
	node.locateChildPtr(key, child_pid);

	// Insert the node
	int n_key;
	PageId n_pid;
	rc = insert_recurse(key, rid, level+1, child_pid, n_key, n_pid);

	// If the node was full then it had to be split, which means
	// we must insert the split value into this node
	if (rc == RC_NODE_FULL)
	{
		rc = node.insert(n_key, n_pid);

		// If the insert succeeded then save changes
		if (rc == 0)
		{
			node.write(currNodePid, pf);		
		}
		// If the node is full then do an insert and split
		else if (rc == RC_NODE_FULL)
		{
			BTNonLeafNode sibling_node;

			int midKey = 0;
		
			node.insertAndSplit(n_key, n_pid, sibling_node, midKey);
			node.write(currNodePid, pf);

			// Save the changes to the sibling node
			PageId sibling_pid = pf.endPid();
			sibling_node.write(sibling_pid, pf);

			// Update the variables for the parent node to modify
			new_key = midKey;
			new_pid = sibling_pid;

			if (new_key < 0)
				cout << "THIS IS THE PROBLEM2::" << new_key << " \n\n\n\n";

		}

	}

	return rc;
}

/**
 * Creates a new root for the index
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTreeIndex::createRoot(PageId pid1, int key, PageId pid2)
{
	BTNonLeafNode new_root;

	RC rc = new_root.initializeRoot(pid1, key, pid2);

	// Update the new root pid
	rootPid = pf.endPid();
	new_root.write(rootPid, pf);

	treeHeight++;

	return rc;
}


// TODO by Alex
/*
 * Find the leaf-node index entry whose key value is larger than or 
 * equal to searchKey, and output the location of the entry in IndexCursor.
 * IndexCursor is a "pointer" to a B+tree leaf-node entry consisting of
 * the PageId of the node and the SlotID of the index entry.
 * Note that, for range queries, we need to scan the B+tree leaf nodes.
 * For example, if the query is "key > 1000", we should scan the leaf
 * nodes starting with the key value 1000. For this reason,
 * it is better to return the location of the leaf node entry 
 * for a given searchKey, instead of returning the RecordId
 * associated with the searchKey directly.
 * Once the location of the index entry is identified and returned 
 * from this function, you should call readForward() to retrieve the
 * actual (key, rid) pair from the index.
 * @param key[IN] the key to find.
 * @param cursor[OUT] the cursor pointing to the first index entry
 *                    with the key value.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
	RC rc = 0;
	int eid;
	
	// Reject if tree is empty, 
	if(treeHeight == 0) {
		return RC_NO_SUCH_RECORD;
	}

	// Initialize NonLeafNode to search through NonLeafNodes in tree
	// in order to locate to the leaves.
	PageId pid = rootPid;
	BTNonLeafNode nonleaf;
	nonleaf.initializeRoot(0, -1,-1);
	
	// Search through the tree until we reach the LeafNodes
	if(treeHeight != 1) {
	for(int curHeight = 1; curHeight < treeHeight; ++curHeight)
	{
		// Read information in the NonLeafNode
		if((rc = nonleaf.read(pid, pf)) < 0) {
			return rc;
		}
		// Figure out where the leaf is located by traversing through
		// the NonLeafNode to find the PageId
		if((rc = nonleaf.locateChildPtr(searchKey, pid)) < 0) {
			return rc;
		}
	}
	}
	// Now we are at the leaf level
	// In the right page
	BTLeafNode leaf;
	// Read information in the LeafNode
	if((rc = leaf.read(pid, pf)) < 0) {
		return rc;
	}
	// Find out eid of the LeafNode entry that contains searchKey
	if((rc = leaf.locate(searchKey, cursor.eid)) < 0) {
		return rc;
	}

	// The leaf was successfully located using the given searchKey.
	// Copy into the Cursor
	cursor.pid = pid;
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	// Must have a valid cursor
	if(cursor.pid <= 0)
		return RC_INVALID_CURSOR;
	RC rc = 0;

	// Assume that we are already pointing at a LeafNode
	BTLeafNode leaf;

	// Extract the information from the cursor
	PageId pid = cursor.pid;
	int eid = cursor.eid;

	// Read in leaf node
	if( (rc = leaf.read(pid,pf)) < 0)
		return rc;

	if(cursor.eid == leaf.getKeyCount() ) {
		int nextPid = leaf.getNextNodePtr();
		if(nextPid == 0) {
			return RC_END_OF_TREE;
		} else {
			cursor.pid = nextPid;
			cursor.eid = 0;
			if((rc = leaf.read(cursor.pid,pf)) < 0)
				return rc;
		}
	}
	leaf.readEntry(cursor.eid, key, rid);
	cursor.eid++;

    return 0;
}

// Need to write/read data that is associated with the B tree
// This data is something we want to read after we use the open function
// Before we close, we write the data to the disk to save it
// Use pf.write()
// treeHeight, rootPid needs to be saved. I don't know what else.

void BTreeIndex::printTree()
{
	printRecurse(rootPid, 1);
}

void BTreeIndex::printRecurse(PageId pid, int level)
{
	cout << "\n========================================\n";
	cout << "Printing out level: " << level << endl;
	cout << "Printing out page pid: " << pid << endl;
	if (level > treeHeight)
		return;
	// Leaf node
	else if (level == treeHeight)
	{
		BTLeafNode node;
		node.read(pid, pf);
		RecordId rid;
		int key = -1;
		for (int i = 0; i < node.getKeyCount(); i++)
		{
			node.readEntry(i, key, rid);
			cout << "[" << key << "]";
		}
	}
	else
	{
		BTNonLeafNode node;
		
		node.read(pid, pf);
		int key = -1;
		for (int i = 0; i < node.getKeyCount(); i++)
		{
			node.readNonLeafEntry(i, key);
			cout << "[" << key << "]";
		}

		key = -1;
		node.readNonLeafEntry(0, key);
		PageId child;
		node.locateChildPtr(key-1, child);
		printRecurse(child, level+1);

		for (int i = 0; i < node.getKeyCount(); i++)
		{
			key = -1;
			node.readNonLeafEntry(i, key);
			PageId child_page;
			node.locateChildPtr(key, child_page);
			printRecurse(child_page, level+1);
		}

	}
	cout << "\n========================================\n";
}
