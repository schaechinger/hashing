//
//  accesspath.h
//  hashing
//
//  Created by Manuel Schächinger on 15.05.13.
//  Copyright (c) 2013 Delivery Studios. All rights reserved.
//

#ifndef __hashing__accesspath__
#define __hashing__accesspath__

#include <iostream>

#include <fstream>

#include "bucket.h"


// path holds the information about a bucket
struct Path
{
    // the local depth to identify the bucket
    long localDepth;
    // the offset of the bucket within the data file
    large offset;
};

// accessPath contains information about the offsets of all
// buckets in the data file
class AccessPath
{
private:
    // an array of path that holds the information
    Path *_paths;
    // the global depth to calculate the bucket index
    long _globalDepth;
    // the number of paths
    large _count;
    // the stream for the accessPath file
    std::fstream _stream;
    
    // load the accessPath information from the file
    void load();
    // double the size of the paths array and fill it with paths
    void reorganize(long globalDepth = -1);
    // display all entries of the access path in a list
    void show();
    // update a path with the given values
    void updatePath(large index, large offset = 0, long localDepth = 1);
    
public:
    // initialize the accessPath
    AccessPath();
    // destroys the access path object with the paths array
    ~AccessPath();

    // get the bitMask for the current global depth
    // or the local depth if given
    large bitMask(long localDepth = -1);
    // get the index for the given hash
    large indexForHash(large hash);
    // get the index for the given hash using the local depth
    // used to split a full bucket
    large indexForHashUsingLocalDepth(large hash, large oldIndex);
    // get the offset of the bucket at the given index
    large offsetAtIndex(large index);
    // get the offset of the bucket with the prefered data
    large offsetForHash(large hash);
    // open the file to read and save data
    void open(std::string filename);
    // save the whole accessPath to the file
    void save();
    // split the bucket with the given index and put overlapping
    // data to the new bucket at the defined offset
    void splitBucket(large index, large offset);
};

#endif /* defined(__hashing__accesspath__) */
