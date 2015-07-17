#include <magic.h>

nosave mapping spells;
mapping mana;	// big mother variable storage thing

mapping query_spells() { return copy(spells); }

void create() {
  string sub, spell;
  spells = ([]);
  foreach( sub : get_dir("/magic/spell/.") ) {
    foreach( spell : get_dir("/magic/spell/"+sub+"/.") ) {
      spell = "/magic/spell/"+sub+"/"+spell;
      if( spell[<2..] != ".c" )
        continue;
      spells[spell->query_name()] = spell;
    }
  }
}

string query_spell(string name) {
  return spells[name];
}

void set_planes( mapping x ) {
   if( mappingp(x) ) mana = x;
}

mapping query_planes() {
   return mana;
}

string query_plane_name(string plane) {
   if (plane == "wind")
     plane = ({"north","south","east","west"})[random(4)];
   return plane;
}

int query_default_mana_level(string plane) {
   switch (query_plane_name(plane)) {
      case "spirit":
         return MAGIC_DEFAULT_MANA_LEVEL * 2;
      case "lightning":
      case "chaos":
      case "order":
         return MAGIC_DEFAULT_MANA_LEVEL / 2;
      case "water":
         return MAGIC_DEFAULT_MANA_LEVEL / 4;
      case "fire":
      case "ice":
      case "earth":
      case "dark":
      case "north":
      case "south":
      case "east":
      case "west":
      default:
         return MAGIC_DEFAULT_MANA_LEVEL;
   }
   return MAGIC_DEFAULT_MANA_LEVEL;
}

/** This function returns either the int value stored in an individual
 * entry in the mapping... or it returns the entire mapping.
 */
varargs mixed query_mana(string plane) {
	if (!restore_object(MAGIC_DAEMON)) {
		debug("Mana daemon failed to load state, regenerating...");
		mana = (["fire": query_default_mana_level("fire"),
		         "ice": query_default_mana_level("ice"),
		         "earth": query_default_mana_level("earth"),
		         "spirit": query_default_mana_level("spirit"),
                 "dark": query_default_mana_level("dark"),
		         "north": query_default_mana_level("north"),
		         "south": query_default_mana_level("south"),
		         "east": query_default_mana_level("east"),
		         "west": query_default_mana_level("west"),
		         "water": query_default_mana_level("water"),
		         "lightning": query_default_mana_level("lightning"),
		         "order": query_default_mana_level("order"),
		         "chaos": query_default_mana_level("chaos")]);
		save_object(MAGIC_DAEMON);
	}
	if (plane) {
		//debug("query_mana: "+plane);
		return mana[query_plane_name(plane)];
	}
	return mana;
}

/** Returns an array containing all planes opposed to the plane in
 * question. In most cases, this is only one entry, but shrug :P
 */
string *query_opposing_plane(string plane) {
	switch (query_plane_name(plane)) {
		case "fire": return ({"ice"});
		case "ice": return ({"fire"});
		case "earth": return ({"spirit","spirit","dark"});
		case "spirit": return ({"earth","earth","earth","dark"});
		case "north": return ({"south","south","east","west"});
		case "south": return ({"north","north","east","west"});
		case "east": return ({"west","west","north","south"});
		case "west": return ({"east","east","north","south"});
		case "water": return ({"lightning"});
		case "lightning": return ({"water"});
		case "order": return ({"chaos"});
		case "chaos": return ({"order"});
        case "dark": return ({"spirit","spirit","earth"});
		default: return 0;
	}
	return 0;
}

/** Returns the personal mana cost to the spellcaster when drawing
 * \a amt mana from the \a plane.
 */
int query_cost(string plane, int amt) {
	float available, cost;
	plane = query_plane_name(plane);
    available = to_float(query_mana(plane));
	if( available == 0 ) {
		//debug(plane+" plane is _empty_");
		return 0;
	}
    cost = amt / to_float(available / query_default_mana_level(plane));
	//debug("amt = "+amt+", avail = "+available+", cost = "+cost);
	return (int)cost;
}

/** Returns the mana cost of a particular \a spell.
 */
int query_spell_cost(string spell) {
	if (!query_spell(spell))
		return -1;
	else {
		spell = query_spell(spell);
		return query_cost(spell->query_type(), spell->query_cost());
	}
}

/** Actually draw mana from a particular plane.
 * \param who The object doing the drawing.
 * \param plane The plane from which to draw the energy.
 * \param amt The amount of mana to be drawn.
 */
void draw_mana(object who, string plane, int amt) {
	plane = query_plane_name(plane);
	who->add_mana(-query_cost(plane,amt));
	mana[plane] -= amt;
	save_object(MAGIC_DAEMON);
	call_out("absorb_mana",MAGIC_ABSORB_DELAY,plane,amt);
}

/** Reincorporate mana previously drawn by adding it to the opposing
 * plane(s). In the case of planes that have multiple opponants, the
 * amount is shared evenly.
 */
void absorb_mana(string plane, int amt) {
	string *list = query_opposing_plane(plane);
	string other;
	if (!list)
		return;
	foreach (other : list)
		mana[other] += amt/sizeof(list);
	save_object(MAGIC_DAEMON);
}

/** Returns the string equivalent of a mana type classification
 */
string query_mana_string(int c) {
	switch( c ) {
		case C_FIRE_MANA:	return "fire";
		case C_ICE_MANA:	return "ice";
		case C_EARTH_MANA:	return "earth";
		case C_SPIRIT_MANA:	return "spirit";
		case C_DARK_MANA:	return "dark";
		case C_WATER_MANA:	return "water";
		case C_LIGHTNING_MANA:	return "lightning";
		case C_WIND_MANA:	return "wind";
		case C_ORDER_MANA:	return "order";
		case C_CHAOS_MANA:	return "chaos";
	}
	return "unknown";
}

/** Reverse of query_mana_string(), returns the int when given the string.
 */
int query_mana_aspect(string str) {
	switch( query_plane_name(str) ) {
		case "fire":	return C_FIRE_MANA;
		case "ice":		return C_ICE_MANA;
		case "earth":	return C_EARTH_MANA;
		case "spirit":	return C_SPIRIT_MANA;
		case "dark":	return C_DARK_MANA;
		case "water":	return C_WATER_MANA;
		case "lightning":	return C_LIGHTNING_MANA;
		case "north": case "south": case "east": case "west":
		case "wind":	return C_WIND_MANA;
		case "order":	return C_ORDER_MANA;
		case "chaos":	return C_CHAOS_MANA;
	}
	return -1;
}

/** Silly little function for use when iterating over all mana types
 */
string *query_mana_types() {
	return ({ "chaos", "dark", "earth", "lightning", "fire",
				"ice", "order", "spirit", "water", "wind" });
}
