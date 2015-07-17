#include <const.h>
inherit "/obj/book.c";

NAME( "paper" )
DISTANT( "a piece of paper" )
SPECIFIC( "the piece of paper" )
PLURAL( "pieces of paper" )
ALT_NAME( ({ "piece of paper", "piece" }) )
ALT_PLURAL( ({ "papers", "pieces" }) )
GETTABLE( 1 )
DROPPABLE( 1 )
WEIGHT( 25 )
BULK( 1 )
TASTE( "Owth, a babercud!" )
// there's no way for a player to know what locations a
// book or piece of paper or whatever has unless you
// tell them in the description. don't forget to do this
LOOK("A piece of creamy white stationary.")


void create() {
  ::create();

  set_pages(0);  // paper doesn't have pages like a book
  add_location("front");
  set_location("front", "");
}

void remove() {
  destruct(this_object());
}

mapping query_aspects() {
   return ([ C_PAPER, C_RAW ]);
}
