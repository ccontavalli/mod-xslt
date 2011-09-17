#
#   mod-xslt -- Copyright (C) 2002-2008
# 		 Carlo Contavalli 
# 		 <ccontavalli at inscatolati.net>
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


XSLTPROC=xsltproc -nonet

all: faq.html faq.fo faq.pdf faq.ps faq.ps.gz

faq.ps: faq.pdf
	pdf2ps faq.pdf faq.ps

faq.ps.gz: faq.ps
	gzip -9c ./faq.ps > ./faq.ps.gz

faq.html: faq.xml ./xslt/faq-html.xsl ./dtd/faq.dtd
	$(XSLTPROC) ./xslt/faq-html.xsl ./faq.xml > faq.html

faq.fo: faq.xml ./xslt/faq-fo.xsl ./dtd/faq.dtd
	$(XSLTPROC) ./xslt/faq-fo.xsl ./faq.xml > faq.fo

faq.pdf: faq.fo 
	fop -fo ./faq.fo -pdf ./faq.pdf

clean:
	rm -f ./faq.{html,fo,pdf,ps,ps.gz}

distclena: clean
