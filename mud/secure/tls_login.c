inherit "/secure/login";

void normal_logon(int result, object who, int quiet) {
	mixed *details;
	debug("normal_logon("+result+") called from "+query_ip_number(who),"tls");
	if( result > 0 ) {
		debug("Still negotiating...","tls");
		tls_deinit_connection(who);
	} else if( result < 0 ) {
		debug("Error: "+tls_error(result),"tls");
		tls_deinit_connection(who);
	} else {
		debug("Connection negotiated","tls");
	}
	details = tls_query_connection_info( who );
	if( pointerp(details) )
		write("You are logging in securely.\n");
	else
		write("You are logging in INSECURELY.\n");
	debug( as_lpc( details ),"tls" );
	::logon( quiet );
}

static int logon( int quiet ) {
	debug("Connection attempt from "+query_ip_number(this_object()),"tls");
	int x = tls_init_connection(this_object(), #'normal_logon, quiet);
	debug("tls_init_connection returned "+x,"tls");	
	return 1;
}
