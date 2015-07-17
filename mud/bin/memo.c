/** Allaryin's Memo system (1.10.02)
 *
 *  This command allows players a very simple interface for keeping a few
 *  sheets of notes handy so they have access to dirs and stuff, even if
 *  mudding during a meeting or from class, or whatever...
 *
 *  All of the real work is done by the verb. The only invasion I made
 *  into the player object was to put the variable (and accompanying query
 *  and set functions) there.
 *
 *  Update: 5/5/2003 - It has been realized after all this time that
 *  things were kind of broken. Multiple people working on memos at the
 *  same time was pretty much... disasterous. Thus, I have broken things
 *  up a bit in the model of every other system of this sort. There is now
 *  a memo client object that does all of the work.
 */

int main(string str) {
  object ob = clone_object("/obj/memoclient.c");
  ob->domemo(this_player(),str);
  return 1;
}
