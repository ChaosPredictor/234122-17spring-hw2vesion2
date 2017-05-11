#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge_system.h"

#define MAX_NAME_LENG 51 //Max leng 50 + 1 of end of string


typedef struct VisitorNodeStr {
	Visitor *visitor;
	struct VisitorNodeStr* next;
} VisitorNode;

static Result findBestTimeOfSystem(ChallengeRoomSystem *sys, char **challenge_name);
static Result readName(char* name, FILE* inputFile);
static Result readNumber(int* number, FILE* inputFile);
static ChallengeRoom* findRoomByName(ChallengeRoomSystem *sys, char* name);
static VisitorNode* createVisitorNode(Visitor* visitor);
static VisitorNode* findVisitorNodebyName(ChallengeRoomSystem *sys, char *name);
static VisitorNode* findVisitorNodebyId(ChallengeRoomSystem *sys, int id);
static Challenge* findChallengeById(ChallengeRoomSystem *sys, int id);
static Challenge* findChallengeByName(ChallengeRoomSystem *sys, char *name);


Result create_system(char *init_file, ChallengeRoomSystem **sys) {
	FILE *file = fopen(init_file, "r");
	if (file == NULL) return NULL_PARAMETER;
	*sys = malloc(sizeof(**sys));
	if (*sys == NULL) {
		fclose(file);
		return MEMORY_PROBLEM;
	}

	char temp_name[MAX_NAME_LENG];
	readName(temp_name, file);
	(*sys)->name = malloc(sizeof(char) * (strlen(temp_name) + 1));
	if ((*sys)->name == NULL) {
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}
	strcpy((*sys)->name, temp_name);

	//TODO separated function
	// READ challenges
	int temp_number = 0;
	readNumber(&temp_number, file); //number of Challenges
	(*sys)->number_of_challenges = temp_number;
	(*sys)->challenges = malloc(sizeof(Challenge) * temp_number);
	if ((*sys)->challenges == NULL) {
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}
	int id, level;
	int offset_level = 1;
	Result result = OK;
	for(int i = 0; i < (*sys)->number_of_challenges; i++) {
		if ( readName(temp_name, file) == OK && \
				fscanf(file, " %d %d\n", &id, &level)) {
			//MINUS 1 caused by offset 1-Easy 2-Medium 3-Hard should by 0-2
			result = init_challenge(&((*sys)->challenges[i]), id, \
					temp_name, level - offset_level);
			if( result == OK ) {
				continue;
			}
		}
		free((*sys)->challenges);
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}


	//TODO separated function
	// READ Challenges Rooms
	readNumber(&temp_number, file); //number of Challenge Rooms
	(*sys)->number_of_challenge_rooms = temp_number;
	(*sys)->challenge_rooms = malloc(sizeof(ChallengeRoom) * temp_number);
	if ((*sys)->challenge_rooms == NULL) {
		for(int i = 0; i < (*sys)->number_of_challenges; i++ ) {
			reset_challenge(&((*sys)->challenges[i]));
		}
		free((*sys)->challenges);
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return MEMORY_PROBLEM;
	}

	int number_of_challenges;
	for (int i = 0; i < (*sys)->number_of_challenge_rooms; i++) {
		if ( readName(temp_name, file) == OK && \
				fscanf(file, " %d", &number_of_challenges)) {
				result = init_room(&((*sys)->challenge_rooms[i]), \
						temp_name, number_of_challenges);
				for(int j = 0; j < number_of_challenges; j++) {
					fscanf(file, " %d", &temp_number);
					Challenge* challenge = findChallengeById(*sys, temp_number);
					init_challenge_activity(&((*sys)->challenge_rooms[i].challenges[j]), challenge);
				}

		}
	}

	(*sys)->last_time = 0;
	(*sys)->visitor_head = NULL;
	fclose(file);
	return OK;
}

Result destroy_system(ChallengeRoomSystem *sys, int destroy_time, char **most_popular_challenge_p, char **challenge_best_time) {
	if ( sys == NULL || most_popular_challenge_p == NULL || \
			challenge_best_time == NULL ) {
		return NULL_PARAMETER;
	}
	if ( sys->last_time > destroy_time) return ILLEGAL_TIME;
	all_visitors_quit(sys, destroy_time);
	Result result = most_popular_challenge(sys, most_popular_challenge_p);
	if ( result != OK ) return result;
	result = findBestTimeOfSystem(sys, challenge_best_time);
	if ( result != OK ) return result;
	int temp;
	for (int i=0; i < sys->number_of_challenge_rooms; i++) {
		temp = sys->challenge_rooms[i].num_of_challenges;
		for (int j=0; j < temp; j++) {
			reset_challenge_activity(&(sys->challenge_rooms[i].challenges[j]));
		}
		reset_room(&(sys->challenge_rooms[i]));
	}
	free(sys->challenge_rooms);
	for (int i=0; i < sys->number_of_challenges; i++) {
		reset_challenge(&(sys->challenges[i]));
	}
	free(sys->challenges);
	free(sys->name);
	free(sys);
	sys = NULL;
	return OK;
}

Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name, char *visitor_name, int visitor_id, Level level, int start_time) {
	if ( sys == NULL) return NULL_PARAMETER;
	if ( sys->last_time > start_time) return ILLEGAL_TIME;
	Visitor* visitor = malloc(sizeof(*visitor));
	if ( visitor == NULL ) return MEMORY_PROBLEM;
	VisitorNode* visitor_node = createVisitorNode(visitor);
	if ( visitor_node == NULL ) {
		free(visitor);
		return MEMORY_PROBLEM;
	}
	if ( room_name == NULL ) {
		free(visitor);
		free(visitor_node);
		return ILLEGAL_PARAMETER;
	}
	ChallengeRoom* room = findRoomByName(sys, room_name);
	int places = 0;
	Result result = num_of_free_places_for_level(room, level, &places);
	if ( result != OK ) {
		free(visitor);
		free(visitor_node);
		return result;
	}
	if ( visitor_name == NULL ) {
		free(visitor);
		free(visitor_node);
		return ILLEGAL_PARAMETER;
	}
	if ( findVisitorNodebyId(sys, visitor_id) != NULL ) {
		free(visitor);
		free(visitor_node);
		return ALREADY_IN_ROOM;
	}
	if ( places == 0 ) {
		free(visitor);
		free(visitor_node);
		return NO_AVAILABLE_CHALLENGES;
	}
	result = init_visitor(visitor, visitor_name, visitor_id);
	if ( result != OK) {
		free(visitor);
		free(visitor_node);
		return result;
	}
	VisitorNode* ptr_visitor = sys->visitor_head;
	if ( ptr_visitor != NULL) {
		while ( ptr_visitor->next ) {
			ptr_visitor = ptr_visitor->next;
		}
		ptr_visitor->next = visitor_node;
	} else {
		sys->visitor_head = visitor_node;
	}
	result = visitor_enter_room(room, visitor, level, start_time);
	if (result != OK ) {
		free(visitor);
		free(visitor_node);
		return result;
	}
	sys->last_time = start_time;
	return OK;
}

Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
	if ( sys == NULL) return NULL_PARAMETER;
	if ( sys->last_time > quit_time) return ILLEGAL_TIME;
	VisitorNode *visitor_node = findVisitorNodebyId(sys, visitor_id);
	if ( visitor_node == NULL ) return NOT_IN_ROOM;
	Result result = visitor_quit_room(visitor_node->visitor, quit_time);
	if (result != OK ) return result;

	Visitor* keep_visitor = visitor_node->visitor;
	reset_visitor(keep_visitor);
	free(keep_visitor);
	if ( visitor_node == sys->visitor_head ) {
		sys->visitor_head = visitor_node->next;
	} else {
		VisitorNode *temp_node = sys->visitor_head;
		while ( temp_node->next != NULL && temp_node->next != visitor_node ) {
			temp_node = temp_node->next;
		}
		temp_node->next = visitor_node->next;
	}
	free(visitor_node);
	sys->last_time = quit_time;
	return OK;
}

Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
	if ( sys == NULL) return NULL_PARAMETER;
	if ( sys->last_time > quit_time) return ILLEGAL_TIME;
	if ( sys->visitor_head == NULL) return NOT_IN_ROOM;
	VisitorNode* ptr_visitor = sys->visitor_head, *temp;
	while ( ptr_visitor ) {
		temp = ptr_visitor;
		ptr_visitor = temp->next;
		visitor_quit(sys, temp->visitor->visitor_id, quit_time);
	}
	sys->last_time = quit_time;
	return OK;
}

Result system_room_of_visitor(ChallengeRoomSystem *sys, char *visitor_name, char **room_name) {
	if (sys == NULL ) return NULL_PARAMETER;
	if (visitor_name == NULL || room_name == NULL) return ILLEGAL_PARAMETER;
	VisitorNode* visitor_node = findVisitorNodebyName(sys, visitor_name);
	if ( visitor_node == NULL ) return NOT_IN_ROOM;
	Result result = room_of_visitor(visitor_node->visitor, room_name);
	if ( result != OK ) return result;
	return OK;
}

Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id, char *new_name) {
	if ( sys == NULL || new_name == NULL ) return NULL_PARAMETER;
	Challenge *challenge = findChallengeById(sys, challenge_id);
	if ( challenge == NULL ) return ILLEGAL_PARAMETER;
	challenge->name = realloc(challenge->name, sizeof(char) * \
			(strlen(new_name) + 1));
	if ( challenge->name == NULL) return MEMORY_PROBLEM;
	strcpy(challenge->name, new_name);
	return OK;
}

Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,  char *new_name) {
	if ( sys == NULL || current_name == NULL || new_name == NULL ) {
		return NULL_PARAMETER;
	}
	ChallengeRoom *room = findRoomByName(sys, current_name);
	if ( room == NULL ) return ILLEGAL_PARAMETER;
	return change_room_name(room, new_name);
}

Result best_time_of_system_challenge(ChallengeRoomSystem *sys, char *challenge_name, int *time) {
	if ( sys == NULL || challenge_name == NULL || time == NULL ) {
		return NULL_PARAMETER;
	}
	Challenge* challenge = findChallengeByName(sys, challenge_name);
	if ( challenge == NULL ) {
		return ILLEGAL_PARAMETER;
	}
	*time = 0;
	Result result = best_time_of_challenge(challenge, time);
	if ( result != OK ) return result;
	return OK;
}

Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name) {
	if ( sys == NULL || challenge_name == NULL ) {
		return NULL_PARAMETER;
	}
	int number_of_challenges = (*sys).number_of_challenges;
	int max = 0, temp;
	char temp_name[MAX_NAME_LENG] = "";
	*challenge_name = NULL;
	for (int i = 0; i < number_of_challenges; i++) {
		Result result = num_visits(&((*sys).challenges[i]), &temp);
		if ( result != OK ){
			return result;
		}
		if ( temp > max) {
			max = temp;
			strcpy(temp_name, (*sys).challenges[i].name);
		} else if( temp == max && strcmp(temp_name, (*sys).challenges[i].name) > 0) {
			strcpy(temp_name, (*sys).challenges[i].name);
		}
	}
	if ( strcmp(temp_name, "") != 0 ) {
		*challenge_name = malloc(sizeof(char) * (strlen(temp_name) + 1) );
		if ( *challenge_name == NULL ) {
			return MEMORY_PROBLEM;
		}
		strcpy(*challenge_name, temp_name);
	} else {
		challenge_name = NULL;
	}
	return OK;
}


static Result findBestTimeOfSystem(ChallengeRoomSystem *sys, char **challenge_name) {
	if ( sys == NULL || challenge_name == NULL ) return NULL_PARAMETER;
	int number_of_challenges = (*sys).number_of_challenges;
	int min = -1, temp;
	char temp_name[MAX_NAME_LENG] = "";
	*challenge_name = NULL;
	for (int i = 0; i < number_of_challenges; i++) {
		Result result = best_time_of_system_challenge(sys, (*sys).challenges[i].name, &temp);
		if ( result != OK ){
			return result;
		}
		if ( temp != 0 && min > temp ) {
			min = temp;
			strcpy(temp_name, (*sys).challenges[i].name);
		} else if( temp != 0 && min == temp && strcmp(temp_name, (*sys).challenges[i].name) > 0) {
			strcpy(temp_name, (*sys).challenges[i].name);
		}
		if ( min == -1 && temp != 0) {
			min = temp;
			strcpy(temp_name, (*sys).challenges[i].name);
		}
	}
	if ( strcmp(temp_name, "") != 0 ) {
		*challenge_name = malloc(sizeof(char) * (strlen(temp_name) + 1) );
		if( *challenge_name == NULL ) return MEMORY_PROBLEM;
		strcpy(*challenge_name, temp_name);
	} else {
		challenge_name = NULL;
	}
	return OK;
}

//TODO - delete
static Result readName(char* name, FILE* inputFile) {
	if (name == NULL) {
		return NULL_PARAMETER;
	}
	if (!fscanf(inputFile,"%s", name)) {
		return ILLEGAL_PARAMETER;
	}
	return OK;
}

//TODO - delete
static Result readNumber(int* number, FILE* inputFile) {
	if (number == NULL) {
		return NULL_PARAMETER;
	}
	if (!fscanf(inputFile, "%d", number)) {
		return ILLEGAL_PARAMETER;
	}
	return OK;
}

static ChallengeRoom* findRoomByName(ChallengeRoomSystem *sys, char* name) {
	int number_of_challenge_rooms = (*sys).number_of_challenge_rooms;
	for (int i = 0; i < number_of_challenge_rooms; i++) {
		if ( strcmp((*sys).challenge_rooms[i].name, name) == 0) {
			return &((*sys).challenge_rooms[i]);
		}
	}
	return NULL;
}

static VisitorNode* createVisitorNode(Visitor* visitor) {
	VisitorNode* visitor_node = malloc(sizeof(VisitorNode));
	if ( visitor_node == NULL ) {
		return NULL;
	}
	visitor_node->visitor = visitor;
	visitor_node->next = NULL;
	return visitor_node;
}

static VisitorNode* findVisitorNodebyId(ChallengeRoomSystem *sys, int id) {
	VisitorNode* ptr_Visitor = sys->visitor_head;
	if ( ptr_Visitor == NULL)
		return NULL;
	while ( ptr_Visitor != NULL ) {
		if ( ptr_Visitor->visitor->visitor_id == id) {
			return ptr_Visitor;
		}
		ptr_Visitor = ptr_Visitor->next;
	}
	return NULL;
}

static VisitorNode* findVisitorNodebyName(ChallengeRoomSystem *sys, char *name) {
	VisitorNode* ptr_Visitor = sys->visitor_head;
	if ( ptr_Visitor == NULL)
		return NULL;
	while ( ptr_Visitor != NULL) {
		if ( strcmp(ptr_Visitor->visitor->visitor_name, name) == 0) {
			return ptr_Visitor;
		}
		ptr_Visitor = ptr_Visitor->next;
	}
	return NULL;
}

static Challenge* findChallengeById(ChallengeRoomSystem *sys, int id) {
	int number_of_challenges = (*sys).number_of_challenges;
	for (int i = 0; i < number_of_challenges; i++) {
		if ( (*sys).challenges[i].id == id) {
			return &((*sys).challenges[i]);
		}
	}
	return NULL;
}

static Challenge* findChallengeByName(ChallengeRoomSystem *sys, char *name) {
	int number_of_challenges = (*sys).number_of_challenges;
	for (int i = 0; i < number_of_challenges; i++) {
		if ( strcmp((*sys).challenges[i].name, name) == 0) {
			return &((*sys).challenges[i]);
		}
	}
	return NULL;
}




