#include <const.h>
inherit "/scn/water/water";

int query_is_ocean() {
   return 1;
}

int query_zlayer() { return LAYER_WATER; }

void create() {
   ::create();
/* OBJEDIT { */
   set_name("ocean");
   set_type("water");
   set_specific("the ocean");
   set_distant("the ocean");
   set_look("The gently pounding surf is really very soothing.");
/* } OBJEDIT */
}

string query_liquid_type() { return "/items/drink/salt_water.c"; }

int query_is_drinkable() { return 0; }
