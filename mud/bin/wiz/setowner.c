#include <file_security.h>
#include <const.h>

// After some thought, I decided that the potential for this function
// to wreak havoc is very great, and thus only admins may use it.
// Be careful with it.

int main(string param) {
   string file, uid;
   int dir;

   ADM_CHECK

   if( getuid() != MASTER_UID ) {
      notify_fail("==> [Error] The setowner command does not work unless its UID is root\n");
      return 0;
   }

   if( !param || sscanf(param, "%s %s", file, uid) != 2  ) {
      notify_fail("==> [Format] setowner file uid\n");
      return 0;
   }


   file = this_player()->make_path_absolute(file);
   dir = 0;

   if( file_size(file) == -2 && file[<1] != '/' ) {
      file += "/";
      dir = 1;
   }
   else if( file_size(file) != -1);
   else if( file_size(file + ".c") != -1 ) file += ".c";
   else if( file_size(file + ".o") != -1 ) file += ".o";
   else {
      notify_fail("==> [Error] Cannot find "+file+"\n");
      return 0;
   }

   if( dir ) "/secure/file_security"->set_dir_owner(file, uid);
   else "/secure/file_security"->set_file_owner(file, uid);

   msg("==> [Setowner] "+file+" is now owned by "+uid);
   return 1;
}