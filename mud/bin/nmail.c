/**
 * This object compromises the verb for interacting with the new mail
 * system.
 * -- Al (Aug 14, 2003)
 *
 * Mail system is moving to a sql database in stead of the flat-file
 * system that was originally planned, ergo... more changes ;)
 */

#define MAIL_DAEMON ("/secure/mail/daemon")
#define MAIL_CLIENT ("/obj/mailclient.c")

int main(string arg) {
   object ob = clone_object(MAIL_CLIENT);
   if (!ob) {
      msg("~CWRNOops!~CDEF The mail client object is refusing to load "+
          "right now. Try again later, eh?");
      return 1;
   }

   if (arg) {
      ob->set_quit(1);
      ob->move(this_player());
      ob->do_command("m "+arg);
   } else
      ob->move(this_player());

   return 1;
}
