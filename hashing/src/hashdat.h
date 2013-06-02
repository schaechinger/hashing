//
//  hashdat.h
//  hashing
//
//  Created by Manuel Schächinger on 14.05.13.
//  Copyright (c) 2013 Delivery Studios. All rights reserved.
//

#ifndef __hashing__hashdat__
#define __hashing__hashdat__

#include <iostream>

#include "accesspath.h"


template<int count>
class HashDat
{
private:
    // the index of the current bucket within the paths array
    large bucketIndex;
    // the object that holds all the paths
    AccessPath _accessPath;
    // the link to the data file
    std::fstream _stream;
    // the bucket that holds the currently needed data
    Bucket<count> _bucket;
    
    // load the paths from the path file
    void loadAccessPath();
        
public:
    // create a new hashdat object and initialize the default data
    HashDat(std::string filename);
    
    // insert a new client to file and update the access path
    void insert(Client client);
    // check if a client with the given id exists and return it
    Client retrieve(int clientId);
    // list all stored clients in the data file
    void show();
    
    // save the access paths to the path file
    void saveAccessPath();
    
    // the bad article exception is thrown if a client should be
    // inserted to the data file that is already stored in there
    class BadArticleException
    {
    public:
        BadArticleException()
        { }
        
        void description()
        {
            std::cout << "ERROR: The client already exists!" << std::endl;
        }
    };
    
    // the not found exception is trown if a client is requested
    // by an id that is not stored in the data file
    class NotFoundException
    {
    private:
        // the id of the not found client
        int _clientId;
        
    public:
        NotFoundException(int clientId)
        {
            _clientId = clientId;
        }
        
        void description()
        {
            std::cout << "ERROR: The client '" << _clientId
                      << "' was not found!" << std::endl;
        }
    };
};


using namespace std;

template<int count>
HashDat<count>::HashDat(string filename)
{
    // init the stream for reading and writing
    // set the flag to override the file
    _stream.open(filename.c_str(), ios::out | ios::in | ios::ate);
    
    if (!_stream.is_open())
    {
        _stream.open(filename.c_str(), ios::out | ios::in | ios::trunc);
    }
    
    bucketIndex = 0;
    _accessPath.open(filename.append(".ap"));
}


template<int count>
void HashDat<count>::insert(Client client)
{
    large hash = _bucket.hash(client.clientId());
    _bucket.read(_accessPath.offsetForHash(hash), _stream);
    
    // try to put the data in a free slot
    if (!_bucket.fillSlot(client))
    {
        // create new bucket for the overlapping slots
        Bucket<count> bucket;
        
        large index = _accessPath.indexForHash(hash);
        _stream.seekp(0, ios::end);
        _accessPath.splitBucket(index, _stream.tellp());
        
        // split the bucket
        for (int i = count - 1; i > 0; i--)
        {
            if (_accessPath.indexForHash(_bucket.hash(_bucket.slot(i).
                                                      clientId())) != index)
            {
                bucket.fillSlot(_bucket.remove(i));
            }
        }
        
        bool success;
        if (_accessPath.indexForHash(hash) != index)
        {
            success = bucket.fillSlot(client);
        }
        else
        {
            success = _bucket.fillSlot(client);
        }
        
        cout << "_bucket " << _bucket.filled() << " | bucket " << bucket.filled() << endl;
        
        large tempIndex = _stream.tellp();
        bucket.write(tempIndex, _stream);
        _bucket.write(bucketIndex, _stream);
        _bucket = bucket;
        bucketIndex = tempIndex;
        
        if (!success)
        {
            insert(client);
        }
    }
    else
    {
        _bucket.write(bucketIndex, _stream);
    }
}


template<int count>
Client HashDat<count>::retrieve(int clientId)
{
    large hash = _bucket.hash(clientId);
    
    _bucket.read(_accessPath.offsetForHash(hash), _stream);
    
    for (int i = 0; i < _bucket.filled(); i++)
    {
        if (_bucket.hash(_bucket.slot(i).clientId()) == hash)
        {
            return _bucket.slot(i);
        }
    }
    
    throw NotFoundException(clientId);
}


template<int count>
void HashDat<count>::show()
{
    int bucketSize = count * (2 * sizeof(int) + MAX_LENGTH);
    large offset = 0;
    large index = 0;
    
    while ((offset = _accessPath.offsetAtIndex(index++)) != -1)
    {
        _bucket.read(offset, _stream);
        for (int i = 0; i < _bucket.filled(); i++)
        {
            _bucket.slot(i).show();
        }
        offset += bucketSize;
    }
}


template<int count>
void HashDat<count>::saveAccessPath()
{
    _accessPath.save();
    _stream.close();
}

#endif /* defined(__hashing__hashdat__) */
