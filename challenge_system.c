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

typedef int (*CompareFunctionDefinition)(Challenge, void*) ;
typedef int (*CompareFunctionDefinition2)(void*, void*) ;

typedef enum {
    NAME,
    ID
} COMPARE_TYPE;


/*finding the challenge with the best time in the system*/
static Result findBestTimeOfSystem(ChallengeRoomSystem *sys, \
		char **challenge_name);
/*finding room by name return pointer to the room*/
static ChallengeRoom* findRoomByName(ChallengeRoomSystem *sys, char* name);
/*create visitor node for visitor linked list return a pointer of visitor node*/
static VisitorNode* createVisitorNode(Visitor* visitor);
/*find visitor node by Name/Id return pointer to it*/
static VisitorNode* findVisitorNode(ChallengeRoomSystem *sys, void* value, \
		COMPARE_TYPE type);
/*find challenge by Name/Id return pointer to it*/
static Challenge* findChallenge(ChallengeRoomSystem *sys, void* value, \
		COMPARE_TYPE type);
/*compare 2 Ids if equal return 0 otherwise 1*/
static int compareId(void* value1, void* value2);
/*compare 2 Names if equal return 0 otherwise -1/1 */
static int compareName(void* value1, void* value2);
/*free visitor and visitor node return 0*/
static int freeVisitorAndVisitorNode(Visitor* visitor, \
		VisitorNode* visitor_node);
/*add visitor node to system linked list*/
static Result addVisitorNode(ChallengeRoomSystem *sys, \
		VisitorNode* visitor_node);
/*part of create system initialize system name*/
static Result initializeSystem(ChallengeRoomSystem *sys, FILE *file);
/*part of create system initialize system challenges*/
static Result initializeChallenges(ChallengeRoomSystem *sys, FILE *file);
/*part of create system initialize system rooms*/
static Result initializeRooms(ChallengeRoomSystem *sys, FILE *file);
/*part of create system initialize system rooms' activity*/
static Result initializeChallengeActivitys(ChallengeRoomSystem *sys, \
		ChallengeRoom room, FILE *file, int number_of_challenges);


/*creating system from file, initialize all necessary fields*/
Result create_system(char *init_file, ChallengeRoomSystem **sys) {
	FILE *file = fopen(init_file, "r");
	if (file == NULL) return NULL_PARAMETER;
	*sys = malloc(sizeof(**sys));
	if (*sys == NULL) {
		fclose(file);
		return MEMORY_PROBLEM;
	}

	Result result  = initializeSystem(*sys, file);
	if (result != OK ) {
		free(*sys);
		fclose(file);
		return result;
	}

	result  = initializeChallenges(*sys, file);
	if (result != OK ) {
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return result;
	}

	result  = initializeRooms(*sys, file);
	if (result != OK ) {
		free((*sys)->challenges);
		free((*sys)->name);
		free(*sys);
		fclose(file);
		return result;
	}

	(*sys)->last_time = 0;
	(*sys)->visitor_head = NULL;
	fclose(file);
	return OK;
}

/*destroy and free all necessary fields*/
Result destroy_system(ChallengeRoomSystem *sys, int destroy_time, \
		char **most_popular_challenge_p, char **challenge_best_time) {
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

/*visitor's request to enter to a certain room & level with start time*/
Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name, \
		char *visitor_name, int visitor_id, Level level, int start_time) {
	if ( sys == NULL) return NULL_PARAMETER;
	if ( sys->last_time > start_time) return ILLEGAL_TIME;

	Visitor* visitor = malloc(sizeof(*visitor));
	if ( visitor == NULL ) return MEMORY_PROBLEM;
	VisitorNode* visitor_node = createVisitorNode(visitor);
	if ( visitor_node == NULL ) {
		free(visitor);
		return MEMORY_PROBLEM;
	} else if ( room_name == NULL ) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return ILLEGAL_PARAMETER;
	}

	ChallengeRoom* room = findRoomByName(sys, room_name);
	int places = 0;
	Result result = num_of_free_places_for_level(room, level, &places);
	if ( result != OK ) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return result;
	} else if ( visitor_name == NULL ) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return ILLEGAL_PARAMETER;
	} else if ( findVisitorNode(sys, &visitor_id, ID) != NULL ) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return ALREADY_IN_ROOM;
	} else if ( places == 0 ) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return NO_AVAILABLE_CHALLENGES;
	}

	result = init_visitor(visitor, visitor_name, visitor_id);
	if ( result != OK) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return result;
	}

	result = visitor_enter_room(room, visitor, level, start_time);
	if (result != OK ) {
		freeVisitorAndVisitorNode(visitor, visitor_node);
		return result;
	}

	addVisitorNode(sys, visitor_node);
	sys->last_time = start_time;
	return OK;
}

/*visitor leaving the room update all necessary fields*/
Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
	if ( sys == NULL) return NULL_PARAMETER;
	if ( sys->last_time > quit_time) return ILLEGAL_TIME;
	VisitorNode *visitor_node = findVisitorNode(sys, &visitor_id, ID);
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

/*all system visitors leaving by quit time*/
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

/*find visitor room in the system*/
Result system_room_of_visitor(ChallengeRoomSystem *sys, \
		char *visitor_name, char **room_name) {
	if (sys == NULL ) return NULL_PARAMETER;
	if (visitor_name == NULL || room_name == NULL) return ILLEGAL_PARAMETER;
	VisitorNode* visitor_node = findVisitorNode(sys, visitor_name, NAME);

	if ( visitor_node == NULL ) return NOT_IN_ROOM;
	Result result = room_of_visitor(visitor_node->visitor, room_name);
	if ( result != OK ) return result;
	return OK;
}

/*change challenge name to new name saving the name in the new copy*/
Result change_challenge_name(ChallengeRoomSystem *sys, \
		int challenge_id, char *new_name) {
	if ( sys == NULL || new_name == NULL ) return NULL_PARAMETER;
	Challenge* challenge = findChallenge(sys, &challenge_id, ID);
	if ( challenge == NULL ) return ILLEGAL_PARAMETER;
	challenge->name = realloc(challenge->name, sizeof(char) * \
			(strlen(new_name) + 1));
	if ( challenge->name == NULL) return MEMORY_PROBLEM;
	strcpy(challenge->name, new_name);
	return OK;
}

/*change room name to new name saving the name in the new copy*/
Result change_system_room_name(ChallengeRoomSystem *sys, \
		char *current_name,  char *new_name) {
	if ( sys == NULL || current_name == NULL || new_name == NULL ) {
		return NULL_PARAMETER;
	}
	ChallengeRoom *room = findRoomByName(sys, current_name);
	if ( room == NULL ) return ILLEGAL_PARAMETER;
	return change_room_name(room, new_name);
}

/*get best time of challenge in the system, the shorter time*/
Result best_time_of_system_challenge(ChallengeRoomSystem *sys, \
		char *challenge_name, int *time) {
	if ( sys == NULL || challenge_name == NULL || time == NULL ) {
		return NULL_PARAMETER;
	}
	Challenge* challenge = findChallenge(sys, challenge_name, NAME);
	if ( challenge == NULL ) {
		return ILLEGAL_PARAMETER;
	}
	*time = 0;
	Result result = best_time_of_challenge(challenge, time);
	if ( result != OK ) return result;
	return OK;
}

/*get most popular challenge in the system the higher number of visits*/
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
		} else if( temp == max && \
				strcmp(temp_name,(*sys).challenges[i].name) > 0) {
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



static Result findBestTimeOfSystem(ChallengeRoomSystem *sys, \
		char **challenge_name) {
	if ( sys == NULL || challenge_name == NULL ) return NULL_PARAMETER;
	int number_of_challenges = (*sys).number_of_challenges;
	int min = -1, temp;
	char temp_name[MAX_NAME_LENG] = "";
	*challenge_name = NULL;
	for (int i = 0; i < number_of_challenges; i++) {
		Result result = best_time_of_system_challenge(sys, \
				(*sys).challenges[i].name, &temp);
		if ( result != OK ){
			return result;
		}
		if ( temp != 0 && min > temp ) {
			min = temp;
			strcpy(temp_name, (*sys).challenges[i].name);
		} else if( temp != 0 && min == temp && \
				strcmp(temp_name, (*sys).challenges[i].name) > 0) {
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

static VisitorNode* findVisitorNode(ChallengeRoomSystem *sys, void* value, \
		COMPARE_TYPE type) {
	VisitorNode* ptr_Visitor = sys->visitor_head;
	if ( ptr_Visitor == NULL)
		return NULL;
	while ( ptr_Visitor != NULL) {
		if ( type == ID && \
				compareId(&(ptr_Visitor->visitor->visitor_id), value) == 0) {
				return ptr_Visitor;
		} else if ( type == NAME && \
				compareName(ptr_Visitor->visitor->visitor_name, value) == 0) {
				return ptr_Visitor;
		}
		ptr_Visitor = ptr_Visitor->next;
	}
	return NULL;
}

static Challenge* findChallenge(ChallengeRoomSystem *sys, void* value, \
		COMPARE_TYPE type) {
	int number_of_challenges = (*sys).number_of_challenges;
	for (int i = 0; i < number_of_challenges; i++) {
		if ( type == ID && \
				compareId(&((*sys).challenges[i].id), value) == 0) {
				return &((*sys).challenges[i]);
		} else if ( type == NAME && \
				compareName((*sys).challenges[i].name, value) == 0){
				return &((*sys).challenges[i]);
		}
	}
	return NULL;
}

static int compareId(void* value1, void* value2) {
    if ( *(int*)value1 == *(int*)value2 ) return 0;
    return 1;
}

static int compareName(void* value1, void* value2) {
    return strcmp((char*)value1, (char*)value2);
}

static int freeVisitorAndVisitorNode(Visitor* visitor, \
		VisitorNode* visitor_node) {
	free(visitor);
	free(visitor_node);
	return 0;
}

static Result addVisitorNode(ChallengeRoomSystem *sys, \
		VisitorNode* visitor_node) {
	VisitorNode* ptr_visitor = sys->visitor_head;
	if ( ptr_visitor != NULL) {
		while ( ptr_visitor->next ) {
			ptr_visitor = ptr_visitor->next;
		}
		ptr_visitor->next = visitor_node;
	} else {
		sys->visitor_head = visitor_node;
	}
	return OK;
}

static Result initializeSystem(ChallengeRoomSystem *sys, FILE *file) {
	char temp_name[MAX_NAME_LENG];
	fscanf(file, "%s", temp_name);
	sys->name = malloc(sizeof(char) * (strlen(temp_name) + 1));
	if (sys->name == NULL) {

		return MEMORY_PROBLEM;
	}
	strcpy(sys->name, temp_name);
	return OK;
}

static Result initializeChallenges(ChallengeRoomSystem *sys, FILE *file) {
	int temp_number = 0;
	fscanf(file, "%d", &temp_number);
	sys->number_of_challenges = temp_number;
	sys->challenges = malloc(sizeof(Challenge) * temp_number);
	if (sys->challenges == NULL) {
		return MEMORY_PROBLEM;
	}
	int id, level;
	int offset_level = 1;
	Result result = OK;
	char temp_name[MAX_NAME_LENG];
	for(int i = 0; i < sys->number_of_challenges; i++) {
		if ( fscanf(file,"%s", temp_name) == 1 && \
				fscanf(file, " %d %d\n", &id, &level)) {
			//offset change 1-Easy 2-Medium 3-Hard to 0-2 values
			result = init_challenge(&(sys->challenges[i]), id, \
					temp_name, level - offset_level);
			if( result == OK ) {
				continue;
			}
		}
		free(sys->challenges);
		return MEMORY_PROBLEM;
	}
	return OK;
}

static Result initializeRooms(ChallengeRoomSystem *sys, FILE *file) {
	int temp_number = 0;
	fscanf(file, "%d", &temp_number);
	sys->number_of_challenge_rooms = temp_number;
	sys->challenge_rooms = malloc(sizeof(ChallengeRoom) * temp_number);
	if ( sys->challenge_rooms == NULL) {
		for(int i = 0; i < sys->number_of_challenges; i++ ) {
			reset_challenge(&(sys->challenges[i]));
		}
		return MEMORY_PROBLEM;
	}
	int number_of_challenges;
	char temp_name[MAX_NAME_LENG];
	Result result = OK;
	for (int i = 0; i < sys->number_of_challenge_rooms; i++) {
		if ( fscanf(file,"%s", temp_name) == 1 && \
				fscanf(file, " %d", &number_of_challenges)) {
				result = init_room(&(sys->challenge_rooms[i]), \
						temp_name, number_of_challenges);
				if ( result != OK) {
					for(int j = 0; j < i; j++ ) {
						reset_room(&(sys->challenge_rooms[j]));
					}
					for(int j = 0; j < sys->number_of_challenges; j++ ) {
						reset_challenge(&(sys->challenges[j]));
					}
					return result;
				}
				result = initializeChallengeActivitys( sys, \
						sys->challenge_rooms[i], \
						file,  number_of_challenges);
				if (result != OK) {
					return result;
				}
		}
	}
	return OK;
}

static Result initializeChallengeActivitys(ChallengeRoomSystem *sys, \
		ChallengeRoom room, FILE *file, int number_of_challenges) {
	int temp_number;
	Result result = OK;
	for(int i = 0; i < number_of_challenges; i++) {
		fscanf(file, " %d", &temp_number);
		Challenge* challenge = \
				findChallenge(sys, &temp_number, ID);
		result = init_challenge_activity(\
				&(room.challenges[i]), \
				challenge);
		if ( result != OK) {
			for(int j = 0; j < i; j++ ) {
				reset_challenge_activity(&(room.challenges[i]));
			}
			for(int j = 0; j < room.num_of_challenges; j++ ) {
				reset_room(&(sys->challenge_rooms[j]));
			}
			for(int j = 0; j < sys->number_of_challenges; j++ ) {
				reset_challenge(&(sys->challenges[j]));
			}
			return result;
		}
	}
	return OK;
}
