<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  	xmlns:db="http://www.masobit.net/ns/sdope/1.0"
  	xmlns="http://www.w3.org/TR/rec-html40">

<xsl:output method="html" 
            omit-xml-declaration="yes" 
	    encoding="ISO-8859-1" 
	    indent="yes" 
	    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" />

<xsl:template match="db:mediaobject">
  <table width="100%" border="0" cellpadding="0" cellspacing="0">
    <xsl:apply-templates />
  </table>
</xsl:template>

<xsl:template match="db:imageobject">
  <xsl:apply-templates />
</xsl:template>

<xsl:template match="db:imagedata">
  <tr><td>
    <xsl:attribute name="align"><xsl:value-of select="@align" /></xsl:attribute><br />
    <table width="1%">
      <tr><td>
        <img border="0"><xsl:attribute name="src"><xsl:value-of select="@fileref" /></xsl:attribute>
          <xsl:attribute name="alt"><xsl:value-of select="@title" /></xsl:attribute>
        </img> 
      </td></tr>

      <tr><td>
        <b><font size="1">Image&#160;<xsl:number level="multiple" count="sect|mediaobject" />:&#160;&#160;</font></b>
        <font size="1"><xsl:value-of select="@title" /></font>
        <br /><br />
      </td></tr>
    </table>
  </td></tr>
</xsl:template>

</xsl:stylesheet> 
