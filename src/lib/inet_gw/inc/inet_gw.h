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

// inet_gw: subset of target_inet for gateway device type

#ifndef INET_GW_H_INCLUDED
#define INET_GW_H_INCLUDED

#include "target.h"

// default defines, allow override

#ifndef INET_IFNAME_WAN
#define INET_IFNAME_WAN                 "br-wan"
//#define INET_IFNAME_WAN                 "eth0"
#endif

#ifndef INET_IFNAME_BR_LAN
#define INET_IFNAME_BR_LAN              "br0"
#endif

#ifndef INET_CONFIG_INIT_LIST
#define INET_CONFIG_INIT_LIST           NULL
#endif

#ifndef INET_STATE_INIT_LIST
//#define INET_STATE_INIT_LIST            INET_IFNAME_WAN, INET_IFNAME_BR_LAN, NULL
#define INET_STATE_INIT_LIST            INET_IFNAME_WAN, NULL
#endif

#ifndef INET_PREPOPULATE_LIST
#define INET_PREPOPULATE_LIST           INET_STATE_INIT_LIST
#endif

#ifndef INET_STATE_POLL_PERIOD
#define INET_STATE_POLL_PERIOD          1     /* 1s polling period */
#endif

// public api used by target_inet

bool inetgw_vif_inet_config_set(
        char                           *ifname,
        struct schema_Wifi_Inet_Config *iconf);

bool inetgw_vif_inet_state_get(
        char                           *ifname,
        struct schema_Wifi_Inet_State  *istate);

bool inetgw_eth_inet_state_get(
        const char                     *ifname,
        struct schema_Wifi_Inet_State  *istate);

bool inetgw_gre_inet_config_set(
        char                           *ifname,
        char                           *remote_ip,
        struct schema_Wifi_Inet_Config *iconf);

bool inetgw_gre_inet_state_get(
        char                           *ifname,
        char                           *remote_ip,
        struct schema_Wifi_Inet_State  *istate);

bool inetgw_config_init(ds_dlist_t *inets);

bool inetgw_state_init(ds_dlist_t *inets);

bool inetgw_state_register(char *ifname, void *istate_cb);

#endif

