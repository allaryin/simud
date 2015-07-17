#include <const.h>
#include <saveload.h>
#include <desc.h>
#include <object.h>
#include <item.h>
#include <quantity.h>
#include <stackobj.c>

DISTANT( "a crop seed" )
SPECIFIC( "the crop seed" )
LOOK( "An undescribed crop seed. It should be described." )
PLURAL( "crop seeds" )
TYPE( "seed" )
GETTABLE( 1 )
DROPPABLE( 1 )
WEIGHT( 30 )
BULK( 30 )
VALUE( 1 )

void create() {
   ::create();
/* OBJEDIT { */
   set_quantity( 1 );
/* } OBJEDIT */
}

mapping query_aspects() {
   return ([ C_CROP, C_SEED, C_RAW ]);
}

int query_is_crop_seed() {
   return 1;
}

string query_crop_type() {
   return "/econ/crop/" + explode(load_name(this_object()), "/")[<1];
}

ICON( '&', 0x8A, LAYER_ITEMS )
