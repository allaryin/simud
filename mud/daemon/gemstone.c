/**
 * This object knows distribution chances of various sorts of gems when
 * digging different kinds of ore.
 *
 * Quartz - SiO2, regular rock
 *    Agate, Amethyst, Citrine
 * Beryl - Be3Al2(SiO3)6, so aluminum veins
 *    Aquamarine, Emerald
 * Diamond - C, so coal
 * Garnet - Fe3Al2(SiO4)3, so iron and aluminum
 * Jade - Ca2(Mg, Fe)5Si8O22(OH)2, so iron
 * Peridot - (Mg,Fe)2SiO4, so iron
 * Opal - SiO2�nH2O, regular rock
 * Corundum - Al2O3, aluminum
 *    Ruby, Sapphire
 * Topaz - (AlF)2SiO4, so aluminum
 * Turquoise - CuAl6(PO4)4(OH)8-5H2O, so copper and aluminum
 */

int handicap;

void increase_handicap() {
   if( handicap >= 60 ) {
      handicap = 0;
	  debug("mining handicap has been reset to ZERO");
   } else {
      handicap++;
      debug("mining handicap has increased to "+handicap);
   }
}

string query_gem_name( string orename ) {
   int chance = random(100 + handicap);
   if( this_player() ) {
      chance -= this_player()->query_skill("craft.mining") / 25;
      this_player()->practice_skill("craft.mining");
   }
   if( chance < 10 )
      increase_handicap();
   switch( orename ) {
      case "iron ore":
         if( chance < 3 )
            return "jade";
         if( chance-3 < 2 )
            return "peridot";
         if( chance-5 < 2 )
            return "garnet";
         break;
      case "copper ore":
         if( chance < 5 )
            return "turquoise";
         break;
      case "aluminum ore":
         if( chance < 2 )
            return "aquamarine";
         if( chance-2 < 1 )
            return "emerald";
         if( chance-3 < 2 )
            return "garnet";
         if( chance-5 < 1 )
            return "ruby";
         if( chance-6 < 1 )
            return "sapphire";
         if( chance-7 < 3 )
            return "topaz";
         if( chance-10 < 5 )
            return "turquoise";
         break;
      case "coal":
         if( chance < 1 )
            return ({"diamond",0})[random(2)];
         break;
      case "stone":
      case "limestone":
	  case "granite":
         if( chance < 5 )
            return ({"agate","amethyst","citrine"})[random(3)];
         if( chance-5 < 1 )
            return "opal";
         break;
      // these do not follow with science, but i don't care
	  case "gold ore":
		if( chance < 4 )
			return "garnet";
	  	if( chance-4 < 3 )
			return "emerald";
		break;
	  case "lead ore":
	  	if( chance < 3 )
			return "topaz";
	  	if( chance-3 < 3 )
			return "agate";
		break;
	  case "platinum ore":
	  	if( chance < 3 )
			return "sapphire";
	  	if( chance-3 < 3 )
			return "ruby";
		break;
	  case "tin ore":
	  	if( chance < 3 )
			return "citrine";
	  	break;
   }
   // always give a small chance of digging quartz crystals from anywhere
   if( chance > 95 && chance < 99 )
      return "small_quartz";
   if( chance == 99 )
      return "large_quartz";
   return 0;
}
