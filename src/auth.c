#include "auth.h"

void net_connect_TLS()
{
	gnutls_global_init();
	//gnutls_check_version();

	gnutls_global_deinit();
}
