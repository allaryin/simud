// displays output of a soul without the spammage of the whole help file
int main( string soul ) {
   if ( !soul ) {
      notify_fail( "Which soul do you want to test?\n" );
      return 0;
   }

   if ( file_exists("/bin/soul/" + soul + ".c") ) {
      string buf1, buf2, buf3;
      set_actor( this_player() );
      set_listener( this_player() );
      buf1 = "~CLABNo arguments:~CACT   " + ("/bin/soul/"+soul)->query_cself();
      buf2 = "~CLABTarget Felicia:~CACT " + ("/bin/soul/"+soul)->query_ctarget();
      buf3 = "~CLABTarget self:~CACT    " + ("/bin/soul/"+soul)->query_ctarget();
      if ( !buf1 || !buf2 || !buf3 ) {
         msg( "Something went wrong with this soul test." );
      } else {
         buf1 = explode(buf1, "@@")[0];
         buf1 = process_grammar( buf1 );
         set_target( load_object("/daemon/felicia") );
         buf2 = explode(buf2, "@@")[0];
         buf2 = process_grammar( buf2 );
         set_target( this_player() );
         buf3 = explode(buf3, "@@")[0];
         buf3 = process_grammar( buf3 );
         msg( buf1 + "\n" + buf2 + "\n" + buf3 + "~CDEF" );
      }
      return 1;
   } else {
      notify_fail( "No such soul exists.\n" );
      return 0;
   }
} // end main()
