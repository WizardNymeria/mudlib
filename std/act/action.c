/*
   /std/act/action.c

   Generic actions: Standard action module for mobiles

   add_act(string)             Set a random actstring

   add_cact(string)            Set a random combat actstring

   set_act_time(int)           Set the mean value for act intervall

   set_cact_time(int)          Set the mean value for combat act intervall

*/
#pragma save_binary
#pragma strict_types

#include <macros.h>

static  int     monster_act_time,      /* Intervall between actions */
                monster_cact_time;     /* Intervall between combat actions */

static  string  *monster_act,          /* Action strings */
                *monster_cact;         /* Combat action strings */
static  int     monster_act_limit,     /* Current ceiling for unused acts */
                monster_cact_limit;    /* Current ceiling for unused cacts */

#define SEQ_ACT   "_mon_ran_act"

static string monster_do_act();

/*
 * Function name: add_act
 * Description:   Adds an action string that the monster will randomly do.
 * Arguments:     str: Text
 *                flag: True if the action is to occur forever (this
 *                      is frowned upon, so have a good reason). If 0 or
 *                      not present, the action will only occur in the
 *                      presence of a player, or for a short time after
 *                      the monster is alone.
 */
varargs void
add_act(mixed str, int flag)
{
    if (!IS_CLONE)
        return;

    if (!this_object()->seq_query(SEQ_ACT))
    {
        this_object()->seq_new(SEQ_ACT, flag);
    }

    this_object()->seq_clear(SEQ_ACT);
    this_object()->seq_addfirst(SEQ_ACT,
        ({ random(monster_act_time + 1), monster_do_act }));

    if (!str)
        return;

    if (!sizeof(monster_act))
        monster_act = ({});

    monster_act += ({ str });
    monster_act_limit = sizeof(monster_act);
}

/*
 * Function name: query_act
 * Description:   Get the array of actions this mobile do
 * Returns:       The array
 */
string *
query_act()
{
    return monster_act;
}

/*
 * Function name: add_cact
 * Description:   Sets a combat act string that the monster will randomly do.
 * Arguments:     str: Text
 */
void
add_cact(mixed str)
{
    if (!IS_CLONE || !str)
        return;

    add_act(0); /* Init act sequence */

    if (!sizeof(monster_cact))
        monster_cact = ({});

    if (!str)
        return;

    monster_cact += ({ str });
    monster_cact_limit = sizeof(monster_cact);
}

/*
 * Function name: query_cact
 * Description:   Give the combat act array
 * Returns:       The array
 */
string *
query_cact()
{
    return monster_cact;
}

/*
 * Function name: set_act_time
 * Description:   Set the number of SEQ_SLOW iterations to wait between acts.
 * Arguments:     tim: Intervall
 */
void
set_act_time(int tim)
{
    monster_act_time = tim;
}

/*
 * Function name: set_cact_time
 * Description:   Set the number of SEQ_SLOW iterations to wait between cacts.
 * Arguments:     tim: Intervall
 */
void
set_cact_time(int tim)
{
    monster_cact_time = tim;
}

/*
 *  Description: The actual function action, called by VBFC in seq_heartbeat
 */
static string
monster_do_act()
{
    int limit, delay;
    mixed *pool;

    if (!this_object()->query_attack())
    {
        pool = monster_act;
        delay = monster_act_time;
        limit = monster_act_limit--;

        if (monster_act_limit < 0)
            monster_act_limit = sizeof(monster_act);
    }
    else  /* In combat */
    {
        pool = monster_cact;
        delay = monster_cact_time;
        limit = monster_cact_limit--;

        if (monster_cact_limit < 0)
            monster_cact_limit = sizeof(monster_cact);
    }

    this_object()->seq_clear(SEQ_ACT);
    this_object()->seq_addfirst(SEQ_ACT, ({ delay, monster_do_act }));

    if (!pointerp(pool) || !sizeof(pool))
        return "";

    int selected = random(limit);

    // Move the selected act to the end of the pool
    mixed act = pool[selected];
    pool[selected] = pool[limit - 1];
    pool[limit - 1] = act;

    act = this_object()->check_call(act);

    if (!stringp(act))
        return "";
    return act;
}

/*
 * Function name: clear_act
 * Description:   clears acts for a monster
 */
void
clear_act()
{
    monster_act = 0;
    monster_act_limit = 0;
}

/*
 * Function name: clear_cact
 * Description:   clears cact's for a monster
 */
void
clear_cact()
{
    monster_cact = 0;
    monster_cact_limit = 0;
}

string
query_seq_act_name()
{
    return SEQ_ACT;
}
