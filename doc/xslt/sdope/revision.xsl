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

<xsl:template match="db:revhistory"><br />
  <table width="100%" border="0" cellspacing="0" cellpadding="1" bgcolor="#000000">
    <tr><td>
      <table width="100%" border="0" cellspacing="0" cellpadding="5" bgcolor="#4B87CB" class="sdope">
        <tr><td>
          <b><font color="#FFFFFF">Revision History:&#160;&#160;</font></b>
	  <br /><br />
          <xsl:apply-templates />
        </td></tr>
      </table>
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="db:revremark">
  Revision Remark:&#160;&#160;<xsl:value-of select="." /><br />
</xsl:template>

<xsl:template match="db:revnumber">
  <b>Revision number:&#160;&#160;</b>
  <xsl:value-of select="." />&#160;&#160;
</xsl:template>

</xsl:stylesheet>
