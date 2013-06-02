//
//  main.cpp
//  hashing
//
//  Created by Manuel Schächinger on 06.05.13.
//  Copyright (c) 2013 Delivery Studios. All rights reserved.
//

#include <iostream>

#include "hashdat.h"


#define COUNT 3
#define EXIT_CODE COUNT + 1

using namespace std;

void menu()
{
	cout << "Hashing" << endl
    << "┌───┬────menu───────┐" << endl
    << "│ 0 │ clear         │" << endl
    << "├───┼───────────────┤" << endl
    << "│ 1 │ insert        │" << endl
    << "│ 2 │ retrieve      │" << endl
    << "│ 3 │ display       │" << endl
    << "├───┼───────────────┤" << endl
    << "│ " << EXIT_CODE
       << " │ exit          │" << endl
    << "└───┴───────────────┘" << endl;
}


int main(int argc, const char * argv[])
{   
//    cout << "Filename: ";
    string filename = "/Users/Manuel/hashfile.txt";
//    cin >> filename;
    
    HashDat<COUNT> hash(filename);
    
	int selection = -1;
    int clientId;
    int discount;
    string name;
    
    // display the menu
    cout << endl;
    menu();
    
	while (selection != EXIT_CODE)
	{
		selection = -1;
        
        cout << endl << ">:";
		cin >> selection;
		
        // insert
        if (selection == 1)
        {
            cout << "name: ";
            cin >> name;
            cout << "id: ";
            cin >> clientId;
            cout << "discount: ";
            cin >> discount;
            
            Client client(clientId);
            client.setDiscount(discount);
            client.setName(name);
            
            try
            {
                hash.insert(client);
            }
            catch (HashDat<COUNT>::BadArticleException& bae)
            {
                bae.description();
            }
        }
        // retrieve
        else if (selection == 2)
        {
            cout << "id: ";
            cin >> clientId;
            
            try
            {
                hash.retrieve(clientId).show();
            }
            catch (HashDat<COUNT>::NotFoundException& nfe)
            {
                nfe.description();
            }
        }
        else if (selection == 3)
        {
            hash.show();
        }
        else if (selection == EXIT_CODE)
        {
            
        }
        else if (selection == 0)
        {
            system("clear");
            // display the menu
            menu();
        }
        else
        {
            cout << "'" << selection << "' is not defined!" << endl;
        }
	}
    
    hash.saveAccessPath();
    
	return 0;
}