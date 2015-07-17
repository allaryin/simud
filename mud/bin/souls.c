/*
 * Coded by : Durahan <durahan@airmail.net>
 * Date     : 2002-01-19
 * Filename : souls.c
 * Notes    : Give a list of all the current souls in, based on letter parameter /bin/soul
 *
 */

#include <const.h>

int main(string param) {
   string *files;
   string file_name;
   string cleanlist, a, b, errormsg;
   int len, i, mod;
   int modby, longest, j;

   if ( sizeof(param) <= 0 || member(param,'?')+1 || member(param,'*')+1 ) {
      files = get_dir( "/bin/soul/*.c" );
      len = sizeof( files );
      cleanlist = "~CBRT"+(string)len+" Current Souls~CDEF\n";
      msg( cleanlist );
      return 1;
   } else if ( param == "#" ) {
      files = get_dir("/bin/soul/0*.c")+
              get_dir("/bin/soul/1*.c")+
              get_dir("/bin/soul/2*.c")+
              get_dir("/bin/soul/3*.c")+
              get_dir("/bin/soul/4*.c")+
              get_dir("/bin/soul/5*.c")+
              get_dir("/bin/soul/6*.c")+
              get_dir("/bin/soul/7*.c")+
              get_dir("/bin/soul/8*.c")+
              get_dir("/bin/soul/9*.c");
      len = sizeof( files );

      if (len == 0) {
         errormsg = "~CWRNSorry there are currently no souls which start with the letters "+param+".~CDEF";
         msg( errormsg );
         return 1;
      }

      cleanlist = "~CBRT"+(string)len+" Current Souls starting with a number:~CLAB\n";
      longest = sizeof( files[0] );

      for ( i = 0; i < len; i++ ) {
         j = sizeof( files[i] );
         if ( j > longest ) longest = j;
      }

      if ( longest != 0 ) {
         longest++;
         modby = this_player()->query_term_width() / longest;
      }

      for( i = 0; i < len; i++ ) {
         file_name = files[i];
         if( sscanf(file_name, "%s.%s", a, b) == 2 ) {
            mod = (i+1) % modby;
            if (mod == 0) cleanlist += a+"\n";
            else cleanlist += pad(a,longest,0);
         }
      }
      cleanlist = cleanlist + "~CDEF";
      msg( cleanlist );
      return 1;
   } else {
      param = lower_case( param );

      files = get_dir( "/bin/soul/"+param+"*.c" );
      len = sizeof( files );
      if ( len == 0 ) {
         errormsg = "~CWRNSorry there are currently no souls which start with the letters "+param+".~CDEF";
         msg( errormsg );
         return 1;
      }

      cleanlist = "~CBRT"+(string)len+" Current Souls starting with "+param+":~CLAB\n";
      longest = sizeof( files[0] );

      for ( i = 0; i < len; i++ ) {
         j = sizeof( files[i] );
         if ( j > longest ) longest = j;
      }

      if ( longest != 0 ) {
         longest++;
         modby = this_player()->query_term_width() / longest;
      }

      for ( i = 0; i < len; i++ ) {
         file_name = files[i];
         if ( sscanf(file_name, "%s.%s", a, b) == 2 ) {
            mod = (i+1) % modby;
            if (mod == 0) cleanlist += a+"\n";
            else cleanlist += pad(a,longest,0);
         }
      }
      cleanlist = cleanlist+"~CDEF";
      msg( cleanlist );
      return 1;
   }
}
