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

#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


#include "log.h"
#include "os.h"
#include "os_nif.h"
#include "os_regex.h"
#include "os_types.h"
#include "os_util.h"
#include "target.h"
#include "util.h"


/******************************************************************************
 *  INTERFACE definitions
 *****************************************************************************/
bool target_is_radio_interface_ready(char *phy_name)
{
   return true;
}

bool target_is_interface_ready(char *if_name)
{
    return true;
}

/******************************************************************************
 *  STATS definitions
 *****************************************************************************/
bool target_radio_tx_stats_enable(radio_entry_t *radio_cfg, bool enable)
{
    return true;
}

bool target_radio_fast_scan_enable(radio_entry_t *radio_cfg, ifname_t if_name)
{
   return true;
}

/******************************************************************************
 *  CLIENT definitions
 *****************************************************************************/
target_client_record_t *target_client_record_alloc()
{

    target_client_record_t *record = NULL;

    record = malloc(sizeof(target_client_record_t));
    if (record) {
        memset(record, 0, sizeof(target_client_record_t));
    }

    return record;
}

void target_client_record_free(target_client_record_t *record)
{
    if(NULL != record) {
        free(record);
    }
}

bool target_stats_clients_get(radio_entry_t *radio_cfg,
                              radio_essid_t *essid,
                              target_stats_clients_cb_t *client_cb,
                              ds_dlist_t *client_list,
                              void *client_ctx)
{
    return true;
}

bool target_stats_clients_convert(radio_entry_t *radio_cfg,
                                  target_client_record_t *data_new,
                                  target_client_record_t *data_old,
                                  dpp_client_record_t *client_record)
{
   return true;
}

/******************************************************************************
 *  SURVEY definitions
 *****************************************************************************/
target_survey_record_t *target_survey_record_alloc()
{
    return 0;
}

void target_survey_record_free(target_survey_record_t *result)
{

}

bool target_stats_survey_get(radio_entry_t *radio_cfg,
                             uint32_t *chan_list,
                             uint32_t chan_num,
                             radio_scan_type_t scan_type,
                             target_stats_survey_cb_t *survey_cb,
                             ds_dlist_t *survey_list,
                             void *survey_ctx)
{
    return true;
}

bool target_stats_survey_convert(radio_entry_t *radio_cfg,
                                 radio_scan_type_t scan_type,
                                 target_survey_record_t *data_new,
                                 target_survey_record_t *data_old,
                                 dpp_survey_record_t *survey_record)
{
   return true;
}

/******************************************************************************
 *  NEIGHBORS definitions
 *****************************************************************************/
bool target_stats_scan_start(radio_entry_t *radio_cfg,
                             uint32_t *chan_list,
                             uint32_t chan_num,
                             radio_scan_type_t scan_type,
                             int32_t dwell_time,
                             target_scan_cb_t *scan_cb,
                             void *scan_ctx)
{
   return true;
}

bool target_stats_scan_stop(radio_entry_t *radio_cfg,
                            radio_scan_type_t scan_type)
{
   return true;
}

bool target_stats_scan_get(radio_entry_t *radio_cfg,
                           uint32_t *chan_list,
                           uint32_t chan_num,
                           radio_scan_type_t scan_type,
                           dpp_neighbor_report_data_t *scan_results)
{
    return true;
}

/******************************************************************************
 *  DEVICE definitions
 *****************************************************************************/

/*bool target_stats_device_get (dpp_device_record_t *device_entry)
{
    return true;
}*/

bool target_stats_device_temp_get(radio_entry_t *radio_cfg,
                                  dpp_device_temp_t *temp_entry)
{
    return true;
}

bool target_stats_device_txchainmask_get(
        radio_entry_t              *radio_cfg,
        dpp_device_txchainmask_t   *txchainmask_entry)
{
    return true;
}

bool target_stats_device_fanrpm_get (uint32_t *fan_rpm)
{
    return true;
}

/******************************************************************************
 *  CAPACITY definitions
 *****************************************************************************/
bool target_stats_capacity_enable(radio_entry_t *radio_cfg, bool enabled)
{
    return true;
}

bool target_stats_capacity_get(radio_entry_t *radio_cfg,
                               target_capacity_data_t *capacity_new)
{
   return true;
}

bool target_stats_capacity_convert(target_capacity_data_t *capacity_new,
                                   target_capacity_data_t *capacity_old,
                                   dpp_capacity_record_t *capacity_entry)
{
   return true;
}


/******************************************************************************
 *  MANAGERS definitions
 *****************************************************************************/
target_managers_config_t target_managers_config[] =
{
    {
        .name = TARGET_MANAGER_PATH("wm"),
        .needs_plan_b = true,
    },

    {
        .name = TARGET_MANAGER_PATH("nm"),
        .needs_plan_b = true,
    },

    {
        .name = TARGET_MANAGER_PATH("cm"),
        .needs_plan_b = true,
    },

    {
        .name = TARGET_MANAGER_PATH("lm"),
        .needs_plan_b = true,
    },

    {
        .name =TARGET_MANAGER_PATH("sm"),
        .needs_plan_b = false,
    },

    {
        .name = TARGET_MANAGER_PATH("om"),
        .needs_plan_b = true,
    },

    {
        .name = TARGET_MANAGER_PATH("qm"),
        .needs_plan_b = false,
    }

};
int target_managers_num =
    (sizeof(target_managers_config) / sizeof(target_managers_config[0]));

bool target_model_get(void *buff, size_t buffsz)
{
    snprintf(
            buff,
            buffsz,
            "%s",
            "OSYNC_REF");

    return true;
}
bool target_serial_get(void *buff, size_t buffsz)
{
    snprintf(
            buff,
            buffsz,
            "%s",
            "RPI001");

    return true;

}

