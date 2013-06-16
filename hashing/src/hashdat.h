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


// hashdat manages the access to the data file
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
        FILE *file = fopen(filename.c_str(), "w");
        fprintf(file, "");
        fclose(file);
        _stream.open(filename.c_str(), ios::out | ios::in | ios::ate);
    }
    
    bucketIndex = 0;
    _accessPath.open(filename.append(".ap"));
}


template<int count>
void HashDat<count>::insert(Client client)
{
    large hash = _bucket.hash(client.clientId());
    _bucket.read(_accessPath.offsetForHash(hash), _stream);
    
    // check if client exists in bucket
    for (int i = 0; i < _bucket.filled(); i++)
    {
        if (_bucket.slot(i).clientId() == client.clientId())
        {
            throw BadArticleException();
        }
    }
    
    // try to put the data in a free slot
    if (!_bucket.fillSlot(client))
    {
        // create new bucket for the overlapping slots
        Bucket<count> bucket;
        
        large index = _accessPath.indexForHash(hash);
        _stream.seekp(0, ios::end);
        _accessPath.splitBucket(index, _stream.tellp());
        
        // split the clients of the bucket
        for (int i = count - 1; i >= 0; i--)
        {
            if (_accessPath.indexForHashUsingLocalDepth(_bucket.hash(_bucket.slot(i).
                                                      clientId()), index) != index)
            {
                bucket.fillSlot(_bucket.remove(i));
            }
        }
        
        bool success;
        if (_accessPath.indexForHashUsingLocalDepth(hash, index) != index)
        {
            success = bucket.fillSlot(client);
        }
        else
        {
            success = _bucket.fillSlot(client);
        }
        
        if (LOG)
        {
            cout << "_bucket " << _bucket.filled() << " | bucket " << bucket.filled() << endl;
        }
        
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
    large offset = 0;
    large i = 0;
    bool first = true;
    
    while (true)
    {
        large localOffset = _accessPath.offsetAtIndex(i);
        
        if (localOffset == -1)
        {
            break;
        }
        
        cout << " --- bucket " << i++ << " ("
             << localOffset << ") ---" << endl;
        
        if (localOffset > offset || first)
        {
            offset = localOffset;
            _bucket.read(offset, _stream);
            
            for (int index = 0; index < _bucket.filled(); index++)
            {
                _bucket.slot(index).show();
            }
            
            first = false;
        }
        else
        {
            cout << "linker path" << endl << endl;
        }
    }
}


template<int count>
void HashDat<count>::saveAccessPath()
{
    _accessPath.save();
    _stream.close();
}

#endif /* defined(__hashing__hashdat__) */
