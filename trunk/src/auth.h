#ifndef AUTH_H_
#define AUTH_H_

#include <gnutls/gnutls.h>
#include <gnutls/extra.h>  /* LZO Compression */

#include "net.h"

void net_connect_TLS();

#endif /* !AUTH_H_ */
