#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "challenge_system.h"

bool flag = true;

#define ASSERT(test_number, test_condition)  \
   if (!(test_condition)) {printf("TEST %s FAILED\n", test_number); flag = false;} \
   else printf("TEST %s OK\n", test_number);

#define MEMORY(p) if(!p) {printf("MEMORY PROBLEM TESTS STOPPED.\n"); return 1;}

/*void printVisitors(ChallengeRoomSystem *sys){
    Node ptr=sys->visitors;
    while(ptr != NULL && ptr->visitor != NULL){
        printf("Visitor %s is in %s in %s\n",ptr->visitor->visitor_name,
               ptr->visitor->current_challenge->challenge->name,*(ptr->visitor->room_name));
        ptr=ptr->next;
    }
}*/

int main2(int argc, char **argv)
{
   /** --------------------------------------------------------------------- **/
   /** --------- HW 2 tests by Michal Guttmann and Amit Shuster ------------ **/
   /** --------------------------------------------------------------------- **/

    Result r=OK;
    char *room=NULL;
    int time;
    char *most_popular_challenge1=NULL, *challenge_best_time=NULL;
    char *popular=NULL;

    /************************* Original staff tests ***************************/
    ChallengeRoomSystem *sys=NULL;
    r=create_system("test_1.txt", &sys);
    r=visitor_arrive(sys, "room_2", "visitor_1", 201, Medium, 5);
    r=visitor_arrive(sys, "room_1", "visitor_2", 202, Easy, 8);
    r=visitor_quit(sys, 203, 10);
    ASSERT("1.1" , r==NOT_IN_ROOM)
    r=visitor_quit(sys, 201, 9);
    ASSERT("1.2" , r==OK)
    r=best_time_of_system_challenge(sys, "challenge_2", &time);
    ASSERT("1.3" , time==4)
    r=change_system_room_name(sys, "room_1", "room_111");
    r=visitor_arrive(sys, "room_1", "visitor_3", 203, Easy, 8);
    ASSERT("1.4" , r==ILLEGAL_TIME)
    r=visitor_arrive(sys, "room_111", "visitor_3", 203, Easy, 8);
    ASSERT("1.5" , r==ILLEGAL_TIME)
    r=visitor_arrive(sys, "room_111", "visitor_3", 203, Easy, 15);
    ASSERT("1.6" , r==OK)
    r=visitor_arrive(sys, "room_111", "visitor_4", 204, Easy, 16);
    ASSERT("1.7" , r==NO_AVAILABLE_CHALLENGES)
    r=change_challenge_name(sys, 11, "challenge_1111");
    r=best_time_of_system_challenge(sys, "challenge_1111", &time);
    ASSERT("1.8" , time==0)
    char *namep=NULL;
    r=most_popular_challenge(sys, &namep);
    ASSERT("1.9" , namep!=NULL && strcmp(namep, "challenge_1111")==0)
    free(namep);
    r=system_room_of_visitor(sys, "visitor_4", &room);
    free(room); room = NULL;
    ASSERT("1.10" , r==NOT_IN_ROOM)
    r=system_room_of_visitor(sys, "visitor_3", &room);
    ASSERT("1.11" , r==OK && room!=NULL && strcmp(room, "room_111")==0)
    free(room); room = NULL;
    r=all_visitors_quit(sys, 17);
    r=best_time_of_system_challenge(sys, "challenge_1111", &time);
    ASSERT("1.12" , time==9)
    r=best_time_of_system_challenge(sys, "challenge_4", &time);
    ASSERT("1.13" , time==2)
    r=destroy_system(sys, 18, &most_popular_challenge1, &challenge_best_time);
    ASSERT("1.14" , most_popular_challenge1!=NULL && strcmp(most_popular_challenge1, "challenge_1111")==0)
    ASSERT("1.15" , challenge_best_time!=NULL && strcmp(challenge_best_time, "challenge_4")==0)
    free(most_popular_challenge1); most_popular_challenge1= NULL;
    free(challenge_best_time); challenge_best_time = NULL;
    /**************************************************************************/

    ChallengeRoomSystem *sys1=NULL;
    ChallengeRoomSystem *sys2=NULL;

    r=create_system("system1_file.txt", &sys1);
    r=create_system("system2_file.txt", &sys2);

    r=visitor_arrive(sys1, "museum", "Guy", 1, Medium, 2);
    r=visitor_arrive(sys1, "blackandwhite", "Amit", 2, Medium, 5);
    r=visitor_arrive(sys1, "nuclear", "Shahar", 3, All_Levels, 8);
    ASSERT("2.1" , r==OK)
    r=visitor_arrive(sys1, "nuclear", "Sagi", 4, All_Levels, 10);
    r=visitor_arrive(sys1, "pandora", "Amir", 5, Medium, 13);
    ASSERT("2.2" , r==OK)
    r=visitor_arrive(sys1, "nuclear", "Michal", 6, Medium, 12);
    ASSERT("2.3" , r==ILLEGAL_TIME)
    r=visitor_arrive(sys1, "nuclear", "Michal", 6, Medium, 14);
    ASSERT("2.4" , r==NO_AVAILABLE_CHALLENGES)
    r=visitor_arrive(sys1, "nuclear", "Michal", 6, Easy, 16);
    ASSERT("2.5" , r==NO_AVAILABLE_CHALLENGES)
    r=visitor_quit(sys1, 1, 15);
    r=visitor_arrive(sys1, "daVinci", "Amir", 5, Medium, 18);
    ASSERT("2.6" , r==ALREADY_IN_ROOM)
    r=visitor_arrive(sys1, "museum", "Michal", 6, Hard, 21);
    ASSERT("2.7" , r==NO_AVAILABLE_CHALLENGES)
    r=visitor_arrive(sys1, "pandora", "Michal", 6, Hard, 23);
    r=system_room_of_visitor(sys1, "Michal", &room);
    ASSERT("2.8" , strcmp(room,"pandora")==0)
    free(room); room = NULL;
    r=visitor_quit(sys1, 7, 24);
    ASSERT("2.9" , r==NOT_IN_ROOM)                  /// or ILLEGAL_PARAMETER?
    r=visitor_quit(sys1, 3, 25);
    r=best_time_of_system_challenge(sys1, "best_time", &time);
    ASSERT("2.10" , time==13)
    r=visitor_arrive(sys1, "bankrobbery", "Guy", 1, Easy, 26);
    r=visitor_arrive(sys1, "daVinci", "Shahar", 3, Easy, 28);
    r=visitor_quit(sys1, 5, 30);
    r=best_time_of_system_challenge(sys1, "the_legend", &time);
    ASSERT("2.11" , time==17)
    r=visitor_quit(sys1, 5, 32);
    ASSERT("2.12" , r==NOT_IN_ROOM)                 // or ILLEGAL_PARAMETER
    r=most_popular_challenge(sys1, &popular);
    ASSERT("2.13" , popular!=NULL && strcmp(popular, "What_a_mystery")==0)
    free(popular); popular = NULL;
    r=visitor_arrive(sys1, "daVinci", "Amir", 5, All_Levels, 35);
    r=visitor_quit(sys1, 5, 36);
    r=visitor_quit(sys1, 4, 37);
    ASSERT("2.14" , r==OK)
    r=best_time_of_system_challenge(sys1, "best_score", &time);
    ASSERT("2.15" , time==0)                        ///????
    r=all_visitors_quit(sys1,38);
    ASSERT("2.16" , r==OK)
    r=most_popular_challenge(sys1, &popular);
    ASSERT("2.17" , popular!=NULL && strcmp(popular, "What_a_mystery")==0)
    free(popular); popular = NULL;
    r=best_time_of_system_challenge(sys1, "tut_ankh_amon", &time);
    ASSERT("2.18" , time==15)
    r=best_time_of_system_challenge(sys1, "ancient_mystery", &time);
    ASSERT("2.19" , time==1)
    r=system_room_of_visitor(sys1, "Michal", &room);
    free(room); room = NULL;
    ASSERT("2.20" , r==NOT_IN_ROOM)
    r=visitor_arrive(sys1, "museum", "Guy", 1, Medium, 40);
    r=destroy_system(sys1, 43, &most_popular_challenge1, &challenge_best_time);
    ASSERT("2.21" , most_popular_challenge1!=NULL && strcmp(most_popular_challenge1, "What_a_mystery")==0)
    ASSERT("2.22" , challenge_best_time!=NULL && strcmp(challenge_best_time, "ancient_mystery")==0)
    free(most_popular_challenge1); most_popular_challenge1= NULL;
    free(challenge_best_time); challenge_best_time = NULL;
    r=visitor_arrive(sys2, "cow_boys", "Amit", 1, Easy, 2);
    ASSERT("3.1" , r==NO_AVAILABLE_CHALLENGES)
    r=visitor_arrive(sys2, "cow_boys", "Amit", 1, Medium, 2);
    r=visitor_arrive(sys2, "cow_boys", "Shahar", 2, Medium, 5);
    r=visitor_arrive(sys2, "elm_street", "Guy", 3, All_Levels, 8);
    ASSERT("3.2" , r==OK)
    r=visitor_arrive(sys2, "cow_boys", "Michal", 5, All_Levels, 10);
    r=visitor_arrive(sys2, "cow_boys", "Sagi", 4, Medium, 13);
    ASSERT("3.3" , r==NO_AVAILABLE_CHALLENGES)
    r=visitor_arrive(sys2, "magician_room", "Sagi", 4, Medium, 13);
    ASSERT("3.4" , r==OK)
    r=visitor_arrive(sys2, "heroes", "Michal", 5, All_Levels, 12);
    ASSERT("3.5" , r==ILLEGAL_TIME)
    r=visitor_arrive(sys2, "space_station", "Amir", 6, Medium, 14);
    ASSERT("3.6" , r==OK)
    r=visitor_quit(sys2, 1, 15);
    r=most_popular_challenge(sys2, &popular);
    ASSERT("3.7" , popular!=NULL && strcmp(popular, "Beer_Sheva")==0)
    r=visitor_arrive(sys2, "i_am_legend", "Amit", 1, Hard, 18);
    ASSERT("3.8" , r==OK)
    r=visitor_quit(sys2,5,19);
    r=visitor_arrive(sys2, "heroes", "Michal", 5, Hard, 21);
    ASSERT("3.9" , r==OK)
    r=visitor_arrive(sys2, "space_station", "Michal", 6, Hard, 23);
    ASSERT("3.10" , r==ALREADY_IN_ROOM)
    r=system_room_of_visitor(sys2, "Michal", &room);
    ASSERT("3.11" , strcmp(room,"heroes")==0)
    free(room); room = NULL;
    r=visitor_quit(sys2, 7, 24);
    ASSERT("3.12" , r==NOT_IN_ROOM)
    r=visitor_quit(sys2, 3, 24);
    r=best_time_of_system_challenge(sys2, "Eilat", &time);
    ASSERT("3.13" , time==9)
    r=visitor_quit(sys2, 1, 25);
    r=visitor_quit(sys2, 2, 26);
    r=visitor_arrive(sys2, "magician_room", "Guy", 3, All_Levels, 27);
    r=visitor_arrive(sys2, "space_station", "Shahar", 2, Hard, 28);
    r=visitor_quit(sys2, 3, 30);
    r=best_time_of_system_challenge(sys2, "Tel_Aviv", &time);
    ASSERT("3.14" , time==16)
    r=visitor_quit(sys2, 1, 32);
    ASSERT("3.15" , r==NOT_IN_ROOM)
    r=change_challenge_name(sys2,7,"Kerem_Maharal");
    ASSERT("3.16" , r==OK)
    r=best_time_of_system_challenge(sys2, "Eilat", &time);
    ASSERT("3.17" , r=ILLEGAL_PARAMETER)
    r=best_time_of_system_challenge(sys2, "Kerem_Maharal", &time);
    ASSERT("3.18" , time==7)
    r=all_visitors_quit(sys2,36);
    ASSERT("3.19" , r==OK)
    r=best_time_of_system_challenge(sys2, "Haifa", &time);
    ASSERT("3.20" , time==21)
    r=system_room_of_visitor(sys2, "Michal", &room);
    ASSERT("3.21" , r==NOT_IN_ROOM)
    r=visitor_arrive(sys2, "space_station", "Guy", 3, Hard, 38);
    r=destroy_system(sys2, 43, &most_popular_challenge1, &challenge_best_time);
    ASSERT("3.22" , most_popular_challenge1!=NULL && strcmp(most_popular_challenge1, "Kerem_Maharal")==0)
    ASSERT("3.23" , challenge_best_time!=NULL && strcmp(challenge_best_time, "Netanya")==0)

    free(room);
    free(popular);
    free(most_popular_challenge1);
    free(challenge_best_time);

    Challenge *c = NULL;
    char* c_name = "TheAmazingChallenge";
    r=init_challenge(c,5,c_name,Easy);
    ASSERT("4.1", r==NULL_PARAMETER);
    c = malloc(sizeof(Challenge));
    MEMORY(c)
    r=init_challenge(c,5,c_name,Easy);
    ASSERT("4.2", r==OK);
    r=change_name(c,"AnotherName");
    ASSERT("4.3", r==OK);
    c_name = "IsNewCopy";
    ASSERT("4.4", strcmp(c_name,c->name) != 0);//checks new copy
    int best_time_c;
    r=best_time_of_challenge(c,&best_time_c);
    ASSERT("4.5", best_time_c == 0);
    r=set_best_time_of_challenge(c,56);
    r=best_time_of_challenge(c,&best_time_c);
    ASSERT("4.6", best_time_c == 56);
    int numvisits;
    r=num_visits(c,&numvisits);
    ASSERT("4.7", numvisits == 0);
    r=inc_num_visits(c);
    r=inc_num_visits(c);
    r=num_visits(c,&numvisits);
    ASSERT("4.8", numvisits == 2);
    Visitor *v = malloc(sizeof(Visitor));
    MEMORY(v)
    ChallengeActivity *ca = malloc(sizeof(ChallengeActivity));
    MEMORY(ca)
    ChallengeRoom *rm = malloc(sizeof(ChallengeRoom));
    MEMORY(rm)
    r=init_challenge_activity(ca,c);
    ASSERT("5.1", r==OK);
    char* visitor_name = "ARIK", *room_name = "BESTROOM";
    r=init_visitor(v,visitor_name,55);
    visitor_name = "BENTZ";
    ASSERT("5.2", strcmp(visitor_name,v->visitor_name) != 0);//checks new copy
    r=init_room(NULL,room_name,0);
    ASSERT("5.3", r == NULL_PARAMETER);
    r=init_room(rm,room_name,0);
    ASSERT("5.3", r == ILLEGAL_PARAMETER);
    r=init_room(rm,room_name,1);
    ASSERT("5.4", r == OK);
    r=init_challenge_activity(rm->challenges,c);
    int c_places;
    r=num_of_free_places_for_level(rm,Medium,&c_places);
    ASSERT("5.5",c_places==0)
    r=num_of_free_places_for_level(rm,Easy,&c_places);
    ASSERT("5.6",c_places==1)
    r=num_of_free_places_for_level(rm,All_Levels,&c_places);
    ASSERT("5.7",c_places==1)
    r=num_of_free_places_for_level(rm,Hard,&c_places);
    ASSERT("5.8",c_places==0)
    r=change_room_name(rm,"ANOTHER");
    ASSERT("5.9", strcmp(room_name,rm->name) != 0);//checks new copy
    char* roomvisitor = NULL;
    r=room_of_visitor(v,&roomvisitor);
    ASSERT("5.10",r==NOT_IN_ROOM);
    r=visitor_enter_room(rm,v,Medium,11);
    ASSERT("5.11", r==NO_AVAILABLE_CHALLENGES);
    r=visitor_enter_room(rm,v,All_Levels,11);
    ASSERT("5.12", r == OK);
    r=visitor_enter_room(rm,v,Medium,11);
    ASSERT("5.13", r == ALREADY_IN_ROOM);
    r=visitor_enter_room(rm,v,Easy,11);
    ASSERT("5.14", r == ALREADY_IN_ROOM);
    r=num_visits(c,&numvisits);
    ASSERT("5.15", numvisits == 3);
    r=room_of_visitor(v,&roomvisitor);
    ASSERT("5.16",r==OK);
    ASSERT("5.17", roomvisitor != NULL && !strcmp(roomvisitor, *v->room_name))
    free(roomvisitor);
    r=visitor_quit_room(v,1);
    ASSERT("5.18", r==OK)
    r=visitor_quit_room(v,1);
    ASSERT("5.19", r==NOT_IN_ROOM)
    r=num_visits(c,&numvisits);
    ASSERT("5.20", numvisits == 3);
    r=reset_room(rm);
    ASSERT("5.21", r == OK);
    r=reset_visitor(v);
    ASSERT("5.22", r == OK);
    r=reset_challenge_activity(ca);
    ASSERT("5.23", r == OK);
    r=reset_challenge(c);
    ASSERT("5.24", r == OK);
    free(c);
    free(ca);
    free(v);
    free(rm);


    if(flag)
        printf("\n~~ All tests passed ! ~~\n");
    else
        printf("\n... some tests failed :( ...\n");
    return 0;
}

