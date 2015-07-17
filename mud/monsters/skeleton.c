// A basic humanoid skeleton

inherit "/obj/monster";

NAME( "skeleton" )
PLURAL( "skeletons" )
SPECIFIC( "the skeleton" )
//DISTANT( "a skeleton" )
LOOK( "It appears to be the bleached-out skeleton of a man-sized humanoid. It's animate, and quite mean looking." )

TYPE( "undead" )

WEIGHT( 27000 )

GETTABLE( 0 )
DROPPABLE( 1 )

ICON( 's', 7, LAYER_LIVING )

void create() {
   object ob;
  ::create();
  set_living_name( name );
  set_gender( 0 );

  set_distant( "a skeleton" );

  set_str( 10 );
  set_con( 10 );
  set_spd( 10 );
  set_dex( 5 );
  set_int( 0 );
  set_emp( 0 );
  set_wil( 10 );
  set_cha( 0 );

  set_food(query_food_capacity());
  set_drink(query_drink_capacity());

  // set_product_aspect( C_UNDEAD );
  // add_product( C_BONE, 12, 10 );
  // add_product( C_SKULL, 1, 20 );

// TODO: Allow the player to take a bone or two from the corpse
//set_env_var( "meat_type", "/home/jeff/items/bone.c" );

  set_race("undead");
  set_wc( (["blunt":6]) );

// TODO: Add basic AI
   ob = clone_object("/ai/aggro");
   ob->set_aggro_level(20);
   ob->move(this_object());
}

// Skeletons don't get hungry
void on_hunger() {
  add_food( 100 );
}

// Skeletons don't get thirsty, either.
void on_thirst() {
  add_drink( 100 );
}
