<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:db="http://www.masobit.net/ns/sdope/1.0"
  xmlns="http://www.w3.org/TR/rec-html40">

<xsl:import href="revision.xsl" />
<xsl:import href="index.xsl" />
<xsl:import href="list.xsl" />
<xsl:import href="table.xsl" />
<xsl:import href="sect.xsl" />
<xsl:import href="styles.xsl" />
<xsl:import href="info.xsl" />
<xsl:import href="objects.xsl" />

<xsl:output method="html" 
            omit-xml-declaration="yes" 
	    encoding="ISO-8859-1" 
	    indent="yes" 
	    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" />
 
<xsl:template match="/">
  <html>
    <head>
      <title><xsl:value-of select="/db:article/db:articleinfo/db:title" /></title>
      <link href="xslt/docbook.css" rel="stylesheet" type="text/css"/>
    </head>

    <body topMargin="0" leftMargin="0" marginwidth="0" marginheight="0">
      <xsl:apply-templates select="db:article" />
    </body>
  </html>  
</xsl:template>

<xsl:template match="db:article|db:Article|db:ARTICLE">
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr><td class="sdope">
      <xsl:apply-templates />
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="db:include|db:Include|db:INCLUDE">
  <xsl:apply-templates select="document(@href)" />
</xsl:template>

</xsl:stylesheet>
