################################################################################
#
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2020 Sky
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################
################################################################################
#
# Copyright [2014] [Cisco Systems, Inc.]
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##########################################################################
bin_PROGRAMS = VlanManager

AM_CPPFLAGS = -I$(top_srcdir)/hal/include \
		-I$(top_srcdir)/source/LogComponent \
		-I$(top_srcdir)/source/TR-181/middle_layer_src \
		-I$(top_srcdir)/source/TR-181/include


VlanManager_DEPENDENCIES = \
        $(EXTRA_DEPENDENCIES) \
        ${top_builddir}/source/TR-181/middle_layer_src/libRdkVlanManager_middle_layer_src.la

VlanManager_CFLAGS = -DFEATURE_SUPPORT_RDKLOG $(DBUS_CFLAGS) $(SYSTEMD_CFLAGS)
VlanManager_SOURCES = ssp_action.c ssp_messagebus_interface.c ssp_main.c dm_pack_datamodel.c
VlanManager_LDFLAGS = -lccsp_common -lrdkloggers -lprivilege $(DBUS_LIBS) $(SYSTEMD_LDFLAGS) -lsecure_wrapper
VlanManager_LDADD = $(VlanManager_DEPENDENCIES)
