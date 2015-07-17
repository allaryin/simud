#include <functionlist.h>
#include <wizard.h>
#include <sys/lpctypes.h>

/// Executes command
main(string what) {
  object ob;
  mixed *flist;
  string out, final;
  int options;
  int i, pointer;

  if (!what) {
    write(help());
    return 1;
  }
  if (sscanf(what, "-%s %s", out, what)) {
    if (member(out, 'h')!=-1)
      options|=NAME_HIDDEN;
    if (member(out, 'i')!=-1)
      options|=NAME_INHERITED;
    if (member(out, 's')!=-1)
      options|=TYPE_MOD_STATIC;
    if (member(out, 'a')!=-1)
      options=~0;
  }
  if ((ob = find_thing(what))==0) {
    notify_fail("Can't find "+what+"\n");
    return 0;
  }
  printf("%O\n", ob);
  flist = functionlist(ob, 1|2|4|8);
  final = "";
  for(i=0;i<sizeof(flist);i+=4) {
    out = "";
    if (flist[i+1]&NAME_UNDEFINED) {
      out+="[undefined] ";
    }
    if (flist[i+1]&NAME_INHERITED) {
      if (!(options&NAME_INHERITED))
        continue;
      out+="inherited ";
    }
    if (flist[i+1]&NAME_HIDDEN) {
      if (!(options&NAME_HIDDEN))
        continue;
      out+="hidden ";
    }
    if (flist[i+1]&TYPE_MOD_PRIVATE) {
      out+="private ";
    }
    if (flist[i+1]&TYPE_MOD_PUBLIC) {
      out+="public ";
    }
    if (flist[i+1]&TYPE_MOD_STATIC) {
      if (!(options&TYPE_MOD_STATIC))
        continue;
      out+="static ";
    }
    if (flist[i+1]&TYPE_MOD_NO_MASK) {
      out+="nomask ";
    }
    if (flist[i+1]&TYPE_MOD_VARARGS) {
      out+="varargs ";
    }
    if (flist[i+1]&TYPE_MOD_PROTECTED) {
      out+="protected ";
    }
    if ((pointer = flist[i+2]&TYPE_MOD_POINTER)!=0)
      flist[i+2]-=TYPE_MOD_POINTER;
    switch(flist[i+2]) {
      case TYPE_UNKNOWN: break;
      case TYPE_NUMBER: out+="int "; break;
      case TYPE_STRING: out+="string "; break;
      case TYPE_VOID: out+="void "; break;
      case TYPE_OBJECT: out+="object "; break;
      case TYPE_MAPPING: out+="mapping "; break;
      case TYPE_FLOAT: out+="float "; break;
      case TYPE_ANY: out+="mixed "; break;
//      case TYPE_SPACE: out+="space "; break;
      case TYPE_CLOSURE: out+="closure "; break;
      case TYPE_SYMBOL: out+="symbol "; break;
      case TYPE_QUOTED_ARRAY: out+="'array' "; break;
//      case TYPE_TERM: out+="term "; break;
    }
    if (pointer)
      out+="*";
    out+=flist[i]+"("+flist[i+3]+" arg"+(flist[i+3]==1?"":"s")+")";
    final += out+"\n";
  }
  // more(final);
  write(final);
  return 1;
}

string help() {
  string out;

  out = ("Usage: flist [-hisa] <object_identifier>\n\n"+
  "The flist (short for function list) command is used to display a list of\n"+
  "functions defined by an object.  The options are as follows:\n"+
  "\th\tshow hidden functions.\n"+
  "\ti\tshow inherited functions.\n"+
  "\ts\tshow static functions.\n"+
  "\ta\tshow all functions.\n");
  return out;
}