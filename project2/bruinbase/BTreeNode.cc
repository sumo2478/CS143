
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

/**
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNode::read(PageId pid, const PageFile& pf)
{
	RC err = pf.read(pid, buffer);
	
	// TODO: implement differently
	m_keycount = getKeyCount();

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

	Entry* e = (Entry*) buffer;
	e = e + eid;

	key = e->key;
	rid.pid = e->pid;
	rid.sid = e->sid;

	return 0;
}

//TODO: REMOVE

void BTNode::printBuffer()
{

	cout << "Print Buffer ===================================\n";
	for (int i = 0; i < m_keycount; i++)
	{
		int key = 0;
		RecordId rid;
		//readLeafEntry(i, key, rid);
		int *p = (int*) buffer;
		p++;
		p = p + i*2;
		key = *p;
		int pid = *(p + 1);


		cout << "[" << key << "]";
		cout << "[" << pid << "]\n";
		// cout << "[" << rid.pid << "]";
		// cout << "[" << rid.sid << "]\n";
	}
	cout << "End Buffer =====================================\n";
}


//============================================================================
// BTLeafNode Implementation
//============================================================================

BTLeafNode::BTLeafNode()
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
	Entry* e = (Entry*) buffer;
	e = e + offset;
	e->key = key;
	e->pid = rid.pid;
	e->sid = rid.sid;
}

/**
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{
	int num_entries = 0;

	// Determine the number of keys in the node
	for (int i = 0; i < RECORDS_PER_PAGE; i++)
	{

		RecordId rid;
		int key;
		BTNode::readLeafEntry(i, key, rid);

		if (rid.pid == -1 && rid.sid == -1 && key == -1)
			break;

		num_entries++;
	}

	return num_entries;
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
		// If no such record then append to the end of the node
		if (status == RC_NO_SUCH_RECORD)
		{
			insertNode(key, rid, m_keycount-1);
			return 0;
		}
		else
			return status;
	}

	
	// TODO: Move to helper function if insert and split needs it?
	// Move all pairs past the insert position one record over
	// Determine the number of records to move ove
	int num_records = m_keycount - eid;
	for (int i = m_keycount-1; i > eid ; i--)
	{
		// Extract values from this record
		int n_key = -1;
		RecordId rid;
		if ((status = readEntry(i-1, n_key, rid)) < 0)
			return status;

		// Insert the values into the correct position
		insertNode(n_key, rid, i);
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
{ 
	int error = 0;

	// If the node is not full then return an error
	if (m_keycount < RECORDS_PER_PAGE)
		return RC_INVALID_FILE_FORMAT;
	// If the sibling node is not empty then return an error
	else if(sibling.getKeyCount() != 0)
		return RC_INVALID_ATTRIBUTE;

	// The middle split point of the node
	int mid = (m_keycount + 1) / 2;
	
	// Set the sibling key to the first entry of the right node
	RecordId tmp;
	readEntry(mid, siblingKey, tmp);

	// Copy over the second half of the node to the sibling
	for (int i = mid; i < m_keycount; i++)
	{
		int record_key = -1;
		RecordId record_rid;

		if ((error = readEntry(i, record_key, record_rid)) < 0)
			return error;

		sibling.insert(record_key, record_rid);
	}

	// Set the rest of the node to empty
	char *p = buffer;
	p = p + mid*RECORD_VALUE;
	memset(p, -1, sizeof(buffer) / 2);

	// Set the keycount of the node
	m_keycount = getKeyCount();

	// Insert the record into the left leaf node if the value is less than 
	// The left most right node
	if (key < siblingKey)
		insert(key, rid);
	else
		sibling.insert(key, rid);

	// Set the new set next pointer
	sibling.setNextNodePtr(getNextNodePtr());

	return 0; 
}

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
{ 
	int* p = (int*) buffer;

	// Move pointer to sibling node position
	p = (p + RECORDS_PER_PAGE*RECORD_VALUE/sizeof(int));

	return *p;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	int* p = (int*) buffer;

	// Move pointer to sibling node position
	p = (p + RECORDS_PER_PAGE*RECORD_VALUE/sizeof(int));

	*p = pid;

	return 0; 
}

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


// Alex
//============================================================================
// BTNonLeafNode Implementation
//============================================================================

/**
* Creates a new BTNonLeaf Node
* @return 0 if successful.
*/
BTNonLeafNode::BTNonLeafNode()
{
	memset(buffer, -1, PageFile::PAGE_SIZE);
	m_keycount = 0;
}



/*
* Looks up an entry and returns its key
* @param eid[IN]  the entry number to read from
* @param key[OUT] the key from the slot
* @return 0 if successful. Return an error code if there is an error
*/
RC BTNonLeafNode::readNonLeafEntry(int eid, int& key)
{
	if (eid > SLOTS_PER_PAGE)
		return RC_FILE_SEEK_FAILED;

	int* p = (int*) buffer;

	// Skip the first PageId
	p += sizeof(PageId)/sizeof(int);
	
	p = p + (eid * KEY_PAGE/sizeof(int));

	key = *p;
	return 0;
}

int BTNonLeafNode::getKeyCount()
{
	int num_entries = 0;

	// Determine the number of keys in the node
	for (int i = 0; i < MAX_NONLEAF_KEYS; i++)
	{
		int key;
		readNonLeafEntry(i, key);
		if (key == -1)
			break;

		num_entries++;
	}

	return num_entries;
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{
	int current_size = getKeyCount() * KEY_PAGE; //getKeyCount() * KEY_PAGE;// + sizeof(PageId);
	//cout << "current_size is: " << current_size  << endl; 
	if(current_size >= PageFile::PAGE_SIZE - KEY_PAGE)
		return RC_NODE_FULL;

	char * buf = (char*) buffer;
	// Skip the first PageId so now we are pointed at the first index
	buf = buf + sizeof(PageId);
	int currentKey;
	int num_keys_before = 0;
	
	while(buf != NULL)
	{
		// Copy into the current key to what we are pointed to
		memcpy(&currentKey, buf, sizeof(int));
		// Key does not exist in buffer yet
		if(currentKey == -1)
			break;
		// Current Key is less than desired key
		// So need to increment by KEY_PAGE value
		if(currentKey < key)
		{
			// Keep track of Index Keys we have seen
			num_keys_before++;
			buf = buf + KEY_PAGE;
		}
		// Else we are in the right place
		else if(currentKey >= key) 
			break;
	}

	// Handle First case: key does not exist in buffer yet so
	// Append to the end where buf is pointing
	// TODO check if we going out of bounds
	if(currentKey == -1) {
		// Copy the key into the buffer
		memcpy(buf, &key, sizeof(int));
		// Increment to point now at PageId
		buf = buf + sizeof(int);
		memcpy(buf, &pid, sizeof(PageId));
		buf = buf + sizeof(PageId);

	}
	// There are values in the Node (not empty)
	// Inserting key inbetween some values
	if(currentKey != -1 && current_size != 0) {
		// Create a temp buff to hold all the values to be shifted down
		char* temp_buf = (char*) malloc(PageFile::PAGE_SIZE * sizeof(char));
		// Initilize every value to -1
		memset(temp_buf, -1, PageFile::PAGE_SIZE);
		// Copy over from buf to temp_buf buf all the values that we
		// want to shift down
		memcpy(temp_buf, buf, current_size - (KEY_PAGE * num_keys_before));
		memset(buf, -1, current_size - (KEY_PAGE * num_keys_before) );

		// Copy the key into the buffer
		memcpy(buf, &key, sizeof(int));
		// Increment to point now at PageId
		buf = buf + sizeof(int);
		memcpy(buf, &pid, sizeof(PageId));
		buf = buf + sizeof(PageId);
		// Copy back everything back from temp_buf buf
		memcpy(buf, temp_buf, current_size - (KEY_PAGE * num_keys_before));
		// Free the temp_buf
		free(temp_buf);
	}

	// Increment key count
	m_keycount++;

	return 0;
}

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
{
	int error = 0;
	int current_size = getKeyCount() * KEY_PAGE;

	// If the node is not full then return an error
	if (m_keycount < MAX_NONLEAF_KEYS)
		return RC_INVALID_FILE_FORMAT;
	
	// If the sibling node is not empty then return an error
	else if(sibling.getKeyCount() != 0)
		return RC_INVALID_ATTRIBUTE;
	
	// The middle split point of the node
	int mid = m_keycount/ 2;

	// Go through buffer
	char * buf = (char*) buffer;
	buf = buf + sizeof(PageId);

	// Go to the halfway point
	for(int i = 0; i < mid; i++) {
		buf = buf + KEY_PAGE;
	}

	// Set midkey node that will be extracted and put in parent node
	int *v = (int*) buf;
	midKey = *v;

	// Remember our place to delete later from buffer
	char * needDelete = (char*) buf;
	
	// Initializing the Sibling
	int sibling_first_key;
	PageId left, right;

	buf = buf + sizeof(int);
	left = *buf;
	
	buf = buf + sizeof(PageId);
	int*p = (int*) buf;
	sibling_first_key = *p;
	
	buf = buf + sizeof(int);
	right = *buf;
	buf = buf + sizeof(PageId);

	sibling.initializeRoot(left, sibling_first_key, right);

	// Copy all the Index and PageId into Sibling node.
	// Stop when hit a -1 in index
	while(buf != NULL)
	{
		int key2;
		PageId pid2;

		memcpy(&key2, buf, sizeof(int));
		if(key2 == -1)
			break;

		buf = buf + sizeof(int);
		memcpy(&pid2, buf, sizeof(PageId));

		buf = buf + sizeof(PageId);
		int status = sibling.insert(key2, pid2);
		if (status != 0)
		{
			return RC_FILE_WRITE_FAILED;
		}
	}

	// Delete old data from current node
	memset(needDelete, -1, current_size - (KEY_PAGE * (getKeyCount() / 2)));
	// Update key Count
	m_keycount = getKeyCount();

	// Insert new key into the original if it is less than middle key
	if(key < midKey)
	{
		int status = insert(key, pid);
		if (status != 0)
		{
			return RC_FILE_WRITE_FAILED;
		}
			
	}
	// Insert new key into new sibling if it is greater than or equal
	// to middle keye
	else
	{
		int status = sibling.insert(key, pid);
		if (status != 0)
		{
			return RC_FILE_WRITE_FAILED;
		}
	}

	return 0;
}
/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
	int current_size = getKeyCount() * KEY_PAGE;
	char* buf = (char*) buffer;
	int currentKey;

	// Skip the first Pid
	buf = buf + sizeof(PageId);
	

	int num_keys = getKeyCount();
	for (int i = 0; i < num_keys; i++)
	{
		memcpy(&currentKey, buf, sizeof(int));
		// If equal, then pid we want is in right
		if(currentKey == searchKey)
		{
			buf = buf + sizeof(int);
			memcpy(&pid, buf, sizeof(PageId));
			return 0;
		}
		// If the current value is smaller than searchKey
		// Continue looking
		if(currentKey < searchKey)
		{
			buf = buf + KEY_PAGE;
		}
		// If the current value is larger than search Key
		// Then we know that we should access the pid to the left
		else if(currentKey > searchKey)
		{
			buf = buf - sizeof(PageId);
			memcpy(&pid, buf, sizeof(PageId));
			return 0;
		}

	}
	// childPtr has to be in the last because greater than all indices
	buf = buf - sizeof(PageId);
	memcpy(&pid, buf, sizeof(PageId));

	return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{
	// TODO Add error code
	//if(m_keycount != 0)

	char * buf = (char*) buffer;
	// Copy the first pid
	memcpy(buf, &pid1, sizeof(PageId));
	// Copy the key
	buf = buf + sizeof(PageId);
	memcpy(buf, &key, sizeof(int));
	// Copy the second pid
	buf = buf + sizeof(int);
	memcpy(buf, &pid2, sizeof(PageId));
	m_keycount = 1;
	return 0;
}
