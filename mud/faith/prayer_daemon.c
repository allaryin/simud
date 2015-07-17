string query_god_name(string god) {
   god = lower_case(god);
   if (!file_exists("/faith/"+god+".c") || god == "prayer_daemon")
      return 0;
   return capitalize(god);
}

void on_offer(string deity, object offering) {
}

void on_prayer(string deity, string subject) {
   string *deities = ({}), *tmp;
   int d;

   deity = lower_case(deity);
   // annoyance fix by gpfault

   if (deity == "everyone")
      deities = ({ "example" });
   else {
      deity = implode(explode(deity, ","),"and ");
      deities = explode(deity, "and");
   }

   tmp = copy(deities);
   deities = ({});
   foreach(deity: tmp) {
      if (!deity || deity == "")
         continue;
      deities += ({ lower_case(trim(deity)) });
   }

   call_out("submit_prayer", 4, deities[random(sizeof(deities))], subject);
}

int is_punctuation(int char) {
   return char == '.' || char == '!' || char == '?';
}

int submit_prayer(string deity, string subject) {
   string answer;
   debug("deity = "+deity+", subj = "+subject,"magic");
   if( file_exists("/faith/"+deity+".c") )
      if (subject) {
         subject = lower_case(subject);
      }
      answer = ("/faith/"+deity)->on_prayer(subject);
   if( answer != 0 ) {
      this_player()->msg_local("~CBRT"+("/faith/prayer_daemon"->query_god_name(deity))+" says, `"+answer+(is_punctuation(answer[<1])?"":".")+"'~CDEF");
      return 1;
   } else {
      msg_object(this_player(),"~CDRKYour prayer goes unheard.~CDEF");
      return 0;
   }
}
