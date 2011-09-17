<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  	xmlns:db="http://www.masobit.net/ns/sdope/1.0"
  xmlns="http://www.w3.org/TR/rec-html40">

<xsl:output method="html" 
            omit-xml-declaration="yes" 
	    encoding="ISO-8859-1" 
	    indent="yes" 
	    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" />

<xsl:template match="db:table">
  <table width="80%" align="center" border="0" cellspacing="0" cellpadding="0" class="sdope">
    <xsl:apply-templates />
  </table>
</xsl:template>

<xsl:template match="db:row">
  <tr>
    <xsl:apply-templates />
  </tr>
</xsl:template>

<xsl:template match="db:desc">
  <td valign="top">
    <b><xsl:apply-templates /></b>&#160;&#160;
  </td>
</xsl:template>

<xsl:template match="db:content">
  <td valign="top">
    <xsl:apply-templates /><br /><br />
  </td>
</xsl:template>

</xsl:stylesheet>
