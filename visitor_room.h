#ifndef VISITOR_ROOM_H_
#define VISITOR_ROOM_H_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "challenge.h"


struct SChallengeActivity;
typedef struct SVisitor
{
  char *visitor_name;
  int visitor_id;
  char **room_name;
  struct SChallengeActivity *current_challenge;
} Visitor;


typedef struct SChallengeActivity
{
   Challenge *challenge;
   Visitor *visitor;
   int start_time;
} ChallengeActivity;


typedef struct SChallengeRoom
{
   char *name;
   int num_of_challenges;
   ChallengeActivity *challenges;
} ChallengeRoom;

//Done
Result init_challenge_activity(ChallengeActivity *activity, Challenge *challenge);
//Done
Result reset_challenge_activity(ChallengeActivity *activity);
//Done
Result init_visitor(Visitor *visitor, char *name, int id);
//Done
Result reset_visitor(Visitor *visitor);
//Done
Result init_room(ChallengeRoom *room, char *name, int num_challenges);
//Done
Result reset_room(ChallengeRoom *room);
//Done
Result num_of_free_places_for_level(ChallengeRoom *room, Level level, int *places);
//Done
Result change_room_name(ChallengeRoom *room, char *new_name);
//Done
Result room_of_visitor(Visitor *visitor, char **room_name);
//Done
Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level, int start_time);
/* the challenge to be chosen is the lexicographically named smaller one that has
   the required level. assume all names are different. */
//Done
Result visitor_quit_room(Visitor *visitor, int quit_time);


#endif // VISITOR_ROOM_H_
