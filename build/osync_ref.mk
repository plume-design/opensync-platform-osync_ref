# Copyright (c) 2015, Plume Design Inc. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#    2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#    3. Neither the name of the Plume Design Inc. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Plume Design Inc. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CC=arm-linux-gnueabihf-gcc

RPICROSSCOMPILE_HOME ?= ../sdk/rpi/
CROSSCOMPILE_SYSROOT = $(RPICROSSCOMPILE_HOME)tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/arm-linux-gnueabihf/sysroot

CFLAGS += -Wno-error=cpp
CFLAGS += -I$(CROSSCOMPILE_SYSROOT)/usr/include/protobuf-c
CFLAGS += -I$(CROSSCOMPILE_SYSROOT)/usr/include/arm-linux-gnueabihf

LDFLAGS += -L$(CROSSCOMPILE_SYSROOT)/usr/lib/arm-linux-gnueabihf
LDFLAGS += -lcares

VENDOR_OVSDB_HOOKS += $(PLATFORM_DIR)/ovsdb

UNIT_DISABLE_src/bm                    := y
UNIT_DISABLE_src/blem                  := y
UNIT_DISABLE_src/nm2                   := n
UNIT_DISABLE_src/sm                    := n 
UNIT_DISABLE_src/wm2                   := n
UNIT_DISABLE_src/lib/bsal              := y
UNIT_DISABLE_src/lib/cev               := n
UNIT_DISABLE_src/lib/inet              := y
UNIT_DISABLE_src/lib/ovs_mac_learn     := y

CONTROLLER_ADDR="ssl:controller.example.com:443"
