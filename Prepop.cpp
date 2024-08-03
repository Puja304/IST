#include "Product.h"
#include "Release.h"
#include "ChangeItem.h"
#include "ChangeRequest.h"
#include "Requester.h"
#include "Constants.h"

#include <cstring>
#include <iostream>
using std::cout;
using std::endl;

int initF()
{
    Product::initProduct();
    Release::initRelease();
    ChangeItemDatabase::init();
    ChangeRequestDatabase::init();
    RequesterDatabase::init();
    return 0;
}

int uninitF()
{
    Product::uninitProduct();
    Release::uninitRelease();
    ChangeItemDatabase::uninit();
    ChangeRequestDatabase::uninit();
    RequesterDatabase::uninit();
    return 0;
}

/*
    data fields
*/

//requires Prod1, Prod4WinOS
const char* productNames[20] = {
    "Prod1", "Prod4WinOS", "Tetris", "PuoyoPuoyo", "Prod42",
    "xxACR", "XRDrev2", "ITSbeta", "ITSstable", "ITSlite",
    "FINCH", "IKARUGA", "SPARROW", "HAWK", "FALCON",
    "PlumJam", "Strawberry", "caffeine", "pastry", "breakFAST"
    };

const char* requesterNames[20] = {
    "John Doe", "Jacob Deer", "Jane Elk", "Jerry Ox", "Jonas Fawn",
    "Jessie Moose", "Judas Caribou", "Janis Gazelle", "Timothee Chalamet", "Masayoshi Takanaka",
    "Nat Cmiel", "Ichiko Aoba", "Kaho Matsui", "Ryo Fukui", "UBOA",
    "Toshifumi Hananata", "CASIOPEA", "Charles Mingus", "Jack Stauber", "King Crimson"
    };

const char* requesterEmails[20] = {
    "john@mail.ca", "jacob@mail.ca", "jane@mail.ca", "cjerry@mail.ca", "jonas@mail.ca",
    "jessie@mail.ca", "judas@mail.ca", "janis@mail.ca", "ServiceBC@gov.bc.ca", "rainbow@goblins.com",
    "perfect@blue.com", "seabed@eden.com", "hotsauce@miku.com", "early@summer.com", "impossible@light.com",
    "sarah@menuet.com", "mint@jams.com", "mood@indigo.com", "pop@opera.com", "moon@child.com" 
};

const char* requesterPhoneNumbers[20] = {
    "1234567890", "0987654321", "0987609876", "1234512345", "1212121212",
    "2323232323", "3434343434", "4545454545", "5656565656", "7878787878",
    "8989898989", "9090909090", "0101010101", "1231234567", "2342345678",
    "3453456789", "4564567890", "5675678901", "6786789012", "7897890123"
};

const char* departments[5] = {
    "QA", "management", "development", "marketing", "justice"
};

const char* theDate = "2024-07-31";

int main()
{
    initF();

    /*
    populate products
    */

    Product::seekToBeginning();
    product bufferedProd;

    // 20 products
    for (int i = 0; i < 20; i++)
    {
        strncpy(bufferedProd.name, productNames[i], MAX_PRODUCT_NAME_SIZE);
        Product::writeProduct(bufferedProd);
    }

    /*
    populate releases
    */

    Release::seekToBeginning();
    release bufferedRelease;

    // add rel 1.00 ... 1.20 for Prod1
    for (int i = 0; i <= 20; i++)
    {
        strncpy(bufferedRelease.name, productNames[0], MAX_PRODUCT_NAME_SIZE);
        strncpy(bufferedRelease.date, theDate, DATE_SIZE);
        if (i < 10) // names with numbers with one byte unicode representation
        {
            snprintf(bufferedRelease.releaseId, MAX_RELEASE_ID_SIZE, "rel.1.0%i", i);
        }
        if (i >= 10) // names with numbers with two byte unicode representations
        {
            snprintf(bufferedRelease.releaseId, MAX_RELEASE_ID_SIZE, "rel.1.%i", i);
        }
        Release::writeRelease(bufferedRelease);
    }

    // add rel4.01 and rel4.02 for Prod4WinOS
    strncpy(bufferedRelease.name, productNames[1], MAX_PRODUCT_NAME_SIZE);
    strncpy(bufferedRelease.date, theDate, DATE_SIZE);
    strncpy(bufferedRelease.releaseId, "rel4.01", MAX_RELEASE_ID_SIZE);
    Release::writeRelease(bufferedRelease);

    strncpy(bufferedRelease.name, productNames[1], MAX_PRODUCT_NAME_SIZE);
    strncpy(bufferedRelease.date, theDate, DATE_SIZE);
    strncpy(bufferedRelease.releaseId, "rel4.02", MAX_RELEASE_ID_SIZE);
    Release::writeRelease(bufferedRelease);

    /*
    populate items
    */
    
    ChangeItemDatabase::seekToBeginning();
    change_item bufferedItem;

    // change item of prod 1 with 20 related requests
    bufferedItem.id = ChangeItemDatabase::getChangeItemCount() + 1;
    bufferedItem.priority = middle;
    bufferedItem.status = unreviewed;
    strncpy(bufferedItem.description, "MyItemDescription", MAX_DESCRIPTION_SIZE);
    strncpy(bufferedItem.product, productNames[0], MAX_PRODUCT_NAME_SIZE);
    strncpy(bufferedItem.release, "rel.1.00", MAX_RELEASE_ID_SIZE);
    ChangeItemDatabase::writeElement(bufferedItem);

    // change item for prod4WinOS with 2 related requests
    bufferedItem.id = ChangeItemDatabase::getChangeItemCount() + 1;
    bufferedItem.priority = low;
    bufferedItem.status = unreviewed;
    strncpy(bufferedItem.description, "123456789012345678901234567890", MAX_DESCRIPTION_SIZE);
    strncpy(bufferedItem.product, productNames[1], MAX_PRODUCT_NAME_SIZE);
    strncpy(bufferedItem.release, "rel.4.02", MAX_RELEASE_ID_SIZE);
    ChangeItemDatabase::writeElement(bufferedItem);

    // update the change item for prod4WinOS to done
    bufferedItem.id = ChangeItemDatabase::getChangeItemCount();
    bufferedItem.priority = low;
    bufferedItem.status = done;
    strncpy(bufferedItem.description, "123456789012345678901234567890", MAX_DESCRIPTION_SIZE);
    strncpy(bufferedItem.product, productNames[1], MAX_PRODUCT_NAME_SIZE);
    strncpy(bufferedItem.release, "rel.4.01", MAX_RELEASE_ID_SIZE);
    ChangeItemDatabase::writeElement(bufferedItem);

    /*
    populate requesters
    */

    RequesterDatabase::seekToBeginning();
    requester bufferedRequester;

    // pool of 20 requesters
    for (int i = 0; i < 20; i++)
    {
        bufferedRequester.requesterId = RequesterDatabase::getRequesterCount() + 1;
        strncpy(bufferedRequester.name, requesterNames[i], MAX_REQUESTER_NAME_SIZE);
        strncpy(bufferedRequester.email, requesterEmails[i], MAX_EMAIL_SIZE);
        strncpy(bufferedRequester.phone, requesterPhoneNumbers[i], PHONE_NUMBER_SIZE);
        strncpy(bufferedRequester.department, "", MAX_DEPARTMENT_SIZE);
        if (i % 4 == 0) // give some requesters a department
        {
            strncpy(bufferedRequester.department, departments[(i / 4) % 5], MAX_DEPARTMENT_SIZE);
        }
        RequesterDatabase::writeElement(bufferedRequester);
    }

    /*
    populate requests
    */

    ChangeRequestDatabase::seekToBeginning();
    change_request bufferedRequest;

    // 20 requests for change item 1
    for (int i = 0; i < 20; i++)
    {
        bufferedRequest.changeItemId = ChangeItemDatabase::getChangeItemCount() - 1;
        bufferedRequest.requesterId = RequesterDatabase::getRequesterCount() - i;
        strncpy(bufferedRequest.requestDate, theDate, DATE_SIZE);
        strncpy(bufferedRequest.release, "rel.1.00", MAX_RELEASE_ID_SIZE);
        ChangeRequestDatabase::writeElement(bufferedRequest);
    }

    // 2 requests for change item 2
    bufferedRequest.changeItemId = ChangeItemDatabase::getChangeItemCount();
    bufferedRequest.requesterId = RequesterDatabase::getRequesterCount();
    strncpy(bufferedRequest.requestDate, theDate, DATE_SIZE);
    strncpy(bufferedRequest.release, "rel.4.01", MAX_RELEASE_ID_SIZE);
    ChangeRequestDatabase::writeElement(bufferedRequest);

    bufferedRequest.changeItemId = ChangeItemDatabase::getChangeItemCount();
    bufferedRequest.requesterId = RequesterDatabase::getRequesterCount() - 19;
    strncpy(bufferedRequest.requestDate, theDate, DATE_SIZE);
    strncpy(bufferedRequest.release, "rel.4.02", MAX_RELEASE_ID_SIZE);
    ChangeRequestDatabase::writeElement(bufferedRequest);

    uninitF();
    return 0;
}