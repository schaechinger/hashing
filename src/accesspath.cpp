//
//  accesspath.cpp
//  hashing
//
//  Created by Manuel Schächinger on 15.05.13.
//  Copyright (c) 2013 Manuel Schächinger
//

#include "accesspath.h"

#include <math.h>


using namespace std;

AccessPath::AccessPath()
{
    _globalDepth = 0;
    _count = 0;
}


AccessPath::~AccessPath()
{
    delete [] _paths;
}


void AccessPath::open(string filename)
{
    // initialize the stream to load the file and override
    // the content if nessessary
    _stream.open(filename.c_str(), ios::in | ios::out | ios::ate);
    
    // if the file does not exist create a new one
    if (!_stream.is_open())
    {
        FILE *file = fopen(filename.c_str(), "w");
        fprintf(file, "");
        fclose(file);
        _stream.open(filename.c_str(), ios::in | ios::out | ios::ate);
    }
    // load existing paths
    else
    {
        load();
    }
}


large AccessPath::indexForHash(large hash)
{
    // there are now access paths
    if (_globalDepth == 0)
    {
        return 0;
    }
    else
    {
        if (LOG)
        {
            cout << "index: " << (hash & bitMask())
                 << " for hash: " << hash
                 << " | mask: " << bitMask()
                 << endl;
        }
        return (hash & bitMask());
    }
}


large AccessPath::indexForHashUsingLocalDepth(large hash, large oldIndex)
{
    // there are now access paths
    if (_globalDepth == 0)
    {
        return 0;
    }
    else
    {
        if (LOG)
        {
            cout << "index uld: " << (hash & bitMask(_paths[oldIndex].localDepth))
            << " for hash: " << hash
            << " | mask: " << bitMask(_paths[oldIndex].localDepth)
            << endl;
        }
        return (hash & bitMask(_paths[oldIndex].localDepth));
    }
}


large AccessPath::offsetAtIndex(large index)
{
    // the index is out of range
    if (index >= _count)
    {
        return -1;
    }
    else
    {
        return _paths[index].offset;
    }
}


large AccessPath::offsetForHash(large hash)
{
    // there is no access path
    if (_globalDepth == 0)
    {
        return 0;
    }
    else
    {
        return _paths[indexForHash(hash)].offset;
    }
}


void AccessPath::splitBucket(large index, large offset)
{
    if (_globalDepth == 0)
    {
        updatePath(0);
    }
    else
    {
        // increase the bucket's local depth
        _paths[index].localDepth++;
    }
    
    // reorganize the table if needed
    if (_paths[index].localDepth > _globalDepth)
    {
        reorganize();
    }
    
    // update the new path
    _paths[_count/2+index].offset = offset;
}


void AccessPath::updatePath(large index, large offset, long localDepth)
{
    // expand the paths array if the index is out of range
    if (index >= _count)
    {
        reorganize();
    }
    
    // create a new path
    Path p;
    p.offset = offset;
    p.localDepth = localDepth;
    _paths[index] = p;
}


void AccessPath::reorganize(long globalDepth)
{
    large oldSize = _count;
    
    // update the global depth if given
    if (globalDepth != -1)
    {
        _globalDepth = globalDepth;
    }
    // increate the global depth
    else
    {
        _globalDepth++;
    }
    
    // update the size of the paths array
    _count = exp2(_globalDepth);
    
    // init the first path if the array is set up new
    if (_globalDepth == 1)
    {
        _paths = new Path[1];
        updatePath(0);
        oldSize = 1;
    }
    
    // copy the content of the access path to a new array with the new length
    Path *ap = new Path[_count];
    for (large l = 0; l < _count; l++)
    {
        ap[l] = Path(_paths[l%oldSize]);
    }
    delete[] _paths;
    _paths = ap;
    
    if (LOG)
    {
        cout << "reorganized accessPath to d = " << _globalDepth << endl;
    }
}


void AccessPath::load()
{
    // init a buffer that holds chars for one path
    short size = sizeof(Path);
    char buf[size];
    
    // get the number of available paths in the file
    _stream.seekg(0, ios::end);
    large available = _stream.tellg();
    _stream.seekg(0, ios::beg);
    
    // no files available
    if (available == 0)
    {
        return;
    }
    
    // calculate the number of access path in the file
    _count = available / size;
    // calculate the global depth
    _globalDepth = log2l(_count);
    // create a new path array
    _paths = new Path[_count];
    // save the number of paths if it increases
    large count = _count;
    
    for (large l = 0; l < count; l++)
    {
        _stream.read(buf, size);
        
        long localDepth = 0;
        large offset = 0;
        for (int j = 0; j < sizeof(offset); j++)
        {
            if (j < sizeof(localDepth))
            {
                localDepth += (short) (buf[j] + 128)
                            << (sizeof(localDepth) - j - 1) * 8;
            }
            offset += (short) (buf[j+sizeof(localDepth)] + 128)
            << (sizeof(int) - j - 1) * 8;
        }
        
        updatePath(l, offset, localDepth);
    }
    
    if (LOG)
    {
        show();
    }
}


void AccessPath::save()
{
    // move the writing cursor the the beginning of the file
    _stream.seekp(0, ios::beg);
    
    short size = sizeof(Path);
    char buf[size];
    for (large index = 0; index < _count; index++)
    {
        Path p = _paths[index];
        
        for (int j = 0; j < sizeof(large); j++)
        {
            short localDepth = p.localDepth >> (sizeof(int) - j - 1) * 8;
            short offset = p.offset >> (sizeof(int) - j - 1) * 8;
            if (j < sizeof(p.localDepth))
            {
                buf[j] = (char) (localDepth - 128);
            }
            buf[j+sizeof(p.localDepth)] = (char) (offset - 128);
        }
        
        if (LOG)
        {
            cout << "SAVE " << p.offset << " :: " << buf << endl;
        }
        
        _stream.write(buf, size);
    }
    
    _stream.flush();
    _stream.close();
}


void AccessPath::show()
{
    cout << "accesspath output:" << endl;
    
    for (int i = 0; i < _count; i++)
    {
        cout << "  " << i << ": " << _paths[i].offset
             << " (" << _paths[i].localDepth << ")" << endl;
    }
}


large AccessPath::bitMask(long localDepth)
{
    // calculate the bitmask for the global depth
    if (localDepth == -1)
    {
        return (large) ((1 << _globalDepth) - 1);
    }
    // bitmask using the local depth
    else
    {
        return (large) ((1 << localDepth) - 1);
    }
}
