#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "visitor_room.h"

Result init_challenge_activity(ChallengeActivity *activity, Challenge *challenge) {
	if ( activity == NULL || challenge == NULL) {
		return NULL_PARAMETER;
	}
	activity->challenge = challenge;
	activity->visitor = NULL;
	activity->start_time = 0;
	return OK;
}

Result reset_challenge_activity(ChallengeActivity *activity) {
	if ( activity == NULL) {
		return NULL_PARAMETER;
	}
	activity->challenge=NULL;
	return OK;
}

Result init_visitor(Visitor *visitor, char *name, int id) {
	if ( visitor == NULL || name == NULL ) {
		return NULL_PARAMETER;
	}
	visitor->visitor_name = malloc(sizeof(char) * (strlen(name)+1));
	if (visitor->visitor_name == NULL) {
		return MEMORY_PROBLEM;
	}

	strcpy(visitor->visitor_name, name);
	visitor->visitor_id = id;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;

	return OK;
}

Result reset_visitor(Visitor *visitor) {
	if ( visitor == NULL ) {
		return NULL_PARAMETER;
	}

	free(visitor->visitor_name);
	visitor->visitor_name = NULL;
	visitor->visitor_id = 0;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;

	return OK;
}

Result init_room(ChallengeRoom *room, char *name, int num_challenges) {
	if ( name == NULL || room == NULL) {
		return NULL_PARAMETER;
	}
	if (num_challenges < 1) {
		return ILLEGAL_PARAMETER;
	}
	room->name = malloc(sizeof(char) * (strlen(name) + 1));
	if ( room->name == NULL) {
		return MEMORY_PROBLEM;
	}
	strcpy(room->name, name);
	room->challenges = malloc(sizeof(ChallengeActivity) * num_challenges);
	if ( room->challenges == NULL) {
		free(room->name);
		return MEMORY_PROBLEM;
	}
	room->num_of_challenges = num_challenges;
	for (int i = 0; i < num_challenges; i++) {
		room->challenges[i].start_time = -1;
		room->challenges[i].visitor = NULL;
		room->challenges[i].challenge = NULL;
	}
	return OK;
}

Result reset_room(ChallengeRoom *room) {
	if ( room == NULL) {
		return NULL_PARAMETER;
	}
	free(room->challenges);
	room->challenges = NULL;
	free(room->name);
	room->name = NULL;
	room->num_of_challenges = 0;
	return OK;
}

Result num_of_free_places_for_level(ChallengeRoom *room, Level level, int *places) {
	if ( room == NULL || places == NULL) {
		return NULL_PARAMETER;
	}
	*places = 0;
	int num_of_challenges = room->num_of_challenges;
	for (int i = 0; i < num_of_challenges; i++) {
		if ( room->challenges[i].visitor == NULL && ( level == All_Levels || level == room->challenges[i].challenge->level) ) {
			(*places)++;
		}
	}
	return OK;
}

Result change_room_name(ChallengeRoom *room, char *new_name) {
	if ( room == NULL || new_name == NULL ) {
		return NULL_PARAMETER;
	}
	room->name = realloc(room->name, sizeof(char) * (strlen(new_name) + 1));
	if ( room->name == NULL) {
		return MEMORY_PROBLEM;
	}
	strcpy(room->name, new_name);
	return OK;
}

Result room_of_visitor(Visitor *visitor, char **room_name) {
	if ( visitor == NULL || room_name == NULL )	{
		return NULL_PARAMETER;
	}
	if ( visitor->room_name == NULL) {
		return NOT_IN_ROOM;
	}
	*room_name = malloc( strlen(*(visitor->room_name)) + 1 );
	if (*room_name == NULL ) {
		return MEMORY_PROBLEM;
	}
	strcpy(*room_name, *(visitor->room_name));
	return OK;
}

Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level, int start_time) {
	if ( room == NULL || visitor == NULL ) {
		return NULL_PARAMETER;
	}
	if ( visitor->current_challenge != NULL ) {
		return ALREADY_IN_ROOM;
	}
	int places = 0;
	num_of_free_places_for_level(room, level, &places);
	if ( places == 0 ) {
		return NO_AVAILABLE_CHALLENGES;
	}
	int index_of_challenge = -1;
	int num_of_challenges = room->num_of_challenges;
	for (int i = 0; i < num_of_challenges; i++) {
		if ( room->challenges[i].visitor == NULL && ( level == All_Levels || level == room->challenges[i].challenge->level) ) {
			if (index_of_challenge == -1) {
				index_of_challenge = i;
			} else if(strcmp((room->challenges[index_of_challenge]).challenge->name,(room->challenges[i]).challenge->name) > 0) {
				index_of_challenge = i;
			}
		}
	}
	Result result = inc_num_visits(room->challenges[index_of_challenge].challenge);
	if ( result != OK ) {
		return result;
	}
	room->challenges[index_of_challenge].start_time = start_time;
	room->challenges[index_of_challenge].visitor = visitor;
	visitor->room_name = &(room->name);
	visitor->current_challenge = &(room->challenges[index_of_challenge]);
	return OK;
}

Result visitor_quit_room(Visitor *visitor, int quit_time) {
	if ( visitor == NULL ) {
		return NULL_PARAMETER;
	}
	if ( visitor->current_challenge == NULL ) {
		return NOT_IN_ROOM;
	}
	struct SChallengeActivity *challenge_activity = visitor->current_challenge;

	int time = quit_time - challenge_activity->start_time;
	set_best_time_of_challenge((*challenge_activity).challenge, time);

	challenge_activity->visitor = NULL;
	challenge_activity->start_time = 0;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;
	return OK;
}


