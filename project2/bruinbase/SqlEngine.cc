/**
* Copyright (C) 2008 by The Regents of the University of California
* Redistribution of this file is permitted under the terms of the GNU
* Public License (GPL).
*
* @author Junghoo "John" Cho <cho AT cs.ucla.edu>
* @date 3/24/2008
*/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
  // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning
  IndexCursor cursor;

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  int useIndex = 0;

  // Try to open index
  BTreeIndex tree;
  rc = tree.open(table + ".idx", 'r');
  
  // Used to store ONLY "=" conditions for key
  vector<SelCond> eq_conditions;
  // Used to store index conditions and other conditions if available
  vector<SelCond> new_conditions;

  // These four variables keep track of the search bounds
 

  // low <= key if true
  // low < key if false
  int low = -1;
  bool greater_equal = false;
  
  // high >= key if true
  // high > key if false
  int high = -1;
  bool less_equal = false;

  // No index file found so go to no index 
  if (rc != 0)
  {
    goto no_index_needed;
  }
  // Go through conditions and look for key conditions
  // that we can use the index
  else
  {
    for (unsigned i = 0; i < cond.size(); i++)
    {
      // Found key condition
      if (cond[i].attr == 1)
      {
        int keyVal;
        switch (cond[i].comp)
        {
        case SelCond::EQ:
          eq_conditions.push_back(cond[i]);
          break;
        case SelCond::NE:
          new_conditions.push_back(cond[i]);
          useIndex++;
          break;
        case SelCond::GT:
          keyVal = atoi(cond[i].value);
          if (keyVal >= low || low == -1)
          {
            low = keyVal;
            greater_equal = false;
          }
          break;    
        case SelCond::LT:
          keyVal = atoi(cond[i].value);
          if ( keyVal <= high || high == -1)
          {
            high = keyVal;
            less_equal = false;
          }
          break;
        case SelCond::GE:
          keyVal = atoi(cond[i].value);
          if (keyVal > low || low == -1)
          {
            low = keyVal;
            greater_equal = true;
          }
          break;
        case SelCond::LE:
          keyVal = atoi(cond[i].value);
          if (keyVal < high || high == -1)
          {
            high = keyVal;
            less_equal = true;
          }
          break;
        }
      }
      // Encounter a non-key condition but keep for later
      else if(cond[i].attr == 2)
      {
        new_conditions.push_back(cond[i]);
      }
    } 
  }

  // Use index if NOT ONLY comprised of "not equal" <>
  if (useIndex == 0)
  {
    // Scan through vector of key = equal and see if there
    // are any conflicts, if there are, then reject the query
    // if there aren't then continue
    int previous_equal_value = -1;
    for(unsigned i = 0; i < eq_conditions.size(); i++) {
      int equal_value = atoi(eq_conditions[i].value);
      
      // The two equal values are not equal and is not the first equal
      // This means that there is a conflict in keys such that we are
      // searching for key = 81 and key = 100 at the same time
      if( equal_value != previous_equal_value && previous_equal_value != -1 )
      {
        count = 0;
        goto count_star;
      }
      // This means that the equal is out of the range
      else if((( equal_value < high && !less_equal || equal_value <= high && less_equal) || (high != -1)) && ((equal_value > low && !greater_equal || equal_value >= low && greater_equal) || low == -1))
      {
        count = 0;
        goto count_star;
      }
      previous_equal_value = equal_value;
    }
    
    // To speed up search if there is only one equal
    // Tighten the search bounds such that 
    unsigned size_eq = eq_conditions.size();
    if (size_eq > 0)
    {
      high = atoi(eq_conditions[0].value);
      greater_equal = true;
      
      low = atoi(eq_conditions[0].value);
      less_equal = true;
    }
    // Set the lower bound such that we can search
    // using the index
    if (low == -1)
    {
      low = 0;
    }
    count = 0;
    tree.locate(low, cursor);

    int readKey;
    RecordId readRid;
    rc = tree.readForward(cursor, readKey, readRid);

    // If there is a greater than condition,
    // skip forward until key is not equal to low
    if (!greater_equal)
    {
      while ( readKey == low && rc == 0)
      {
        rc = tree.readForward(cursor, readKey, readRid);
      }
    }
    if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
      fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
      return rc;
    }

    // Read the tuples until out of the range
    while (rc == 0 && (high == -1 || (readKey <= high && less_equal || readKey < high && !less_equal)))
    {
      // Only read values unless we are looking for count
      // then we do not need to read values
      if(attr != 4) {
        if ((rc = rf.read(readRid, key, value)) < 0)
        {
          fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
          goto exit_select;
        }
      }
      // Similar to the no_index_needed case
      // check the conditions on the tuple
      for (unsigned i = 0; i < new_conditions.size(); i++) {
        // compute the difference between the tuple value and the condition value
        switch (new_conditions[i].attr)
        {
        case 1:
          diff = key - atoi(new_conditions[i].value);
          break;
        case 2:
          diff = strcmp(value.c_str(), new_conditions[i].value);
          break;
        }

        // skip the tuple if any condition is not met
        switch (new_conditions[i].comp) {
        case SelCond::EQ:
          if (diff != 0) goto next_indexed_record;
          break;
        case SelCond::NE:
          if (diff == 0) goto next_indexed_record;
          break;
        case SelCond::GT:
          if (diff <= 0) goto next_indexed_record;
          break;
        case SelCond::LT:
          if (diff >= 0) goto next_indexed_record;
          break;
        case SelCond::GE:
          if (diff < 0) goto next_indexed_record;
          break;
        case SelCond::LE:
          if (diff > 0) goto next_indexed_record;
          break;
        }
      }

      // the condition is met for the tuple. 
      // increase matching tuple counter
      count++;

      // print the tuple 
      switch (attr) {
      case 1:  // SELECT key
        fprintf(stdout, "%d\n", key);
        break;
      case 2:  // SELECT value
        fprintf(stdout, "%s\n", value.c_str());
        break;
      case 3:  // SELECT *
        fprintf(stdout, "%d '%s'\n", key, value.c_str());
        break;
      }
      // Move on to the next value
next_indexed_record:
      rc = tree.readForward(cursor, readKey, readRid);
    }

    goto count_star;
  }

no_index_needed:
  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  // scan the table file from the beginning
  rid.pid = rid.sid = 0;
  count = 0;

  while (rid < rf.endRid()) {
    // Only read values unless we are looking for count
    // then we do not need to read values
    if(attr != 4) {  
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto exit_select;
      }
    }
    // check the conditions on the tuple
    for (unsigned i = 0; i < cond.size(); i++) {
      // compute the difference between the tuple value and the condition value
      switch (cond[i].attr) {
      case 1:
        diff = key - atoi(cond[i].value);
        break;
      case 2:
        diff = strcmp(value.c_str(), cond[i].value);
        break;
      }

      // skip the tuple if any condition is not met
      switch (cond[i].comp) {
      case SelCond::EQ:
        if (diff != 0) goto next_tuple;
        break;
      case SelCond::NE:
        if (diff == 0) goto next_tuple;
        break;
      case SelCond::GT:
        if (diff <= 0) goto next_tuple;
        break;
      case SelCond::LT:
        if (diff >= 0) goto next_tuple;
        break;
      case SelCond::GE:
        if (diff < 0) goto next_tuple;
        break;
      case SelCond::LE:
        if (diff > 0) goto next_tuple;
        break;
      }
    }

    // the condition is met for the tuple. 
    // increase matching tuple counter
    count++;

    // print the tuple 
    switch (attr) {
    case 1:  // SELECT key
      fprintf(stdout, "%d\n", key);
      break;
    case 2:  // SELECT value
      fprintf(stdout, "%s\n", value.c_str());
      break;
    case 3:  // SELECT *
      fprintf(stdout, "%d '%s'\n", key, value.c_str());
      break;
    }

    // move to the next tuple
next_tuple:
    ++rid;
  }
 count_star:
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
exit_select:
  tree.close();
  rf.close();
  
  return rc;

}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  // TODO: Implement index part

  RC rc = 0;

  // Open table for reading
  RecordFile record_file;
  record_file.open(table + ".tbl", 'w');

  // If there is an index file then we must also open the index file
  BTreeIndex b;
  if (index)
  {
    rc = b.open(table + ".idx", 'w');
    if (rc < 0)
    {
      b.close();
      return rc;
    }
  }

  // Read in the load file
  ifstream infile;
  string   line;

  infile.open(loadfile.c_str());

  if (infile.is_open())
  {
    // Enter each key value pair into the table
    while (getline(infile, line))
    {
      int key;
      string value;

      if (parseLoadLine(line, key, value) == 0)
      {
        RecordId rd = record_file.endRid();

        record_file.append(key, value, rd);

        // If index is used insert it into the index
        if (index)
        {
          rc = b.insert(key, rd);
          if (rc < 0)
          {
            b.close();
            return rc;
          }
        }
      }
    }
  }

  b.close();

  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
  const char *s;
  char        c;
  string::size_type loc;

  // ignore beginning white spaces
  c = *(s = line.c_str());
  while (c == ' ' || c == '\t') { c = *++s; }

  // get the integer key value
  key = atoi(s);

  // look for comma
  s = strchr(s, ',');
  if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

  // ignore white spaces
  do { c = *++s; } while (c == ' ' || c == '\t');

  // if there is nothing left, set the value to empty string
  if (c == 0) { 
    value.erase();
    return 0;
  }

  // is the value field delimited by ' or "?
  if (c == '\'' || c == '"') {
    s++;
  } else {
    c = '\n';
  }

  // get the value string
  value.assign(s);
  loc = value.find(c, 0);
  if (loc != string::npos) { value.erase(loc); }

  return 0;
}
