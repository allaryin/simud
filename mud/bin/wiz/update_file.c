// Made separate messages for updated vs nonexistant files,
// and fixed formatting.  -- Zarylse, 2003-09-07
#include <const.h>
#include <wizard.h>

void update_ancestors( object ob ) {
   string *ancestors, curr;

   ancestors = inherit_list( ob );
   foreach( curr : ancestors ) destruct( find_object(curr) );
}

int main(string str) {
   object ob, env;
   string a, b;
   int obid;

   WIZ_CHECK

   if ( !str )
      return notify_fail("==> [Format] update <object>\n");

   // Check if it's a directory update.
   if( legal_filename(a = this_player()->make_path_absolute(str)) && file_size(a) == -2 ) {
      // Update all files in the directory.
      if( a[<1] != '/' ) a += "/";
      msg("Updating all blueprints in " + a);
      foreach( b : get_dir(a) )
         if( ob = find_object(a+b) ) destruct(ob);
      return 1;
   }

   if ( ob ) {
      // If we're updating a clone, we should restore the clone afterwards.
      if ( sscanf(object_name(ob), "%s#%s", a, b) == 2 || object_name(ob) == "/world/world" ) {
         if( ob->query_is_player() )
            return notify_fail( "==> [Error] Oops! You can't update a player. Get them to use 'flash' instead.\n" );
         env = environment( ob );
         obid = ob->query_unique_id();
         // Force an error if the object has syntax errors, etc.
         load_object( a );
         all_inventory(ob)->move( this_object(), 1 );
         ob->save_self_only();
         if( object_name(ob) != "/world/world" ) {
            destruct( ob );
            ob = clone_object( a );
            ob->load_self( obid );
         }
         else {
            destruct( ob );
            ob = load_object("/world/world");
         }
         if ( env ) ob->move( env, 1 );
         all_inventory( this_object() )->move( ob, 1 );
         ob->save_self_only();
         msg( "==> [Notice] Object "+str+" updated and restored." );
         if ( function_exists("query___FLIST", ob) && !pointerp(ob->query___FLIST()) )
            msg("~CWRNWarning: No __FLIST, but was expected. Did you forget to call ::create()?~CDEF");
      }
      // When updating a file, leave it destructed.
      else {
         update_ancestors( ob );
         destruct( ob );
         msg( "==> [Notice] Object "+str+" and all its ancestors updated." );
      }
   } else if ( file_exists(this_player()->make_path_absolute(str)) || file_exists(this_player()->make_path_absolute(str) + ".c") )
      return notify_fail( "==> [Notice] "+str+" is already updated.\n" );
   else
      return notify_fail("==> [Error] Could not find "+str+".\n");

   return 1;
}
