/* Main.cpp
description:
    module functioning as program entry point provides top level menus interactions
    calls mid level control module to perform program processes

version history:
ver4 -24/07/24, update by Puja Shah
     -added query control
ver3 -24/07/24, by Nicolao
     -all of module's print output is now cpp style instead of a mix between c style and cpp style
ver2 -24/07/16 update by Nicolao
     -small update to reflect changes in scm.h
ver1 -24/07/02 Original by Nicolao
*/

// included to print menus to screen and to collect user input and to select menu options
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <string>
using std::string;
using std::getline;

// mid level module handling input collection
// handles calls to lower level modules to interact with system database
#include "ScenarioControl.h"

// declaration of error messages used in main
const char *MAIN_OPTION_NOT_AVAILABLE = "The option that you have entered does not exist in the system.\n";

//==================

//program entry point
int main();

// menu and contol logic for main menu
int mainMenu();

// menu and contol logic for manage product menu
int manageProductsMenu();

// menu and control logic for generate reports menu
int generateReportsMenu();


//==================
//main function

int main()
{
    initControl();
    mainMenu();
    uninitControl();
    return 0;
}

//==================
// top level menus

// menu and contol logic for main menu
// implemented by simple switch statement
int mainMenu()
{   
    while (1)
    {
        cout << endl << endl;
        cout << "Main Menu:" << endl;  
        cout << "[1]  Handle Change Request" << endl;
        cout << "[2]  Update Change Item" << endl;
        cout << "[3]  Manage Products" << endl;
        cout << "[4]  Generate Reports" << endl;
        cout << "[0]  Exit" << endl;

        int userSelection = makeSelection();

        switch (userSelection)
        {
        case ('1'):
            // begin the process to add a request
            // may create a new item or requester if needed
            addRequestControl();
            break;

        case ('2'):
            // begin the process to update a change item
            // may mutate existing change items
            updItemControl();
            break;

        case ('3'):
            // sub menu to decide what product managing process to begin
            manageProductsMenu();
            break;

        case ('4'):
            // sub menu to decide what report to produce
            generateReportsMenu();
            break;

        case('0'):
            return 0;
            break;

        default:
            cout << MAIN_OPTION_NOT_AVAILABLE << endl;
            break;
        }
    }
}

//========

// menu and contol logic for manage product menu
// implemented by simple switch statement
int manageProductsMenu()
{
    while (1)
    {
        cout << endl << endl;
        cout << "Manage Products:" << endl;
        cout << "[1]  Create Product" << endl;
        cout << "[2]  Create Product Release" << endl;
        cout << "[0]  Back" << endl;

        int userSelection = makeSelection();
        switch (userSelection)
        {
        case ('1'):
            // process to add product
            if (addProductControl())
            {
                return 1;
            }            
            break;

        case ('2'):
            // process to add product relese
            if (addReleaseControl())
            {
                return 1;
            } 
            break;

        case('0'):
            return 0;

        default:
            cout << MAIN_OPTION_NOT_AVAILABLE << endl;
            break;
        }
    }
}

//========

// menu and control logic for generate reports menu
// implemented by simple switch statement
int generateReportsMenu()
{
    while (1)
    {
        cout << endl << endl;
        cout << "Generate Reports:" << endl;
        cout << "Select Report Type:" << endl;
        cout << "[1]  All Requesters That Have Requested Some Change" << endl;
        cout << "[2]  All Change Items of a Product Not Done or Cancelled" << endl;
        cout << "[3]  View Change Item" << endl;
        cout << "[0]  Back" << endl;

        int userSelection = makeSelection();
        switch (userSelection)
        {
        case ('1'):
            // process to generate a report of requesters who have requested some change item
            if (generateReportRequesters())
            {
                return 1;
            }   
            break;

        case ('2'):

            // process to generate a report of of a product's release
            if (generateReportItems())
            {
                return 1;
            }
            break;
        case ('3'):

            // process to see details for a specific change item
            if(queryItemControl())
            {
                return 1;
            }
            break;
        case('0'):
            return 0;
            break;

        default:
            cout << MAIN_OPTION_NOT_AVAILABLE << endl;
            break;
        }
    }
}

/*
=========================================================
CODING CONVENTION
=========================================================
The modules will be organised as so. The module name,
description, and version history will be written at the top
of each module file. Then include statements, then variables,
then structures, and finally prototypes. In a class, public 
variables, structures, and prototypes, will be written preceding
private variables, structures, and prototypes

The version history will follow these conventions:
-Will be written in descending chronological order; the most recent
revision will be written first.
-The first line of each entry will contain the version number,
the author, and the date of that version’s release as YY/MM/DD.

Function Prototypes will follow these conventions:
-Each variable will be written on its own line with a multi-line
comment written above, describing that variable, and stating whether
that variable is used as an input, output, or input and output of that
function.
-The function will have a multiline comment below the prototype providing
a description of the function, the preconditions, the postconditions
*/