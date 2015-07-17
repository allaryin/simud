// A basic humanoid skeleton with meat

inherit "/obj/monster";

NAME( "zombie" )
PLURAL( "zombie" )
SPECIFIC( "zombie" )
//DISTANT( "a skeleton" )
LOOK( "Two words. Corpsified. Gross. It's a sack of bones and skin with bits of meat, hair, and rotting cloth attached in assorted places. Generally foul smelling, this is not the sort of chap a gal'd be proud to bring home to mother." )

TYPE( "undead" )

WEIGHT( 57000 )

GETTABLE( 0 )
DROPPABLE( 1 )

ICON( 'z', 3, LAYER_LIVING )

void create() {
   object ob;
  ::create();
  set_living_name( name );
  set_gender( 0 );

  set_distant( "a zombie" );

  set_str( 20 );
  set_con( 15 );
  set_spd( 10 );
  set_dex( 10 );
  set_int( 0 );
  set_emp( 0 );
  set_wil( 15 );
  set_cha( 0 );

  set_food(query_food_capacity());
  set_drink(query_drink_capacity());

  // set_product_aspect( C_UNDEAD );
  // add_product( C_BONE, 12, 10 );
  // add_product( C_SKULL, 1, 20 );

// TODO: Allow the player to take a bone or two from the corpse
//set_env_var( "meat_type", "/home/jeff/items/bone.c" );

  set_race("undead");
  set_wc( (["blunt":8,"dark":3]) );

// TODO: Add basic AI
   ob = clone_object("/ai/aggro");
   ob->set_aggro_level(20);
   ob->move(this_object());
}

// Skeletons don't get hungry
void on_hunger() {
  add_food( 1000 );
}

// Skeletons don't get thirsty, either.
void on_thirst() {
  add_drink( 1000 );
}
