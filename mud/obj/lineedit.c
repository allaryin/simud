#include <const.h>
#include <object.h>

static nomask varargs void get_multi(string str);
static void edit_help(string what);

static int *parse_lines(string str);

private static status in_use;

static string *input;
static int state, cur;
static object ret_object;
static string ret_function;

#define NOT_EDITING -1
#define INSERT 0
#define OVERWRITE 1

varargs mixed * get_line_numbers(string str, int single) {
    mixed * lines;

    if (0 == (lines = parse_lines(str))) {
        write("Not a valid line selection\n");
        return 0;
    }
    if (single && (sizeof(lines)!=1)) {
        write("Multiple line selection is not possible with this command.\n");
        return 0;
    }
    if ((lines[0] < 1) || (lines[0] > sizeof(input))) {
        write("Invalid line number:  "+lines[0]+"\n");
        return 0;
    }
    if (!single) {
        int n;

        n = sizeof(lines);
        if ((n>1) && ((lines[n-1] < 1) || (lines[n-1] > sizeof(input)))) {
            write("Invalid line number: "+lines[n-1]+"\n");
            return 0;
        }
    }
    return lines;
}

static nomask varargs void get_multi(string str) {
    mixed *lines;
    int i, n, m;
    string tmp;

    if (!str) {
        state = INSERT;
        msg("~CCOM/h~CDEF for help. ~CCOM/s~CDEF, ~CCOM**~CDEF, or ~CCOM.~CDEF when done. ~CCOM/q~CDEF or ~CCOM~TILDq~CDEF to abort.\n");
    }
    else if (str) {
        if (str == "~q")
            str = "/q";
        if (str == "**" || str == ".")
            str = "/s";
        if ((strlen(str) > 1) && (str[0]=='/')) {
            switch(str[1]) {
            case '?':
                if (!ret_object || !function_exists("help", ret_object)) {
                    write("There is no local help available.\n");
                }
                else {
                    tmp = str[2..];
                    if (tmp && strlen(tmp) && tmp[0]==' ')
                        tmp = tmp[1..];
                    ret_object->help(tmp);
                }
                break;
            case 'l': /* list */
                write("Current input:\n");
                for(i=0,n=sizeof(input);i<n;i++) {
                    tmp=(pad(""+(i+1)+"", 2, JUST_LEFT)+":");
                    if (i==cur)
                        tmp+=("*");
                    else
                        tmp+=(" ");
                    tmp+=(input[i]+"\n");
                    write(tmp);
                }
                break;
            case 'd': /* delete a line */
                if (!input) {
                    write("Nothing to delete.\n");
                    break;
                }
                lines = parse_lines(str[2..]);
                if (!lines) {
                    write("Not a valid line selection\n");
                    break;
                }
                if ((lines[0] < 1) || (lines[0] > sizeof(input))) {
                    write("Invalid line number:  "+lines[0]+"\n");
                    break;
                }
                n = sizeof(lines);
                if ((n>1) && ((lines[n-1] < 1) || (lines[n-1] > sizeof(input)))) {
                    write("Invalid line number: "+lines[n-1]+"\n");
                    break;
                }
                if (cur < sizeof(input))
                    write("Inputing at end\n");
                for(i=0;i<n;i++) {
                    input[lines[i]-1] = 0;
                }
                input = filter(input, lambda( ({ 'x }), ({ #'return, 'x }) ) );
                cur = sizeof(input);
                break;
            case 'a': /* alter a line */
                if (!input) {
                    write("Nothing to change.\n");
                    break;
                }
                lines = parse_lines(str[2..]);
                if (!lines) {
                    write("Not a valid line selection\n");
                    break;
                }
                if (sizeof(lines)!=1) {
                    write("Multiple line change is not possible\n");
                    break;
                }
                n = lines[0];
                if (n < 1 || n > sizeof(input)) {
                    write("No such line: "+n+".\n");
                    break;
                }
                cur = n - 1;
                write("Line reads:\n"+input[cur]+"\n");
                state = OVERWRITE;
                break;
            case 'c': /* copy */
                write("'/c' is now copy.  Use '/a' (alter) to change a line\n");
                if (!input) {
                    write("Nothing to change.\n");
                    break;
                }
                if (0 == (lines = get_line_numbers(str[2..]))) {
                    break;
                }
                for(i=0;i<sizeof(lines);i++) {
                    lines[i] = input[lines[i]-1];
                }
                this_player()->remember("copy_buffer", lines);
                write("lines copied\n");
                break;
            case 'p': /* paste */
                if (0 == (lines = get_line_numbers(str[2..], 1))) {
                    break;
                }
                n = lines[0];
                lines = this_player()->recall("copy_buffer");
                if (!n) {
                    input = lines + input;
                } else if (n < sizeof(input)) {
                    input = input[..n-1] + lines + input[n..];
                } else {
                    input = input + lines;
                }
                cur = n + sizeof(lines);
                write("Lines pasted\n");
                if (cur == sizeof(input)) {
                    write("Inputing at end.\n");
                }
                break;
            case 'i':
                if (strlen(str)>2) {
                    lines = parse_lines(str[2..]);
                    if (!lines) {
                        write("Not a valid line selection\n");
                        break;
                    }
                    if (sizeof(lines)!=1) {
                        write("Inputing at multiple lines is not possible\n");
                        break;
                    }
                    n = lines[0];
                    if ((n < 1) || (n > sizeof(input))) {
                        write("Invalid input line\n");
                        break;
                    }
                    write("Inputing before line "+n+"\n");
                    cur = n - 1;
                    break;
                }
                write("Inputing at end\n");
                cur = sizeof(input);
                break;
            case 'h': /* help */
                edit_help("editor");
                break;
            case 'q': /* abort */
                input = 0;
            case 's': /* save */
                in_use = 0;
                this_player()->forget("copy_buffer");
                call_other(ret_object, ret_function, input);
                return;
            default:
                write("Unknown editor command.\n|");
            }
        }
        else {
            switch(state) {
            case INSERT:
                if (!input)
                    input = ({ });
                if (cur < sizeof(input)) {
                    if (!cur) {
                        input = ({ str }) + input;
                    }
                    else {
                        input = input[0..cur-1]+ ({ str }) + input[cur..];
                    }
                }
                else {
                    input += ({ str });
                }
                cur++;
                break;
            case OVERWRITE:
                input[cur] = str;
                state = INSERT;
                break;
            }
        }
    }
    write("|");
    input_to("get_multi");
}

static void edit_help(string what) {
    if (what == "editor") {
        write("Commands available:\n"+
              "/q             abort.\n"+
              "/s             save.\n"+
              "/h             help (this).\n"+
              "/? [<string>]  local help. (May not exist)\n"+
              "/l             list buffer.\n"+
              "/i [n]         input at line n, default is end of the text\n"+
              "/d [n]         delete line n, default is last line\n"+
              "/d [m-n]       deletes from line m to line n\n"+
              "/a [n]         alters a line, default is last line\n"+
              "               this will change ONE line of text\n"+
              "               into whatever you type until you type\n"+
              "               /i.\n"+
              "/c [m-n]       copy lines m through n, default is last line\n"+
              "/p [n]         paste copied lines at n, default is last line\n"
             );
        return;
    }
    write("No help on that available sorry.\n");
}

varargs nomask int edit_buffer(mixed blah, mixed ob, string func) {
    if (!ob)
        ret_object = previous_object();
    else
        ret_object = ob;
    if (!func)
        ret_function = "end_of_input";
    else
        ret_function = func;
    if (blah) {
        if (pointerp(blah))
            input = blah;
        else {
            /* possible read file, huh */
            input = explode(blah, "\n");
        }
    }
    else
        input = ({ });
    cur = sizeof(input);
    in_use = 1;
    get_multi();
    return 1;
}

nomask status query_editing() {
    return in_use;
}

void recover() {
    get_multi();
}

static int *recognise(string str) {
    int *lines;
    int i;
    mixed m, n;

    if (sscanf(str, "%s-%s", m, n)==2) {
        if ((sscanf(m, "%d", m)!=1)||(sscanf(n, "%d", n)!=1))
            return 0;
        lines = ({});
        if (n<m) {
            i = m;
            m = n;
            n = i;
        }
        for(i=m;i<=n;i++) {
            lines += ({ i });
        }
        return lines;
    }
    if (sscanf(str, "%d", n)==1)
        return ({ n });
    return 0;
}

static int *parse_lines(string str) {
    mixed *sections;
    int *lines;
    int i;

    if (!str)
        return 0;
    if (strlen(str)==0)
        return ({ cur });
    lines = ({});
    sections = explode(str, ",");
    for(i=0;i<sizeof(sections);i++) {
        int *tmp;
        tmp = recognise(sections[i]);
        if (tmp)
            lines += tmp;
        else
            return 0;
    }
    lines = sort_array(lines, #'>);
    return lines;
}
