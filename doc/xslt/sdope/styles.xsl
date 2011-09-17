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

<xsl:template match="db:it|db:It">
  <i><xsl:apply-templates /></i>
</xsl:template>

<xsl:template match="db:Emphasis">
  <xsl:choose>
    <xsl:when test="@remap = 'bf'">
      <b><xsl:apply-templates /></b>
    </xsl:when>
    <xsl:when test="@remap = 'it'">
      <i><xsl:apply-templates /></i>
    </xsl:when>
    <xsl:otherwise>
      <em><xsl:apply-templates /></em>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="db:emphasis">
  <em><xsl:apply-templates /></em>
</xsl:template>

<xsl:template match="db:term|db:Term">
  <b><xsl:apply-templates /></b>
</xsl:template>

<xsl:template match="db:screen|db:Screen">
  <table width="100%" border="0" cellpadding="0" cellspacing="0" align="center">
    <tr><td align="center">
      <table width="98%" border="0" cellspacing="0" cellpadding="2" bgcolor="#ababc0">
        <tr><td class="sdope-screen">
          <pre>
            <xsl:value-of select="." />
          </pre>
        </td></tr>
      </table>
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="db:para|db:Para">
  <xsl:param name="class">text<xsl:value-of select="count(ancestor::db:sect|ancestor::db:Sect|ancestor::db:Sect1|ancestor::db:Sect2|ancestor::db:Sect3|ancestor::db:Sect4|ancestor::db:Sect5)" /></xsl:param>

  <p>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:apply-templates />
  </p>
</xsl:template>

<xsl:template match="db:bf|db:Bf">
  <b><xsl:apply-templates /></b>
</xsl:template>

<xsl:template match="db:ulink">
  <xsl:param name="class">sdope-link</xsl:param>
  <a>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:attribute name="href"><xsl:value-of select="@url" /></xsl:attribute>

    <xsl:choose>
      <xsl:when test="child::text()">
        <xsl:apply-templates />
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@url" />
      </xsl:otherwise>
    </xsl:choose>
  </a>
</xsl:template>

</xsl:stylesheet>
