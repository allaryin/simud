#include <const.h>
#include <item.h>
#include <condition.h>
#include <saveload.h>
#include <object.h>

DISTANT("a firecracker")
ALT_NAME( ({"cracker", "party cracker", "fire cracker"}) )
SPECIFIC("the firecracker")
PLURAL("firecrackers")
NAME("firecracker")
LOOK( "A little party cracker. Use it to make noise!" )
GETTABLE(1)
DROPPABLE(1)
WEIGHT(200)
BULK(1)
ALT_PLURAL( ({ "crackers", "party crackers" }) )
TASTE( "It tastes like sulphur and... and something else!" )

string cracker_picture() {
   string *pics = ({
      "                          '.\n"
      "  .       .:::.   ..// `\\'/\�\n"
      "  /))/   :::::::\\(//.     *-O-*\n"
      "-\\O/-   ::::.-/( )\\-.    \�/'\\`\n"
      " \\((\\      ::  '//)\\\n"
      "   '          //''"
   });
   string pic = pics[random(sizeof(pics))];
   return ({"~CBRT","~CTIT","~CWRN","~CCOM","~CACT"})[random(5)]+pic+"~CDEF";
}

varargs int on_use(object who) {
   set_actor(this_player());
   if( who ) {
      set_target( who );
      this_player()->msg_local("~CACT~Name ~verbpoint ~poss firecracker at ~targ and ~verbactivate!~CDEF");
      this_player()->msg_local( cracker_picture() );
      who->add_held(1);
   } else {
      this_player()->msg_local("~CACT~Name ~verbset off a firecracker!~CDEF");
      this_player()->msg_local( cracker_picture() );
   }
   this_player()->add_readiness(-250);
   return 1;
}

mapping query_aspects() {
   return ([ C_PAPER ]);
}
