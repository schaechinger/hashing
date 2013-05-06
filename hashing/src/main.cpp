//
//  main.cpp
//  hashing
//
//  Created by Manuel Schächinger on 06.05.13.
//  Copyright (c) 2013 Delivery Studios. All rights reserved.
//

#include <iostream>


#define EXIT_CODE 8

using namespace std;

void menu()
{
	cout << "Hashing" << endl
    << "┌───┬────menu───────┐" << endl
    << "│ 0 │ clear         │" << endl
    << "├───┼───────────────┤" << endl
    << "│   │               │" << endl
    << "├───┼───────────────┤" << endl
    << "│ " << EXIT_CODE
       << " │ exit          │" << endl
    << "└───┴───────────────┘" << endl;
}


int main(int argc, const char * argv[])
{
	int selection;
	
	while (selection != EXIT_CODE)
	{
		selection = -1;
        
		// display the menu
		cout << endl;
		menu();
		
        cout << ">:";
		cin >> selection;
		
		switch (selection)
        {
			case 1:
				break;
                // exit
			case EXIT_CODE:
				break;
            // clear display
			case 0:
				system("clear");
				break;
			default:
				cout << "'" << selection << "' is not defined!" << endl;
				break;
		}
	}
	
	return 0;
}