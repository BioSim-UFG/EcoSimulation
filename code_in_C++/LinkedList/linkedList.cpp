#include <iostream>
#include <stdlib.h>

#define NUMELEM 3

using namespace std;

typedef struct no{
    int value;
    no *next;
}Node;

Node** initializeHead(){
    Node** head =NULL;

    head = (Node**) malloc(sizeof(Node)*NUMELEM);
    return head; 
}

//receives a copy of the line in wich the new node shall be inserted, EX (Node &(N[2]), int 10);
void insertElem(Node **linkList,int value){

    Node *newNode = (Node*) malloc(sizeof(Node));
    newNode->value=value;
    newNode->next = NULL;

    //if list is empty
    if( *linkList == NULL){
        (*linkList)=newNode;
    }   

    else{
        //if newNode is smaller than Head
        if( (*linkList)->value > newNode->value){
            newNode->next = *linkList;
            *linkList = newNode;
        }

        //insert in the middle of list
        else{
            Node *listPtr= *linkList;

            while(listPtr->next != NULL){
                if(listPtr->value < newNode->value && listPtr->next->value > newNode->value ){
                    newNode->next = listPtr->next;
                    listPtr->next = newNode;
                    break;
                }

                listPtr = listPtr->next;
            }

            //if inserting at the end of list
            if(listPtr->next == NULL){
                listPtr->next = newNode;
            }
        }
    }

    return;
}

//same parameters as insertElem
//Deletes the FIRST node found with the value. Does nothing if it doesn't find a compatible Node
void deleteElem(Node **linkList, int value){
    Node *toErase;
    Node *auxErase;

    toErase= *linkList;

    //delete head
    if(toErase->value == value){
        *linkList = (*linkList)->next;
        free(toErase);
    }

    else{
        auxErase = toErase;
        toErase= toErase->next;
        
        while(toErase != NULL){
            if(toErase->value == value){
                auxErase->next = toErase->next;

                free(toErase);
                break;
            }
            auxErase = toErase;
            toErase = toErase->next;
        }
    }

    return;
}

void showLine(Node *head){
    Node *aux = head;

    if(aux == NULL){
        cout << "No elements in line";
    }

    else{
        while(aux != NULL){
            cout << aux->value << " ";

            aux = aux->next;
        }
    }
    cout << endl;
    return;
}

//linkList is the address of the line you want to erase EX &(N[2])
void clearLine(Node **linkList){
    Node *aux;

    while(aux != NULL){
        aux = *linkList;
        *linkList= (*linkList)->next;

        free(aux);
    }

    *linkList = NULL;
    return;
}

/*

//Function DOESN'T WORK yet
void clearList(Node **linkList){
    
    for(int i=0 ; i< NUMELEM ; i++){
        clearLine( *linkList[i] );
    }

    free(linkList);

    linkList = NULL;
    return;
}
*/
int main(){
    Node **head = initializeHead();

    insertElem(&(head[2]),10 );
    insertElem(&(head[2]),12 );
    insertElem(&(head[2]),3);
    insertElem(&(head[2]), 9);
    insertElem(&(head[2]), 6);
    insertElem(&(head[2]), 22);

    showLine(head[2]);


    //showLine(head[2]);

   // cout << head;

    return 0;
}