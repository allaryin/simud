// Seconds before attempting to autoload destructed objects.
// (sometimes wizards update code and don't reload).
// Only necessary if orphans exist. Note that the actual
// time until attempted reload may be up to twice as long
// as this.
#define AUTO_LOAD_TIME 900

inherit "/inherit/stopwatch";

mapping load_times;
mapping destruct_times;
mapping orphans;
mapping file_rename;

//int suppress_rep
int usec_load;

void set_usec_load(int new_load) {

   usec_load = new_load;
}

void add_file_rename( string src, string dest ) {
   if( !mappingp(file_rename) ) file_rename = ([ src: dest ]);
   else file_rename += ([ src: dest ]);
}

void remove_file_rename(string src) {
   m_delete(file_rename, src);
}

mapping query_file_rename() {
   return copy(file_rename);
}

mapping query_load_times() {
   return load_times;
}

mapping query_destruct_times() {
   return destruct_times;
}

mapping query_orphans() {
   return orphans;
}

void create() {
   mixed old_vals;

   old_vals = "/secure/master"->query_hold_var();
   usec_load = 1000000;

   if( pointerp(old_vals) ) {
      if( sizeof(old_vals) >= 1 ) load_times = old_vals[0];
      if( sizeof(old_vals) >= 2 ) orphans = old_vals[1];
      if( sizeof(old_vals) >= 3 ) destruct_times = old_vals[2];
   }

   set_heart_beat(1);
}

void on_load( string file ) {
   catch("/daemon/program"->on_load(file));
   if( !mappingp(load_times) ) load_times = ([ ]);
   load_times[file] = time();
   set_heart_beat(1);
}

void on_destruct( object ob ) {
   if( !mappingp(destruct_times) ) destruct_times = ([ ]);
   destruct_times[load_name(ob)] = time();
   if( !mappingp(orphans) ) orphans = ([ ]);
   if( is_clone(ob) && pointerp(orphans[load_name(ob)]) ) {
      orphans[load_name(ob)] -= ({ ob });
      if( sizeof(orphans[load_name(ob)]) == 0 )
         orphans -= ([ load_name(ob) ]);
   }
   if( !is_clone(ob) ) {
      object *clone_list;
      clone_list = clones(ob);
      catch("/daemon/program"->on_destruct(load_name(ob)));
      if( sizeof(clone_list) ) {
         if( pointerp(orphans[load_name(ob)]) )
            orphans[load_name(ob)] += clone_list;
         else
            orphans[load_name(ob)] = clone_list;
      }
   }
   set_heart_beat( 1 );
   if( find_call_out("force_reload") < 0 )
      call_out("force_reload", AUTO_LOAD_TIME + 10);
}

void force_load() {
   string *files = m_indices(orphans);

   foreach(string file: files) {
      catch(load_object(file));
   }

   set_heart_beat(1);
}

void force_reload() {
   mixed file;
   int time;

   if( !mappingp(destruct_times) || !mappingp(orphans) ) {
      debug("/daemon/uptodate: force_reload trying to cope with confusing data; no reloads.");
      return;
   }
   // Discourage multiple reload cycles
   remove_call_out( "force_reload" );
   // Loop it, so that it'll try again in however long, even if errors occur.
   call_out( "force_reload", AUTO_LOAD_TIME );
   foreach( file, time : destruct_times ) {
      if( !orphans[file] ) destruct_times -= ([ file ]);
      else if( time() - destruct_times[file] >= AUTO_LOAD_TIME ) {
         // If the file still has errors, this line will raise a publically visible error
         // and hopefully inspire some wiz to action.
         load_object( file );
         destruct_times -= ([ file ]);
         set_heart_beat(1);
      }
   }

   // Don't bother cycling the daemon if nothing's left to reload.
   if( sizeof(destruct_times) == 0 ) remove_call_out("force_reload");
}

mixed first_orphaned_file() {
   string *files = m_indices(orphans);

   files = sort_array( files, (: sizeof(orphans[$1]) > sizeof(orphans[$2]) :) );

   foreach( mixed file : files ) {
      if( find_object(file) )
         return file;
   }
   return 0;
}

void heart_beat() {
   mixed file;
   int iupdate;
   int starttime;

   if( !mappingp(orphans) || sizeof(orphans) == 0 ) {
      set_heart_beat( 0 );
      return;
   }
   orphans -= ([ "/obj/player" ]);
   file = first_orphaned_file();

   if( !file ) {
      set_heart_beat(0);
      return;
   }

   stopwatch_start();
   while (get_eval_cost() > 9500000 && stopwatch_read() < usec_load) {
      if( !pointerp(orphans[file]) || !sizeof(orphans[file]) ) {
         orphans -= ([ file ]);

         file = first_orphaned_file();

         if( !file ) {
            set_heart_beat(0);
            return;
         }
      } else {
         object ob, env;
         int obid;

         ob = orphans[file][0];
         if( !ob )
            orphans[file] -= ({ ob });
         else {
            if( !file_exists(file+".c") ) {
               debug( "Can't update clones of " + file + " because the file is missing! Put it back!" );
               return;
            }
            env = environment(ob);
            obid = ob->query_unique_id();
            all_inventory(ob)->move(this_object(),1);
            ob->save_self_only();
            destruct(ob);
            if( mappingp(file_rename) && stringp(file_rename[file]) )
               file = file_rename[file];
            ob = clone_object(file);
            ob->load_self(obid);
            if( env ) ob->move(env,1);
            if( !ob ) continue;
            all_inventory(this_object())->move(ob,1);
            if( !ob ) continue;
            ob->save_self_only();
         }
      }
   }
}

string destructor( object prev ) {
   "/secure/master"->set_hold_var( ({ load_times, orphans, destruct_times }) );
   return 0;
}
