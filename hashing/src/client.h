//
//  client.h
//  hashing
//
//  Created by Manuel Schächinger on 14.05.13.
//  Copyright (c) 2013 Delivery Studios. All rights reserved.
//

#ifndef __hashing__client__
#define __hashing__client__

#include <iostream>


#define MAX_LENGTH 50

// client represents the data of a persion with his name,
// client id and discount.
class Client
{
private:
    // the name of the client with up to 50 characters
    std::string _name;
    // the unique id of the client
    int _clientId;
    // the current discount for the client
    int _discount;
    
public:
    // creates a new client with the default id
    Client();
    // creates a new client object with his id
    Client(int clientId);
    
    // returns the name of the client
    std::string name();
    // replaces the name of the client and shrinks it
    // to the maximum number of characters if necessary
    void setName(std::string value);
    // returns the client's id
    int clientId();
    // returns the discount of the client
    int discount();
    // sets the discount of the client
    void setDiscount(int value);
    // displays all the information about a client
    void show();
    // check if the client has data
    bool isEmpty();
};

#endif /* defined(__hashing__client__) */
