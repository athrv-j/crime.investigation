
#ifndef STRUCT_H
#define STRUCT_H
#include <time.h>
#include <string.h>
struct Person{
    int id;
    char name [100];
    char role[50]; // sus - witness - victim
    int age;
    char contact[100];
    char notes[500]; // any additional info if met
};

struct Location{
    int id;
    char name[100];
    char type[50]; // building - street - park - etc
    float latitude;
    float longitude;
    char description[500]; // any additional info if met
};

struct Evidence{
    int id;
    char type[50];   // weapons , object , photos
    char description[500]; // any additional info if met
    int location_id; // location where the evidence was found
    int person_id; // person who found the evidence
    time_t timespan;

};

struct Event{
    int id;
    char type[50]; // murder - theft - assault - visit
    time_t timespan;
    int personid;
    int locationid;
    char description[500];
};

struct Edge{
    int sourceid; // strt of a relation
    int destinationid; // end of a relation
    char type[100]; // what type of relation- contact visited, has a evidence etc
    int weight; // strenght cost, freqwuenc importance of relation. (just like weighted graphs)
};

#endif
