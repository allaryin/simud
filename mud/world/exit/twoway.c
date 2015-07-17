#include <const/exitsconst.h>
inherit "/world/exit/exit";

nosave object backlink;

varargs void check_backlink( int create_flag ) {
   string inv_exit;
   object ob, dest;
   string loc;

   if( !dir || !link ) return;
   inv_exit = INVERSE_EXITS[dir];
   if( !inv_exit ) {
      if( !this_object()->query_out_backlink() ) return;
      inv_exit = "out";
   }
   dest = get_destination();
   if( !dest || !inv_exit ) return;
   foreach( ob: all_inventory(dest) )
   if( ob->get_destination() == environment() ) {
      backlink = ob;
      break;
   }
   if( !backlink && create_flag ) {
      int dx, dy;

      // Create an inverse exit.
      if( file_exists(load_name()+"_mirror.c") ) {
         backlink = clone_object( load_name() + "_mirror" );
         backlink->set_master_ob( this_object() );
      }
      else
         backlink = clone_object( load_name() );
      backlink->move( dest );
      backlink->set_dir( inv_exit );
      if( environment(environment(backlink)) == environment(environment()) )
         backlink->set_link( environment()->query_name() );
      else if( environment(environment(environment(backlink))) == environment() )
         backlink->set_link( "env" );
      else
         backlink->set_link( get_location(environment(this_object())) );
      // A quick 'n dirty attempt to get positioning right.
      // This won't always work, but it should work reasonably often.
      dx = environment(backlink)->query_x() - environment()->query_x();
      if( dx < 0 ) dx = dx * 5 - 1;
      if( dx > 0 ) dx = dx * 5 + 1;
      dy = environment(backlink)->query_y() - environment()->query_y();
      if( dy < 0 ) dy = dy * 3 - 1;
      if( dy > 0 ) dy = dy * 3 + 1;
//      msg("dx is "+dx+" and dy is "+dy);
      backlink->set_x( this_object()->query_x() - dx );
      backlink->set_y( this_object()->query_y() - dy );
   }
}

object query_backlink() {
   if( !backlink ) check_backlink();
   return backlink;
}

void set_dir( string x ) {
   ::set_dir( x );
   check_backlink( 1 );
}

void set_link( string x ) {
   if( objectp(backlink) ) destruct(backlink);
   ::set_link( x );
   check_backlink( 1 );
}