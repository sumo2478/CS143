#include "BTreeNode.h"
#include <iostream>


using namespace std;


//============================================================================
// BTNode Implementation
//============================================================================
BTNode::BTNode()
{
	m_keycount = 0;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNode::getKeyCount()
{
	return m_keycount;
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNode::read(PageId pid, const PageFile& pf)
{
	RC err = pf.read(pid, buffer);

	int num_entries = 0;

	// Determine the number of keys in the node
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
	{

		RecordId rid;
		int key;
		readLeafEntry(i, key, rid);

		if (rid.pid == -1 && rid.sid == -1 && key == -1)
			break;

		num_entries++;
	}

	m_keycount = num_entries;

	return err;
}

/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNode::write(PageId pid, PageFile& pf)
{
	RC err = pf.write(pid, buffer);
	return err;
}

/**
* Looks up an entry and returns its key and rid
* @param eid[IN]  the entry number to read from
* @param key[OUT] the key from the slot
* @param rid[OUT] the recordid entry 
* @return 0 if successful. Return an error code if there is an error
*/
RC BTNode::readLeafEntry(int eid, int& key, RecordId& rid)
{
	if (eid > RECORDS_PER_PAGE)
		return RC_NO_SUCH_RECORD;

	int* p = (int*) buffer;
	p = p + (eid*RECORD_VALUE);

	key = *p;
	rid.pid = *(p + sizeof(int));
	rid.sid = *(p + 2*sizeof(int));
	return 0;
}

//TODO: REMOVE

void BTNode::printBuffer()
{
	cout << "Print Buffer ===================================\n";
	
	for (int i = 0; i < m_keycount; i++)
	{
		int* p = (int*) buffer;
		p = p + (i*RECORD_VALUE);

		cout << "[" << *p << "]";
		cout << "[" << *(p + sizeof(int)) << "]";
		cout << "[" << *(p + 2*sizeof(int)) << "]\n";
	}
	cout << "End Buffer =====================================\n";
}


//============================================================================
// BTLeafNode Implementation
//============================================================================

/**
* Creates a new BTLeaf Node
* @return 0 if successful.
*/
RC BTLeafNode::create()
{
		memset(buffer, -1, sizeof(buffer));
		m_keycount = 0;
}

/**
 * Inserts a (key, rid) pair to the memory for the node
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @param offset[IN] the eid position to insert the node into
 * @return void
 */
void BTLeafNode::insertNode(int key, const RecordId& rid, int offset)
{
	int* p = (int*) buffer;
	p = p + (offset*RECORD_VALUE);
	*p = key;
	*(p + 1*sizeof(int)) = rid.pid;
	*(p + 2*sizeof(int)) = rid.sid;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{ 
	// Check to make sure there is space in the node
	if (!incrementKey())
		return RC_NODE_FULL;

	int eid = -1;
	int status = locate(key, eid);

	// If the status is the error no such record then append pair to the end
	// Otherwise return status code error
	if (status < 0)
	{
		if (status == RC_NO_SUCH_RECORD)
			eid = m_keycount-1;
		else
			return status;
	}

	// TODO: Move to helper function if insert and split needs it?
	// Move all pairs past the insert position one record over
	// Determine the number of records to move ove
	int num_records = m_keycount - eid;
	for (int i = num_records; i > eid-1 ; i--)
	{
		// Extract values from this record
		int key = -1;
		RecordId rid;
		if ((status = readEntry(i, key, rid)) < 0)
			return status;

		// Insert the values into the correct position
		insertNode(key, rid, i + 1);
	}

	// Insert the new value into the correct position
	insertNode(key, rid, eid);

	return 0; 
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ return 0; }

/*
 * Find the entry whose key value is larger than or equal to searchKey
 * and output the eid (entry number) whose key value >= searchKey.
 * Remeber that all keys inside a B+tree node should be kept sorted.
 * @param searchKey[IN] the key to search for
 * @param eid[OUT] the entry number that contains a key larger than or equalty to searchKey
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{ 
	int err;

	// TODO: Change to binary search?
	for (int i = 0; i < m_keycount; i++)
	{
		int key = -1;
		RecordId rid;
		err = readEntry(i, key, rid);

		if (key >= searchKey)
		{
			eid = i;
			return err;
		}
	}

	return RC_NO_SUCH_RECORD; 
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	return BTNode::readLeafEntry(eid, key, rid);
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ return 0; }

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ return 0; }

bool BTLeafNode::incrementKey()
{
	if(m_keycount < RECORDS_PER_PAGE)
	{
		m_keycount++;
		return true;
	}
	else
		return false;
}

//============================================================================
// BTNonLeafNode Implementation
//============================================================================

/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ return 0; }
