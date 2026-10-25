#include<stdio.h>
#include<stdlib.h>

#include"../headers/hash_table.h"

int main(){
    HashTable* ht= createHashTable(100);

    // add5 people;
    struct Person p1={1,"Atharva","suspect",21,"8767787014","xyz"};
    struct Person p2={2,"Aditya","witness",20,"8767787015","yz"};
    struct Person p3={3,"Anushka","officer",31,"8767787016","xz"};
    struct Person p4={4,"Aman","suspect",18,"8767787017","xy"};
    struct Person p5={5,"Arnav","witness",29,"8767787018",""};

    insertPerson(ht,p1);
    insertPerson(ht,p2);
    insertPerson(ht,p3); 
    insertPerson(ht,p4);
    insertPerson(ht,p5);   

    printf("----- HASHTABLE TEST -----\n\n");

    // test search
    printf("searching for person id 1:");
    struct Person* found= searchPerson(ht,1);
    if(found!=NULL)
        printf("found: %s\n", found->name);
    else
        printf("not found\n");
    

    printf("Searching for id 5:");
    found=searchPerson(ht,5);
    if(found!=NULL)
        printf("found : %s\n", found->name);
    else
        printf("not found\n");


    printf("Searching for person id 999:");
    found=searchPerson(ht,999);
    if(found!=NULL)
        printf("found : %s\n", found->name);
    else
        printf("not found\n");

    printf("Hashtable count is %d", ht->count);
    return 0;
}

