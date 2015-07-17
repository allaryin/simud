/* variables */

mapping player_stages;
mapping player_substages;

/* stage 0 */

string *skin_colors = ({
	"pale", "white", "tan", "deep tan", "brown",
	"copper", "olive", "ebony", "fair"
});

string *eye_colors = ({
	"blue", "brown", "green", "black", "gray", "hazel",
	"pink", "light brown"
});

string *hair_colors = ({
	"brown", "blonde", "red", "white",
	"platinum", "black", "orange", "gray", "indigo"
});

string *hair_styles = ({
	"straight", "wavy", "curly", "slicked back", "spiked",
	"puffy", "afro", "bowl-cut", "ponytail", "braided"
});

string *hair_lengths = ({ "short", "medium", "long" });

string *stages = ({
	"skin_color",
	"eye_color",
	"hair_color",
	"hair_length",
	"hair_style"
});

string *prompts = ({
	"Desired skin color: ",
	"Desired eye color: ",
	"Desired hair color: ",
	"Desired hair length: ",
	"Desired hair style: "
});

mixed *choices = ({ skin_colors, eye_colors, hair_colors, hair_lengths, hair_styles });

/* general */

static void create()
{
	player_stages = ([ ]);
	player_substages = ([ ]);
}

int query_stage() {
	return player_stages[this_player()->query_name()];
}

void set_stage(int new_stage)
{
	if (new_stage) {
		player_stages[this_player()->query_name()] = new_stage;
	} else {
		m_delete(player_stages, this_player()->query_name());
	}
}

int query_substage() {
	return player_substages[this_player()->query_name()];
}

void set_substage(int new_substage)
{
	if (new_substage) {
		player_substages[this_player()->query_name()] = new_substage;
	} else {
		m_delete(player_substages, this_player()->query_name());
	}
}

void help()
{
	switch(query_stage()) {
	case 0:
		switch(query_substage()) {
		case 0: msg("Welcome to character generation.  First up, deciding your skin color."); break;
		case 1: msg("Second, your eye color."); break;
		case 2: msg("Third, your hair color."); break;
		case 3: msg("Fourth, your hair length."); break;
		case 4: msg("Fifth, your hair style."); break;
		}
		break;
	case 1:
		msg("Finally, a single adjective to describe yourself with."); break;
	}
}

void set_desc_element(string key, string param)
{
	mapping desc = this_player()->query_env_var("desc");

	if (!desc) {
		desc = ([ ]);
	}

	desc[key] = param;

	this_player()->set_env_var("desc", desc);
}

void complete_generation() {
	clone_object("/items/weapon/hatchet")->move(this_player());
	clone_object("/items/weapon/knife")->move(this_player());
	this_player()->set_creation_flag(0);
	this_player()->move_player(";ch;oldtown;temple;temple2");
}

void activate()
{
	msg("Character generation");
	msg("--------------------\n");
	set_stage(0);
	help();
	msg("Valid choices are: ");
	msg(implode(choices[query_substage()], ", "));
	add_interface( "config", prompts[0] );
}

void config(string params)
{
	if (!params || params == "") {
		return;
	}
	
	switch(query_stage()) {
	case 0: /* eye, hair, and skin */
		if (member(choices[query_substage()], params) != -1) {
			msg(params + " it is!");
			set_desc_element(stages[query_substage()], params);
			
			if (query_substage() == 4) {
				set_substage(0);
				set_stage(1);
				help();
				shift_interface( "config", "Please choose an adjective: " );
			} else {
				set_substage(query_substage() + 1);
				help();
				msg("Valid choices are: ");
				msg(implode(choices[query_substage()], ", "));
				shift_interface( "config", prompts[query_substage()] );
			}
		} else {
			msg("Sorry, but that is not a valid choice.");
			msg("Valid choices are: ");
			msg(implode(choices[query_substage()], ", "));
		}
		return;
	case 1: /* adjective */
		set_desc_element("adj", params);
		
		msg("Welcome to Walraven.");
		
		set_stage(0);
		set_substage(0);
		
		if (this_player()->query_creation_flag()) {
			complete_generation();
		}
		
		drop_interface();
	}
}
