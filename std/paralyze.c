/*
 * Clone and move this object to a player if you want to paralyze him.
 */
#pragma strict_types

inherit "/std/object";

#include <cmdparse.h>
#include <stdproperties.h>
#include <macros.h>

/*
 * Variables
 */
static string  stop_verb,	  /* What verb to stop this paralyze ? */
		       stop_fun;	  /* What function to call when stopped */
static mixed   fail_message,  /* Message to write when command failed */
		       stop_message,  /* Message to write when paralyze stopped */
		       extra_commands;/* Extra commands this paralyze allows */
static int	   remove_time,	  /* Shall it go away automatically? */
               combat_stop,   /* If true, stop when we're attacked. */
		       talkable;      /* Can player talk during this paralyze */
static object  stop_object;   /* Object to call stop_fun in when stopped */

/*
 * Prototypes
 */
void set_standard_paralyze(string str);
int stop(string str);
varargs void stop_paralyze();

/*
 * Function name: create_paralyze
 * Description:	  Set up standard paralyze
 */
void
create_paralyze()
{
    set_standard_paralyze("paralyze");
}

/*
 * Function name: create_object
 * Description:   The standard create routine.
 */
nomask void
create_object()
{
    create_paralyze();
    add_name("_std_paralyze_");

    set_no_show();

    add_prop(OBJ_M_NO_GIVE, 1);
    add_prop(OBJ_M_NO_DROP, 1);
    add_prop(OBJ_M_NO_STEAL, 1);
    add_prop(OBJ_M_NO_TELEPORT, 1);
}

/*
 * Function name: init
 * Description:   Called when meeting an object
 */
void
init()
{
    ::init();

    if (remove_time)
    {
        set_alarm(itof(remove_time), 0.0, stop_paralyze);
    }

    add_action(stop, "", 1);
}

/*
 * Function name: stop
 * Description  : Here all commands the player gives comes.
 * Argument     : string str - The command line argument.
 * Returns      : int 1/0    - success/failure.
 */
int
stop(string str)
{
	string verb = query_verb();
	
    /* Only paralyze our environment */
    if (environment() != this_player())
    {
        return 0;
    }

    /* Some commands may always be issued. */
    if (CMDPARSE_PARALYZE_CMD_IS_ALLOWED(verb))
    {
        return 0;
    }
	
	/* Special allowed commands for this specific paralyze */
	if (extra_commands && IN_ARRAY(verb, extra_commands))
	{
		return 0;
	}
	
	/* Check if the first character of the string is a ' for talkable 
	   paralyzes*/
    if (talkable && verb[0..0] == "'")
    {
        return 0; 
    }
	
    /* If there is a verb stopping the paralyze, check it. */
    if (stringp(stop_verb) && (verb == stop_verb))
    {
        /* If a stop_fun is defined, the paralysis STOPS if it returns 0.
	 * Returning 1 will cause the the paralysis to continue.
         */
        if (objectp(stop_object) &&
            call_other(stop_object, stop_fun, str))
        {
            return 1;
        }

	if (stop_message)
        {
            this_player()->catch_msg(stop_message);
        }

        remove_object();
        return 1;
    }

    /* We allow VBFC, so here we may use catch_msg(). */
    if (fail_message)
    {
        this_player()->catch_msg(fail_message);
    }

    /* Only paralyze mortals. */
    if (!this_player()->query_wiz_level())
    {
        return 1;
    }

    write("Since you are a wizard this paralyze won't affect you.\n");
    return 0;
}

/*
 * Function name: dispel_magic
 * Description  : If this paralyze should be able to lift magical, define this
 *		  function.
 * Arguments    : int i   - a number indicating how strong the dispel spell is.
 * Returns      : int 1/0 - 1 if dispelled; otherwise 0.
 */
int
dispel_magic(int i)
{
    return 0;
}

/*
 * Function name: set_stop_verb
 * Description  : Set the verb to stop paralyze, if possible.
 * Arguments    : string verb - the verb to use.
 */
void
set_stop_verb(string verb)
{
    stop_verb = verb;
}

/*
 * Function name: query_stop_verb
 * Description  : Return the stopping verb.
 * Returns      : string - the verb.
 */
string
query_stop_verb()
{
    return stop_verb;
}

/*
 * Function name: set_stop_fun
 * Description  : Set function to call when paralyze stops if there is such
 *                a function.
 * Arguments    : string fun - the function to call.
 */
void
set_stop_fun(string fun)
{
    stop_fun = fun;
}

/*
 * Function name: query_stop_fun
 * Description  : Returns the function to call when paralyze stops.
 * Returns      : string - the function name.
 */
string
query_stop_fun()
{
    return stop_fun;
}

/*
 * Function name: set_stop_object.
 * Description  : Set which object to call the stop function in.
 * Arguments    : object ob - the object.
 */
void
set_stop_object(object ob)
{
    stop_object = ob;
}

/*
 * Function name: query_stop_object
 * Description  : Returns which object to call the stop function in.
 * Returns      : object - the object.
 */
object
query_stop_object()
{
    return stop_object;
}

/*
 * Function name: set_fail_message
 * Description  : Set the fail message when player tries to do something.
 *                This supports VBFC and uses this_player().
 * Arguments    : mixed - the fail message.
 */
void
set_fail_message(mixed message)
{
    fail_message = message;
}

/*
 * Function name: query_fail_message
 * Description  : Returns the fail message when player tries to do something.
 *                This returns the real value, not resolved for VBFC.
 * Returns      : mixed - the message.
 */
mixed
query_fail_message()
{
    return fail_message;
}

/*
 * Function name: set_remove_time
 * Description  : Set how long time player should be paralyzed (in seconds).
 * Arguments    : int time - the time to set.
 */
void
set_remove_time(int time)
{
    remove_time = time;
}

/*
 * Function name: query_remove_time
 * Description  : Returns the paralyze time (in seconds).
 * Returns      : int - the time.
 */
int
query_remove_time()
{
    return remove_time;
}

/*
 * Function name: set_combat_stop
 * Description  : Set if we should stop the paralysis when we are attacked.
 * Arguments    : int cb - if true, stop upon combat.
 */
void
set_combat_stop(int stop)
{
    combat_stop = stop;
}

/*
 * Function name: query_combat_stop
 * Description  : Find out if this paralysis should stop when we are attacked.
 * Returns      : int - 1/0 - if true, break upon combat.
 */
int
query_combat_stop()
{
    return combat_stop;
}

/*
 * Function name: set_stop_message
 * Description  : Set the message written when paralyze stops. This may
 *                support VBFC and uses this_player().
 * Arguments    : mixed - the message.
 */
void
set_stop_message(mixed message)
{
    stop_message = message;
}

/*
 * Function name: query_stop_message
 * Description  : Returns the message written when paralyze stops. It returns
 *                the real value, not solved for VBFC.
 * Returns      : mixed - the message.
 */
mixed
query_stop_message()
{
    return stop_message;
}

/*
 * Function name: set_talkable
 * Description  : Set if standard say alias can be used when paralyzed
 * Arguments    : 1/0 - 1 can talk, 0 cannot talk. Default is 0;
 */
void
set_talkable(int talk)
{
	talkable = talk;
}

/*
 * Function name: query_talkable
 * Description  : Returns if player can talk during paralyze
 * Returns      : int - 1 can talk, 0 cannot talk.
 */
int
query_talkable()
{
	return talkable;
}

/*
 * Function name: set_allowed_commands
 * Description  : Set additional commands allowed during paralyze other 
 *                than those allowed in CMDPARSE_PARALYZE_ALLOWED
 * Arguments    : mixed array of - extra verbs.
 */
void
set_allowed_commands(mixed verbs)
{
    extra_commands = verbs;
} 

/*
 * Function name: query_allowed commands
 * Description  : Returns the additionally allowed commands other than 
 *                those in CMDPARSE_PARALYZE_ALLOWED
 * Returns      : mixed - array of additional allowed commands
                  0 - No additional commands allowed
				  array of strings - additional allowed commands
 */
mixed
query_allowed_comands()
{
    return extra_commands;
}

/*
 * Function name: set_standard_paralyze
 * Description  : Set up standard settings for a paralyze.
 * Arguments    : string str - When the player uses the stop-verb, 'stop',
 *                             the message 'You stop <str>.\n' is printed
 *                             to the player.
 */
void
set_standard_paralyze(string str)
{
    set_stop_verb("stop");
    set_stop_fun("stop_paralyze");
    set_stop_object(previous_object());
    set_stop_message("You stop " + str + ".\n");
    set_fail_message("You are busy with other things right now. You must " +
        "'stop' to do something else.\n");
	set_talkable(0);
	set_allowed_commands(0);
}

/*
 * Function name: try_stop_combat
 * Description  : When combat initiates against us, we may try to stop the
 *                paralysis if we are e.g. counting or searching.
 */
public void
try_combat_stop()
{
    object old_tp;

    if (!combat_stop)
        return;

    /* We need to modify this_player() as the stop-fun may depend on it. */
    if (this_player() != environment())
    {
        old_tp = this_player();
	set_this_player(environment());
    }

    /* We call the stop_fun if it exists, but we don't honour the result since
     * combat is forcing the break.
     */
    if (objectp(stop_object))
    {
        call_other(stop_object, stop_fun, "");
    }
    if (stop_message)
    {
        environment()->catch_msg(stop_message);
    }

    /* And clean up after ourselves. */
    if (old_tp)
    {
        set_this_player(old_tp);
    }

    remove_object();
}

/*
 * Function name: stop_paralyze
 * Description  : This function is called if time runs out and the paralysis
 *                stops due to expiration.
 */
void
stop_paralyze()
{
    if (!objectp(environment()))
    {
        remove_object();
        return;
    }

    set_this_player(environment());

    if (objectp(stop_object) && stringp(stop_fun) &&
        (stop_object != this_object()))
    {
        call_other(stop_object, stop_fun, environment());
    }
    else if (stop_message)
    {
        environment()->catch_msg(stop_message);
    }

    remove_object();
}

/*
 * Function name: stat_object
 * Description  : Function called when wiz tries to stat this object.
 * Returns      : string - the extra information to print.
 */
string
stat_object()
{
    string str = ::stat_object();

    if (strlen(stop_verb))
    {
        str += "Stop verb: " + stop_verb + "\n";
    }
    if (strlen(stop_fun))
    {
        str += "Stop fun:  " + stop_fun + "\n";
    }
    if (strlen(stop_message))
    {
        str += "Stop mess: " + stop_message + "\n";
    }
    if (strlen(fail_message))
    {
        str += "Fail mess: " + fail_message + "\n";
    }
    if (remove_time)
    {
        str += "Duration:  " + remove_time + "\n";
    }
    if (objectp(stop_object))
    {
        str += "Stop obj:  " + file_name(stop_object) + "\n";
    }
	if (talkable)
	{
		str += "Normal speech is allowed.\n";
	}

    return str;
}
