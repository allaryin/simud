#include <object.h>
#include <saveload.h>

// mine flood
int kill;      //

void create() {
   ::create();
}

string query_name() {
   return "mine_flood";
}

string query_specific() {
   return "mine_flood";
}

object id( string param ) {
   if( param == "mine_flood" ) {
      return this_object();
   }
   return 0;
}

int query_invis() {
   return 1;
}

void start_flood() {
   call_out("finish_flood", 5);
}

static void finish_flood()
{
   object ob;
   object room;

   kill = 1;   // instakill anyone who enters from this moment

   room = environment();

   // kill everything inside
   foreach( ob : all_inventory(room) ) {
      if( ob == this_object() )
         continue;
      if( ob->query_is_living() ) {
         msg_object( ob, "~CWRNWater fills the passageway, and you quickly drown!~CDEF");
         ob->set_env_var("cause_of_death", "has drowned in a flooded mine.");
         ob->set_hp(0);
         ob->die();
      }
   }

   foreach( ob : all_inventory(room) ) {
      if ( ob && ob->query_is_exit() ) {
         if( ob->query_backlink() ) {
            destruct(ob->query_backlink());
         }
         if( ob ) {
            destruct(ob);
         }
      }
   }
}

// if flooded, kill anyone who enters
// closes teleportation/respawn exploits that dodged
// the flood.
// -gpfault
void on_sight( object who, object what ) {
   debug("Mine flood sees entry");
   if( kill && who->query_is_living() ) {
      msg_object( who, "~CWRNYou fall into the water and drown!" );
      who->set_env_var( "cause_of_death", "has drowned in a flooded mine." );
      who->set_hp(0);
      who->die();
   } else {
      msg_object( who, "~CWRNThe room is flooding!" );
   }
}
