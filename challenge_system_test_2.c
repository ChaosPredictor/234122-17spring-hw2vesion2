#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "challenge_system.h"

#define ASSERT(test_number, test_condition)  \
   if (!(test_condition)) {printf("TEST %s FAILED\n", test_number); } \
   else printf("TEST %s OK\n", test_number);

/*FOR DEBUG*/
void printResult(Result r) {
	switch (r) {
	case OK:
		printf("OK\n");
		break;
	case NULL_PARAMETER:
		printf("NULL_PARAMETER\n");
		break;
	case ILLEGAL_PARAMETER:
		printf("ILLEGAL_PARAMETER\n");
		break;
	case ILLEGAL_TIME:
		printf("ILLEGAL_TIME\n");
		break;
	case MEMORY_PROBLEM:
		printf("MEMORY_PROBLEM\n");
		break;
	case NOT_IN_ROOM:
		printf("NOT_IN_ROOM\n");
		break;
	case NO_AVAILABLE_CHALLENGES:
		printf("NO_AVAILABLE_CHALLENGES\n");
		break;
	case ALREADY_IN_ROOM:
		printf("ALREADY_IN_ROOM\n");
		break;
	}
}

void test_1() {
	printf("test 1\n");
	ChallengeRoomSystem *sys = NULL;
	Result r = OK;
	r = create_system("test_1.txt", &sys);
	r = visitor_arrive(sys, "room_2", "visitor_1", 201, Medium, 5);
	r = visitor_arrive(sys, "room_1", "visitor_2", 202, Easy, 8);
	r = visitor_quit(sys, 203, 10);
	ASSERT("1.1", r == NOT_IN_ROOM)
	r = visitor_quit(sys, 201, 9);
	ASSERT("1.2", r == OK)
	int time;
	r = best_time_of_system_challenge(sys, "challenge_2", &time);
	ASSERT("1.3", time == 4)
	r = change_system_room_name(sys, "room_1", "room_111");
	r = visitor_arrive(sys, "room_1", "visitor_3", 203, Easy, 8);
	//CHECKED WITH TIME=10; RESULT NULL_PARAMETER. SUPPORTED BY EX2 RULES?
	ASSERT("1.4", r == ILLEGAL_TIME)
	r = visitor_arrive(sys, "room_111", "visitor_3", 203, Easy, 8);
	ASSERT("1.5", r == ILLEGAL_TIME)
	r = visitor_arrive(sys, "room_111", "visitor_3", 203, Easy, 15);
	ASSERT("1.6", r == OK)
	r = visitor_arrive(sys, "room_111", "visitor_4", 204, Easy, 16);
	ASSERT("1.7", r == NO_AVAILABLE_CHALLENGES)
	r = change_challenge_name(sys, 11, "challenge_1111");
	r = best_time_of_system_challenge(sys, "challenge_1111", &time);
	ASSERT("1.8", time == 0)
	char *namep = NULL;
	r = most_popular_challenge(sys, &namep);
	ASSERT("1.9", namep!=NULL && strcmp(namep, "challenge_1111")==0)
	free(namep);
	char *room = NULL;
	r = system_room_of_visitor(sys, "visitor_4", &room);
	ASSERT("1.10", r == NOT_IN_ROOM)
	free(room);
	r = system_room_of_visitor(sys, "visitor_3", &room);
	ASSERT("1.11", r==OK && room!=NULL && strcmp(room, "room_111")==0)
	free(room);
	r = all_visitors_quit(sys, 17);
	r = best_time_of_system_challenge(sys, "challenge_1111", &time);
	ASSERT("1.12", time == 9)
	r = best_time_of_system_challenge(sys, "challenge_4", &time);
	ASSERT("1.13", time == 2)
	char *most_popular_challenge = NULL, *challenge_best_time = NULL;
	r = destroy_system(sys, 18, &most_popular_challenge, &challenge_best_time);
	ASSERT("1.14",
			most_popular_challenge!=NULL && strcmp(most_popular_challenge, "challenge_1111")==0)
	ASSERT("1.15",
			challenge_best_time!=NULL && strcmp(challenge_best_time, "challenge_4")==0)
	free(most_popular_challenge);
	free(challenge_best_time);
}

void test_2() {
	printf("test 2\n");
	ChallengeRoomSystem *sys = NULL;
	Result r = OK;
	r = create_system("test_2.txt", &sys);
	r = visitor_arrive(sys, "room_1", "visitor_1", 201, Medium, 2);
	ASSERT("2.1", r == NO_AVAILABLE_CHALLENGES)
	r = visitor_arrive(sys, "room_1", "visitor_1", 201, Hard, 2);
	ASSERT("2.2", r == OK)
	r = visitor_arrive(sys, "room_1", "visitor_2", 202, All_Levels, 4);
	ASSERT("2.3", r == NO_AVAILABLE_CHALLENGES)
	r = visitor_quit(sys, 201, 5); //duration = 3 challenge_2

	int time;

	r = best_time_of_system_challenge(sys, "challenge_2", &time);
	ASSERT("2.4", time == 3);

	r = visitor_arrive(sys, "room_1", "visitor_2", 202, All_Levels, 6);
	ASSERT("2.5", r == OK)
	r = visitor_arrive(sys, "room_2", "visitor_1", 201, All_Levels, 8); //challenge_1
	ASSERT("2.6", sys->rooms[1].challenges[0].visitor->visitor_id == 201)

	r = visitor_arrive(sys, "room_3", "visitor_11", 211, Easy, 9);
	r = visitor_arrive(sys, "room_3", "visitor_12", 212, Medium, 9);
	r = visitor_arrive(sys, "room_3", "visitor_13", 213, Hard, 9);
	r = visitor_arrive(sys, "room_3", "visitor_14", 214, All_Levels, 100);
	ASSERT("2.7", r == NO_AVAILABLE_CHALLENGES)

	r = visitor_arrive(sys, "room_2", "visitor_3", 223, Medium, 12); //challenge_6
	ASSERT("2.8", sys->rooms[1].challenges[5].visitor->visitor_id == 223)
	r = visitor_arrive(sys, "room_2", "visitor_4", 224, All_Levels, 13); //challenge_10
	ASSERT("2.9", sys->rooms[1].challenges[9].visitor->visitor_id == 224)
	r = visitor_arrive(sys, "room_2", "visitor_5", 225, All_Levels, 14); //challenge_2
	ASSERT("2.10", r == OK)

	char* namep = NULL;
	r = system_room_of_visitor(sys, "visitor_1", &namep);
	ASSERT("2.11", namep!=NULL && strcmp(namep, "room_2")==0)
	free(namep);
	r = system_room_of_visitor(sys, "visitor_9999", &namep);
	ASSERT("2.12", r == NOT_IN_ROOM)
	r = system_room_of_visitor(sys, "visitor_2", &namep);
	ASSERT("2.13", namep!=NULL && strcmp(namep, "room_1")==0)
	free(namep);

	r = visitor_quit(sys, 223, 5);
	ASSERT("2.14", r == ILLEGAL_TIME)
	r = visitor_quit(sys, 202, 15); //duration = 9 challenge_2
	r = visitor_quit(sys, 201, 18); //duration = 10  challenge_1

	r = visitor_quit(sys, 211, 19); //duration = 10 challenge_3
	r = visitor_quit(sys, 212, 19); //duration = 10 challenge_1
	r = visitor_quit(sys, 213, 19); //duration = 10 challenge_2

	r = system_room_of_visitor(sys, "visitor_13", &namep);
	ASSERT("2.15", r == NOT_IN_ROOM)
	r = best_time_of_system_challenge(sys, "challenge_2", &time);
	ASSERT("2.16", time == 3);
	r = best_time_of_system_challenge(sys, "challenge_8", &time);
	ASSERT("2.17", time == 0);
	ASSERT("2.18", r == OK)
	r = best_time_of_system_challenge(sys, "challenge_12", &time);
	ASSERT("2.19", r == ILLEGAL_PARAMETER);
	r = best_time_of_system_challenge(sys, "challenge_1", &time);
	ASSERT("2.20", time == 10);

	/* in the system now: (room_2)
	 * visitor_3 223 in challenge_6
	 * visitor_4 224 in challenge_1
	 * visitor_5 225 in challenge_2 */

	r = change_challenge_name(sys, 22, "challenge_22");
	ASSERT("2.21", r == OK);
	r = change_challenge_name(sys, 22, NULL);
	ASSERT("2.22", r == NULL_PARAMETER);
	r = change_challenge_name(sys, 222222, "challenge_22");
	ASSERT("2.23", r == ILLEGAL_PARAMETER);

	r = best_time_of_system_challenge(sys, "challenge_2", &time);
	ASSERT("2.24", r == ILLEGAL_PARAMETER);
	r = best_time_of_system_challenge(sys, "challenge_22", &time);
	ASSERT("2.25", time == 3);
	r = most_popular_challenge(sys, &namep);
	ASSERT("2.26", namep!=NULL && strcmp(namep, "challenge_22")==0)
	free(namep);
	r = visitor_arrive(sys, "room_1", "visitor_9", 99, Hard, 20);
	ASSERT("2.27", r == OK);

	r = visitor_quit(sys, 99, 30); //duration = 10 challenge_2

	r = visitor_arrive(sys, "room_4", "visitor_11", 1111, Hard, 30);
	ASSERT("2.28", r == NO_AVAILABLE_CHALLENGES);
	r = visitor_arrive(sys, "room_4", "visitor_11", 1111, Easy, 30);
	r = visitor_arrive(sys, "room_4", "visitor_22", 2222, Easy, 30);
	r = visitor_arrive(sys, "room_4", "visitor_33", 3333, Easy, 30);
	ASSERT("2.28", r == NO_AVAILABLE_CHALLENGES);
	r = visitor_arrive(sys, "room_4", "visitor_33", 3333, Medium, 30);
	ASSERT("2.29", r == OK);

	r = system_room_of_visitor(sys, "visitor_11", &namep);
	ASSERT("2.30", namep!=NULL && strcmp(namep, "room_4")==0)
	free(namep);
	r = change_system_room_name(sys, "room_4", "room_99");
	r = system_room_of_visitor(sys, "visitor_11", &namep);
	ASSERT("2.31", namep!=NULL && strcmp(namep, "room_99")==0)
	free(namep);
	r = visitor_arrive(sys, "room_4", "visitor_44", 4444, All_Levels, 30);
	ASSERT("2.32", r == ILLEGAL_PARAMETER);
	r = visitor_arrive(sys, "room_99", "visitor_44", 4444, All_Levels, 30);
	ASSERT("2.33", r == OK);

	char *most_popular_challenge = NULL, *challenge_best_time = NULL;
	r = destroy_system(sys, 25, &most_popular_challenge, &challenge_best_time);
	ASSERT("2.34", r == ILLEGAL_TIME);
	r = destroy_system(sys, 40, &most_popular_challenge, &challenge_best_time);
	ASSERT("2.35",
			most_popular_challenge!=NULL && strcmp(most_popular_challenge, "challenge_22")==0)
	ASSERT("2.36",
			challenge_best_time!=NULL && strcmp(challenge_best_time, "challenge_22")==0)
	free(most_popular_challenge);
	free(challenge_best_time);
}

void test_3() {
	printf("test 3\n");
	ChallengeRoomSystem *sys = NULL;
	Result r = OK;
	r = create_system("test_3.txt", &sys);
	ASSERT("3.1", r == ILLEGAL_PARAMETER);
}

void test_4() {
	printf("test 4\n");
	ChallengeRoomSystem *sys = NULL;
	Result r = OK;
	r = create_system("test_4.txt", &sys);
	ASSERT("4.1", r == OK);
	char *most_popular_challenge = NULL, *challenge_best_time = NULL;
	r = destroy_system(sys, 25, &most_popular_challenge, &challenge_best_time);
	ASSERT("4.2", r == OK);
	ASSERT("4.3", most_popular_challenge==NULL && challenge_best_time==NULL)
	free(most_popular_challenge);
	free(challenge_best_time);
}

//test lexicographical differences
void test_5() {
	printf("test 5:\n");
	ChallengeRoomSystem *sys = NULL;
	Result r = OK;
	r = create_system("test_5.txt", &sys);
	ASSERT("5.1", r == OK);
	r = visitor_arrive(sys, "room_1", "visitor_1", 111, All_Levels, 0);
	r = visitor_quit(sys, 111, 2); //duration = 2 bhallenge_2
	char* namep = NULL;
	r = most_popular_challenge(sys, &namep);
	ASSERT("5.2", namep !=NULL && strcmp(namep, "bhallenge_2")==0)
	free(namep);
	r = visitor_arrive(sys, "room_1", "visitor_1", 111, All_Levels, 2);
	r = visitor_arrive(sys, "room_1", "visitor_2", 222, All_Levels, 2);
	r = visitor_arrive(sys, "room_2", "visitor_3", 333, All_Levels, 2);
	r = all_visitors_quit(sys, 4);
	ASSERT("5.3", r == OK);

	char *popular_challenge = NULL, *challenge_best_time = NULL;
	r = most_popular_challenge(sys, &popular_challenge);
	ASSERT("5.4",
			popular_challenge !=NULL && strcmp(popular_challenge, "bhallenge_2")==0)
	free(popular_challenge);

	r = change_challenge_name(sys, 22, "challenge_2");
	r = most_popular_challenge(sys, &popular_challenge);
	ASSERT("5.5",
			popular_challenge !=NULL && strcmp(popular_challenge, "challenge_1")==0)
	free(popular_challenge);

	r = change_challenge_name(sys, 22, "bhallenge_2");
	r = most_popular_challenge(sys, &popular_challenge);
	ASSERT("5.6",
			popular_challenge !=NULL && strcmp(popular_challenge, "bhallenge_2")==0)
	free(popular_challenge);

	r = visitor_arrive(sys, "room_1", "visitor_5", 555, All_Levels, 5);
	r = visitor_arrive(sys, "room_1", "visitor_6", 666, All_Levels, 5);

	r = destroy_system(sys, 25, &popular_challenge, &challenge_best_time);
	ASSERT("5.7", r == OK);
	ASSERT("5.8",
			popular_challenge!=NULL && strcmp(popular_challenge, "bhallenge_2")==0)
	ASSERT("5.9",
			challenge_best_time!=NULL && strcmp(challenge_best_time, "bhallenge_2")==0)
	free(popular_challenge);
	free(challenge_best_time);
}

//test order of errors
void test_6() {
	printf("test 6:\n");
	ChallengeRoomSystem *sys = NULL;
	Result r = OK;
	r = create_system("test_6.txt", &sys);
	ASSERT("6.1", r == OK);

	char* namep= NULL;
	int time=1;
	r = most_popular_challenge(sys, &namep);
	ASSERT("6.2", namep == NULL)
	r= best_time_of_system_challenge(sys,"challenge_1",&time);
	ASSERT("6.3", time== 0)

	r = visitor_arrive(sys, "room_1", "visitor_1", 1111, Easy, 5);
	r = visitor_arrive(sys, "room_1", NULL, 2222, Hard, 2);
	ASSERT("6.3", r == ILLEGAL_TIME);
	r = visitor_arrive(NULL, "room_1", NULL, 2222, Hard, 5);
	ASSERT("6.4", r == NULL_PARAMETER);
	r = visitor_arrive(sys, "room_1", NULL, 2222, Hard, 5);
	ASSERT("6.5", r == ILLEGAL_PARAMETER);
	r = visitor_arrive(sys, NULL, "visitor_2", 2222, Hard, 5);
	ASSERT("6.6", r == ILLEGAL_PARAMETER);
	r = visitor_arrive(sys, NULL, "visitor_1", 1111, Hard, 5);
	ASSERT("6.7", r == ILLEGAL_PARAMETER);
	r = visitor_arrive(sys, NULL, "visitor_1", 1111, Hard, 5);
	ASSERT("6.8", r == ILLEGAL_PARAMETER);
	r = visitor_arrive(sys, "room_1", "visitor_1", 1111, Hard, 5);
	ASSERT("6.9", r == ALREADY_IN_ROOM);
	r = visitor_arrive(sys, "room_1", "visitor_2", 2222, Hard, 5);
	ASSERT("6.10", r == NO_AVAILABLE_CHALLENGES);
	r = visitor_arrive(sys, "room_1", "visitor_2", 2222, All_Levels, 5);
	ASSERT("6.11", r == OK);

	r = visitor_quit(NULL, 3333, 10);
	ASSERT("6.12", r == NULL_PARAMETER);
	r = visitor_quit(sys, 3333, 2);
	ASSERT("6.13", r == ILLEGAL_TIME);

	r = system_room_of_visitor(NULL, "visitor_101010", NULL);
	ASSERT("6.14", r == NULL_PARAMETER)
	r = system_room_of_visitor(sys, "visitor_101010", NULL);
	ASSERT("6.15", r == ILLEGAL_PARAMETER);
	r = system_room_of_visitor(sys, NULL, &namep);
	ASSERT("6.16", r == ILLEGAL_PARAMETER);

	r = change_challenge_name(NULL, 11, "matam");
	ASSERT("6.17", r == NULL_PARAMETER);
	r = change_challenge_name(sys, 11111, NULL);
	ASSERT("6.18", r == NULL_PARAMETER);
	r = change_challenge_name(sys, 1111, "matam");
	ASSERT("6.19", r == ILLEGAL_PARAMETER);

	r = change_system_room_name(NULL, "room_1", ".rom.");
	ASSERT("6.20", r == NULL_PARAMETER);
	r = change_system_room_name(sys, NULL, ".rom.");
	ASSERT("6.21", r == NULL_PARAMETER);
	r = change_system_room_name(sys, "room_111", NULL);
	ASSERT("6.22", r == NULL_PARAMETER);
	r = change_system_room_name(sys, "room_111", ".rom.");
	ASSERT("6.23", r == ILLEGAL_PARAMETER);

	char *most_popular_challenge = NULL, *challenge_best_time = NULL;
	r = destroy_system(sys, 25, &most_popular_challenge, &challenge_best_time);
	ASSERT("6.23", r == OK);
	free(most_popular_challenge);
	free(challenge_best_time);

}
int main(int argc, char **argv) {
	setvbuf(stdout, NULL, _IONBF, 0); //FOR OR ECLIPSE
	test_1();
	test_2();
	test_3();
	test_4();
	test_5();
	test_6();
	return 0;
}

