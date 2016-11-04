//
//  main.cpp
//  hashing
//
//  Created by Manuel Schächinger on 06.05.13.
//  Copyright (c) 2013 Manuel Schächinger
//

#include <iostream>

#include "hashdat.h"


// the number of slots in one bucket
#define COUNT 1
// the index of exit in the menu
#define EXIT_CODE 4

using namespace std;

void menu()
{
	cout << "Hashing v" << VERSION << endl
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
    // default path for the data and access path files
    string path = "/Users/Manuel/Documents";
    
    string filename =  path.append("/hashfile.dat");
    
    // hash has access to all the data
    HashDat<COUNT> hash(filename);
    
    // the selected menu index
	int selection = -1;
    // the index of a client
    int clientId;
    // the discount of a client
    int discount;
    // the name of a client
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
        // show
        else if (selection == 3)
        {
            hash.show();
        }
        // exit
        else if (selection == EXIT_CODE)
        {
            
        }
        // clear
        else if (selection == 0)
        {
            system("clear");
            // display the menu
            menu();
        }
        // unknown index
        else
        {
            cout << "'" << selection << "' is not defined!" << endl;
        }
	}
    
	return 0;
}