int main(string arg) {
   string what, whom, god, result;
   object obj;

   if (!arg) {
      msg("What do you plan on offering to the gods?");
      return 1;
   }

   if (sscanf(arg,"%s to %s",what, whom) != 2) {
      what = arg;
      whom = ({})[random(0)];
   }

   obj = single_present( what, environment(this_player()) );
   if (!obj) {
      if ( single_present(what, this_player()) )
         msg("You must place your offering on the ground before it will be accepted.");
      else
         msg("I cannot find any such item '"+what+"' to sacrifice.");
      return 1;
   }
   if ( !obj->query_dead() && !obj->query_gettable() ) {
      msg("You cannot offer that.");
      return 1;
   }

   whom = lower_case(whom);
   if (!legal_filename("/faith/"+whom+".c") || file_size( "/faith/"+whom+".c") < 0) {
       msg("There is no such god '"+whom+"' to accept your offering.");
       return 1;
   }
   god = get_dir("/faith/"+whom+".c")[0];
   if (!god || whom == "prayer_daemon" ) {
      msg("There is no such god '"+whom+"' to accept your offering.");
      return 1;
   }
   whom = "/faith/prayer_daemon"->query_god_name(whom);

   set_actor(this_player());
   this_player()->msg_local("~CACT~Name ~verbpray to "+whom+".~CDEF");

   result = ("/faith/"+god)->on_offer(obj);
   if (result != "ok") {
      if (result == 0)
         msg(capitalize(whom)+" does not find " + obj->query_distant() + " to be an acceptable sacrifice.");
	  else {
         msg(capitalize(whom)+" tells you `"+result+"'");
         // attempts at making specifically disliked offerings result in a loss
         // of favor
         this_player()->add_favor(whom, -5);
      }
      return 1;
   }

   this_player()->add_favor(whom, 1);

   // offering has been accepted, dest that sucker
   set_target(obj);
   this_player()->msg_local("~[060~Targ is briefly engulfed in blue flames before disappearing completely.~CDEF");
   while( first_inventory(obj) )
      destruct( first_inventory(obj) );
   destruct(obj);

   return 1;
}
