inherit "/obj/book.c";

TASTE( "It tastes like knowledge." )

void create() {
  ::create();

  set_distant("a book");
  set_specific("the book");
  set_plural("books");
  set_name("book");
  set_gettable(1);
  set_droppable(1);
  set_weight(500);
  set_bulk(1);
  set_value(100);

  // there's no way for a player to know what locations a
  // book or piece of paper or whatever has unless you
  // tell them in the description. don't forget to do this
  set_look("A rather plain book, with a spine and cover.");

  set_pages(8);
  add_location("cover");
  set_location("cover", "A Treatsie on Entomology");
  add_location("spine");
  set_location("spine", "Entomology");
  //add_location("back");
}
