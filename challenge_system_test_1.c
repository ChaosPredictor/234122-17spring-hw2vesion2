#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "challenge_system.h"

#define ASSERT(test_number, test_condition)  \
   if (!(test_condition)) {printf("\nTEST %s FAILED", test_number); } \
   else printf("\nTEST %s OK", test_number);

int official();
int unittest();
int utChallenge();
int utVisitorRoom();

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
	printf("\n   Visitor_Room   \n");
	utVisitorRoom();
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
	r = init_challenge(challenge, 1, NULL, 1);
	ASSERT("2.0c - init_challenge" , r==NULL_PARAMETER)
	r = init_challenge(challenge, 1, "name", 1);
	ASSERT("2.0d - init_challenge" , r==OK && \
			strcmp(challenge->name,"name") == 0 && \
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
	r = set_best_time_of_challenge(challenge, 15);
	ASSERT("2.3d - set_best_time_of_challenge" , r==ILLEGAL_PARAMETER && \
			challenge->best_time == 25)

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
	ASSERT("3.0cPre - init_challenge" , r==OK && \
			activity->challenge != challenge)
	r = init_challenge_activity(activity, challenge);
	ASSERT("3.0c - init_challenge" , r==OK && \
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
	r = init_visitor(NULL, "visitor_name", 1001);
	ASSERT("3.2a - init_visitor" , r==NULL_PARAMETER)
	r = init_visitor(visitor, NULL, 1001);
	ASSERT("3.2b - init_visitor" , r==NULL_PARAMETER)

	ASSERT("3.2cPre - init_visitor" , visitor->visitor_name == NULL);
	char* visitor_name = "VisitorName";
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



	free(visitor);
	r=reset_challenge(challenge);
	ASSERT("3.x - init_challenge" , r==OK)
	free(activity);
	free(challenge);
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
