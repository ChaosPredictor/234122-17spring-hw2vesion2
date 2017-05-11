#ifndef CHALLENGE_H_
#define CHALLENGE_H_

#include "constants.h"

typedef struct SChallenge
{
   int id;
   char *name;
   Level level;
   int best_time;
   int num_visits_param;
} Challenge;
//Done
Result init_challenge(Challenge *challenge, int id, char *name, Level level);
//Done
Result reset_challenge(Challenge *challenge);
//Done
Result change_name(Challenge *challenge, char *name);
//Done
Result set_best_time_of_challenge(Challenge *challenge, int time);
//Done
Result best_time_of_challenge(Challenge *challenge, int *time);
//Done
Result inc_num_visits(Challenge *challenge);
//Done
//TODO - change function name
Result num_visits(Challenge *challenge, int *visits);

#endif // CHALLENGE_H_
