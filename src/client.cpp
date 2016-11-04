//
//  client.cpp
//  hashing
//
//  Created by Manuel Schächinger on 14.05.13.
//  Copyright (c) 2013 Manuel Schächinger
//

#include "client.h"


using namespace std;

Client::Client() : Client::Client(0)
{
    
}


Client::Client(int clientId)
{
    _clientId = clientId;

    _name = "";
    _discount = 0;
}


string Client::name()
{
    return _name;
}


void Client::setName(string value)
{
    if (value.length() > MAX_LENGTH)
    {
        _name = value.substr(0, MAX_LENGTH);
        cout << "WARNING: name was cut!" << endl;
    }
    else
    {
        _name = value;
    }
}


int Client::clientId()
{
    return _clientId;
}


int Client::discount()
{
    return _discount;
}


void Client::setDiscount(int value)
{
    _discount = value;
}


void Client::show()
{
    cout << "Client: " << _name << endl
         << "#" << _clientId << endl
         << "discount: " << _discount << endl
         << endl;
}
