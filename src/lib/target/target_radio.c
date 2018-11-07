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

#include <target.h>

static struct target_radio_ops rops;

bool target_radio_init(const struct target_radio_ops *ops)
{
    /* Register callbacks */
    rops = *ops;

    return true;
}

bool target_radio_config_init2()
{
    /**********************************************
    * Initial radio and vif config and state setup
    **********************************************/
    struct schema_Wifi_VIF_Config   vconfig;
    struct schema_Wifi_VIF_State    vstate;

    struct schema_Wifi_Radio_Config rconfig;
    struct schema_Wifi_Radio_State  rstate;

    rconfig._partial_update = true;
    rstate._partial_update  = true;

    memset(&vconfig, 0, sizeof(vconfig));
    memset(&vstate,  0, sizeof(vstate));

    SCHEMA_SET_INT(rconfig.channel,        1);
    SCHEMA_SET_STR(rconfig.country,        "US");
    SCHEMA_SET_INT(rconfig.enabled,        true);
    SCHEMA_SET_STR(rconfig.freq_band,      "2.4G");
    SCHEMA_SET_STR(rconfig.ht_mode,        "HT40");
    SCHEMA_SET_STR(rconfig.hw_mode,        "11n");
    SCHEMA_SET_STR(rconfig.hw_type,        "rpi");
    SCHEMA_SET_STR(rconfig.if_name,        "br0");

    SCHEMA_SET_INT(rstate.channel,    rconfig.channel);
    SCHEMA_SET_STR(rstate.country,    rconfig.country);
    SCHEMA_SET_INT(rstate.enabled,    rconfig.enabled);
    SCHEMA_SET_STR(rstate.freq_band,  rconfig.freq_band);
    SCHEMA_SET_STR(rstate.ht_mode,    rconfig.ht_mode);
    SCHEMA_SET_STR(rstate.hw_mode,    rconfig.hw_mode);
    SCHEMA_SET_STR(rstate.hw_type,    rconfig.hw_type);
    SCHEMA_SET_STR(rstate.if_name,    rconfig.if_name);

    if(NULL != rops.op_rconf)  { rops.op_rconf(&rconfig); }
    if(NULL != rops.op_rstate) { rops.op_rstate(&rstate); }

    vconfig._partial_update = true;
    vstate._partial_update  = true;

    SCHEMA_SET_STR(vconfig.if_name,       "br0");
    SCHEMA_SET_INT(vconfig.enabled,        true);
    SCHEMA_SET_STR(vconfig.mode,           "ap");
    SCHEMA_SET_STR(vconfig.ssid,           "network_ssid");
    SCHEMA_SET_STR(vconfig.ssid_broadcast, "enabled");
    SCHEMA_SET_STR(vconfig.mac_list_type,  "none");
    SCHEMA_SET_INT(vconfig.vif_radio_idx,  1);

    SCHEMA_SET_STR(vstate.if_name,        vconfig.if_name);
    SCHEMA_SET_INT(vstate.enabled,        vconfig.enabled);
    SCHEMA_SET_STR(vstate.mode,           vconfig.mode);
    SCHEMA_SET_STR(vstate.ssid,           vconfig.ssid);
    SCHEMA_SET_STR(vstate.ssid_broadcast, vconfig.ssid_broadcast);
    SCHEMA_SET_STR(vstate.mac_list_type,  vconfig.mac_list_type);
    SCHEMA_SET_INT(vstate.vif_radio_idx,  vconfig.vif_radio_idx);

    if(NULL != rops.op_rconf)  { rops.op_vconf(&vconfig, "br0"); }
    if(NULL != rops.op_vstate) { rops.op_vstate(&vstate); }

    return true;
}

bool target_radio_config_need_reset()
{
    return true;
}
bool target_radio_config_set2(const struct schema_Wifi_Radio_Config *rconf,
                              const struct schema_Wifi_Radio_Config_flags *changed)
{
    return true;
}


bool target_vif_config_set2(const struct schema_Wifi_VIF_Config *vconf,
                            const struct schema_Wifi_Radio_Config *rconf,
                            const struct schema_Wifi_Credential_Config *cconfs,
                            const struct schema_Wifi_VIF_Config_flags *changed,
                            int num_cconfs)
{
    return true;
}

bool target_bridge_inet_state_get(char *ifname,
                                  struct schema_Wifi_Inet_State *istate)
{

    SCHEMA_SET_STR(istate->inet_addr, "10.9.8.7");
    SCHEMA_SET_STR(istate->broadcast, "10.9.8.255");
    SCHEMA_SET_STR(istate->netmask,   "255.255.255.0");



    return true;
}
