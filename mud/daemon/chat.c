#include <const.h>

#define FILE "/daemon/chat"
#define HIST_LENGTH   15   // how many entries to store in history

#define PERM_LOCKED   1   // nobody may join or leave
#define PERM_WIZARD   2   // must be wizard to join
#define PERM_PRIVATE   4   // must be invited to join
#define PERM_VOICE   8   // must be voiced to speak
#define PERM_SPAMMY   16   // channel shows entrance/exit messages

#define IRCD   "/secure/sockets/ircd"

mapping list;   // who's listening   (string*)
mapping perm;   // basic permissions   (int)
mapping owner;   // who owns a chan   (string*)
mapping invite;   // who's been invited   (string*)
mapping voice;   // who has voice   (string*)
mapping color;   // color of messages   ({chan, name})
mapping ban;   // who's been banned   (string*)
mapping hist;   // history of line   (string[<=HIST_LENGTH])

mapping color_key;   // just like it sounds ;)

nosave int tick;   // number of chats until next save

//// prototypes ////
void load();
void save();
int new_chan(string chan);
void add_owner(string chan, string name);

mapping query_list() { return copy(list); }
mapping query_perm() { return copy(perm); }
mapping query_owner() { return copy(owner); }
mapping query_invite() { return copy(invite); }
mapping query_voice() { return copy(voice); }
mapping query_color() { return copy(color); }
mapping query_ban() { return copy(ban); }
mapping query_hist() { return copy(hist); }

void create() {
   load();
   debug("daemon is ready for action","chat");
}

void destructor() {
   debug("destructor called","chat");
   save();
}

void log_chat(string chan, string name, string text) {
   string buf;
   object speaker = find_player(name);
   if (speaker && speaker->query_toad())
      return;

   if (!pointerp(hist[chan]))
      hist[chan] = ({});
   else if (sizeof(hist[chan]) == HIST_LENGTH)
      hist[chan] = (hist[chan])[1..];

   // Note: This array format is identical to that used in tellhist
   hist[chan] += ({ ({name,ctime(time())[11..18],text}) });

   // Save messages to disk
   if( file_size("/log/chat") == -2 )
      log_file( "chat/"+chan, text + "\n" );
}

void transmit(object ob, string chan, string mssg) {
   int raw = 0, indent = strlen(chan) + 3;
   object chat_ob;
   mixed reco_save;

   if( lower_case(chan[0..4]) == "debug" || lower_case(chan) == "error" ) {
      if( !find_object("/daemon/ansi") || !find_object("/obj/verb") ||
          !find_object("/secure/simul_efun") || !find_object("/secure/master") ||
          !find_object("/obj/chat") ) {
         raw = 1;
         efun::write("MUDlib is missing system-critical objects; color not processed.\n");
      }
   }

   set_listener(ob);

   chat_ob = present("chat", ob);
   if (chat_ob && chat_ob->query_muted())
      return;

   reco_save = query_global_desc_var("RECO");
   set_global_desc_var("RECO", "");
   if ( raw )
      efun::tell_object( ob, mssg );
   else
      msg_object( ob, mssg, indent );
   set_global_desc_var("RECO", reco_save);
}

varargs void chat(string chan, string name, string text, int is_external) {
   object ob, speaker = find_player(name);
   string who, buf, *words;
   int is_soul = 0, is_i3 = 0;

   debug( chan+", "+name+", "+text, "chat" );
   if( chan == "chat" && IRCD->is_connected() && !is_external ) {
      //debug( "chatting to irc" );
// masked because IRCD is broken
//      IRCD->chat( name, process_ansi(text, this_object()) );
   } else
   if( is_external ) {
      // clean up incoming from irc
      string *tmp;
      tmp = explode( text, "ACTION " );
      //debug( "tmp = "+as_lpc(tmp) );
      if( sizeof(tmp) == 2 )
         text = ":"+tmp[1];
   }

   if (name == "system")
      name = "";
   else if (speaker && speaker->query_invis() == 1 && chan != "wiz")
      name = "someone";

   if (chan == "igossip" || chan == "icode" || is_external)
      is_i3 = 1;

   if (!speaker && !is_i3 && name != "") {
      debug(capitalize(name)+" cannot speak on "+chan+": no such player","chat");
      return;
   }
   if (speaker && speaker->query_level() < ADMLEVEL) {
      if (member(list[chan],name) == -1) {
         debug(capitalize(name)+" cannot speak on "+chan+": not listening","chat");
         return;
      }
      if (perm[chan] & PERM_VOICE && member(voice[chan],name) == -1) {
         debug(capitalize(name)+" cannot speak on "+chan+": not voiced","chat");
         return;
      }
   }

   if (!is_i3) {
      // detect souls
      words = explode(text, " ");
      //debug("  words = "+sizeof(words),"chat");
      if ( name != "someone" && sizeof(words) < 3) {
         string file = "/bin/soul/"+words[0];
         int uses_target;
         //debug("  checking for file '"+file+".c'","chat");
         if (file_exists(file+".c")) {
            string new_text;
            is_soul = 1;
            //debug("  soul found","chat");

            // lowercase player names
            if (sizeof(words) > 1)
               words[1] = lower_case(words[1]);

            if (sizeof(words) == 1) {
               uses_target = 0;
               new_text = file->query_cself();
            } else if (find_player(words[1])) {
               uses_target = 1;
               new_text = file->query_ctarget();
            } else {
               is_soul = 0;
               new_text = text;
            }

            if (is_soul && new_text)
               new_text = explode(new_text,"@@")[0];
            if( uses_target && member(list[chan], words[1]) == -1) {
               is_soul = 0;
               uses_target = 0;
            } else {
               text = new_text;
               is_soul += uses_target;
            }
         }
      }
   }

   if (is_soul) { // soul
      buf = "~CDEF["+color[chan][0]+capitalize(chan)+"~CDEF] "+color[chan][1]+text+"~CDEF";
      if (is_soul == 2)
         log_chat(chan,name,capitalize(name)+": *"+words[0]+" "+words[1]+"*");
      else
         log_chat(chan,name,capitalize(name)+": *"+words[0]+"*");
   } else if ( (text[0] == ':' || text[0] == ';') && strlen(text) > 3 && text != ":~LPAR" ) {
//      debug("length of ~CACT'" + as_lpc(to_array(text)) + "' is " + strlen(text));
      buf = "~CDEF["+color[chan][0]+capitalize(chan)+"~CDEF] "+
            color[chan][1]+capitalize(name)+" "+text[1..]+"~CDEF";
      log_chat(chan,name,capitalize(name)+" "+text[1..]);
   } else { // normal chat or smiley
      if( speaker && speaker->query_env_var("accent") )
          text = (speaker->query_env_var("accent"))->garble(text);
      buf = "~CDEF["+color[chan][0]+capitalize(chan)+"~CDEF] "+
            color[chan][1]+capitalize(name)+"~CDEF: "+text+"~CDEF";
      log_chat(chan,name,capitalize(name)+": "+text);
   }

   set_actor(this_player());
   if (sizeof(words) == 2)
      set_target(find_player(words[1]));

   // if someone's been toaded, only they see their chat
   if (speaker && speaker->query_toad()) {
      transmit(speaker, chan, buf);
   } else {
      foreach (who : list[chan]) {
         if (ob = find_player(who)) {
            transmit(ob, chan, buf);
         }
      }
   }

   if (tick-- == 0)
      save();
}

int enter(string chan, object who, int quiet) {
   string name = who->query_name();

   if (!member(list,chan)) {
      if (who->query_level() < WIZLEVEL) {
         string buf = "";
         if (who->query_age() < 604800 || // one week
             who->query_level() >= WIZLEVEL)
            buf = ", but you could create it.";
         debug(name+" could not enter "+chan+": no such channel","chat");
         if (!quiet)
           msg_object(who,"~CWRNThere is no channel by the name of '"+chan+"'"+buf+"~CDEF");
         return 0;
      } else if (chan[0..4] != "debug" && chan != "error") {
         debug(name+" attempted to enter nonexistant channel "+chan+": creating","chat");
         if (!quiet)
           msg_object(who,"~CWRNThere is no channel by the name of '"+chan+"', creating it for you ;)~CDEF");
         if (new_chan(chan))
            add_owner(chan, name);
         else
            return 0;
      }
   } else {
      if (!pointerp(list[chan]))
         list[chan] = ({});

      if (member(list[chan],name) != -1) {
         debug(name+" could not enter "+chan+": already listening","chat");
         if (!quiet)
           msg_object(who,"~CWRNYou are already listening to "+chan+".~CDEF");
         return 0;
      }
   }

   if (who->query_level() < ADMLEVEL && member(perm, chan)) {
      if ((perm[chan] & PERM_WIZARD || chan[0..4] == "debug" ||
           chan == "error") && who->query_level() < WIZLEVEL) {
         debug(name+" could not enter "+chan+": wizard channel","chat");
         if (!quiet)
           msg_object(who,"~CWRN"+capitalize(chan)+" is a wizard-only channel, you cannot enter.~CDEF");
         who->remove_env_var_item("channels", chan);
         return 0;
      }
      if (perm[chan] & PERM_PRIVATE && member(invite[chan],name) == -1) {
         debug(name+" could not enter "+chan+": not invited","chat");
         if (!quiet)
           msg_object(who,"~CWRN"+capitalize(chan)+" is a private channel, and you haven't been invited.~CDEF");
         who->remove_env_var_item("channels", chan);
         return 0;
      }
      if (perm[chan] & PERM_LOCKED) {
         debug(name+" could not enter "+chan+": locked","chat");
         if (!quiet)
           msg_object(who,"~CWRN"+capitalize(chan)+" is a locked channel, you cannot enter.~CDEF");
         who->remove_env_var_item("channels", chan);
         return 0;
      }
   }

   // only add people to channels if they actually exist
   if (member(list, chan) && chan[0..4] != "debug" && chan != "error") {
      list[chan] += ({ name });
      if (perm[chan] & PERM_SPAMMY && who->query_invis() != 1)
         chat(chan,name,":has entered the channel.",0);
   }
   return 1;
}

int exit(string chan, object who) {
   string name = who->query_name();
   // only remove people from channels if the chan actually exists ;)
   if (chan[0..4] == "debug" || chan == "error")
      return 0;
   if (!pointerp(list[chan]))
      list[chan] = ({});
   if (member(list[chan],name) == -1) {
      debug(name+" could not exit "+chan+": not listening","chat");
      msg_object(who,"~CWRNYou aren't even listening to "+chan+"!~CDEF");
      return 0;
   }
   if (who->query_level() < ADMLEVEL && perm[chan] & PERM_LOCKED) {
      debug(name+" could not exit "+chan+": locked","chat");
      msg_object(who,"~CWRN"+capitalize(chan)+" is a locked channel, you cannot escape!~CDEF");
      return 0;
   }
   if (perm[chan] & PERM_SPAMMY && who->query_invis() != 1)
      chat(chan,name,":has left the channel.",0);
   list[chan] -= ({ name });
   return 1;
}

void login(object who) {
   string* list = who->query_env_var("channels");
   string chan;
   int total;
   foreach (chan : list) {
      // if (chan[0..4] == "debug" || chan == "error") {
      //    debug("skipping channel '"+chan+"' in login","chat");
      //    continue;
      // }
      total += enter(chan,who,1);
   }
   debug(who->query_cap_name()+" entered "+total+" channels.","chat");
// Slain by the spam reduction crusaders!
//   msg_object(who,"~CBRTYou are listening to "+total+" channels.~CDEF");
}

void logout(object who) {
   string* list = who->query_env_var("channels");
   string chan;
   foreach (chan : list) {
      // if (chan[0..4] == "debug" || chan == "error") {
      //    debug("skipping channel '"+chan+"' in logout","chat");
      //    continue;
      // }
      exit(chan,who);
   }
}

void set_color(string chan, string one, string two) {
   if (!member(color_key,one) || !member(color_key,two)) {
      debug("invalid colours in: '"+one+"' or '"+two+"'","chat");
      return;
   }
   debug("color on "+chan+" set to ["+one+"] "+two+":","chat");
   color[chan] = ({color_key[one], color_key[two]});
}

string query_perm_str(string chan) {
   string buf = "";
   if (perm[chan] & PERM_LOCKED)
      buf += "locked ";
   if (perm[chan] & PERM_WIZARD)
      buf += "wizard ";
   if (perm[chan] & PERM_PRIVATE)
      buf += "private ";
   if (perm[chan] & PERM_VOICE)
      buf += "voice ";
   if (perm[chan] & PERM_SPAMMY)
      buf += "spammy ";
   return trim(buf);
}

void set_perm(string chan, int x) {
   perm[chan] = x;
   debug("permissions on "+chan+" set to: "+query_perm_str(chan),"chat");
}

void toggle_perm(string chan, string str);

void add_owner(string chan, string name) {
   if (!pointerp(owner[chan]))
      owner[chan] = ({});
   owner[chan] += ({ name });
}

void del_owner(string chan, string name) {
   if (!pointerp(owner[chan]))
      owner[chan] = ({});
   owner[chan] -= ({ name });
}

void add_invite(string chan, string name) {
   if (!pointerp(invite[chan]))
      invite[chan] = ({});
   invite[chan] += ({ name });
}

void del_invite(string chan, string name) {
   if (!pointerp(invite[chan]))
      invite[chan] = ({});
   invite[chan] -= ({ name });
}

int new_chan(string chan) {
   if (member(m_indices(list),chan) != -1) {
      debug("cannot create new channel "+chan+", channel already exists","chat");
      msg("~CWRNA channel by the name of "+chan+" already exists.~CDEF");
      return 0;
   }
   if (chan[0..4] == "debug") {
      debug("cannot create new channel "+chan+", debug chan ;)","chat");
      msg("~CWRNYou cannot create 'debug' channels~CDEF");
      return 0;
   }
   if (chan == "admin") {
      debug("cannot create new channel "+chan+", admin channel","chat");
      msg("~CWRNYou cannot create a channel with that name.~CDEF");
      return 0;
   }
   debug("creating new channel "+chan,"chat");
   list[chan] = ({});
   owner[chan] = ({});
   invite[chan] = ({});
   ban[chan] = ({});
   hist[chan] = ({});
   voice[chan] = ({});
   set_perm(chan,0);
   set_color(chan,"hicyan","higreen");
   save();
   return 1;
}

void del_chan(string chan) {
   if (chan == "chat" || chan == "wiz" || chan == "igossip" || chan == "icode") {
      debug("cannot delete channel "+chan,"chat");
      return;
   }
   debug("deleting channel "+chan,"chat");
   list -= ([chan]);
   owner -= ([chan]);
   invite -= ([chan]);
   ban -= ([chan]);
   hist -= ([chan]);
   voice -= ([chan]);
   perm -= ([chan]);
   color -= ([chan]);
   save();
}

void load() {
   if (restore_object(FILE))
      debug("success loading from "+FILE,"chat");
   else {
      debug("failure loading from "+FILE,"chat");
      debug("initializing colour key","chat");
      color_key = ([ "red"      : "~[010",
                     "green"    : "~[020",
                     "brown"    : "~[030",
                     "blue"     : "~[040",
                     "magenta"  : "~[050",
                     "cyan"     : "~[060",
                     "gray"     : "~[070",
                     "black"    : "~[080",
                     "hired"    : "~[090",
                     "higreen"  : "~[100",
                     "yellow"   : "~[110",
                     "hiblue"   : "~[120",
                     "himagenta": "~[130",
                     "hicyan"   : "~[140",
                     "white"    : "~[150" ]);
      debug("creating default channels","chat");
      list = ([]); perm = ([]); owner = ([]); invite = ([]);
      color = ([]); ban = ([]); hist = ([]); voice = ([]);

      new_chan("chat");

      new_chan("wiz");
      set_color("wiz","white","cyan");
      set_perm("wiz",PERM_WIZARD);

      new_chan("igossip");
      set_color("igossip","hired","brown");
      set_perm("igossip",PERM_WIZARD | PERM_PRIVATE | PERM_VOICE);

      new_chan("icode");
      set_color("icode","hired","brown");
      set_perm("icode",PERM_WIZARD | PERM_PRIVATE | PERM_VOICE);

      save();
   }
}

void save() {
   if (save_object(FILE) == 0)
      debug("success saving to "+FILE,"chat");
   else
      debug("failure saving to "+FILE,"chat");
   tick = 10;
   load();
}
