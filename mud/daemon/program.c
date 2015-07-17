mapping programs;

static void create()
{
   programs = "/secure/master"->query_hold_var();

   if (!mappingp(programs)) {
      programs = ([ ]);
   }
}

string destructor(object ob)
{
   "/secure/master"->set_hold_var(programs);
}

string *query_programs()
{
   return m_indices(programs);
}

void on_load(string file)
{
   programs += ([ file ]);

   debug("Program list now has " + sizeof(programs), "gp");
}

void on_destruct(string file)
{
   m_delete(programs, file);
}

string *query_inheriters(string file)
{
   return filter(m_indices(programs), (: member(inherit_list(find_object($1)), file) != -1 :) );
}

string *query_includers(string file)
{
   return filter(m_indices(programs), (: member(include_list(find_object($1)), file) != -1 :) );
}