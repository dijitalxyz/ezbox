#
# Copyright (C) 2010 ezbox project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Generic
  NAME:=Generic
  PACKAGES:= 
endef

define Profile/Generic/Description
	Generic Profile
endef
$(eval $(call Profile,Generic))
