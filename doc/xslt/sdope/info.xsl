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

<xsl:template match="db:articleinfo|db:ArticleInfo|db:Articleinfo|db:ArtHeader">
  <table width="100%" border="0" cellspacing="0" cellpadding="0" class="sdope">
    <tr><td>
      <xsl:apply-templates />
    </td></tr>
  </table>

  <xsl:if test="../db:sect|../db:Sect|../db:SECT|../db:Sect1">
    <ul>
      <xsl:call-template name="index" />
    </ul>
  </xsl:if>
</xsl:template>

<xsl:template match="db:title|db:Title|db:TITLE">
  <xsl:param name="class">sdope-title</xsl:param>
  <h1>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:apply-templates />
  </h1>
</xsl:template>

<xsl:template match="db:author|db:Author|db:AUTHOR">
  <table width="100%" border="0" cellspacing="0" cellpadding="1" bgcolor="#4A7Ac9">
    <tr><td>
      <table width="100%" border="0" cellspacing="0" cellpadding="5" bgcolor="#F3F3F3" class="sdope">
        <tr><td>
          <b>Author:&#160;&#160;</b>
          <xsl:apply-templates />
        </td></tr>
      </table>
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="db:firstname|db:FirstName|db:Firstname">
  <xsl:value-of select="." />&#160;
</xsl:template>

<xsl:template match="db:surname|db:SurName|db:Surname">
  <xsl:value-of select="." />&#160;&#160;&#160;&#160;&#160;&#160;
</xsl:template>

<xsl:template match="db:email|db:Email">
  <b>email:&#160;&#160;</b><xsl:value-of select="." /><br />
</xsl:template>

<xsl:template match="db:date|db:Date">
  <xsl:value-of select="." />&#160;&#160;
</xsl:template>

<xsl:template match="db:authorinitials|db:AuthorInitials">
  Author:&#160;&#160;
  <xsl:value-of select="." /><br />
</xsl:template>

<xsl:template match="db:abstract|db:Abstract">
  <b>
    <xsl:apply-templates>
      <xsl:with-param name="class">sdope-abstract</xsl:with-param>
    </xsl:apply-templates>
  </b>
</xsl:template>


</xsl:stylesheet>
