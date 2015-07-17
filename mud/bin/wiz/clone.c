#include <const.h>
#include <wizard.h>

string my_file_name;
int do_not_differ;
int my_copy_num;

main(param) {
   object scratchob, areaob;
   string dest;
   int is_temp;

   WIZ_CHECK

   if(!param || param == "") {
      notify_fail("==> [Format] clone object [destination]\n==> [Notice] object may be either a source file (.c) or an object file (.o)\n");
      return 0;
   }

   sscanf(param, "%s %s", param, dest);
   if(dest) areaob = find_thing(dest);
   else areaob = environment(this_player());
   if(!areaob) areaob = this_player();

   param = this_player()->make_path_absolute(param);
   if( file_size(param + ".o") > 0 ) is_temp = 1;
   else is_temp = 0;
   if( !is_temp && !file_exists(param) && !file_exists(param+".c") ) {
      notify_fail("==> [Error] I don't understand what you're trying to clone (wrong file name?).\n");
      return 0;
   }

   if( is_temp ) {
      my_file_name = 0;
      // A rather unusual hack to retrieve the filename for the template.
      restore_object(param);
      if( !my_file_name ) {
         msg("==> [Error] That object file does not contain a valid my_file_name field, so can't be used as a template.");
         return 1;
      }

      scratchob = clone_object(my_file_name);
      scratchob->load_template(param);
   }
   else {
      if( param[<2..<1] == ".c" ) param = param[..<3];
      scratchob = clone_object( param );
   }

   if( scratchob->on_get() & MOVE_OK_FLAG ) areaob = this_player();
   scratchob->move( areaob );
   if( !environment(scratchob) ) {
      msg("==> [Error] Object would not move. Perhaps its move() function does not work. I destructed it.");
      destruct(scratchob);
      return 1;
   }
   scratchob->validate_position();

   scratchob->save_self();
   set_target( scratchob );
   if( scratchob->query_distant() )
      this_player()->msg_local("~CBRT~Name ~verbconjure up ~atar from thin air.~CDEF");
   else
      this_player()->msg_local("~CBRT~Name ~verbcreate an invisible lump of matter.~CDEF");

   if( function_exists("query___FLIST", scratchob) && !pointerp(scratchob->query___FLIST()) )
      msg("~CWRNWarning: No __FLIST, but was expected. Did you forget to call ::create()?~CDEF");

   return 1;
}
