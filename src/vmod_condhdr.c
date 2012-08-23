#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"

#include "vcc_if.h"

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
	return (0);
}

/*
 * This mutex is used to avoid having two threads that initializes the same
 * regex at the same time. While it means that there's a single, global
 * lock for all libvmod-header actions dealing with regular expressions,
 * the contention only applies on the first request that calls that
 * specific function.
 */
pthread_mutex_t header_mutex;

/*
 * Initialize the regex *s on priv, if it hasn't already been done.
 * XXX: We have to recheck the condition after grabbing the lock to avoid a
 * XXX: race condition.
 */
static void
header_init_re(struct vmod_priv *priv, const char *s)
{
	if (priv->priv == NULL) {
		assert(pthread_mutex_lock(&header_mutex) == 0);
		if (priv->priv == NULL) {
			VRT_re_init(&priv->priv, s);
			priv->free = VRT_re_fini;
		}
		pthread_mutex_unlock(&header_mutex);
	}
}
/**
* Set a header if input string matches regexp
* @param struct sess
* @param vmod_priv priv for mutex lock
* @param char tested string to be tested can be beresp.url ...
* @param char regexp regexp to apply on "tested"
* @param char header_name header field without : eq Cache-Control....
* @param char header_value string value of header field eq max-age=60s ...
*/
void
vmod_set(struct sess *sp, struct vmod_priv *priv, const char *tested, const char *regexp, const char *header_name, const char *header_value)
{
        char *formatted_header=malloc((strlen(header_name)+3) * sizeof(char)) ;

        if (tested && regexp && header_name && header_value)
        {
            header_init_re(priv, regexp);
            if (VRT_re_match(tested,priv->priv) != 0){
                sprintf(formatted_header,"%c%s:", strlen(header_name)+1,header_name);
                VRT_SetHdr(sp, HDR_RESP, formatted_header,header_value,vrt_magic_string_end);
            }
        }
        free(formatted_header);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
