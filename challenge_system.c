#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge_system.h"

#define MAX_NAME_LENG 51 //Max leng 50 + 1 of end of string

//typedef char* Name;

typedef struct VisitorNodeStr {
	Visitor *visitor;
	struct VisitorNodeStr* next;
} VisitorNode;

Result find_best_time_of_system(ChallengeRoomSystem *sys, char **challenge_name);
Result nameRead(char* name, FILE* inputFile);
Result numberRead(int* number, FILE* inputFile);
Result challengeRead(Challenge* challenge, FILE* inputFile);
Result challengeRoomRead(ChallengeRoom* challengeRoom, FILE* inputFile);
Challenge* findChallengeById(ChallengeRoomSystem *sys, int id);
Challenge* findChallengeByName(ChallengeRoomSystem *sys, char *name);
ChallengeRoom* findRoomByName(ChallengeRoomSystem *sys, char* name);
VisitorNode* createVisitorNode(Visitor* visitor);
VisitorNode* findVisitorNodebyName(ChallengeRoomSystem *sys, char *name);
Result printAllVisitor(ChallengeRoomSystem *sys);
VisitorNode* findVisitorNodebyId(ChallengeRoomSystem *sys, int id);
Result removeVisitorNodebyId(ChallengeRoomSystem *sys, int id);




Result create_system(char *init_file, ChallengeRoomSystem **sys) {

	FILE *file = fopen(init_file, "r");
	if (file == NULL) {
		return NULL_PARAMETER;
	}

	*sys = malloc(sizeof(**sys));
	if (*sys == NULL) {
		fclose(file);
		return MEMORY_PROBLEM;
	}

	char tempName[MAX_NAME_LENG];
	nameRead(tempName, file);
	//TODO - check return
	(*sys)->name = malloc(sizeof(char) * (strlen(tempName) + 1));
	if ((*sys)->name == NULL) {
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}
	//snprintf((*sys)->name, strlen(tempName)+1, "%s", tempName);
	strcpy((*sys)->name, tempName);

	// READ challenges
	int tempNumber = 0;
	numberRead(&tempNumber, file); //number of Challenges
	//TODO - check return
	(*sys)->numberOfChallenges = tempNumber;
	(*sys)->challenges = malloc(sizeof(Challenge) * tempNumber);
	if ((*sys)->challenges == NULL) {
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}
	int id;
	unsigned int level;
	Result result = OK;
	for(int i = 0; i < (*sys)->numberOfChallenges; i++) {
		if ( nameRead(tempName, file) == OK && \
				fscanf(file, " %d %u\n", &id, &level)) {
				//MINUS 1 caused by offset 1-Easy 2-Medium 3-Hard should by 0-2
				result = init_challenge(&((*sys)->challenges[i]), id, tempName, level-1);
				if ( result == OK ) {
					continue;
				}
						//TODO - what to do if only part succeed
		}
		free((*sys)->challenges);
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}


	fclose(file);
	return OK;
}


Result destroy_system(ChallengeRoomSystem *sys, int destroy_time, char **most_popular_challenge_p, char **challenge_best_time) {
	//TODO - testing
	if( sys == NULL || most_popular_challenge_p == NULL || challenge_best_time == NULL ) {
		return NULL_PARAMETER;
	}
	/*if( sys->lastTime > destroy_time) {
		return ILLEGAL_TIME;
	}
	Result result = most_popular_challenge(sys, most_popular_challenge_p);
	if( result != OK ) {
		return result;
	}
	result = find_best_time_of_system(sys, challenge_best_time);
	if( result != OK ) {
		return result;
	}*/
	reset_challenge(&(sys->challenges[0]));
	reset_challenge(&(sys->challenges[1]));
	reset_challenge(&(sys->challenges[2]));
	reset_challenge(&(sys->challenges[3]));
	reset_challenge(&(sys->challenges[4]));
	reset_challenge(&(sys->challenges[5]));
	free(sys->challenges);
	free(sys->name);
	free(sys);
	//TODO release mallocs
	return OK;
}


Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name, char *visitor_name, int visitor_id, Level level, int start_time) {

	if( sys == NULL) {
		//printf("1\n");
		return NULL_PARAMETER;
	}

	if( sys->lastTime > start_time) {
		//printf("2\n");
		return ILLEGAL_TIME;
	}

	if( (room_name == NULL) || (visitor_name == NULL) ) {
		//printf("3\n");
		return ILLEGAL_PARAMETER;
	}

	ChallengeRoom* room = findRoomByName(sys, room_name);
	int places = 0;
	Result result = num_of_free_places_for_level(room, level, &places);
	if( result != OK ) {
		//printf("\n4\n");
		return result;
	}
	if( places == 0 ) {
		//printf("5\n");
		return NO_AVAILABLE_CHALLENGES;
	}

	if( findVisitorNodebyId(sys, visitor_id) != NULL ) {
		//printf("6\n");
		return ALREADY_IN_ROOM;
	}

	Visitor* visitor = malloc(sizeof(*visitor));
	if( visitor == NULL ) {
		//printf("7\n");
		return MEMORY_PROBLEM;
	}
	//printf("\n\n	NAME:%s\n\n", visitor_name);
	result = init_visitor(visitor, visitor_name, visitor_id);
	if( result != OK) {
		free(visitor);
		//printf("8\n");
		return result;
	}
	VisitorNode* visitorNode = createVisitorNode(visitor);
	if(visitorNode == NULL ) {
		free(visitor);
		//printf("9\n");
		return MEMORY_PROBLEM;
	}
	VisitorNode* pVisitor = sys->visitor_head;
	while ( pVisitor->next ) {
		pVisitor = pVisitor->next;
	}
	pVisitor->next = visitorNode;
	result = visitor_enter_room(room, visitor, level, start_time);
	if (result != OK ) {
		free(visitor);
		//printf("10\n");
		return result;
	}
	sys->lastTime = start_time;
	//printAllVisitor(sys);
	//printf("0\n");
	return OK;
}


Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
	if( sys == NULL) {
		return NULL_PARAMETER;
	}
	if( sys->lastTime > quit_time) {
		return ILLEGAL_TIME;
	}
	VisitorNode *visitorNode = findVisitorNodebyId(sys, visitor_id);
	if( visitorNode == NULL ) {
		return NOT_IN_ROOM;
	}
	//printf("\n");
	//printAllVisitor(sys);
	Result result = visitor_quit_room(visitorNode->visitor, quit_time);
	if (result != OK ) {
		return result;
	}
	Visitor* keepVisitor = visitorNode->visitor;
	removeVisitorNodebyId(sys, visitor_id);
	free(keepVisitor);
	//TODO can free visitor
	//printf("\n");
	//printAllVisitor(sys);
	//Visitor* visitor = visitorNode->visitor;
	//free(visitor);
	//TODO - why memory problem?
	sys->lastTime = quit_time;
	return OK;
}

Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
	//TODO - testing
	if( sys == NULL) {
		return NULL_PARAMETER;
	}
	if( sys->lastTime > quit_time) {
		return ILLEGAL_TIME;
	}

	Result result;
	VisitorNode* pVisitor = sys->visitor_head, *temp;

	while ( pVisitor->next ) {
		//printf("\nvisitor %s left\n", pVisitor->visitor->visitor_name);
		temp = pVisitor;
		pVisitor = temp->next;
		//printf("\nvisitor %s left\n", pVisitor->visitor->visitor_name);
		//printf("run\n");
		result = visitor_quit_room(pVisitor->visitor, quit_time);
		if (result != OK ) {
			return result;
		}
		//printf("\nvisitor %s left\n", pVisitor->visitor->visitor_name);
		free(temp->visitor);
    free(temp);
	}
	return OK;
}


Result system_room_of_visitor(ChallengeRoomSystem *sys, char *visitor_name, char **room_name) {
	//TODO - testing
	if (sys == NULL ) {
		return NULL_PARAMETER;
	}
	if (visitor_name == NULL || room_name == NULL) {
		return ILLEGAL_PARAMETER;
	}
	VisitorNode* visitorNode = findVisitorNodebyName(sys, visitor_name);
	if( visitorNode == NULL ) {
		return NOT_IN_ROOM;
	}
	Result result = room_of_visitor(visitorNode->visitor, room_name);
	if( result != OK ) {
		return result;
	}
	return OK;
}

Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id, char *new_name) {
	//TODO testing
	if( sys == NULL || new_name == NULL ) {
		return NULL_PARAMETER;
	}
	Challenge *challenge = findChallengeById(sys, challenge_id);
	if ( challenge == NULL ) {
		return ILLEGAL_PARAMETER;
	}
	challenge->name = realloc(challenge->name, sizeof(char) * (strlen(new_name) + 1));
	if ( challenge->name == NULL) {
		return MEMORY_PROBLEM;
	}
	strcpy(challenge->name, new_name);
	return OK;
}

Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,  char *new_name) {
	//TODO testing
	if( sys == NULL || current_name == NULL || new_name == NULL ) {
		return NULL_PARAMETER;
	}
	ChallengeRoom *room = findRoomByName(sys, current_name);
	if ( room == NULL ) {
		return ILLEGAL_PARAMETER;
	}
	return change_room_name(room, new_name);
}

Result best_time_of_system_challenge(ChallengeRoomSystem *sys, char *challenge_name, int *time) {
	//TODO testing
	if( sys == NULL || challenge_name == NULL || time == NULL ) {
		return NULL_PARAMETER;
	}
	Challenge* challenge = findChallengeByName(sys, challenge_name);
	if( challenge == NULL ) {
		return ILLEGAL_PARAMETER;
	}
	Result result = best_time_of_challenge(challenge, time);
	if( result != OK ) {
		return result;
	}
	return OK;
}


Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name) {
	//TODO testing
	if( sys == NULL || challenge_name == NULL ) {
		return NULL_PARAMETER;
	}
	int number_of_challenges = (*sys).numberOfChallenges;
	int max = 0, temp;
	char tempName[MAX_NAME_LENG] = "";
	*challenge_name = NULL;
	for(int i = 0; i < number_of_challenges; i++) {
		Result result = num_visits_function(&((*sys).challenges[i]), &temp);
		if( result != OK ){
			return result;
		}
		//temp = (*sys).challenges[i].num_visits;
		if ( temp > max) {
			max = temp;
			strcpy(tempName, (*sys).challenges[i].name);
		} else if( temp == max && strcmp(tempName, (*sys).challenges[i].name) > 0) {
			strcpy(tempName, (*sys).challenges[i].name);
		}
	}
	//printf("tempName: %s\n", tempName);
	if ( strcmp(tempName, "") != 0 ) {
		*challenge_name = malloc(sizeof(char) * (strlen(tempName) + 1) );
		if( *challenge_name == NULL ) {
			return MEMORY_PROBLEM;
		}
		strcpy(*challenge_name, tempName);
	}
	return OK;
}



Result find_best_time_of_system(ChallengeRoomSystem *sys, char **challenge_name) {
	//TODO testing
	if( sys == NULL || challenge_name == NULL ) {
		return NULL_PARAMETER;
	}
	int number_of_challenges = (*sys).numberOfChallenges;
	int min = -1, temp;
	char tempName[MAX_NAME_LENG] = "";
	*challenge_name = NULL;
	for(int i = 0; i < number_of_challenges; i++) {
		Result result = best_time_of_system_challenge(sys, (*sys).challenges[i].name, &temp);
		//printf("\n\n %d \n\n", temp);
		if( result != OK ){
			return result;
		}
		if( min == -1 && temp != 0) {
			min = temp;
		}
		//temp = (*sys).challenges[i].num_visits;
		if ( temp != 0 && min > temp ) {
			min = temp;
			strcpy(tempName, (*sys).challenges[i].name);
		} else if( temp != 0 && min == temp && strcmp(tempName, (*sys).challenges[i].name) > 0) {
			strcpy(tempName, (*sys).challenges[i].name);
		}
	}
	//printf("tempName: %s\n", tempName);
	if ( strcmp(tempName, "") != 0 ) {
		*challenge_name = malloc(sizeof(char) * (strlen(tempName) + 1) );
		if( *challenge_name == NULL ) {
			return MEMORY_PROBLEM;
		}
		strcpy(*challenge_name, tempName);
	}
	return OK;
}

//TODO private
Result nameRead(char* name, FILE* inputFile) {
	if (name == NULL) {
		return NULL_PARAMETER;
	}
	if (!fscanf(inputFile,"%s", name)) {
	//if (!fgets(name, NAME_LENG, inputFile)) {
		return ILLEGAL_PARAMETER;
	}

	return OK;
}

Result numberRead(int* number, FILE* inputFile) {
	if (number == NULL) {
		return NULL_PARAMETER;
	}

	if (!fscanf(inputFile, "%d", number)) {
		return ILLEGAL_PARAMETER;
	}

	return OK;
}

Result challengeRead(Challenge* challenge, FILE* inputFile) {
	if (challenge == NULL) {
		return NULL_PARAMETER;
	}

	//char* mystring;
  //mystring = (char*)malloc(50); // set this to any size
	if (!fscanf(inputFile, "%s %d %u\n", challenge->name, &(challenge->id), &(challenge->level))) {
		return ILLEGAL_PARAMETER;
	}
	//printf("%s   %d   %i\n", challenge->name, challenge->id, challenge->level);

	return OK;
}

Result challengeRoomRead(ChallengeRoom* challengeRoom, FILE* inputFile) {
	if (challengeRoom == NULL) {
		return NULL_PARAMETER;
	}

	//char* mystring;
  //mystring = (char*)malloc(50); // set this to any size
	if (!fscanf(inputFile, "%s %d\n", challengeRoom->name, &(challengeRoom->num_of_challenges) )) {
		return ILLEGAL_PARAMETER;
	}

	return OK;
}

Challenge* findChallengeById(ChallengeRoomSystem *sys, int id) {
	//printf("temp %d\n", (*sys).numberOfChallenges);
	int number_of_challenges = (*sys).numberOfChallenges;
	for(int i = 0; i < number_of_challenges; i++) {
		if ( (*sys).challenges[i].id == id) {
			return &((*sys).challenges[i]);
			//printf("%d %s  ",i , (*sys).challenges[i].name);
		}
	}
	//printf("\n");
	return NULL;
}

Challenge* findChallengeByName(ChallengeRoomSystem *sys, char *name) {
	//printf("temp %d\n", (*sys).numberOfChallenges);
	int number_of_challenges = (*sys).numberOfChallenges;
	for(int i = 0; i < number_of_challenges; i++) {
		if ( strcmp((*sys).challenges[i].name, name) == 0) {
			return &((*sys).challenges[i]);
			//printf("%d %s  ",i , (*sys).challenges[i].name);
		}
	}
	//printf("\n");
	return NULL;
}


ChallengeRoom* findRoomByName(ChallengeRoomSystem *sys, char* name) {
	//printf("ROOM: %s\n", name);
	int number_of_challengeRooms = (*sys).numberOfChallengeRooms;
	for(int i = 0; i < number_of_challengeRooms; i++) {
		//printf("%d %s  \n", i , (*sys).challengeRooms[i].name);
		if ( strcmp((*sys).challengeRooms[i].name, name) == 0) {
			//printf("%d %s  \n", i , (*sys).challengeRooms[i].name);
			return &((*sys).challengeRooms[i]);
		}
	}
	//printf("\n");
	return NULL;
}


VisitorNode* createVisitorNode(Visitor* visitor) {
	VisitorNode* visitorNode = malloc(sizeof(*visitorNode));
	if ( visitorNode == NULL ) {
		return NULL;
	}
	visitorNode->visitor = visitor;
	visitorNode->next = NULL;
	return visitorNode;
}

Result printAllVisitor(ChallengeRoomSystem *sys) {
	VisitorNode* pVisitor = sys->visitor_head;
	int i = 0;
	while ( pVisitor->next ) {
		i++;
		pVisitor = pVisitor->next;
		printf("	visitor %d: %s  id:%d", i, pVisitor->visitor->visitor_name, pVisitor->visitor->visitor_id);
	}
	return OK;
}

VisitorNode* findVisitorNodebyId(ChallengeRoomSystem *sys, int id) {
	VisitorNode* pVisitor = sys->visitor_head;
	while ( pVisitor->next ) {
		pVisitor = pVisitor->next;
		if( pVisitor->visitor->visitor_id == id) {
			return pVisitor;
		}
	}
	return NULL;
}

VisitorNode* findVisitorNodebyName(ChallengeRoomSystem *sys, char *name) {
	VisitorNode* pVisitor = sys->visitor_head;
	while ( pVisitor->next ) {
		pVisitor = pVisitor->next;
		if( strcmp(pVisitor->visitor->visitor_name, name) == 0) {
			return pVisitor;
		}
	}
	return NULL;
}

Result removeVisitorNodebyId(ChallengeRoomSystem *sys, int id) {
	VisitorNode* pVisitor = sys->visitor_head;
	if( pVisitor->next->visitor->visitor_id == id ) {
		if ( pVisitor->next == NULL ) {
			free(sys->visitor_head);
			sys->visitor_head = NULL;
			return OK;
		} else {
			free(pVisitor->next);
			pVisitor->next = pVisitor->next->next;
			return OK;
		}
	}
	while ( pVisitor->next->next ) {
		pVisitor = pVisitor->next;
		if( pVisitor->next->visitor->visitor_id == id ) {
			if( pVisitor->next->next == NULL ) {
				free(pVisitor->next);
				return OK;
			} else {
				free(pVisitor->next);
				pVisitor->next = pVisitor->next->next;
				return OK;
			}
			//TODO free
		}
	}
	return OK;
}


