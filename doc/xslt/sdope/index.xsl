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

<xsl:template name="index">
  <xsl:apply-templates select="/db:article/db:sect" mode="index" />
</xsl:template>

<xsl:template name="index-link">
  <a class="sdope">
    <xsl:attribute name="href">#<xsl:number level="multiple" count="sect" /></xsl:attribute>
    <b><xsl:number level="multiple" count="sect" /></b>
    &#160;<xsl:value-of select="db:title/." />
  </a>
</xsl:template>

<xsl:template match="db:sect|db:Sect|db:SECT|db:Sect1|db:Sect2|db:Sect3" mode="index">
  <xsl:param name="class">sdope-index<xsl:value-of select="count(ancestor-or-self::db:sect)" /></xsl:param>

  <li>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:call-template name="index-link">
      <xsl:with-param name="class" select="$class" />
    </xsl:call-template>
    
    <xsl:if test="db:sect">
      <ul>
        <xsl:apply-templates select="db:sect" mode="index" />
      </ul>
    </xsl:if>
  </li>
</xsl:template>

</xsl:stylesheet>
