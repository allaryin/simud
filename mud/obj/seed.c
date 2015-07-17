#include <const.h>
#include <seed.h>
#include <saveload.h>
#include <desc.h>
#include <object.h>
#include <item.h>
#include <quantity.h>
#include <stackobj.c>

DISTANT( "a seed" )
SPECIFIC( "the seed" )
LOOK( "An undescribed seed. It should be described." )
PLURAL( "seeds" )
TYPE( "seed" )
GETTABLE( 1 )
DROPPABLE( 1 )
WEIGHT( 30 )
BULK( 30 )
VALUE( 1 )

void create() {
   ::create();
/* OBJEDIT { */
   set_seed_strength( 10 );
   set_quantity( 1 );
/* } OBJEDIT */
}

mapping query_aspects() {
   return ([ C_SEED, C_RAW ]);
}

int query_is_seed() {
   return 1;
}

ICON( '&', 0x8A, LAYER_ITEMS )
