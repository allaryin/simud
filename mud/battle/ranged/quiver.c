//  Made by the magnificent Kriatorr

#include <container.h>
#include<desc.h>
#include<item.h>

inherit "/inherit/apparel.c";
inherit "/battle/ranged_weapon.c";

void create() {
/* OBJEDIT { */
   set_name("quiver");
   set_distant("a quiver");
   set_specific("the quiver");
   set_plural("quivers");
   set_look("A quiver. You can put arrows in it.");
   set_gettable( 1 );
   set_droppable( 1 );
   set_material( "leather" );
   set_prevent_insert("Only arrows go in this container.");
   set_prevent_remove( "This shouldn't be in a quiver. Notify a wizard." );
   set_weight_message( "The quiver is already full of arrows." );
   set_weight_capacity(20);
   set_body_parts(BP_NECK);

/* } OBJEDIT */
   ::create();
}

object container_id( string val ) {
   return present( val, this_object() );
}

