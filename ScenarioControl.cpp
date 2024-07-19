/*
Scenario Control.cpp
description : A mid level module that connect the main-menu user interface with lower-level modules that deal with databases.
              Guides the user through the process of completing their main menu selection
    ver 1: added helper function and implementations for generateReportItems(), generateReportRequester() and updItemControl()
        originally by Puja
    
*/

#include "ScenarioControl.h"

    const char* OPTION_NOT_AVAILABLE = "The option that you have entered does not exist in the system.\n";
    const char* EXCEEDS_BOUNDS = "The value that you have entered exceeds the bounds set by the data format requirements.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n"; 
    const char* BELOW_BOUNDS = "The value that you have entered is below the bounds set by the data format requirements.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n";
    const char* INCORRECT_CHARACTER_TYPE = "The value that you have entered does not meet the character type requirements for the given field.\n\nEnter Y to try again. Enter N to return to the Main Menu.\n";
    const char* CHANGE_ITEM_STATUS_ERROR = "The change item's status cannot be changed out of its status.\n\nEnter N to return to the Main Menu.\n";
    const char* REQEUSTER_UNIQUENESS_ERROR = "The requester already exists.\n\nEnter Y to continue creating a change request. Enter N to return to the main menu.\n";
    const char* PRODUCT_UNIQUENESS_ERROR = "The product already exists.\n\nEnter Y to try again. Enter N to return to the main menu.\n";
    const char* PRODUCT_RELEASE_UNIQUENESS_ERROR = "The product release already exists.\n\nEnter Y to try again. Enter N to return to the main menu.\n";




/*
Description: Initializes the Scenario Control module by initializing each of the lower level modules it calls
*/
void initControl(){
    // initializes all lower level modules

    Product::initProduct();
    Release::initRelease();
    RequesterDatabase::init();
    ChangeItemDatabase::init();
    ChangeRequestDatabase::init();

    // sets initialized to true
    initialized = true;

 }  

/*
Uninitializes the Scenario Control Module by uninitializing each of the lower level modules it used
*/
void uninitControl(){

    //uninitializes all lower level modules 
    Product::uninitProduct();
    Release::uninitRelease();
    RequesterDatabase::uninit();
    ChangeItemDatabase::uninit();
    ChangeRequestDatabase::uninit();

    //set initialize to false
    initialized = false;
}



    /*
    Description: function that allows the user to select a product from the product database
    returns a 16-bit integer whose first 8 bits are the product selection and lower 8 bits are the number of prints in that that run (out of 16)
    returns selection value 0 for back, -1 for back to main, and between 1 - 16 for anything valid.
    return values are meant to be viable arguments for an database::select() function that may be called outside the function
    */

    int16_t selectProduct(){
        //the value to be returned:

        int16_t selNind = 0;

        //create a product
        product readInto;
        bool selected = false;
        bool lastPage = false;
        int reqSelection = 0;  // default is back
        int entries = 0;

        // loop runs until a selection has been made: 

        while(!selected){ 
            //store the selection as a character array (maximum 2 values and a null)
            char selection[3];
            entries = 0;

            // make sure the file has not reached the end / is not empty before looping 15 more times. check each time for file reaching the end
            std::cout << "Generate Reports:" << std::endl ;
            std::cout << "Select a Change Item From What Product?" << std::endl ;
            std:: cout << "    Product Name:" << std::endl;

            if (Product::getNext(readInto)) {  
                // if the file has more entries: 
                 // initialize to 0 before every run 
                entries= 0;
                productReportShow(entries+1, readInto);
                entries++;


                // loop 15 times or until the file ends
                while (Product::getNext(readInto) && entries <= MAX_PRINTS) {
                    productReportShow(entries+1, readInto);
                    entries++;
                }



                // if the loop ended when before 16 entries had been printed, it ended because the file ended. So then this is the last page of the file
                if(entries < MAX_PRINTS) lastPage = true;

                // will be true until the response needs to keep being processed (is invalid)
                bool responseProcessed = true;

                // while user input needs to keep getting taken in :
                while(responseProcessed){
                    //ask for selection
                    std::cout << "Selection: ";
                    std::cin >> selection;

                    // if they wish to go back to the main menu, return -1. 
                    if(selection == "00"){
                    // back to the main menu : first 8 bits are -1 and the next 8 bits (irrelevant  in this case) are the number of entries printed
                        selNind |= (static_cast<int16_t>(-1) & 0xFF) << 8 ;
                        selNind |= (static_cast<int16_t>(entries) & 0xFF);
                        return selNind;
                            } 
                    else{
                        try{
                            // if it is not back to main menu ,try converting it into a number
                            reqSelection = atoi(selection);
                            // if the number is valid (between 0 and 16) return it
                            if(reqSelection >= 0 && reqSelection <= 16){
                                selNind |= (static_cast<int16_t>(reqSelection) & 0xFF) << 8 ;
                                selNind |= (static_cast<int16_t>(entries) & 0xFF);
                                return selNind;
                            }
                            // if the number is invalid, show error and ask for input again
                            else{
                                 std::cout << OPTION_NOT_AVAILABLE << std::endl;
                                 responseProcessed = true;
                            }
                                }
                        // if it is not a number:
                        catch(const std::exception& e){
                            // if it is continue and the file is not on its last page, exit the user input loop and print the next 16 entries. 
                            if(selection == "C" && !lastPage){
                                //selected = false;
                                responseProcessed = false;
                            }
                            // in all other cases, indicate that the response is not appropriate and ask for it again
                             else{
                                std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                                responseProcessed = true;
                                }
                        }
                    }

                }
                        
                    
            }
            // if the file could never get a single entry read, the database must be empty. show that nessaged and return 0 to show -- same as back
            std::cout << "Empty database" << std::endl;
            return selNind;
        } 
        
        // once selection is true, return the value + entries:
        selNind |= (static_cast<int16_t>(reqSelection) & 0xFF) << 8 ;
        selNind |= (static_cast<int16_t>(entries) & 0xFF) ;
        return selNind;
    }
    
    /*
    function that takes in a filter item and allows user to choose products from a list of all products that match the filter
    returns a 16 bits value where the first 8 bits are the selection and the lower 8 the number of entries printed for selection in that round
    selections of 0 and -1 indicate back and main menu respectively.
    the main use case for this function's return values is as arguments for any databse::select() functions that may be called after
    */

    int16_t selectItem(change_item& filter){
        // the change item into which each read item will be stored:
        change_item readInto;

        // value to be returned:
        int64_t selNInd = 0;

        // while loop conditions
        bool selected = false;
        bool lastPage = false;
        // default return value = back
        int8_t reqSelection = 0; 
        int8_t entries = 0;

        // loop that runs until a selection has been made.
        while(!selected){
            char selection[3];
            entries = 0;
            // make sure the file has not reached the end before looping 15 more times
            std::cout << "Generate Reports:" << std::endl;
            std::cout << "Generate Report for Which Change Item?:" << std::endl;
            std:: cout << "ID      Description                     Status       Release" << std::endl;

            if (ChangeItemDatabase::getNext(readInto, filter)) { 
                entries = 0; 
                itemReportShow(entries+1, readInto);
                entries++;
                

                while (ChangeItemDatabase::getNext(readInto, filter) && entries < MAX_PRINTS) {
                    itemReportShow(entries+1, readInto);
                    entries++;
                }


                // if it stops printing but has not reached 16 values yet, the file has ended.
                if(entries < MAX_PRINTS) lastPage = true;

                bool responseProcessed = true;
                // while responseProcessed is true, this loops keeps running

                while(responseProcessed){
                    //User makes a selection
                    std::cout << "Selection: ";
                    std::cin >> selection;

                    // If they select going back to the main menu, return -1 as the selection
                    if(selection == "00"){
                        selNInd |= (static_cast<int16_t>(-1) & 0xFF) << 8 ;
                        selNInd |= (static_cast<int16_t>(entries) & 0xFF) ;
                        return selNInd;// back to the main menu;
                        } 
                    else{
                        // if it is not -1
                        try{
                            // try converting it to an integer
                            // if that is true, selection has been made. 
                        reqSelection = atoi(selection);
                        if(reqSelection >= 0 && reqSelection <= 16){
                            selected = true;
                            responseProcessed = false;
                        }
                        else{
                            responseProcessed = true;
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                        }
                            }
                            // if it cannot be converted to a number
                        catch(const std::exception& e){
                            // if we are not on the last page and there are more entries to be read, and they say continue, exit this loop and print the next 16
                            if(selection == "C" && !lastPage){
                                // we are printing the next values but a selection has not been made yet
                                selected = false;
                                responseProcessed = false;
                            } 
                            // in all other cases stay in loop and ask for input again
                            else{
                                    std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                                }
                            }
                        }

                    }
                    // return selection if made
                    selNInd |= (static_cast<int16_t>(reqSelection) & 0xFF) << 8 ;
                    selNInd |= (static_cast<int16_t>(entries) & 0xFF);
                    return selNInd;
                    
                
                } else{
                    // if the loop never runs, the database is empty
                    std::cout << "Empty of Database" << std::endl;
                    return selNInd;
                }
            } 

        // once selection has been made, return it:
        selNInd |= (static_cast<int16_t>(reqSelection) & 0xFF) << 8 ;
        selNInd |= (static_cast<int16_t>(entries) & 0xFF);
        return selNInd;
    }

    /*
    Desription: Function that shows users a list of all requesters for change request with a given change item id. (filtered by chage id)
    returns 0 for back and -1 for back to main menu
    */
    int listOfRequesters(change_request& filterId){
        // create a requester 
        change_request aRequest;
        bool fileEnded = false;
        while (!fileEnded) {
                            
            int count = 0;

            // print up to 16 reports:
            std::cout << "Release Date: “RELEASE DATE”  Product: “PRODUCT”  Release: “RELEASE”" << std::endl;
            std::cout << "    Name                            Phone            Email" << std::endl;
            if(ChangeRequestDatabase::getNext(aRequest, filterId)){
                requester a;
                requester afil;
                afil.requesterId = aRequest.requesterId;
                RequesterDatabase::getNext(a, afil);
                count = 0;
                requesterReportShow(count + 1, afil);
                count++;

                    // print 15 more times or until eof is reached
                    while (count < 16 && ChangeRequestDatabase::getNext(aRequest, filterId)) {
                        requester a;
                        requester afil;
                        afil.requesterId = aRequest.requesterId;
                        RequesterDatabase::getNext(a, afil);
                        requesterReportShow(count + 1, afil);
                        count++;
                    }

                    // Check if the inner loop ended due to reaching the end of the file:
                    if (count < 16) {
                        fileEnded = true;
                    }

                    bool selected = false;
                    char selection[3];
                    int reqSelection;
                    bool responseProcessed = true;

                    // remains true until response needs to be processed
                    while(responseProcessed){

                        std::cout << "Selection: ";
                        std::cin >> selection;

                        if(selection == "00"){
                        // back to the main menu
                            return -1;
                                } 
                        else{
                            try{
                                // try converting to number - only valid option is back
                                reqSelection = atoi(selection);
                                if(reqSelection == 0){
                                    return reqSelection;
                                    // for all other cases - show error and prompt user to respond again
                                } else{
                                    std::cout << OPTION_NOT_AVAILABLE << std::endl;
                                }


                                    }

                            catch(const std::exception& e){
                                // if they want to see 16 more requesters, and it is not the end of the file, do so
                                if(selection == "C" && !fileEnded){
                                    responseProcessed = false;
                                }
                                // for all other options show error message and reprompt
                                else{
                                    std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                                    }
                            }
                        }

                    }
            }



        }
        // if the database was empty, back to the main menu  (cannot generate a report for requesters if there are no requesters)
        std::cout << "Empty Database" << std::endl;
        return -1;

    }


/*
Description: function that formats a requester for when listing them in a report
Fixed length prints that include their name, email, and phone number
Also display the item number on the left -- so as to make it easy for the user to choose a number when displayed in a list
*/

void requesterReportShow(int number,requester& requester) {
    char nameTruncated[MAX_REQUESTER_NAME_SIZE];
    strncpy(nameTruncated, requester.name, MAX_REQUESTER_NAME_SIZE - 1);
    nameTruncated[MAX_REQUESTER_NAME_SIZE - 1] = '\0'; // Ensure null-termination

    char phoneTruncated[PHONE_NUMBER_SIZE];
    strncpy(phoneTruncated, requester.phone, PHONE_NUMBER_SIZE - 1);
    phoneTruncated[PHONE_NUMBER_SIZE - 1] = '\0'; // Ensure null-termination

    char emailTruncated[MAX_EMAIL_SIZE];
    strncpy(emailTruncated, requester.email, MAX_EMAIL_SIZE - 1);
    emailTruncated[MAX_EMAIL_SIZE - 1] = '\0'; // Ensure null-termination

    // Print formatted output with number in square brackets
    std::cout << "[" << number << "] "
              << std::left << std::setw(MAX_REQUESTER_NAME_SIZE) << nameTruncated
              << std::setw(PHONE_NUMBER_SIZE) << phoneTruncated
              << std::setw(MAX_EMAIL_SIZE) << emailTruncated << std::endl;
}



/*
Description: function that formats a product before printing it in a list for users to choose from
*/

    void productReportShow(int number, product& product){
        // formatting for printing
        
        char nameTruncated[MAX_PRODUCT_NAME_SIZE];
        strncpy(nameTruncated, product.name, MAX_PRODUCT_NAME_SIZE - 1);
        nameTruncated[MAX_PRODUCT_NAME_SIZE - 1] = '\0'; // Ensure null-termination

        std::cout << "[" << number << "] "
                  << std::left << std::setw(MAX_REQUESTER_NAME_SIZE) << nameTruncated << std::endl;
    }



/*
Description: function that format a change item it takes in as an argument and prints out its change id, description, status, and release id
Allows for a number to the left to help with printing as a list a user can select from in a list
*/

    // formatting for printing
    void itemReportShow(int number, change_item& item){
        char descTruncated[MAX_DESCRIPTION_SIZE];
        strncpy(descTruncated, item.description, MAX_DESCRIPTION_SIZE - 1);
        descTruncated[MAX_DESCRIPTION_SIZE - 1] = '\0'; // Ensure null-termination

        char relTruncated[MAX_RELEASE_ID_SIZE];
        strncpy(relTruncated, item.release, MAX_RELEASE_ID_SIZE - 1);
        relTruncated[MAX_RELEASE_ID_SIZE - 1] = '\0'; // Ensure null-termination

        // Print formatted output
        std::cout << "[" << number << "] "
                    << std::left << std::setw(ID_DIGITS) << item.id
                    << std::setw(MAX_DESCRIPTION_SIZE) << descTruncated
                    << std::setw(STATUS_DIGITS) << item.status
                    << std::setw(MAX_RELEASE_ID_SIZE) << relTruncated << std::endl;
    }
    
    /*
    Description: prints all fields of a for the user to see, one line per attribute
    */
   
    void itemConfirmationShow(change_item& selectedItem){
         // formatting for printing
        std::cout << "Generate Reports:" << std::endl;
        std::cout << "Change Item: " << selectedItem.id << std::endl;
        std::cout << "Product:  " << selectedItem.product << std::endl;
        std::cout << "Status: "<< selectedItem.status <<  std::endl;
        std::cout << "Description: "<< selectedItem.description << std::endl;
        std::cout << "Priority: "<< selectedItem.priority <<std::endl;
        std::cout<< "Release: "<< selectedItem.release << std::endl;
        release corresponding;
        release filtering;
        strncpy(filtering.name, selectedItem.release, MAX_RELEASE_ID_SIZE);
        Release::getNext(corresponding, filtering);
        std::cout << "Release Date: "<< corresponding.date <<std::endl;
        std::cout << "Create a Report for Change Item " << selectedItem.id << "?" << std::endl;
    }
    

    /*
    Description: updates status of a given change item
    no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
    no back to main menu option
    */
    void updateStatus(change_item& item){
        
        while(1){
            // status options:
            std::cout << "New Status: " << std::endl;
            std::cout << "[1] Reviewed " << std::endl;
            std::cout << "[2] Inprogress " << std::endl;
            std::cout << "[3] Done " << std::endl;
            std::cout << "[4] Cancelled " << std::endl;
            std::cout << "[0] Back " << std::endl;

            // get new status from user.
            int newStatus;

            bool selecting = true;


            // ask for an integer selection
            while(selecting){
                try
                    {
                    std::cout << "Select: " << std::endl;    
                    std::cin >> newStatus;
                    selecting = false; // finished selecting so can move forward
                    }
                catch(const std::exception& e)
                    {
                    std::cout << INCORRECT_CHARACTER_TYPE  << std::endl;
                    }
            }

            // if they select back, exit function
            if(newStatus = 0){
                return;
            }

            if (newStatus > 4 || newStatus << 0) {
                // reprompt if the option is incorrect
                std::cout << OPTION_NOT_AVAILABLE << std::endl;
                continue; 
            }

            // if the option is valid: 
            char* states[4];
            char one[] = "reviewed";
            states[0] = one;
            char two[] = "inProgress";
            states[1] = two;
            char three[] = "done";
            states[2] = three;
            char four[] = "cancelled";
            states[3] = four;

            // ask for confirmation
            std::cout << "Are you sure you want to change the status to " << states[newStatus-1] << std::endl;

            // if confirmed, update it and end
            char confirmation;
            std::cin >> confirmation;
            if(confirmation == 'y'){
                uint8_t stat = 1;
                // change status to the selected value
                item.status = (stat << newStatus);

                //write it back to the database
                ChangeItemDatabase::writeElement(item);
                
                // return when successfully changed 
                return;
            } 
            // if they say no, go back to showing all the options again
            else{
                continue;
            }
        }
        

    }


    /*
    Description: updates release of a given change item
    no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
    no back to main menu option
    */
    void updateRelease(change_item& item){
        // finding the associated product with the item
        product relFor;
        strncpy(relFor.name, item.product, MAX_PRODUCT_NAME_SIZE);
        // finding associated released with the product
        release toChange;
        release filler;
        strncpy(filler.name, relFor.name, MAX_PRODUCT_NAME_SIZE);

       // bool fileEnded = (changeItemCount >= 0);           // CHANGE TO THIS ONCE FUNCTION IS READY
        bool fileEnded = false;
        // while we haven't reached the end of the file:
        while (!fileEnded) {
                            
            int count = 0;  
            // print up to 16 reports:
            std::cout << "Product     Release   Date" << std::endl;
            while (count < 16 && Release::getNext(toChange, filler)) {
                releaseUpdateShow(count, toChange);
                count++;
            }

            // Check if the inner loop ended due to reaching the end of the file:
            if (count < 16) {
                fileEnded = true;
            }

            bool selected = false;
            char selection[3];
            int reqSelection;
            bool responseProcessed = true;

            // response processed must be true while responses are being processed.
            while(responseProcessed){

                std::cout << "Selection: ";
                std::cin >> selection;
                // ask user for selection
        
                    try{
                        // try converting it to a number.
                        reqSelection = atoi(selection);
                        // if they select back, exit the function (go back)
                        if(reqSelection == 0){
                            return;
                        }
                        // if they select a valid release, update the change item accordingly and exit the function
                        else if (reqSelection <= 16 && reqSelection > 0){
                            strncpy(item.release, toChange.name, MAX_PRODUCT_NAME_SIZE);
                            ChangeItemDatabase::writeElement(item);
                            std::cout << "Release Updated" << std::endl;
                            return;
                        } 
                        else{
                            // if they select a different value, show error message and reprompt
                            std::cout << OPTION_NOT_AVAILABLE << std::endl;
                        }
                            }

                        // if they did not enter a number
                    catch(const std::exception& e){
                        // if they selected C and the file has not yet ended, exit this loop and print the next 16 values
                        if((selection == "C" || selected == 'c' ) && !fileEnded){
                            responseProcessed = false;
                        }
                        else{
                            // in all other cases, show an error message and reprompt 
                            std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                            }
                    }

            }


        }
        // exit function if file ended was true
        std::cout << "Empty Database" << std::endl;
        return;

    }


    /*
    Description: updates priority of a given change item
    no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
    no back to main menu option
    */
    void updatePriority(change_item& item){
        while(1){
            // menu options for priority
            std::cout << "Update Change Priority" << std::endl;
            std::cout << "Update Priority" << std::endl;
            std::cout << "[1] Lowest" << std::endl;
            std::cout << "[2] Low" << std::endl;
            std::cout << "[3] Middle" << std::endl;
            std::cout << "[4] High" << std::endl;
            std::cout << "[5] Highest" << std::endl;

            // take in selection
            int selected;

            // in the process of making a selection
            bool selecting = true;

            while(selecting){
                try
                    {
                    std::cout << "Select: " << std::endl;    
                    std::cin >> selected;
                    selecting = false; // finished selecting so can move forward
                    }
                catch(const std::exception& e)
                    {
                    std::cout << INCORRECT_CHARACTER_TYPE  << std::endl;
                    }
            }

            

            // case for back : exit function
            if(selected == 0){
                return;
            }

            // case for valid selection: update and exit function
            if(selected <= 5 && selected >= 0){
                item.priority = selected;
                //write back to file
                ChangeItemDatabase::writeElement(item);
                // exit
                return;
            }
            // all other cases, show error and reprompt
            else{
                std::cout << OPTION_NOT_AVAILABLE << std::endl;
                continue;
            }
        }

    }

    /*
    Description: updates description of a given change item
    no return type - performs the operation and goes back to the previous menu. if user selects back, simply returns
    no back to main menu option
    */
    void updateDescription(change_item& item){
        while(1){
            // prompt
            std::cout << "Update Change Item:" << std::endl;
            std::cout << "Update Description:" << std::endl;
            std::cout << "[0] Back" << std::endl << std::endl;
            std::cout << "Field Requirements: " << std::endl;
            std::cout << "(1-30 alphanumeric characters)" << std::endl;
            std::cout << "Enter Description: " << std::endl;

            char input[23];

            // get user input
            std::cin.getline(input, MAX_DESCRIPTION_SIZE);
    
            // Check if input exceeded maxSize - 1 characters
            if (std::cin.fail() && std::cin.gcount() == MAX_DESCRIPTION_SIZE - 1) {
                std::cin.clear(); // Clear error state
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore rest of line
                std::cout << EXCEEDS_BOUNDS << std::endl;
                return;
            } else {
                // update and write to file before exiting the function
                strncpy(item.description, input, MAX_DESCRIPTION_SIZE);
                ChangeItemDatabase::writeElement(item);
                return;
            }
        }
        
    }


    /*
    Description: prints change item in a multi-line format to make it easier for users to see attributes they would like to update
    Associates numbers with associated attributes that allow users to make a selection
    */
    void printUpdateCIMenu(change_item& item){

        std::cout << "Update Change Item:" << std::endl;
        std::cout << "Change Item: " << item.id << std::endl;
        std::cout << "Product:  " << item.product << std::endl;
        std::cout << "[1] Status: "<< item.status <<  std::endl;
        std::cout << "[2] Description: "<< item.description << std::endl;
        std::cout << "[3] Priority: "<< item.priority <<std::endl;
        std::cout<< "[4] Release: "<< item.release << std::endl;
        release corresponding;
        release filtering;
        strncpy(filtering.name, item.release, MAX_RELEASE_ID_SIZE);
        Release::getNext(corresponding, filtering);
        std::cout << "Release Date: "<< corresponding.date <<std::endl;
        std::cout << "[0] Back  [00] Back to Main Menu" << std::endl;
    }
    

    /*
    Description: Displays release names for the user to select from to update to
    only displays its id
    */
    void releaseUpdateShow (int number, release& release){
        char idTruncated[MAX_RELEASE_ID_SIZE];
        strncpy(idTruncated, release.releaseId, MAX_RELEASE_ID_SIZE - 1);
        idTruncated[MAX_RELEASE_ID_SIZE - 1] = '\0'; // Ensure null-termination

        // Print formatted output
        std::cout << "[" << number << "] "
                    << std::left << std::setw(ID_DIGITS) << release.releaseId << std::endl;
        return;
    }
    
    bool addProductControl(){
        return true;
    }
    /* prompts the user for inputs to create a product and calls Product::addProduct() to
    createa new product and write it to disk.
    precondition: none.
    postcondition: a product is successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    bool addReleaseControl(){
        return true;

    }
    /* prompts the user for inputs to create a product release and calls Release::addRelease()
    to create a new product release and write it to disk.
    postcondition: a product release is successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */

    bool addRequestControl(){
        return true;

    }
    /* prompts the user for inputs to create a request. this involves calling
    Requester::addRequester if the requester does not already exist, calling Request::addRequest
    and calling Item::addItem.
    precondition: none.
    postcondition: a request and item (and requester, if the requester does not exist yet) is
    successfully created and written in disk.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */


    /* allows the user to update the attributes of a change item.
    precondition: none.
    postcondition: the change item is successfully updated.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    */
    bool updItemControl(){
        // make sure the module has been initialized
         if(initialized){    
            while(1){
                bool toContinue  = true;
                // ask user to select a product from a list of all products and rememeber the selection value and elements printed in that selection
                std::cout << "Update Change Item: " << std::endl;
                std::cout << "[1] View Items for All Products" << std::endl;
                std::cout << "[2] View Items for Product" << std::endl;
                // initialie a blank product
                product selectedProduct;

                int prodFilter;
                bool filterByProduct = false;
                bool selecting = true;
                //take in user input:
                while (selecting){
                    try
                    {
                    std::cin >> prodFilter;
                    if(prodFilter == 1 || prodFilter == 2){
                        if(prodFilter == 2) filterByProduct = true;
                        selecting = false;
                    } else{
                        std::cout << OPTION_NOT_AVAILABLE << std::endl;
                    }
                    }
                    catch(const std::exception& e)
                    {
                    std::cout << INCORRECT_CHARACTER_TYPE << std::endl;
                    }
                }


                if(filterByProduct){
                    int16_t selNind = selectProduct();
                    int8_t productSelected = static_cast<int8_t>(selNind & 0xFF); 
                    int8_t arrayIndex = static_cast<int8_t>((selNind >> 8) & 0xFF);

                    // processing selections:

                    // back to main menu:
                    if (productSelected == -1){
                        return false;
                    // back:
                    } 
                    else if(productSelected == 0){
                        return true;  // end function since back form here is back to the main menu.
                    //option selected:
                    } 
                    // go back if the database is empty
                    else if(productSelected = -2)
                    {
                        std::cout << "Empty Database";
                        return true;
                    }
                    // if a valid selection has been made, update selectedProduct accordingly
                    Product::select(selectedProduct, productSelected, arrayIndex);
                }
                
                // in case it is a valid selection (value from 1 - 16)
                else
                {
                    while(1){

                            // prompt user to select a change item from a list of change items for that product through a filter with the same name 
                            change_item filter;
                            // copy add its name to the filter change_item's product parameter
                            strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);

                            // show all change items for that product and ask user to select one
                            int16_t selNind2 = selectItem(filter);
                            int8_t itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                            int8_t arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);

                            // processing selecition:

                            // case for back to main menu
                            if(itemSelected == -1){
                                return false;
                            }   // case for back
                            else if (itemSelected == 0) {
                                toContinue = false;
                            
                            }  // case for back again if databse is empty 
                            else if (itemSelected == -2){
                                std::cout << "Empty Database" << std::endl;
                                return true;
                            }
                        // if it is a valid selection:
                        if(toContinue){
                            while(1){
                                // ask user how they want their items sorted
                                std::cout  << "Update Change Item" << std::endl;
                                std::cout << "Show what change item?" << std::endl;
                                std::cout << "[1] New Change Items" << std::endl;
                                std::cout << "[2] Reviewed Change Items" << std::endl;
                                std::cout << "[3] In Progress Change Items" << std::endl;
                                std::cout << "[4] All Not Done Or Cancelled" << std::endl;
                                std::cout << "[5] All" << std::endl;
                                std::cout << "[0] Back" << std::endl;
                                int updateX;
                                // take in their input
                                std::cin >> updateX;

                                //decalre a change_item that will be updated
                                change_item mainSelection;

                                // control conditions for while loops
                                bool toContinue = true;
                                bool askAgain = true;

                                // a filter change item to filter by the selected input from the above menu
                                change_item filter;
                                // make sure it is for the selected product
                                strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);
                                switch (updateX)
                                {
                                    // filtered by new status
                                case 1:
                                    filter.status = unreviewed;
                                    // make a selection from that filtered list
                                    selNind2 = selectItem(filter);
                                    itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                                    arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);

                                    // if user selected to go back, exit this loop and go not move on to the next step
                                    if(itemSelected == 0){
                                        toContinue = false;
                                        break;
                                        // if they select to go back to the main menu, exit the function
                                    } else if(itemSelected == -1){
                                        return false;
                                        // in all other cases, exit the switch
                                    } else{
                                        break;
                                    }
                                
                                case 2:
                                    // filter and select
                                    filter.status = reviewed;
                                    selNind2 = selectItem(filter);
                                    itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                                    arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);
                                    // go back
                                    if(itemSelected == 0){
                                        toContinue = false;
                                        break;
                                        // main menu
                                    } else if(itemSelected == -1){
                                        return false;
                                        //continue ahead
                                    } else{
                                        break;
                                    }
                                    break;
                                
                                case 3:
                                    // filter and select
                                    filter.status = inProgress;
                                    selNind2 = selectItem(filter);
                                    itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                                    arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);
                                    // go back
                                    if(itemSelected == 0){
                                        toContinue = false;
                                        break;
                                        // main menu
                                    } else if(itemSelected == -1){
                                        return false;
                                        // continue ahead
                                    } else{
                                        break;
                                    }

                                case 4:    
                                    //filter and select
                                    filter.status = unreviewed & reviewed & inProgress;
                                    selNind2 = selectItem(filter);
                                    itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                                    arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);

                                    //go back
                                    if(itemSelected == 0){
                                        toContinue = false;
                                        break;
                                    // back to the main menu
                                    } else if(itemSelected == -1){
                                        return false;
                                    // continue ahead
                                    } else{
                                        break;
                                    }
                                
                                case 5:
                                    // no filter select (other than product, if filtered by it)
                                    selNind2 = selectItem(filter);
                                    itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                                    arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);
                                    // back case
                                    if(itemSelected == 0){
                                        toContinue = false;
                                        break;
                                        // main menu
                                    } else if(itemSelected == -1){
                                        return false;
                                        //continue ahead
                                    } else{
                                        break;
                                    }

                                case 0:
                                    // if they go back, exit switch and while loop  
                                    toContinue = false;
                                    askAgain = false;
                                    break;

                                } 

                                // if they are not to contiue
                                if(!toContinue){
                                    // if they said back in the switch, exit this while loop and go back to previous menu
                                    if(!askAgain) break;
                                    // if they said back in the function, restart loop
                                    continue;
                                } else{
                                    // otherwise, continue ahead
                                    while(1){
                                        // ask user what they would like to update and display details of the change_item they selected
                                        ChangeItemDatabase::select(mainSelection, itemSelected, arrayIndexI);
                                        std::cout << "What do you want to do for this item? " << std::endl;
                                        printUpdateCIMenu(mainSelection);
                                        // take in response
                                        char responseC[3];
                                        std::cin >> responseC;

                                        // go back to main menu
                                        if(responseC == "00"){
                                            return false;
                                        }
                                        // convert to number and process selection
                                        int response = atoi(responseC);
                                        toContinue = true;
                                        // call function based on selection
                                        switch (response) {
                                            case 1:
                                                updateStatus(mainSelection);
                                                break;
                                            case 2: 
                                                updateDescription(mainSelection);
                                                break;
                                            case 3:
                                                updatePriority(mainSelection);
                                                break;
                                            case 4:
                                                updateRelease(mainSelection);
                                                break;
                                            // if they select back, exit this loop and go back to selecting change item
                                            case 0:
                                                toContinue = false;
                                                break;
                                        }
                                        // break out of this loop
                                        if(!toContinue){
                                            break;
                                        } 
                                        // once an update has been done, rerun the loop and ask for updates
                                        else{
                                            continue;
                                        }
                                    }
                                }


                            }



                        }
                    }
                }
            }
         }

         return false;
    }


    //==================

        /* this is a function that generates a report of users that need to be informed about a
    specific change item.
    precondition: none.
    postcondition: none.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    returns: false to go back to main menu, true otherwise
    */
    bool generateReportRequesters(){
        // if module has been initialized
        if(initialized){    
            while(1){

                bool toContinue  = true;
                // ask user to select a product from a list of products
                int selNind = selectProduct();
                int productSelected = static_cast<int8_t>(selNind & 0xFF); 
                int arrayIndex = static_cast<int8_t>((selNind >> 8) & 0xFF);

                // back to main menu:
                if (productSelected == -1){
                    return false;
                // back:
                } 
                else if(productSelected == 0){
                    return true;
               
                } 
                 //option empty DB (equivalent to back)
                else if(productSelected == -2)
                {
                    std::cout << "Empty Database";
                    return true;
                }
                // option selected:
                else
                {
                    while(1){
                    // store product 
                    product selectedProduct;
                    Product::select(selectedProduct, productSelected, arrayIndex);

                    //create a change item filter to filter by product
                    change_item filter;
                    strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);

                    //prompt user to select from filtered change_items
                    int16_t selNind2 = selectItem(filter);

                    int8_t itemSelected = static_cast<int8_t>(selNind2 & 0xFF);
                    int8_t arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);

                    //process selection:
                    //main menu
                    if(itemSelected == -1){
                        return false;

                    }  // back
                    else if (itemSelected == 0) {
                        toContinue = false;
                    }  // empty db
                    else if (itemSelected == -2){
                        std::cout << "Empty Database" << std::endl;
                        return true;
                    } // all other cases (if it is to be continue (not if they selected back))
                    if(toContinue){
                        // prompt user to confirm selected change_item
                        change_item selectedItem;
                        char confirmation;
                        ChangeItemDatabase::select(selectedItem, itemSelected, arrayIndexI);
                        itemConfirmationShow(selectedItem);
                        std::cout << "[y/n]:  " << std::endl;
                        std::cin >> confirmation;
                        // if they confrim : print it  and rerun loop to show all change_items filtered by product again
                        if (confirmation == 'y'){
                            change_request filterId;
                            filterId.requesterId = selectedItem.id; 
                            int requestersSeen = listOfRequesters(filterId);\
                            // if they select to go back to the main menu, exit function
                            if(requestersSeen == -1){
                            return false;
                        }
                        // if they say no, show all filtered items again
                        }
                        
                     }
                    }
                    
                }

            }
            
            
        }
        // back to main menu if not initialized
        else{
            return false;
        }
    }


    /* this is a function that generates a report of change items for a specific product that are unresolved
    precondition: none.
    postcondition: none.
    exceptions raised: throws an exception if the user enters an option that does not exist
    during the input collection process.
    returns : false for going back to main menu, true otherwise
    */

    bool generateReportItems(){
        // only runs if the module has been initialized
        if(initialized){
            while(1){
                // prompt user to select a product from a list of all products
                bool toContinue = true;
                int selNind = selectProduct();
                int productSelected = static_cast<int8_t>(selNind & 0xFF); 
                int arrayIndex = static_cast<int8_t>((selNind >> 8) & 0xFF);

                //processing selection
                
                //main menu
                if(productSelected == -1){
                    return false;
                } 
                //back
                else if (productSelected == 0){
                    return true;
                }
                // all other cases:
                else{
                    while(1){
                        // store selected product
                        product selectedProduct;
                        Product::select(selectedProduct, productSelected, arrayIndex);
                        // filter change items by it
                        change_item filter;
                        strncpy(filter.product, selectedProduct.name, MAX_PRODUCT_NAME_SIZE);
                        
                        // control condition that keeps the loop running as long as the user wishes to keep looking at items
                        bool stillLooking = 0;
                        while(stillLooking){
                        // filter all items by product and prompt to select one
                        int16_t selNind2 = selectItem(filter);
                        int8_t itemSelected = static_cast<int8_t>(selNind2 & 0xFF); 
                        int8_t arrayIndexI =  static_cast<int8_t>((selNind2 >> 8) & 0xFF);

                        //process selections:

                        // main menu
                        if(itemSelected == -1){
                            return false;
                        }  // back
                        else if (itemSelected == 0){
                            break;
                        }  // a valid selection
                        else{
                            // store change_item information
                            change_item toShow;
                            ChangeItemDatabase::select(toShow, itemSelected, arrayIndexI);
                            // print information out to user in a well-formatted layout
                            itemConfirmationShow(toShow);
                            // give options of going back to see items again or back to the main menu
                            std::cout << "[0]  Back    [00] Back to Main Menu" << std::endl;
                            char toDo[3];
                            std::cin >> toDo;
                            // keep prompting for an input until a valid one is entered
                            bool gotResponse = false;
                            while (gotResponse){
                                // go back (show all items again)
                                if (toDo == "0"){
                                    stillLooking = true;
                                    gotResponse = true;
                            }  //back to the main menu
                            else if (toDo == "00"){
                                return -1;
                            }
                            // invalid input; error message + reprompt
                             else{
                                std::cout << OPTION_NOT_AVAILABLE << std::endl;
                                // ensuring it remains false
                                gotResponse = false;
                            }
                            }


                        }
                        }


                    }
                }
            }
        }

        else{
            return false;
        }

    }


