#include <file_security.h>
#include <const.h>

int main(string param) {
   string result;

   WIZ_CHECK

   if( getuid() != MASTER_UID ) {
      notify_fail("==> [Error] The owner verb does not work unless its UID is root\n");
      return 0;
   }

   if( !param ) param = ".";
   param = this_player()->make_path_absolute(param);
   if( file_size(param) == -2 ) {
      result = "/secure/file_security"->query_dir_owner(param+"/");
   }
   else if( file_size(param) != -1) {
      result = "/secure/file_security"->query_file_owner(param);
   }
   else if( file_size(param + ".c") != -1 ) {
      param += ".c";
      result = "/secure/file_security"->query_file_owner(param);
   }
   else if( file_size(param + ".o") != -1 ) {
      param += ".o";
      result = "/secure/file_security"->query_file_owner(param);
   }
   else {
      notify_fail("==> [Error] Cannot find file "+param+"\n");
      return 0;
   }

   msg("==> [Owner] "+param+" is owned by "+result);
   return 1;
}