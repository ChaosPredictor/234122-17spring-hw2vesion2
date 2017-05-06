#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "challenge_system.h"
#define MAX_NAME_LENG 51 //Max leng 50 + 1 of end of string


#define ASSERT(test_number, test_condition)  \
   if (!(test_condition)) {printf("\nTEST %s FAILED", test_number); } \
   else printf("\nTEST %s OK", test_number);

int official();
int unittest();
int utChallenge();
int utVisitorRoom();
int utChallengeSystem_public();

int main(int argc, char **argv)
{
	printf("\n================Official================\n");
	official();
	printf("\n");

	printf("\n================Unit test================\n");
	unittest();
	printf("\n");

}

int unittest() {
	printf("\n   Challenge   \n");
	utChallenge();
	printf("\n\n   Visitor_Room   \n");
	utVisitorRoom();
	printf("\n\n   Challenge System   \n");
	utChallengeSystem_public();
	return 0;
}

int official() {
	ChallengeRoomSystem *sys=NULL;
	Result r=OK;

	r=create_system("test_1.txt", &sys);
	ASSERT("1.0" , r==OK)

	r=visitor_arrive(sys, "room_2", "visitor_1", 201, Medium, 5);

	r=visitor_arrive(sys, "room_1", "visitor_2", 202, Easy, 8);

	r=visitor_quit(sys, 203, 10);
	ASSERT("1.1" , r==NOT_IN_ROOM)

	r=visitor_quit(sys, 201, 9);
	ASSERT("1.2" , r==OK)

	int time;
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

	char *room=NULL;
	r=system_room_of_visitor(sys, "visitor_4", &room);
	ASSERT("1.10" , r==NOT_IN_ROOM)
	free(room);

	r=system_room_of_visitor(sys, "visitor_3", &room);
	ASSERT("1.11" , r==OK && room!=NULL && strcmp(room, "room_111")==0)
	free(room);

	r=all_visitors_quit(sys, 17);

	r=best_time_of_system_challenge(sys, "challenge_1111", &time);
	ASSERT("1.12" , time==9)

	r=best_time_of_system_challenge(sys, "challenge_4", &time);
	ASSERT("1.13" , time==2)

	char *most_popular_challenge=NULL, *challenge_best_time=NULL;
	r=destroy_system(sys, 18, &most_popular_challenge, &challenge_best_time);
	ASSERT("1.14" , most_popular_challenge!=NULL && strcmp(most_popular_challenge, "challenge_1111")==0)
	ASSERT("1.15" , challenge_best_time!=NULL && strcmp(challenge_best_time, "challenge_4")==0)

	free(most_popular_challenge);

	free(challenge_best_time);

	return 0;
}


int utChallenge() {
	Result r=OK;

	r = init_challenge(NULL, 1, NULL, 1);
	ASSERT("2.0a - init_challenge" , r==NULL_PARAMETER)
	r = init_challenge(NULL, 1, "name", 1);
	ASSERT("2.0b - init_challenge" , r==NULL_PARAMETER)

	Challenge *challenge = malloc(sizeof(Challenge));
	if (challenge == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	char* challenge_name = "name";
	Level level = Easy;
	r = init_challenge(challenge, 1, NULL, 1);
	ASSERT("2.0c - init_challenge" , r==NULL_PARAMETER)
	r = init_challenge(challenge, 1, challenge_name, level);
	ASSERT("2.0d - init_challenge" , r==OK && \
			strcmp(challenge->name, challenge_name) == 0 && \
			&(challenge->name) != &(challenge_name) && \
			challenge->level == level && \
			challenge->best_time == 0 && \
			challenge->best_time == 0)

	r = reset_challenge(NULL);
	ASSERT("2.1a - reset_challenge" , r==NULL_PARAMETER)
	r = reset_challenge(challenge);
	ASSERT("2.1b - reset_challenge" , r==OK)

	r = init_challenge(challenge, 2, "name", 1);
	r = change_name(NULL, "newName");
	ASSERT("2.2a - change_name" , r==NULL_PARAMETER)
	r = change_name(challenge, NULL);
	ASSERT("2.2b - change_name" , r==NULL_PARAMETER)
	ASSERT("2.2cPre - change_name" , strcmp(challenge->name,"name") == 0)
	r = change_name(challenge, "newName");
	ASSERT("2.2c - change_name" , r==OK && \
			strcmp(challenge->name,"newName") == 0)

	r = set_best_time_of_challenge(NULL, 25);
	ASSERT("2.3a - set_best_time_of_challenge" , r==NULL_PARAMETER)
	r = set_best_time_of_challenge(challenge, -15);
	ASSERT("2.3b - set_best_time_of_challenge" , r==ILLEGAL_PARAMETER && \
			challenge->best_time == 0)
	ASSERT("2.3cPre - set_best_time_of_challenge" , challenge->best_time == 0)
	r = set_best_time_of_challenge(challenge, 25);
	ASSERT("2.3c - set_best_time_of_challenge" , r==OK && \
			challenge->best_time == 25)
	r = set_best_time_of_challenge(challenge, 30);
	ASSERT("2.3d - set_best_time_of_challenge" , r==OK && \
			challenge->best_time == 30)
	r = set_best_time_of_challenge(challenge, 15);
	ASSERT("2.3e - set_best_time_of_challenge" , r==ILLEGAL_PARAMETER && \
			challenge->best_time == 30)

	int time = 0;
	r = best_time_of_challenge(NULL, &time);
	ASSERT("2.4a - best_time_of_challenge" , r==NULL_PARAMETER)
	r = best_time_of_challenge(challenge, NULL);
	ASSERT("2.4b - best_time_of_challenge" , r==NULL_PARAMETER)
	r = set_best_time_of_challenge(challenge, 30);
	ASSERT("2.4cPre - best_time_of_challenge" , r==OK && time == 0)
	r = best_time_of_challenge(challenge, &time);
	ASSERT("2.4c - best_time_of_challenge" , r==OK && time == 30)

	r = inc_num_visits(NULL);
	ASSERT("2.5a - inc_num_visits" , r==NULL_PARAMETER)
	ASSERT("2.5bPre - inc_num_visits" , challenge->num_visits == 0)
	r = inc_num_visits(challenge);
	ASSERT("2.5b - inc_num_visits" , r==OK && challenge->num_visits == 1)

	int visits = -1;
	r = num_visits_function(NULL, &visits);
	ASSERT("2.6a - num_visits" , r==NULL_PARAMETER)
	r = num_visits_function(challenge, NULL);
	ASSERT("2.6b - num_visits" , r==NULL_PARAMETER)
	challenge->num_visits = 0;
	ASSERT("2.6cPre - num_visits" , visits == -1)
	r = num_visits_function(challenge, &visits);
	ASSERT("2.6c - num_visits" , r==OK && visits == 0)
	r = inc_num_visits(challenge);
	r = num_visits_function(challenge, &visits);
	ASSERT("2.6d - num_visits" , r==OK && visits == 1)

	r = reset_challenge(challenge);

	free(challenge);

	return 0;
}

int utVisitorRoom() {

	Result r=OK;
	Challenge *challenge = malloc(sizeof(Challenge));
	if (challenge == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	ChallengeActivity *activity = malloc(sizeof(ChallengeActivity));
	if (activity == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}

	r = init_challenge_activity(NULL, challenge);
	ASSERT("3.0a - init_challenge_activity" , r==NULL_PARAMETER)
	r = init_challenge_activity(activity, NULL);
	ASSERT("3.0b - init_challenge_activity" , r==NULL_PARAMETER)


	r = init_challenge(challenge, 1, "name", 1);
	activity->challenge = NULL;
	ASSERT("3.0cPre - init_challenge_activity" , r==OK && \
			activity->challenge == NULL)
	r = init_challenge_activity(activity, challenge);
	ASSERT("3.0c - init_challenge_activity" , r==OK && \
			activity->challenge == challenge && \
			activity->visitor == NULL && \
			activity->start_time == 0);

	r = reset_challenge_activity(NULL);
	ASSERT("3.1a - reset_challenge" , r==NULL_PARAMETER)

	ASSERT("3.1bPre - init_challenge" , activity->challenge == challenge)
	r = reset_challenge_activity(activity);
	ASSERT("3.1b - init_challenge" , r==OK && \
			activity->challenge == NULL && \
			activity->visitor == NULL && \
			activity->start_time == 0)

	Visitor *visitor = malloc(sizeof(Visitor));
	if (visitor == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	char* visitor_name = "VisitorName";
	r = init_visitor(NULL, visitor_name, 1001);
	ASSERT("3.2a - init_visitor" , r==NULL_PARAMETER)
	r = init_visitor(visitor, NULL, 1001);
	ASSERT("3.2b - init_visitor" , r==NULL_PARAMETER)
	visitor->visitor_name = NULL;
	ASSERT("3.2cPre - init_visitor" , visitor->visitor_name == NULL);
	int id = 1001;
	r = init_visitor(visitor, visitor_name, id);
	ASSERT("3.2c - init_visitor" , r==OK && \
			strcmp(visitor->visitor_name, "VisitorName") == 0 && \
			&(visitor->visitor_name) != &(visitor_name) && \
			visitor->visitor_id == id && \
			visitor->room_name == NULL && \
			visitor->current_challenge == NULL);

	r = reset_visitor(NULL);
	ASSERT("3.3a - reset_visitor" , r==NULL_PARAMETER)
	ASSERT("3.3bPre - reset_visitor" , visitor->visitor_name != NULL)
	r = reset_visitor(visitor);
	ASSERT("3.3b - reset_visitor" , r==OK && \
			visitor->visitor_name == NULL && \
			visitor->visitor_id == 0 && \
			visitor->room_name == NULL && \
			visitor->current_challenge == NULL)

	ChallengeRoom *room = malloc(sizeof(ChallengeRoom));
	char roomName[MAX_NAME_LENG];
	char *room_name = roomName;
	strcpy(room_name,"RoomName");
	int number_of_challenges = 3;
	r = init_room(NULL, room_name, number_of_challenges);
	ASSERT("3.4a - init_room" , r==NULL_PARAMETER)
	r = init_room(room, NULL, number_of_challenges);
	ASSERT("3.4b - init_room" , r==NULL_PARAMETER)
	r = init_room(room, room_name, 0);
	ASSERT("3.4c - init_room" , r==ILLEGAL_PARAMETER)
	r = init_room(NULL, NULL, 0);
	ASSERT("3.4d - init_room" , r==NULL_PARAMETER)

	r = init_room(room, room_name, number_of_challenges);
	ASSERT("3.4e - init_room" , r==OK && \
			strcmp(room->name, room_name) == 0 && \
			&(room->name) != &room_name && \
			room->num_of_challenges == number_of_challenges)
	char text[] = "3.4e xx - init_room";
	for(int i = 0; i < number_of_challenges; i++){
		sprintf(text,"3.4e %d - init_room", i);
		ASSERT(text, room->challenges[i].start_time == -1 && \
				room->challenges[i].visitor == NULL && \
				room->challenges[i].challenge == NULL);
	}
	r = reset_room(NULL);
	ASSERT("3.5a - reset_room" , r==NULL_PARAMETER)
	ASSERT("3.5bPre - reset_room" , room->name != NULL && \
			room->challenges != NULL && \
			room->num_of_challenges > 0)
	r = reset_room(room);
	ASSERT("3.5b - reset_room" , r==OK && \
			room->name == NULL && \
			room->challenges == NULL && \
			room->num_of_challenges == 0)

	Level level = Easy;
	int places = 0;
	r = num_of_free_places_for_level(NULL, level, &places);
	ASSERT("3.6a - num_of_free_places_for_level" , r==NULL_PARAMETER)
	r = num_of_free_places_for_level(room, level, NULL);
	ASSERT("3.6b - num_of_free_places_for_level" , r==NULL_PARAMETER)

	number_of_challenges = 9;
	Challenge *challenges = malloc(sizeof(Challenge) * number_of_challenges);
	if (challenge == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	char challenges_name[] = "challege_xx";
	//ChallengeRoom *room = malloc(sizeof(ChallengeRoom));
	if (challenge == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	r = init_room(room, room_name, number_of_challenges);
	for(int i = 0; i < number_of_challenges; i++) {
		sprintf(challenges_name,"challege_%02d", i);
		//printf("challe name: %s\n", challenges_name);
		if( i > 1 ) {
			if ( i < 5) {
				level = Medium;
			} else {
				level = Hard;
			}
		} else {
			level = Easy;
		}
		r = init_challenge(&challenges[i], i, challenges_name, level);
		if( r != OK ){
			printf("fail on 3.6Pre - num_of_free_places_for_level (init_challenge)");
			return 1;
		}
		r = init_challenge_activity(&(room->challenges[i]), &challenges[i]);
		if( r != OK ){
			printf("fail on 3.6Pre - num_of_free_places_for_level (init_challenge_activity)");
			return 1;
		}


	}


	ASSERT("3.6cPre - num_of_free_places_for_level" , places == 0)
	r = num_of_free_places_for_level(room, All_Levels, &places);
	ASSERT("3.6c0 - num_of_free_places_for_level" , r==OK && places == 9)
	r = num_of_free_places_for_level(room, Easy, &places);
	ASSERT("3.6c1 - num_of_free_places_for_level" , r==OK && places == 2)
	int time = 1;
	visitor_enter_room(room, visitor, Easy, time);
	r = num_of_free_places_for_level(room, Easy, &places);
	ASSERT("3.6c2 - num_of_free_places_for_level" , r==OK && places == 1)
	r = num_of_free_places_for_level(room, Medium, &places);
	ASSERT("3.6c3 - num_of_free_places_for_level" , r==OK && places == 3)
	r = num_of_free_places_for_level(room, Hard, &places);
	ASSERT("3.6c4 - num_of_free_places_for_level" , r==OK && places == 4)
	r = num_of_free_places_for_level(room, All_Levels, &places);
	ASSERT("3.6c5 - num_of_free_places_for_level" , r==OK && places == 8)


	//printf("room challe name: %s\n", (room->challenges[0]).challenge->name);

	char* new_name = "NewRoomName";
	ASSERT("3.7aPre - change_room_name" , strcmp(room->name,room_name) == 0)
	r = change_room_name(NULL, new_name);
	ASSERT("3.7a - change_room_name" , r==NULL_PARAMETER)
	r = change_room_name(room, NULL);
	ASSERT("3.7b - change_room_name" , r==NULL_PARAMETER)
	ASSERT("3.7cPre - change_room_name" , strcmp(room->name,room_name) == 0)
	r = change_room_name(room, new_name);
	ASSERT("3.7c - change_room_name" , r==OK && \
			strcmp(room->name,new_name) == 0 && \
			&(room->name) != &new_name)

	char *name = NULL;
	r = room_of_visitor(NULL, &name);
	ASSERT("3.8a - room_of_visitor" , r==NULL_PARAMETER)
	r = room_of_visitor(visitor, NULL);
	ASSERT("3.8b - room_of_visitor" , r==NULL_PARAMETER)

	Visitor *visitor2 = malloc(sizeof(Visitor));
	if (visitor2 == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	r = init_visitor(visitor2, visitor_name, id);
	r = room_of_visitor(visitor2, &name);
	ASSERT("3.8c - room_of_visitor" , r==NOT_IN_ROOM)
	ASSERT("3.8dPre - room_of_visitor" , name == NULL)
	r = room_of_visitor(visitor, &name);
	ASSERT("3.8d - room_of_visitor" , r==OK && \
			strcmp(name,new_name) == 0 && \
			&name != visitor->room_name)
	free(name);


	level = Easy;
	r = visitor_enter_room(NULL, visitor, level, time);
	ASSERT("3.9a - visitor_enter_room" , r==NULL_PARAMETER)
	r = visitor_enter_room(room, NULL, level, time);
	ASSERT("3.9b - visitor_enter_room" , r==NULL_PARAMETER)
	r = visitor_enter_room(room, visitor, level, time);
	ASSERT("3.9c - visitor_enter_room" , r==ALREADY_IN_ROOM)

	ASSERT("3.9dPre - visitor_enter_room" , visitor2->room_name == NULL && \
			visitor2->current_challenge == NULL && \
			(room->challenges[1]).visitor == NULL && \
			(room->challenges[1]).start_time == 0)
	r = visitor_enter_room(room, visitor2, level, time);
	ASSERT("3.9d - visitor_enter_room" , r==OK && \
			strcmp(*(visitor2->room_name), room->name) == 0 && \
			visitor2->room_name == &(room->name) && \
			visitor2->current_challenge == &(room->challenges[1]) && \
			(room->challenges[1]).visitor == visitor2 && \
			(room->challenges[1]).start_time == time )

	Visitor *visitor3 = malloc(sizeof(Visitor));
	if (visitor3 == NULL) {
		printf("MEMORY ISSUE!!!\n");
		return 1;
	}
	r = init_visitor(visitor3, visitor_name, id);

	r = visitor_enter_room(room, visitor3, level, time);
	ASSERT("3.9d - visitor_enter_room" , r==NO_AVAILABLE_CHALLENGES)
	r = visitor_enter_room(room, visitor2, level, time);
	ASSERT("3.9e - visitor_enter_room" , r==ALREADY_IN_ROOM)
	r = visitor_enter_room(room, NULL, level, time);
	ASSERT("3.9f - visitor_enter_room" , r==NULL_PARAMETER)
	r = visitor_enter_room(room, visitor3, Hard, time);
	ASSERT("3.9g - visitor_enter_room" , r==OK)

	int quitTime = 10;
	r = visitor_quit_room(NULL, quitTime);
	ASSERT("3.10a - visitor_quit_room" , r==NULL_PARAMETER)
	ASSERT("3.10bPre - visitor_quit_room" , \
			strcmp(*(visitor->room_name), room->name) == 0 && \
			visitor->current_challenge == &(room->challenges[0]) && \
			(room->challenges[0]).visitor == visitor && \
			(room->challenges[0]).start_time == time )
	r = visitor_quit_room(visitor, quitTime);
	best_time_of_challenge(&challenges[0], &time);
	ASSERT("3.10b - visitor_quit_room" , r==OK && \
			visitor->room_name == NULL && \
			visitor->current_challenge == NULL && \
			(room->challenges[0]).visitor == NULL && \
			(room->challenges[0]).start_time == 0 && \
			time == 9)
	r = visitor_quit_room(visitor, quitTime);
	ASSERT("3.10c - visitor_quit_room" , r==NOT_IN_ROOM)
	r = visitor_quit_room(visitor2, quitTime);
	ASSERT("3.10d - visitor_quit_room" , r==OK)
	r = visitor_quit_room(visitor3, quitTime);
	ASSERT("3.10e - visitor_quit_room" , r==OK)

	reset_visitor(visitor3);
	free(visitor3);
	reset_visitor(visitor2);
	free(visitor2);

	for(int i = 0; i < number_of_challenges; i++) {
		r = reset_challenge(&challenges[i]);
	}
	reset_room(room);
	free(challenges);


	free(room);
	free(visitor);
	r=reset_challenge(challenge);
	free(activity);
	free(challenge);
	return 0;
}


int utChallengeSystem_public() {
	Result r = OK;

	ChallengeRoomSystem *sys=NULL;
	r = create_system("text_1.txt", NULL);
	ASSERT("4.0a - create_system" , r==NULL_PARAMETER)
	ASSERT("4.0bPre - create_system" , sys == NULL)
	r = create_system("test_1.txt", &sys);
	ASSERT("4.0b - create_system" , r==OK && \
			sys != NULL)

	int destroy_time = 4;
	char *most_popular_name = NULL;
	char *best_time_name = NULL;
	r = destroy_system(NULL, destroy_time, &most_popular_name, &best_time_name);
	ASSERT("4.1a - destroy_system" , r==NULL_PARAMETER)
	r = destroy_system(sys, destroy_time, &most_popular_name,  &best_time_name);
	ASSERT("4.1b - destroy_system" , r==OK && \
			best_time_name == NULL && \
			most_popular_name == NULL)
	create_system("test_1.txt", &sys);
	char room_name[] = "room_1";
	char visitor1_name[] = "visitor_1", visitor2_name[] = "visitor_2";
	int visitor1_id = 101, visitor2_id = 102 ;
	Level level1 = Easy, level2 = Easy;
	int start_time = 5;
	r = visitor_arrive(sys, room_name, visitor1_name, \
			visitor1_id, level1, start_time);
	r = visitor_arrive(sys, room_name, visitor2_name, \
			visitor2_id, level2, start_time);
	r = destroy_system(sys, destroy_time, &most_popular_name, &best_time_name);
	ASSERT("4.1c - destroy_system", r==ILLEGAL_TIME )
	destroy_time = 10;
	ASSERT("4.1dPre - destroy_system", most_popular_name == NULL && \
			best_time_name == NULL)
	r = destroy_system(sys, destroy_time, &most_popular_name, &best_time_name);
	ASSERT("4.1d - destroy_system", r==OK && \
			most_popular_name != NULL && \
			strcmp(most_popular_name, "challenge_1") == 0 && \
			best_time_name != NULL && \
			strcmp(best_time_name, "challenge_1") == 0)
	free(most_popular_name);
	free(best_time_name);

	start_time = 5;
	create_system("test_1.txt", &sys);
	r = visitor_arrive(NULL, room_name, visitor1_name, visitor1_id, level1, start_time);
	ASSERT("4.2a - visitor_arrive", r==NULL_PARAMETER)
	r = visitor_arrive(sys, room_name, visitor1_name, visitor1_id, level1, -1);
	ASSERT("4.2b - visitor_arrive", r==ILLEGAL_TIME)
	r = visitor_arrive(sys, room_name, visitor1_name, visitor1_id, Medium, start_time);
	ASSERT("4.2c - visitor_arrive", r==NO_AVAILABLE_CHALLENGES)
	r = visitor_arrive(sys, NULL, visitor1_name, visitor1_id, level1, start_time);
	ASSERT("4.2d - visitor_arrive", r==ILLEGAL_PARAMETER)

	r = visitor_arrive(sys, room_name, NULL, visitor1_id, level1, start_time);
	ASSERT("4.2e - visitor_arrive", r==ILLEGAL_PARAMETER)
	char *tempName = NULL;
	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.2fPre - visitor_arrive", r==NOT_IN_ROOM)
	r = visitor_arrive(sys, room_name, visitor1_name, visitor1_id, level1, start_time);
	ASSERT("4.2f - visitor_arrive", r==OK)
	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.2fPost - visitor_arrive", r==OK && \
			strcmp(tempName, room_name) == 0)
	free(tempName);
	r = visitor_arrive(sys, room_name, visitor1_name, visitor1_id, Hard, start_time);
	ASSERT("4.2g - visitor_arrive", r==ALREADY_IN_ROOM)
	r = visitor_arrive(NULL, NULL, NULL, visitor2_id, level1, start_time);
	ASSERT("4.2h - visitor_arrive", r==NULL_PARAMETER)
	r = visitor_arrive(sys, room_name, NULL, visitor2_id, Medium, start_time);
	ASSERT("4.2i - visitor_arrive", r==NO_AVAILABLE_CHALLENGES)
	r = visitor_arrive(sys, NULL, visitor2_name, visitor2_id, level1, start_time);
	ASSERT("4.2j - visitor_arrive", r==ILLEGAL_PARAMETER)

	int quit_time = 3;
	r = visitor_quit(sys, visitor1_id, quit_time);
	ASSERT("4.3a - visitor_quit", r==ILLEGAL_TIME)
	quit_time = 6;
	r = visitor_quit(NULL, visitor1_id, quit_time);
	ASSERT("4.3b - visitor_quit", r==NULL_PARAMETER)
	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.3cPre - visitor_quit", r==OK && \
			strcmp(tempName, room_name) == 0)
	free(tempName);
	r = visitor_quit(sys, visitor1_id, quit_time);
	ASSERT("4.3c - visitor_quit", r==OK)	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.3cPost - visitor_quit", r==NOT_IN_ROOM)
	r = visitor_quit(sys, visitor1_id, quit_time);
	ASSERT("4.3d - visitor_quit", r==NOT_IN_ROOM)

	quit_time = 3;
	r = all_visitors_quit(sys, quit_time);
	ASSERT("4.4a - all_visitors_quit", r==ILLEGAL_TIME)
	quit_time = 8;
	r = all_visitors_quit(NULL, quit_time);
	ASSERT("4.4b - all_visitors_quit", r==NULL_PARAMETER)
	start_time = 7;
	r = visitor_arrive(sys, room_name, visitor1_name, visitor1_id, level1, start_time);
	ASSERT("4.4cPre1 - all_visitors_quit", r==OK)
	r = visitor_arrive(sys, room_name, visitor2_name, visitor2_id, level2, start_time);
	ASSERT("4.4cPre2 - all_visitors_quit", r==OK)
	r = all_visitors_quit(sys, quit_time);
	ASSERT("4.4c - all_visitors_quit", r==OK)
	r = all_visitors_quit(sys, quit_time);
	ASSERT("4.4d - all_visitors_quit", r==NOT_IN_ROOM)


	r = system_room_of_visitor(NULL, visitor1_name, &tempName);
	ASSERT("4.5a - system_room_of_visitor", r==NULL_PARAMETER)
	r = system_room_of_visitor(sys, NULL, &tempName);
	ASSERT("4.5b - system_room_of_visitor", r==ILLEGAL_PARAMETER)
	r = system_room_of_visitor(sys, visitor1_name, NULL);
	ASSERT("4.5c - system_room_of_visitor", r==ILLEGAL_PARAMETER)
	r = system_room_of_visitor(NULL, NULL, NULL);
	ASSERT("4.5d - system_room_of_visitor", r==NULL_PARAMETER)
	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.5e - system_room_of_visitor", r==NOT_IN_ROOM)
	start_time = 10;
	r = visitor_arrive(sys, room_name, visitor1_name, visitor1_id, level1, start_time);
	ASSERT("4.5fPre - system_room_of_visitor", r==OK)
	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.5f - system_room_of_visitor", r==OK)
	free(tempName);
	r = system_room_of_visitor(sys, visitor1_name, &tempName);
	ASSERT("4.5g - system_room_of_visitor", r==OK)
	free(tempName);

	char new_name[] = "challengeNewName";
	int challenge_id = 11;
	r = change_challenge_name(NULL, challenge_id, new_name);
	ASSERT("4.6a - change_challenge_name", r==NULL_PARAMETER)
	r = change_challenge_name(sys, challenge_id, NULL);
	ASSERT("4.6b - change_challenge_name", r==NULL_PARAMETER)
	challenge_id = 12;
	r = change_challenge_name(sys, challenge_id, new_name);
	ASSERT("4.6c - change_challenge_name", r==ILLEGAL_PARAMETER)
	challenge_id = 11;
	int temp_time = 0;
	r=best_time_of_system_challenge(sys, "challenge_1", &temp_time);
	ASSERT("4.6dPre1 - change_challenge_name", r==OK && \
			temp_time == 1)
	r=best_time_of_system_challenge(sys, "challengeNewName", &temp_time);
	ASSERT("4.6dPre1 - change_challenge_name", r==ILLEGAL_PARAMETER && \
			temp_time == 0)
	r = change_challenge_name(sys, challenge_id, new_name);
	ASSERT("4.6d - change_challenge_name", r==OK)
	r=best_time_of_system_challenge(sys, "challenge_1", &temp_time);
	ASSERT("4.6dPost1 - change_challenge_name", r==ILLEGAL_PARAMETER && \
			temp_time == 0)
	r=best_time_of_system_challenge(sys, "challengeNewName", &temp_time);
	ASSERT("4.6dPost2 - change_challenge_name", r==OK && \
			temp_time == 1)


	r = destroy_system(sys, destroy_time, &most_popular_name, &best_time_name);
	free(most_popular_name);
	free(best_time_name);


	return 0;
}

