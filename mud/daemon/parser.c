// Note: The reason for the odd sort is to make sure that even if some of
// the tags occur as substrings of others, longer tags are
// always sought out first. You could still break this with some
// tricky regexp's; don't. The canonical example is HELDOBJECTS
// and HELDOBJECT: You must check for HELDOBJECTS first. --Acius
string *keywords = sort_array( ({
"CLOSELIVING",		// A living that you are adjacent to.
"CLOSEOBJECT",		// Like HELDOBJECT, but adjacency is also fine.
"HELDOBJECT",		// An object in your inventory
"HELDOBJECTS",		// One or more objects in your inventory
"LIVING",		// A living object anywhere around you
"OBJECT",		// An object that you can reach, but not necessarily next to you
"OBJECTS",		// One or more objects that are reachable
"OPT\\([a-zA-Z0-9]*\\)",	// Options for wiz commands ... -s -t, or -st, etc.
"PLAYER",		// A player (any player anywhere in the world). Must be logged in, though.
"STRING",		// Any string.
"WILDCARD",		// Wildcard for files; sends an array of file names.
"PREFIX\\(.\\)"
}), (: strlen($1) < strlen($2) :) );

string *query_keywords() {
   return keywords;
}

// give CLOSELIVING HELDOBJECTS\n$2 $1
// give HELDOBJECTS to CLOSELIVING
// eat CLOSEOBJECTS
// ls OPT(lta) WILDCARD
// wi WOBJECT

mapping command_forms = ([ ]);
mapping prefix_commands = ([ ]);

void add_command_form( string verb, string format ) {
   string *parse_chain;

   parse_chain = regexplode( format, implode(keywords,"|") );
   msg("parse_chain is " + as_lpc(parse_chain) );
}

object *find_verb_files( string name ) {
   mapping m = ([ ]);
   string *bin_paths;

   if( sizeof(get_dir("/bin/"+name+".c")) )
      m += ([ load_object("/bin/" + name) ]);

   if( sizeof(get_dir("/bin/wiz/"+name+".c")) )
      m += ([ load_object("/bin/wiz/" + name) ]);

   if( sizeof(get_dir("/bin/soul/"+name+".c")) )
      m += ([ load_object("/bin/soul/" + name) ]);

   if( this_player() ) {
      bin_paths = this_player()->query_env_var( "bin_paths" );
      if( pointerp(bin_paths) ) {
         string path;
         foreach( path : bin_paths )
         if( stringp(path) && sizeof(get_dir(path+"/"+name+".c")) ) {
            m += ([ load_object(path+"/"+name) ]);
         }
      }
   }

   return m_indices( m );
}

int parse( string com ) {
   string a, b, *split;
   string verb, param;

   if( !stringp(com) ) {
      debug( load_name() + ": parse_command only accepts a string. (from " + as_lpc(previous_object()) + ")" );
      return 0;
   }
   if( sscanf( com, "%s %s", a, b ) == 2 ) {
      if( member("/daemon/dictionary"->get_adverb_list(), a) ) {
         set_adverb(a);
         com = b;
      }
      else
         set_adverb(0);
   }
   else {
      // Handle adverb-less commands.
      set_adverb(0);
   }

   // Todo: translate prefix forms to verb forms.

   // Adverb's gone, extract the verb (if any)
   split = explode( com, " " );
   verb = split[0];
   param = implode( split[1..], " " );

   // This method is used for 'standard' (i.e. unregistered) bin verbs.

}

void create() {
   // Load all bin verbs here!
}
