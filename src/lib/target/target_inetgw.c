/*
Copyright (c) 2015, Plume Design Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. Neither the name of the Plume Design Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Plume Design Inc. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "target.h"
#include "inet_gw.h"

bool target_vif_inet_config_set(
        char                           *ifname,
        struct schema_Wifi_Inet_Config *iconf)
{
    return inetgw_vif_inet_config_set(ifname, iconf);
}

bool target_vif_inet_state_get(
        char                           *ifname,
        struct schema_Wifi_Inet_State  *istate)
{
    return inetgw_vif_inet_state_get(ifname, istate);
}

bool target_eth_inet_state_get(
        const char                     *ifname,
        struct schema_Wifi_Inet_State  *istate)
{
    return inetgw_eth_inet_state_get(ifname, istate);
}

bool target_gre_inet_config_set(
        char                           *ifname,
        char                           *remote_ip,
        struct schema_Wifi_Inet_Config *iconf)
{
    return inetgw_gre_inet_config_set(ifname, remote_ip, iconf);
}

bool target_gre_inet_state_get(
        char                           *ifname,
        char                           *remote_ip,
        struct schema_Wifi_Inet_State  *istate)
{
    return inetgw_gre_inet_state_get(ifname, remote_ip, istate);
}

bool target_inet_config_init(ds_dlist_t *inets)
{
    return inetgw_config_init(inets);
}

bool target_inet_state_init(ds_dlist_t *inets)
{
    return inetgw_state_init(inets);
}

bool target_inet_state_register(char *ifname, void *istate_cb)
{
    return inetgw_state_register(ifname, istate_cb);
}

