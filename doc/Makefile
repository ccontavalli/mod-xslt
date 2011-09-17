#
#   mod-xslt -- Copyright (C) 2002, 2003 
# 		 Carlo Contavalli 
# 		 <ccontavalli at masobit.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

.PHONY: manual faq patches clean-manual \
	clean-faq clean-patches
	
all: manual faq patches

manual: 
	$(MAKE) -C manual

faq:
	$(MAKE) -C faq

patches:
	$(MAKE) -C patches

clean: clean-manual clean-faq clean-patches

clean-manual:
	$(MAKE) -C manual clean

clean-faq:
	$(MAKE) -C faq clean

clean-patches:
	$(MAKE) -C patches clean

