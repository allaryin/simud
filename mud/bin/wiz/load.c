int main(string obj) {

   WIZ_CHECK

   if(!obj || obj == "") {
      notify_fail("==> [Format] load <object>\n");
      return 0;
   }

   if( obj[<2..<1] == ".c" ) {
      notify_fail("==> [Error] The load command takes object references ONLY. (leave off the .c)\n");
      return 0;
   }

   obj = this_player()->make_path_absolute(obj);
   if( file_size(obj) == -2 ) {
      string f;

      // Load a directory
      if( obj[<1] != '/' ) obj += "/";
      msg( "Loading directory " + obj );
      foreach( f : get_dir(obj + "*.c") )
         load_object( obj + f );
      return 1;
   }
   if( !obj || !file_exists(obj + ".c") ) {
      notify_fail("==> [Error] File error/Access denied\n");
      return 0;
   }
   else {
      object ob;
      ob = load_object(obj);
      msg("==> [Notice] Object loaded");

      if( function_exists("query___FLIST", ob) && !pointerp(ob->query___FLIST()) )
         msg("~CWRNWarning: No __FLIST, but was expected. Did you forget to call ::create()?~CDEF");
   }

   return 1;
}
