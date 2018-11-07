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

#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/wireless.h>

#include <ev.h>

#include "os.h"
#include "log.h"
#include "util.h"
#include "os_nif.h"
#include "schema.h"
#include "ds_dlist.h"
#include "ds_tree.h"
#include "target.h"

#include "target.h"
#include "inet_gw.h"

#define MAX_DEVNAME_SIZE                256
#define WAN_DUMMY_IFNAME                INET_IFNAME_WAN

static char *g_wan_ifname = NULL;

typedef void inet_state_fn(
        struct schema_Wifi_Inet_State *state,
        schema_filter_t *filter);

static inet_state_fn *inet_state_cb;

/**
 * Network interface cache
 */
struct inet
{
    // ifname first field so strcmp key works
    char                            in_ifname[MAX_DEVNAME_SIZE];
    ds_tree_node_t                  in_node;
    struct schema_Wifi_Inet_Config  in_config;
    struct schema_Wifi_Inet_State   in_state;
    bool                            in_dhcpd_started;
    struct
    {
        bool created;
    } in_gre;
};

/**
 * Target network interface list -- indexed by interface name
 */
static ds_tree_t inet_iface_list = DS_TREE_INIT(
        (ds_key_cmp_t *)strcmp,
        struct inet,
        in_node);

static ev_timer inet_state_timer;

static char *inet_config_init_iflist[] =
{
    INET_CONFIG_INIT_LIST
};

static char *inet_state_init_iflist[] =
{
    INET_STATE_INIT_LIST
};

static struct inet *inet_fetch(const char *ifname, char *iftype);

static void inet_state_periodic(
        struct ev_loop *loop,
        ev_timer *w,
        int revents);

static void inet_init_timer()
{
    /*
     * Start the Inet State polling timer
     */
    ev_timer_init(
            &inet_state_timer,
            inet_state_periodic,
            INET_STATE_POLL_PERIOD,
            INET_STATE_POLL_PERIOD);

    ev_timer_start(EV_DEFAULT, &inet_state_timer);
}

static const char* inet_map_wan(const char *ifname)
{
    if (g_wan_ifname && strcmp(ifname, WAN_DUMMY_IFNAME) == 0)
    {
        return g_wan_ifname;
    }

    return ifname;
}

/**
 * Find or allocate a new Inet State element structure
 */
static struct inet *inet_fetch(const char *ifname, char *iftype)
{
    struct inet *nif;

    // Map WAN interface if needed
    ifname = inet_map_wan(ifname);

    nif = ds_tree_find(&inet_iface_list, (char *)ifname);
    if (nif != NULL)
    {
        /* Element already exists -- return it */
        return nif;
    }

    /* Unable to create the interface if the type is not specified */
    if (iftype == NULL) return NULL;

    /* Create new element */
    nif = calloc(1, sizeof(*nif));
    STRSCPY(nif->in_ifname, ifname);
    STRSCPY(nif->in_config.if_name, ifname);
    STRSCPY(nif->in_state.if_name, ifname);

    STRSCPY(nif->in_config.if_type, iftype);
    STRSCPY(nif->in_state.if_type, iftype);

    ds_tree_insert(
            &inet_iface_list,
            nif,
            nif->in_ifname);

    return nif;
}


/**
 * Update the @p state structure with the current interface state as acquired from the system
 */
static bool inet_state_refresh(
        struct inet *nif,
        struct schema_Wifi_Inet_State *state)
{
   /* Hardcoded sample data */
    STRSCPY(state->inet_addr, "213.250.20.88");
    state->inet_addr_exists = true;

    state->mtu = 1500;
    state->mtu_exists = true;

    state->enabled = true;


    return true;
}


/**
 * Periodic timer for Inet status update
 *///
static void inet_state_periodic(
        struct ev_loop *loop,
        ev_timer *w,
        int revents)
{
    struct inet *nif;

    ds_tree_foreach(&inet_iface_list, nif)
    {
        struct schema_Wifi_Inet_State old;

        memcpy(&old, &nif->in_state, sizeof(old));

        if (!inet_state_refresh(nif, &nif->in_state))
        {
            /* Silence this, this might get really verbose when an interface doesn't exist. */
            continue;
        }

        /*
         * Compare new and old strucutres, call callback if any of the relevant fields changed
         */
        bool change_detected = false;

        /* Compare inet_addr */
        if (old.inet_addr_exists != nif->in_state.inet_addr_exists)
        {
            change_detected = true;
        }
        else if (old.inet_addr_exists && strcmp(old.inet_addr, nif->in_state.inet_addr) != 0)
        {
            change_detected = true;
        }

        /* Compare network */
        if (old.netmask_exists != nif->in_state.netmask_exists)
        {
            change_detected = true;
        }
        else if (old.netmask_exists && strcmp(old.netmask, nif->in_state.netmask) != 0)
        {
            change_detected = true;
        }

        /* Compare broadcast */
        if (old.broadcast_exists != nif->in_state.broadcast_exists)
        {
            change_detected = true;
        }
        else if (old.broadcast_exists && strcmp(old.broadcast, nif->in_state.broadcast) != 0)
        {
            change_detected = true;
        }

        if (old.mtu_exists != nif->in_state.mtu_exists)
        {
            change_detected = true;
        }
        else if (old.mtu_exists && old.mtu != nif->in_state.mtu)
        {
            change_detected = true;
        }

        /* No change detected, continue */
        if (!change_detected) continue;

        /* Process change */
        LOGI("Interface change detected on: %s. New state: IP:%s NETMASK:%s BRDADDR:%s MTU:%d",
                nif->in_ifname,
                nif->in_state.inet_addr_exists ? nif->in_state.inet_addr : "(none)",
                nif->in_state.netmask_exists ? nif->in_state.netmask : "(none)",
                nif->in_state.broadcast_exists ? nif->in_state.broadcast : "(none)",
                nif->in_state.mtu_exists ? nif->in_state.mtu : -1);

        if (inet_state_cb)
        {
            LOGD("Calling callback.");

            if (g_wan_ifname && strcmp(nif->in_ifname, g_wan_ifname) == 0)
            {
                // Modify and restore expected WAN ifname
                STRSCPY(nif->in_state.if_name, WAN_DUMMY_IFNAME);
                inet_state_cb(&nif->in_state, NULL);
                STRSCPY(nif->in_state.if_name, g_wan_ifname);
            }
            else
            {
                inet_state_cb(&nif->in_state, NULL);
            }
        }
        else
        {
            LOGD("No callback registered.");
        }
    }
}


static bool inet_config_apply(struct inet *nif)
{
    return true;
}

static bool inet_config_set(
        struct inet *nif,
        struct schema_Wifi_Inet_Config *iconf)
{
    /* Cache the current config */
    memcpy(&nif->in_config, iconf, sizeof(nif->in_config));

    /*
     * Update state by copying some of the config fields
     */

    if (!inet_config_apply(nif))
    {
        LOGD("Error applying config for interface: %s", nif->in_ifname);
        return false;
    }

    nif->in_state.network = iconf->network;
    nif->in_state.enabled = iconf->enabled;

    STRSCPY(nif->in_state.ip_assign_scheme, iconf->ip_assign_scheme);
    nif->in_state.ip_assign_scheme_exists = iconf->ip_assign_scheme_exists;

    nif->in_state.NAT = iconf->NAT;
    nif->in_state.NAT_exists = iconf->NAT_exists;

    STRSCPY(nif->in_state.gateway, iconf->gateway);
    nif->in_state.gateway_exists = iconf->gateway_exists;

    STRSCPY(nif->in_state.gre_ifname, iconf->gre_ifname);
    nif->in_state.gre_ifname_exists = iconf->gre_ifname_exists;

    STRSCPY(nif->in_state.gre_remote_inet_addr, iconf->gre_remote_inet_addr);
    nif->in_state.gre_remote_inet_addr_exists = iconf->gre_remote_inet_addr_exists;

    STRSCPY(nif->in_state.gre_local_inet_addr, iconf->gre_local_inet_addr);
    nif->in_state.gre_local_inet_addr_exists = iconf->gre_local_inet_addr_exists;

    STRSCPY(nif->in_state.upnp_mode, iconf->upnp_mode);
    nif->in_state.upnp_mode_exists = iconf->upnp_mode_exists;

    STRSCPY(nif->in_state.parent_ifname, iconf->parent_ifname);
    nif->in_state.parent_ifname_exists = iconf->parent_ifname_exists;

    nif->in_state.vlan_id = iconf->vlan_id;
    nif->in_state.vlan_id_exists = iconf->vlan_id_exists;

    if (sizeof(nif->in_state.dns) != sizeof(iconf->dns) ||
            sizeof(nif->in_state.dns_keys) != sizeof(iconf->dns_keys))
    {
        LOGE("State dns map and Config dns map out of sync. Please update the schema.");
        return false;
    }

    memcpy(nif->in_state.dns_keys, iconf->dns_keys, sizeof(nif->in_state.dns_keys));
    memcpy(nif->in_state.dns, iconf->dns, sizeof(nif->in_state.dns));
    nif->in_state.dns_len = iconf->dns_len;

    if (sizeof(nif->in_state.dhcpd) != sizeof(iconf->dhcpd) ||
            sizeof(nif->in_state.dhcpd_keys) != sizeof(iconf->dhcpd_keys))
    {
        LOGE("State dhcpd map and Config dhcpd map out of sync. Please update the schema.");
        return false;
    }

    memcpy(nif->in_state.dhcpd, iconf->dhcpd, sizeof(nif->in_state.dhcpd));
    memcpy(nif->in_state.dhcpd_keys, iconf->dhcpd_keys, sizeof(nif->in_state.dhcpd_keys));
    nif->in_state.dhcpd_len = iconf->dhcpd_len;

    return true;
}

// INIT

static bool inet_detect_wan()
{
    FILE *fp;
    char  buf[512];
    bool  success = false;

    fp = fopen("/proc/net/route", "r");
    if (fp == NULL)
    {
        LOGE("Unable to determine WAN ifname! :: Reading route table failed!");
        return false;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        char ifname[32];
        char ifmask[32];
        char ifdest[32];
        // Example output:
        // Iface   Destination     Gateway         Flags   RefCnt  Use     Metric  Mask            MTU     Window  IRTT
        // eth0    00000000        0A01070F        0003    0       0       0       00000000        0       0       0
        if (3 != sscanf(buf, "%31s %31s %*s %*s %*s %*s %*s %31s %*s %*s %*s",
                        ifname, ifdest, ifmask))
        {
            continue;
        }

        // Default route
        if (strcmp("00000000", ifdest) == 0 && strcmp("00000000", ifmask) == 0)
        {
            g_wan_ifname = strdup(ifname);
            LOGI("WAN interface detected! :: ifname=%s", g_wan_ifname);
            success = true;
            break;
        }
    }

    fclose(fp);

    return success;
}

static void inet_prepopulate()
{
    char *inet_prep_iflist[] = { INET_PREPOPULATE_LIST };
    char **piface;
    //struct inet *nif;

    for (piface = inet_prep_iflist; *piface != NULL; piface++) {
        inet_fetch(*piface, "bridge");
    }
}

static bool inet_init()
{
    // Wait for WAN connection to initialize
    while (!inet_detect_wan())
    {
        sleep(5);
        LOGI("WAN interface detection retry...");
    }

    inet_prepopulate();
    inet_init_timer();
    return true;
}


/******************************************************************************
 *  TARGET INET PUBLIC API
 *****************************************************************************/

bool inetgw_vif_inet_config_set(
        char                           *ifname,
        struct schema_Wifi_Inet_Config *iconf)
{
    struct inet *nif;

    nif = inet_fetch(ifname, "vif");
    if (nif == NULL)
    {
        LOGE("Error allocating interface %s.", ifname);
        return false;
    }

    return inet_config_set(nif, iconf);
}

bool inetgw_vif_inet_state_get(
        char                           *ifname,
        struct schema_Wifi_Inet_State  *istate)
{
    struct inet *nif;

    nif = inet_fetch(ifname, "vif");
    if (nif == NULL)
    {
        LOGE("Unable to fetch INET structure for interface %s.", ifname);
        return false;
    }

    memcpy(istate, &nif->in_state, sizeof(*istate));

    return true;
}

bool inetgw_eth_inet_state_get(
        const char                     *ifname,
        struct schema_Wifi_Inet_State  *istate)
{
    struct inet *nif;

    nif = inet_fetch(ifname, "eth");
    if (nif == NULL)
    {
        LOGE("Unable to fetch INET structure for interface %s.", ifname);
        return false;
    }

    memcpy(istate, &nif->in_state, sizeof(*istate));

    return true;
}

bool inetgw_gre_inet_config_set(
        char                           *ifname,
        char                           *remote_ip,
        struct schema_Wifi_Inet_Config *iconf)
{
    return 0;
}

bool inetgw_gre_inet_state_get(
        char                           *ifname,
        char                           *remote_ip,
        struct schema_Wifi_Inet_State  *istate)
{
    struct inet *nif;

    // Find GRE
    ds_tree_foreach(&inet_iface_list, nif)
    {
        if (strcmp(nif->in_config.if_type, "gre") != 0) continue;
        if (strcmp(nif->in_config.if_name, ifname) != 0) continue;
        if (!nif->in_config.gre_remote_inet_addr_exists) continue;
        if (strcmp(nif->in_config.gre_remote_inet_addr, remote_ip) != 0) continue;

        memcpy(istate, &nif->in_state, sizeof(*istate));

        return true;
    }

    LOGE("GRE interface with parent interface %s not found.", ifname);
    return false;
}


bool inetgw_config_init(ds_dlist_t *inets)
{
    target_inet_config_init_t *init;
    struct inet *nif;
    char **piface;

    inet_init();

    ds_dlist_init(inets, target_inet_config_init_t, dsl_node);

    for (piface = inet_config_init_iflist; *piface != NULL; piface++)
    {
        nif = inet_fetch(*piface, NULL);
        if (nif == NULL)
        {
            LOGW("INET: Interface %s not initialized, cannot populate config.", *piface);
            continue;
        }

        init = calloc(1, sizeof(target_inet_config_init_t));
        if (init == NULL)
        {
            LOGW("INET: Error allocating init config structures for %s, cannot populate config.", *piface);
            continue;
        }

        memcpy(&init->iconfig, &nif->in_config, sizeof(init->iconfig));

        /* Add interface to the list */
        ds_dlist_insert_tail(inets, init);
    }

    return true;
}


bool inetgw_state_init(ds_dlist_t *inets)
{
    target_inet_state_init_t *init;
    struct inet *nif;
    char **piface;

    ds_dlist_init(inets, target_inet_state_init_t, dsl_node);

    for (piface = inet_state_init_iflist; *piface != NULL; piface++)
    {
        nif = inet_fetch(*piface, NULL);
        if (nif == NULL)
        {
            LOGW("INET: Interface %s not initialized, cannot populate state.", *piface);
            return false;
        }

        init = calloc(1, sizeof(target_inet_state_init_t));
        if (init == NULL)
        {
            LOGW("INET: Error allocating init config structures for %s, cannot populate state.", *piface);
            return false;
        }

        memcpy(&init->istate, &nif->in_state, sizeof(init->istate));

        /* Add interface to the list */
        ds_dlist_insert_tail(inets, init);
    }

    return true;
}

bool inetgw_state_register(char *ifname, void *istate_cb)
{
    inet_state_cb = istate_cb;
    return true;
}


