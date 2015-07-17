#define WIZPOINT_START  1061371633
#define SECS_PER_WEEK       604800

int less_than( string name1, string name2, mapping weekpoints, mapping points ) {
   int sc1, sc2;
   sc1 = sizeof(weekpoints[name1]) * 10 + points[name1];
   sc2 = sizeof(weekpoints[name2]) * 10 + points[name2];
   if( sc1 == sc2 ) {
      if( sizeof(weekpoints[name1]) == 0 || sizeof(weekpoints[name2]) == 0 )
         return sizeof(weekpoints[name1]) < sizeof(weekpoints[name2]);
      return weekpoints[name1][<1] < weekpoints[name2][<1];
   }
   return sc1 < sc2;
}

int extra_levels( string wizname, int points ) {
   int base, levels, scale, extra;
   int level = "/secure/master_security.c"->query_player_level(wizname);

   scale = level / 100 + 4;

   return points / scale + base - level;
}

int main( string param ) {
   mapping points, weekpoints, birthdays;
   string *wiznames;
   string buf;
   int i, lazy, weekssince, weekswiz, wizdate, extra_levels;

   points = "/daemon/point"->query_points();
   weekpoints = "/daemon/point"->query_weekpoints();
   weekssince = (time() - WIZPOINT_START) / SECS_PER_WEEK;
   wiznames = sort_array( m_indices(points), (: less_than($1,$2,weekpoints,points) :) );

   buf = "\n~CBRT--==** ~CTITWalraven Wizard Scores ~CBRT**==--\n\n~CBRTRank  Name             Total   Week   RankSc  Levels\n";
   for( i = 0; i < sizeof(wiznames); i++ ) {
      wizdate = "/secure/master_security.c"->query_wiz_creation_date( wiznames[i] );
      weekswiz = (time() - wizdate) / SECS_PER_WEEK;

      if ( wizdate < WIZPOINT_START )
         lazy = ( points[wiznames[i]] < weekssince );
      else
         lazy = ( points[wiznames[i]] < weekswiz );

      buf += "~CBRT" + pad(""+(i+1), 3, JUST_RIGHT) + "   ~CLAB" + pad(capitalize(wiznames[i]), 15, JUST_LEFT) + "  ~CDEF";
      if ( lazy ) buf += "~CWRN";
      buf += pad(""+points[wiznames[i]], 5, JUST_RIGHT) + "  ~CDEF";
      buf += pad(""+sizeof(weekpoints[wiznames[i]]), 5, JUST_RIGHT) + "  ";
      buf += pad(""+(sizeof(weekpoints[wiznames[i]])*10 + points[wiznames[i]]), 7, JUST_RIGHT);
      extra_levels = extra_levels(wiznames[i], points[wiznames[i]]);
      if ( !extra_levels ) buf += "~CDRK"; else buf += "~CBRT";
      buf += pad(extra_levels, 8, JUST_RIGHT) + "\n";
   }
   buf += "~CTIT\nWeeks since point system started: ~CBRT" + weekssince + "~CDEF\n";

   msg(buf);

   return 1;
}
