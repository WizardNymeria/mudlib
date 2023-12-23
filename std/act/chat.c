/*
   /std/act/chat.c

   Chatting: Standard action module for mobiles

   add_chat(string)            Set a random chatstring

   add_cchat(string)           Set a random combat chatstring

   clear_chat()                Clear random chatstring

   clear_cchat()               Clear random combat chatstring

   set_chat_time(int)          Set the mean value for chat intervall

   set_cchat_time(int)         Set the mean value for combat chat intervall
*/

#pragma save_binary
#pragma strict_types

#include <macros.h>

static  int     monster_chat_time = 1, /* Intervall between chat */
                monster_cchat_time;    /* Intervall between combat chat */
static  string  *monster_chat,         /* Chat strings */
                *monster_cchat;        /* Combat Chat strings */
static  int     monster_chat_limit,    /* Current ceiling for unused chats */
                monster_cchat_limit;   /* Current ceiling for unused cchats */
static  string  monster_chat_verb = "say";  /* Verb to use for chat */

#define SEQ_CHAT  "_mon_chat"

void monster_do_chat();

/*
 * Function name: add_chat
 * Description:   Adds a chat string that the monster will randomly say.
 * Arguments:     str: Text
 */
void
add_chat(mixed str)
{
    if (!IS_CLONE)
        return;

    if (!this_object()->seq_query(SEQ_CHAT))
    {
        this_object()->seq_new(SEQ_CHAT);
    }

    this_object()->seq_clear(SEQ_CHAT);
    this_object()->seq_addfirst(SEQ_CHAT,
        ({ random(monster_chat_time + 1), monster_do_chat }));

    if (!str)
        return;

    if (!sizeof(monster_chat))
        monster_chat = ({});

    monster_chat += ({ str });
    monster_chat_limit = sizeof(monster_chat);
}

string *query_chat() { return monster_chat; }

/*
 * Function name: add_cchat
 * Description:   Sets a combat chat string that the monster will randomly say
 * Arguments:     str: Text
 */
void
add_cchat(string str)
{
    if (!IS_CLONE)
        return;

    add_chat(0); /* Init chat sequence */

    if (!sizeof(monster_cchat))
        monster_cchat = ({});

    monster_cchat += ({ str });
    monster_cchat_limit = sizeof(monster_cchat);
}

string *query_cchat() { return monster_cchat; }

/*
 * Function name: clear_chat
 * Description:   Clear random chatstring
 */
void
clear_chat()
{
    monster_chat = monster_chat_limit = 0;
}

/*
 * Function name: clear_cchat
 * Description:   Clear random combat chatstring
 */
void
clear_cchat()
{
    monster_cchat = monster_cchat_limit = 0;
}

/*
 * Function name: set_chat_time
 * Description:   Set the mean value for chat intervall
 * Arguments:     tim: Intervall
 */
void
set_chat_time(int tim)
{
    monster_chat_time = tim;
}

/*
 * Function name: set_chat_verb
 * Description:   Set the verb to use for chat (default: "say")
 * Arguments:     verb: Verb
 */
void
set_chat_verb(string verb)
{
    monster_chat_verb = verb;
}

/*
 * Function name: set_cchat_time
 * Description:   Set the mean value for cchat intervall
 * Arguments:     tim: Intervall
 */
void
set_cchat_time(int tim)
{
    monster_cchat_time = tim;
}

/*
 * Sequence functions
 */

/*
 * Description: The actual function chatting, called by VBFC in seq_heartbeat
 *
 * This is responsible for selecting the strings to say, it does this by
 * by moving the used string to the end of the array and then selecting
 * a random string from the unused strings.
 */
void
monster_do_chat()
{
    int limit, delay;
    mixed *pool;

    if (!this_object()->query_attack())
    {
        pool = monster_chat;
        delay = monster_chat_time;
        limit = monster_chat_limit--;

        if (monster_chat_limit < 0)
            monster_chat_limit = sizeof(monster_chat);
    }
    else
    {
        pool = monster_cchat;
        delay = monster_cchat_time;
        limit = monster_cchat_limit--;

        if (monster_cchat_limit < 0)
            monster_cchat_limit = sizeof(monster_cchat);
    }

    this_object()->seq_clear(SEQ_CHAT);
    this_object()->seq_addlast(SEQ_CHAT, ({ delay, monster_do_chat }) );

    if (!pointerp(pool) || !sizeof(pool))
        return;

    int selected = random(limit);

    // Move the selected chat to the end of the pool
    mixed chat = pool[selected];
    pool[selected] = pool[limit - 1];
    pool[limit - 1] = chat;

    if (functionp(chat))
        chat = chat(this_object());

    return monster_chat_verb + " " + chat;
}

public string
query_seq_chat_name()
{
    return SEQ_CHAT;
}

#if 0
/*
 * Function name: catch_whisper
 * Description  : Called whenever someone whisper to this living. It does
 *                not indicate when this living is an onlooker. Use
 *                speech_hook for that.
 * Arguments    : string str - the text being whispered.
 */
public void
catch_whisper(string str)
{
}
#endif 0
