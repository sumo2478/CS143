/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 5/28/2008
 */

#ifndef BTNODE_H
#define BTNODE_H

#include "RecordFile.h"
#include "PageFile.h"
#include "Bruinbase.h"

typedef struct {
  int     key;  // key for that entry
  PageId  pid;  // page number. the first page is 0
  int     sid;  // slot number. the first slot is 0
} Entry;

 
/**
 * BTNode: The base class representing a B+tree node
 */
class BTNode {
  public:
    BTNode();

    /**
    * Insert the (key, rid) pair to the node.
    * Remember that all keys inside a B+tree node should be kept sorted.
    * @param key[IN] the key to insert
    * @param rid[IN] the RecordId to insert
    * @return 0 if successful. Return an error code if the node is full.
    */

    // TODO make this not virtual because nonleaf is slightly different
    
    RC insert(int key, const RecordId& rid);
    RC insert(int key, PageId pid);

    /**
    * Return the number of keys stored in the node.
    * @return the number of keys in the node
    */
    virtual int getKeyCount() = 0; 

    /**
    * Read the content of the node from the page pid in the PageFile pf.
    * @param pid[IN] the PageId to read
    * @param pf[IN] PageFile to read from
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC read(PageId pid, const PageFile& pf);

    /**
    * Write the content of the node to the page pid in the PageFile pf.
    * @param pid[IN] the PageId to write to
    * @param pf[IN] PageFile to write to
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC write(PageId pid, PageFile& pf);
    
    // TODO: Remove
    void printBuffer(); // Prints the contents of the buffer for testing
  protected:
    /**
    * Looks up an entry and returns its key and rid
    * @param eid[IN]  the entry number to read from
    * @param key[OUT] the key from the slot
    * @param rid[OUT] the recordid entry 
    * @return 0 if successful. Return an error code if there is an error
    */
    RC readLeafEntry(int eid, int& key, RecordId& rid);


    /**
    * The main memory buffer for loading the content of the disk page 
    * that contains the node.
    */
     char buffer[PageFile::PAGE_SIZE];
    //char buffer[124];

    static const int RECORD_VALUE = sizeof(Entry);
    static const int KEY_PAGE = sizeof(int) + sizeof(PageId);
    static const int RECORDS_PER_PAGE = (PageFile::PAGE_SIZE - sizeof(PageId)) / RECORD_VALUE;
    static const int SLOTS_PER_PAGE = (PageFile::PAGE_SIZE - 2 * sizeof(PageId)) / KEY_PAGE;
    static const int MAX_NONLEAF_KEYS = (PageFile::PAGE_SIZE - sizeof(PageId))/ KEY_PAGE;
   // static const int RECORDS_PER_PAGE = (124 - sizeof(PageId)) / RECORD_VALUE;

    int m_keycount;


  private:


};


/**
 * BTLeafNode: The class representing a B+tree leaf node.
 */
class BTLeafNode: public BTNode {
  public:

    BTLeafNode();

    /**
    * Insert the (key, rid) pair to the node.
    * Remember that all keys inside a B+tree node should be kept sorted.
    * @param key[IN] the key to insert
    * @param rid[IN] the RecordId to insert
    * @return 0 if successful. Return an error code if the node is full.
    */
    RC insert(int key, const RecordId& rid);

    /**
    * Return the number of keys stored in the node.
    * @return the number of keys in the node
    */
    int getKeyCount(); 


    /**
    * Insert the (key, rid) pair to the node
    * and split the node half and half with sibling.
    * The first key of the sibling node is returned in siblingKey.
    * Remember that all keys inside a B+tree node should be kept sorted.
    * @param key[IN] the key to insert.
    * @param rid[IN] the RecordId to insert.
    * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
    * @param siblingKey[OUT] the first key in the sibling node after split.
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC insertAndSplit(int key, const RecordId& rid, BTLeafNode& sibling, int& siblingKey);

   /**
    * Find the index entry whose key value is larger than or equal to searchKey
    * and output the eid (entry id) whose key value &gt;= searchKey.
    * Remember that keys inside a B+tree node are sorted.
    * @param searchKey[IN] the key to search for.
    * @param eid[OUT] the entry number that contains a key larger              
    *                 than or equalty to searchKey.
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC locate(int searchKey, int& eid);

   /**
    * Read the (key, rid) pair from the eid entry.
    * @param eid[IN] the entry number to read the (key, rid) pair from
    * @param key[OUT] the key from the slot
    * @param rid[OUT] the RecordId from the slot
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC readEntry(int eid, int& key, RecordId& rid);

   /**
    * Return the pid of the next slibling node.
    * @return the PageId of the next sibling node 
    */
   PageId getNextNodePtr();

   /**
    * Set the next slibling node PageId.
    * @param pid[IN] the PageId of the next sibling node 
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC setNextNodePtr(PageId pid);
    bool incrementKey(); 
  private:
    /**
    * Compares the current key count with the maximum number of records a
    * a page can hold and increments key count if possible
    * @return true if key was incremented. False otherwise
    */
    

    /**
     * Inserts a (key, rid) pair to the memory for the node
     * @param key[IN] the key to insert
     * @param rid[IN] the RecordId to insert
     * @param offset[IN] the eid position to insert the node into
     * @return void
     */
    void insertNode(int key, const RecordId& rid, int offset);

}; 


/**
 * BTNonLeafNode: The class representing a B+tree nonleaf node.
 */
class BTNonLeafNode: public BTNode {
  public:
    BTNonLeafNode();
   /**
    * Insert a (key, pid) pair to the node.
    * Remember that all keys inside a B+tree node should be kept sorted.
    * @param key[IN] the key to insert
    * @param pid[IN] the PageId to insert
    * @return 0 if successful. Return an error code if the node is full.
    */
    RC insert(int key, PageId pid);

    /*
    * Looks up an entry and returns its key
    * @param eid[IN]  the entry number to read from
    * @param key[OUT] the key from the slot
    * @return 0 if successful. Return an error code if there is an error
    */
    RC readNonLeafEntry(int eid, int& key);
   /**
    * Insert the (key, pid) pair to the node
    * and split the node half and half with sibling.
    * The sibling node MUST be empty when this function is called.
    * The middle key after the split is returned in midKey.
    * Remember that all keys inside a B+tree node should be kept sorted.
    * @param key[IN] the key to insert
    * @param pid[IN] the PageId to insert
    * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
    * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey);

   /**
    * Given the searchKey, find the child-node pointer to follow and
    * output it in pid.
    * Remember that the keys inside a B+tree node are sorted.
    * @param searchKey[IN] the searchKey that is being looked up.
    * @param pid[OUT] the pointer to the child node to follow.
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC locateChildPtr(int searchKey, PageId& pid);

   /**
    * Initialize the root node with (pid1, key, pid2).
    * @param pid1[IN] the first PageId to insert
    * @param key[IN] the key that should be inserted between the two PageIds
    * @param pid2[IN] the PageId to insert behind the key
    * @return 0 if successful. Return an error code if there is an error.
    */
    RC initializeRoot(PageId pid1, int key, PageId pid2);

    int getKeyCount();

}; 

#endif /* BTNODE_H */
