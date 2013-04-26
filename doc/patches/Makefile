#
#   mod-xslt -- Copyright (C) 2002, 2003
#   		 Riccardo Setti <giskard at autistici.org> and
# 		 Carlo Contavalli <ccontavalli at masobit.net>
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

source = $(word 1,$(wildcard *.xml))
source := $(strip $(source:.xml=))
source := $(strip $(source:=))

all: $(source).xml $(source).xml $(source).pdf $(source).ps.gz $(source).tex \
     $(source).txt $(source).rtf $(source).ps $(source).html.tar.gz $(source)/index.html


$(source).html.tar.gz: $(source)/index.html Makefile
	@echo "Generating html tarball..."
	@tar --exclude '*.arch*' -cvzf ./$(source).html.tar.gz ./$(source)

$(source)/index.html: $(source).xml Makefile
	@echo "Genereting html.." 
	@mkdir -p $(source) || true 
	@cd $(source) && docbook2html ../$(source).xml 

$(source).txt: $(source).xml Makefile
	@echo "Genereting txt..."	
	@docbook2txt $(source).xml 

$(source).rtf: $(source).xml Makefile
	@echo "Genereting rtf..."	
	@docbook2rtf $(source).xml 

$(source).pdf: $(source).xml Makefile
	@echo "Genereting pdf..."	
	@docbook2pdf $(source).xml 

$(source).ps.gz: $(source).ps Makefile
	@echo "Compressing ps..."
	@gzip -9c < $(source).ps > $(source).ps.gz

$(source).ps: $(source).xml Makefile
	@echo "Genereting ps.."
	@docbook2ps $(source).xml 

$(source).dvi: $(source).tex Makefile
	@echo Generating .dvi version...;\
	 elatex '\nonstopmode\input{$(source).tex}'
	@echo "  Generating index...";\
	 elatex '\nonstopmode\input{$(source).tex}'
	@echo "  Removing temporary files...";\
	 rm -f ./$(source).{log,toc,aux}

$(source).tex: $(source).xml Makefile
	@echo "Genereting tex.."
	@docbook2tex $(source).xml 

clean:
	@echo "Cleaning up"
	@for extension in tex ps ps.gz pdf txt rtf log dvi html.tar.gz; do \
	   rm -f $(source).$$extension; \
	 done
	@rm -rf ./info/
	@rm -rf $(source)/*

.PHONY: all clean
