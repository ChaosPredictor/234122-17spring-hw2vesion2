#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"

/*initialize challenge with id, name and level save in a new copy of challenge*/
Result init_challenge(Challenge *challenge, int id, char *name, Level level) {
	if ( name == NULL || challenge == NULL) {
		return NULL_PARAMETER;
	}
	challenge->name = malloc(sizeof(char) * (strlen(name) + 1));
	if ( challenge->name == NULL) {
		return MEMORY_PROBLEM;
	}
	strcpy(challenge->name, name);
	challenge->id = id;
	challenge->level = level;
	challenge->best_time = 0;
	challenge->num_visits = 0;
	return OK;
}

/*reset and free single copy of a challenge*/
Result reset_challenge(Challenge *challenge) {
	if ( challenge == NULL) {
		return NULL_PARAMETER;
	}
	free(challenge->name);
	challenge->name = NULL;
	challenge->level = 0;
	challenge->best_time = 0;
	challenge->num_visits = 0;
	return OK;
}

/*change challenge name to new name*/
Result change_name(Challenge *challenge, char *name) {
	if ( name == NULL || challenge == NULL) {
		return NULL_PARAMETER;
	}
	Challenge *temp = challenge;
	challenge->name = realloc(temp->name, sizeof(char) * (strlen(name) + 1));
	if ( challenge->name == NULL) {
		free(temp->name);
		return MEMORY_PROBLEM;
	}
	strcpy(challenge->name, name);
	return OK;

}

/*set best challenge time*/
Result set_best_time_of_challenge(Challenge *challenge, int time) {
	if ( challenge == NULL) {
		return NULL_PARAMETER;
	}
	if ( time < 0 ) {
		return ILLEGAL_PARAMETER;
	} else if ( time > challenge->best_time && challenge->best_time != 0 ) {
		return ILLEGAL_PARAMETER;
	}
	challenge->best_time = time;
	return OK;
}

/*get best challenge time*/
Result best_time_of_challenge(Challenge *challenge, int *time) {
	//TODO - to ask what to do if int* NULL
	if ( challenge == NULL || time == NULL) {
		return NULL_PARAMETER;
	}
	*time = challenge->best_time;
	return OK;
}

/*increase number of visits of the challenge by one*/
Result inc_num_visits(Challenge *challenge) {
	if ( challenge == NULL) {
		return NULL_PARAMETER;
	}
	challenge->num_visits += 1;
	return OK;
}

/*get number of visits of the challenge*/
Result num_visits(Challenge *challenge, int *visits) {
	if ( challenge == NULL || visits == NULL) {
		return NULL_PARAMETER;
	}
	*visits = challenge->num_visits;
	return OK;
}
