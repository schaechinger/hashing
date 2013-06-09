//
//  bucket.h
//  hashing
//
//  Created by Manuel Schächinger on 14.05.13.
//  Copyright (c) 2013 Delivery Studios. All rights reserved.
//

#ifndef __hashing__bucket__
#define __hashing__bucket__

#include <iostream>

#include <fstream>
#include <math.h>

#include "client.h"
#include "helper.h"


// defines a new type to make it easy to change
// the maximum size of the addressroom
typedef long large;

// the maximum number of buckets depending on the size of large
#define b 2147483647
// the fibonaccy quotient
#define r (sqrt(5) - 1) / 2

// a bucket is a part of the hashfile that contains all
// the hashed data. It contains several slots for records.
template<int count>
class Bucket
{
private:
    // marks the next free slot
    int nextFree;
    // the array that contains the data
    Client _slots[count];
    
public:
    // creates a new bucket
    Bucket();
    
    // reads the bucket and the containing data from
    // the hashfile
    bool read(large l, std::fstream& stream);
    // writes the bucket with it's data to the hashfile
    void write(large l, std::fstream& stream);
    
    // retrieve the client at the given slot
    Client slot(int i);
    // add a client to the bucket
    bool fillSlot(Client client);
    // return the number of filled slots
    int filled();
    // remove the client in the given slot
    Client remove(int i);
    
    // reurn the hash value for the given key
    large hash(int k);
};


using namespace std;

template<int count>
Bucket<count>::Bucket()
{
    nextFree = 0;
}


template<int count>
bool Bucket<count>::read(large l, fstream& stream)
{
    stream.seekg(0, ios::end);
    if (stream.tellg() <= l)
    {
        return false;
    }
    
    // move to the beginning of the desired bucket
    stream.seekg(l, ios::beg);
    
    nextFree = 0;
    
    if (LOG)
    {
        cout << " --- read bucket (" << l << ") ---" << endl;
    }
    
    for (int i = 0; i < count; i++)
    {
        int size = MAX_LENGTH;
        char buf[size];
        stream.read(buf, 2 * sizeof(int));
        
        int clientId = 0;
        int discount = 0;
        
        for (int j = 0; j < sizeof(int); j++)
        {
            clientId += (short) (buf[j] + 128) << (sizeof(int) - j - 1) * 8;
            discount += (short) (buf[j+sizeof(int)] + 128)
                        << (sizeof(int) - j - 1) * 8;
        }
        
        stream.read(buf, size);
        string name = buf;
        
        Client client(clientId);
        client.setDiscount(discount);
        client.setName(name);
        
        fillSlot(client);
    }
    
    return true;
}


template<int count>
void Bucket<count>::write(large l, fstream& stream)
{
    if (LOG)
    {
        cout << " --- write bucket (" << l << ") ---" << endl;
    }
    
    // set the position of the stream to override the right bucket
    stream.seekp(l, ios::beg);
    
    int size = 2 * sizeof(int) + MAX_LENGTH;
    char buf[size];
    Client client;
    
    for (int i = 0; i < count; i++)
    {
        if (i >= nextFree)
        {
            for (int i = 0; i < 2 * sizeof(int); i++)
            {
                buf[i] = (char) -128;
            }
            for (int i = 0; i < MAX_LENGTH; i++)
            {
                buf[i+2*sizeof(int)] = 0;
            }
        }
        else
        {
            client = slot(i);
            
            for (int j = 0; j < sizeof(int); j++)
            {
                short clientId = client.clientId() >> (sizeof(int) - j - 1) * 8;
                short discount = client.discount() >> (sizeof(int) - j - 1) * 8;
                buf[j] = (char) (clientId - 128);
                buf[j+sizeof(int)] = (char) (discount - 128);
            }
            
            string name = client.name();
            for (int j = 0; j < MAX_LENGTH; j++)
            {
                char c = name[j];
                
                if (j >= strlen(name.c_str()))
                {
                    c = 0;
                }
                
                buf[j+2*sizeof(int)] = c;
            }
        }
        
        stream.write(buf, size);
        stream.flush();
    }
}


template<int count>
Client Bucket<count>::slot(int i)
{
    if (i >= count)
    {
        if (WARN)
        {
            cout << "WARNING: slot index too big!" << endl;
        }
        return NULL;
    }
    else if (i >= nextFree)
    {
        if (WARN)
        {
            cout << "WARNING: slot " << i << " is free!" << endl;
        }
        return NULL;
    }
    else
    {
        return _slots[i];
    }
}


template<int count>
bool Bucket<count>::fillSlot(Client client)
{
    // bucket is full
    if (nextFree == count)
    {
        if (WARN)
        {
            cout << "WARNING: no empty slot!" << endl;
        }
    }
    // client is empty
    else if (client.clientId() == 0 &&
             client.name() == "")
    {
        if (WARN)
        {
            cout << "WARNING: empty client!" << endl;
        }
    }
    else
    {
        _slots[nextFree++] = client;
        return true;
    }
    
    return false;
}


template<int count>
Client Bucket<count>::remove(int i)
{
    if (i >= nextFree)
    {
        return NULL;
    }
    
    Client client = _slots[i];
    for (int j = i + 1; j < nextFree; j++)
    {
        _slots[j-1] = _slots[j];
    }
    
    nextFree--;
    
    return client;
}


template<int count>
int Bucket<count>::filled()
{
    return nextFree;
}


template<int count>
large Bucket<count>::hash(int k)
{
    return (fmod(k * r, 1)) * b;
}

#endif /* defined(__hashing__bucket__) */
