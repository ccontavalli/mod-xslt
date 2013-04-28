<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
     Copyright (C) 2002,2003,2004 Marco Merlin <mmerlin at masobit.net> 

  Slightly modified by Carlo Contavalli, <ccontavalli at masobit.net>,
  to suit mod-xslt needs.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

-->


<xsl:stylesheet version="1.0" xmlns:faq="http://www.masobit.net/ns/faq/1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">
<xsl:output method="xml" version="1.0" indent="yes" />
<xsl:template match="faq:faq">
	<fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format">
		<fo:layout-master-set>
			<fo:simple-page-master master-name="cover-page-master" page-height="297mm" page-width="210mm" margin="5mm 5mm 5mm 5mm">
                <fo:region-body />
            </fo:simple-page-master>
			<fo:simple-page-master master-name="PageMaster" page-height="297mm" page-width="210mm" margin="5mm 25mm 5mm 25mm">
				<fo:region-body margin="30mm 0mm 20mm 0mm"/>
				<fo:region-before extent="30mm" />
				<fo:region-after extent="20mm" />
			</fo:simple-page-master>
		</fo:layout-master-set>

		<fo:page-sequence master-reference="PageMaster" initial-page-number="1" format="1">
		<fo:static-content flow-name="xsl-region-after" display-align="after">
                    <fo:block>
                        <fo:table width="100%" space-before.optimum="1pt" space-after.optimum="2pt">
                            <fo:table-column />
                            <fo:table-column column-width="30pt" />
                            <fo:table-body>
                                <fo:table-row>
                                    <fo:table-cell padding-bottom="0pt" padding-left="0pt" padding-right="0pt" padding-top="0pt" border-style="solid" border-width="1pt" border-color="white" height="30pt" number-columns-spanned="2" padding-start="3pt" padding-end="3pt" padding-before="3pt" padding-after="3pt" display-align="center" text-align="start">
                                        <fo:block />
                                    </fo:table-cell>
                                </fo:table-row>

                                <fo:table-row>
                                    <fo:table-cell padding-bottom="0pt" padding-left="0pt" padding-right="0pt" padding-top="0pt" border-style="solid" border-width="1pt" border-color="white" number-columns-spanned="2" padding-start="3pt" padding-end="3pt" padding-before="3pt" padding-after="3pt" display-align="center" text-align="start">
                                        <fo:block>
                                            <fo:block color="black" space-before.optimum="-8pt">
                                                <fo:leader leader-length="100%" leader-pattern="rule" rule-thickness="0.5pt" />
                                            </fo:block>
                                        </fo:block>
                                    </fo:table-cell>
                                </fo:table-row>
                                <fo:table-row>
                                    <fo:table-cell font-size="inherited-property-value(&apos;font-size&apos;) - 2pt" padding-bottom="0pt" padding-left="0pt" padding-right="0pt" padding-top="0pt" border-style="solid" border-width="1pt" border-color="white" text-align="left" padding-start="3pt" padding-end="3pt" padding-before="3pt" padding-after="3pt" display-align="center">
                                        <fo:block font-size="8pt">mod-xslt project -- http://www.modxslt.org/ -- Generated using faq-fo.xsl, Copyright (C) 2003,2004 Marco Merlin</fo:block>
                                    </fo:table-cell>
                                    <fo:table-cell font-size="inherited-property-value(&apos;font-size&apos;) - 2pt" padding-bottom="0pt" padding-left="0pt" padding-right="0pt" padding-top="0pt" border-style="solid" border-width="1pt" border-color="white" text-align="right" width="150pt" padding-start="3pt" padding-end="3pt" padding-before="3pt" padding-after="3pt" display-align="center">
                                        <fo:block>
                                            <fo:page-number />
                                        </fo:block>
                                    </fo:table-cell>
                                </fo:table-row>
                            </fo:table-body>
                        </fo:table>
                    </fo:block>
        </fo:static-content>
		<fo:static-content flow-name="xsl-region-before">
                    <fo:block>
                        <fo:table width="100%" space-before.optimum="1pt" space-after.optimum="2pt">
                            <fo:table-column />
                            <fo:table-column column-width="150pt" />
                            <fo:table-body>
								<fo:table-row>
                                    <fo:table-cell font-size="inherited-property-value(&apos;font-size&apos;) - 2pt" padding-bottom="0pt" padding-left="0pt" padding-right="0pt" padding-top="0pt" border-style="solid" border-width="1pt" border-color="white" text-align="right" number-columns-spanned="2" padding-start="3pt" padding-end="3pt" padding-before="3pt" padding-after="3pt" display-align="center">
                                        <fo:block>
                                            <fo:inline><xsl:value-of select="@module" />&#160;FAQ -- version <xsl:value-of select="@version" />, <xsl:value-of select="@released" /></fo:inline>
                                        </fo:block>
                                    </fo:table-cell>
                                </fo:table-row>
                                <fo:table-row>
                                    <fo:table-cell padding-bottom="0pt" padding-left="0pt" padding-right="0pt" padding-top="0pt" border-style="solid" border-width="1pt" border-color="white" height="25pt" number-columns-spanned="2" padding-start="3pt" padding-end="3pt" padding-before="3pt" padding-after="3pt" display-align="center" text-align="start">
                                        <fo:block>
                                        </fo:block>
                                    </fo:table-cell>
                                </fo:table-row>
                            </fo:table-body>
                        </fo:table>
                    </fo:block>
         </fo:static-content>
			<fo:flow flow-name="xsl-region-body">
				<fo:block font-size="30pt" text-align="center" font-weight="bold">
				  <xsl:value-of select="@module" /> FAQ 
				</fo:block>

                                <fo:block font-size="8pt" text-align="center">
                                  version:  <xsl:value-of select="@version" />, relased on <xsl:value-of select="@released" />
				  Copyright <xsl:value-of select="faq:copyright/faq:notice" />
				</fo:block>
                                <fo:block font-size="8pt" text-align="center" space-after="20pt">
                                  written by <xsl:value-of select="faq:author/faq:name" />, &lt;<xsl:value-of select="faq:author/faq:email" />&gt;
				</fo:block>


				<fo:block text-align="justify" font-size="10pt" margin-left="40pt" margin-right="40pt">
				  <xsl:apply-templates select="faq:copyright/faq:text" />
				</fo:block>

				<fo:block font-size="10pt" space-before="10pt" space-after="10pt">
						<fo:block break-after="page">
							<xsl:apply-templates mode="index" select="faq:content" />
						</fo:block>				
						<xsl:apply-templates mode="text" select="faq:content" />
				</fo:block>
			</fo:flow>
		</fo:page-sequence>
	</fo:root>
</xsl:template>

<xsl:template match="faq:url">
  <xsl:value-of select="@url" />
</xsl:template>

<xsl:template match="faq:author">
  <fo:block><xsl:apply-templates select="faq:name" /></fo:block>
  <fo:block>
	   <fo:leader leader-pattern="space" />
  </fo:block>
  <fo:block font-size="14pt" font-weight="100">&lt;<xsl:apply-templates select="faq:email" />&gt;</fo:block>
</xsl:template>

<xsl:template mode="index" match="faq:content">
  <xsl:apply-templates mode="index" select="faq:sect" />
</xsl:template>

<xsl:template mode="index" match="faq:sect">
	<fo:block font-weight="bold" text-align-last="justify" line-height="2em">
        <xsl:number level="multiple" count="faq:sect" />
        &#160;
        <xsl:value-of select="@title" />
        &#160;<fo:leader leader-pattern="dots" />&#160;
        <fo:page-number-citation><xsl:attribute name="ref-id"><xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute></fo:page-number-citation>
	</fo:block><xsl:apply-templates mode="index" select="faq:sect|faq:entry" />
</xsl:template>

<xsl:template mode="index" match="faq:entry">
	<fo:block font-size="8pt" space-after="0.7em">
	<xsl:attribute name="id">I<xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute>
		<fo:block margin-left="15pt" text-align-last="justify" line-height="1.4em">
			<xsl:number level="multiple" count="faq:sect|faq:entry" />
			&#160;<fo:leader leader-pattern="dots" />&#160;
			<fo:page-number-citation><xsl:attribute name="ref-id"><xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute></fo:page-number-citation>
        </fo:block> 
		<fo:block margin-left="15pt" margin-right="25pt" line-height="1.4em">
			<xsl:apply-templates mode="index" select="faq:question" />
		</fo:block>
	</fo:block> 
</xsl:template>

<xsl:template mode="index" match="faq:question">
	<fo:block>
		<fo:basic-link>
			<xsl:attribute name="internal-destination"><xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute>
			-<xsl:apply-templates mode="index" />
		</fo:basic-link>
	</fo:block>
</xsl:template>

<xsl:template mode="text" match="faq:content">
	<xsl:apply-templates mode="text" select="faq:sect" />
</xsl:template>

<xsl:template mode="text" match="faq:sect/faq:sect">
	<fo:block space-after="2em">
		<xsl:attribute name="id"><xsl:number level="multiple" count="faq:sect" /></xsl:attribute>
		<fo:block  font-weight="bold" font-size="14pt" space-after="0.7em">
			<xsl:number level="multiple" count="faq:sect" />
            &#160;
            <xsl:value-of select="@title" />
		</fo:block>
		<xsl:apply-templates mode="text" select="faq:entry" />
		<xsl:apply-templates mode="text" select="faq:sect" />
	</fo:block>
</xsl:template>


<xsl:template mode="text" match="faq:sect">
	<fo:block space-after="2em">
  	  <xsl:attribute name="id"><xsl:number level="multiple" count="faq:sect" /></xsl:attribute>
	  <fo:block  font-weight="bold" font-size="16pt" space-after="0.7em">
	    <xsl:number level="multiple" count="faq:sect" />&#160;
            <xsl:value-of select="@title" />
	  </fo:block>

	  <xsl:apply-templates mode="text" select="faq:entry" />
	  <xsl:apply-templates mode="text" select="faq:sect" />
	</fo:block>
</xsl:template>

<xsl:template mode="text" match="faq:entry">
	<fo:block>
		<xsl:attribute name="id"><xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute>

		<fo:block font-weight="bold">
			<xsl:number level="multiple" count="faq:sect|faq:entry" />
			<xsl:apply-templates mode="text" select="faq:question" />
		</fo:block>

		<xsl:apply-templates mode="text" select="faq:answer" />
	</fo:block> 
</xsl:template>

<xsl:template mode="text" match="faq:question">
	<fo:basic-link>
	  <xsl:attribute name="internal-destination">I<xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute>
	  <fo:inline>
	    <xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
	  </fo:inline>
	</fo:basic-link>
	<xsl:apply-templates />
</xsl:template>

<xsl:template mode="text" match="faq:answer">
	<fo:block margin-left="10pt" space-after="0.5em" margin-right="10pt">
		<xsl:apply-templates />
	</fo:block>
</xsl:template>

<xsl:template match="faq:itemize">
	<fo:block>
		<fo:list-block margin-left="10pt" provisional-distance-between-starts="7mm" provisional-label-separation="2mm" start-indent="2mm" space-before.optimum="4pt" space-after.optimum="4pt">
			<xsl:apply-templates select="faq:item" />
		</fo:list-block>
	</fo:block>
</xsl:template>

<xsl:template match="faq:enum">
	<fo:block>
		<fo:list-block provisional-distance-between-starts="7mm" provisional-label-separation="2mm" start-indent="2mm" space-before.optimum="4pt" space-after.optimum="4pt">
			<xsl:apply-templates select="faq:item" />
		</fo:list-block>
	</fo:block>
</xsl:template>

<xsl:template match="faq:itemize/faq:item">
	<fo:list-item>
		<fo:list-item-label end-indent="label-end()">
			<fo:block font-family="Courier" font-size="15pt" line-height="14pt" padding-before="2pt">&#x2022;</fo:block>
        </fo:list-item-label>
		<fo:list-item-body start-indent="body-start()">
			<fo:block>
				<xsl:apply-templates />
			</fo:block>
        </fo:list-item-body>
    </fo:list-item>
</xsl:template>

<xsl:template match="faq:enum/faq:item">
	<fo:list-item>
		<fo:list-item-label end-indent="label-end()">
            <fo:block>
                <xsl:number format="1" />.
            </fo:block>
        </fo:list-item-label>
        <fo:list-item-body start-indent="body-start()">
            <fo:block>
                <xsl:apply-templates />
            </fo:block>
		</fo:list-item-body>
    </fo:list-item>
</xsl:template>

<xsl:template match="faq:p">
	<fo:block>
		<xsl:apply-templates />
	</fo:block>
</xsl:template>

<xsl:template match="faq:bf">
	<fo:inline font-weight="bold">
		<xsl:apply-templates />
	</fo:inline>
</xsl:template>

<xsl:template match="faq:output" mode="index">
	<fo:block line-height="2em" font-size="0.8em" text-align="left" background-color="#ebebeb" font-family="monospace">
		<fo:block margin-left="10pt" margin-right="10pt" margin="3pt">
			<xsl:apply-templates />
		</fo:block>
	</fo:block>
</xsl:template>

<xsl:template match="faq:output">
	<fo:block line-height="2em" font-size="0.8em" text-align="left" background-color="#ebebeb" font-family="monospace">
		<fo:block margin-left="10pt" margin-right="10pt" margin="3pt">
			<xsl:apply-templates />
		</fo:block>
	</fo:block>
</xsl:template>

<xsl:template match="faq:ref">
	<fo:inline color="blue">
		<fo:basic-link>
			<xsl:attribute name="internal-destination"><xsl:value-of select="@id" /></xsl:attribute>
			<xsl:apply-templates />
		</fo:basic-link>
	</fo:inline>
</xsl:template>

</xsl:stylesheet>
