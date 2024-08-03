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

int printStatus(change_item& input)
{
    int state = input.status;
    if (state == unreviewed)
    {
        cout << "unreviewed";
    }
    else if (state == reviewed)
    {
        cout << "reviewed";
    }
    else if (state == inProgress)
    {
        cout << "inProgress";
    }
    else if (state == done)
    {
        cout << "done";
    }
    else if (state == cancelled)
    {
        cout << cancelled;
    }
    return 0;
}

int main(){

    cout << endl << "================" << endl << "RELEASES:" << endl;
    Release::initRelease();
    Release::seekToBeginning();
    release A;
    for (int i = 1; i <= 22; i++)
    {
        Release::getNext(A);
        cout << endl;
        cout << "release " << i << " in file" << endl;
        cout << "date: " << A.date << endl;
        cout << "release ID: " << A.releaseId << endl;
        cout << "related product: " << A.name << endl;
    }
    Release::uninitRelease();

    cout << endl << "================" << endl << "PRODUCTS:" << endl;
    Product::initProduct();
    Product::seekToBeginning();
    product B;
    for (int i = 1; i <= 20; i++)
    {
        Product::getNext(B);
        cout << endl;
        cout << "product " << i << " in file" << endl;
        cout << "name: " << B.name << endl;
    }
    Product::uninitProduct();

    cout << endl << "================" << endl << "CHANGEITEMS:" << endl;
    ChangeItemDatabase::init();
    ChangeItemDatabase::seekToBeginning();
    change_item C;
    for (int i = 1; i <= 2; i++)
    {
        ChangeItemDatabase::getNext(C);
        cout << endl;
        cout << "item " << i << " in file" << endl;
        cout << "item ID: " << C.id << endl;
        cout << "related product: " << C.product << endl;
        cout << "related release: " << C.release << endl;
        cout << "description: " << C.description << endl;
        cout << "priority: " << (int)C.priority << endl;
        cout << "status: "; printStatus(C); cout << endl;
    }
    ChangeItemDatabase::uninit();

    cout << endl << "================" << endl << "REQUESTS:" << endl;
    ChangeRequestDatabase::init();
    change_request D;
    for (int i = 1; i <= 22; i++)
    {
        ChangeRequestDatabase::getNext(D);
        cout << endl;
        cout << "request " << i << " in file" << endl;
        cout << "requester ID: " << D.requesterId << endl;
        cout << "related change item ID: " << D.changeItemId << endl;
        cout << "release request reqeusted for: " << D.release << endl;
        cout << "date requested: " << D.requestDate << endl;
    }
    ChangeItemDatabase::uninit();

    cout << endl << "================" << endl << "REQUESTERS:" << endl;
    RequesterDatabase::init();
    RequesterDatabase::seekToBeginning();
    requester E;
    for (int i = 1; i <= 20; i++)
    {
        RequesterDatabase::getNext(E);
        cout << endl;
        cout << "requester " << i << " in file" << endl;
        cout << "requester ID: " << E.requesterId << endl;
        cout << "name: " << E.name << endl;
        cout << "phone: " << E.phone << endl;
        cout << "email: " << E.email << endl;
        if (strlen(E.department) != 0)
        {
        cout << "department: " << E.department << endl;
        }
    }
    RequesterDatabase::uninit();
    return 0;
}