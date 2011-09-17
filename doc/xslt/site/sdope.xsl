<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" extension-element-prefixes="mxslt"
	xmlns:mxslt="http://www.mod-xslt2.com/ns/1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:db="http://www.masobit.net/ns/sdope/1.0"
	xmlns:s="http://www.masobit.net/ns/site/1.0"
	xmlns="http://www.w3.org/TR/rec-html40">

<xsl:import href="../sdope/docbook.xsl" />

  <!-- Override standard docbook processing -->
<xsl:template name="index" /> 
<xsl:template match="db:author" /> 

  <!-- Main body template -->
<xsl:template match="db:article|db:Article|db:ARTICLE">
  <xsl:choose>
    <xsl:when test="$realDocument/db:sect|$realDocument/db:Sect|$realDocument/db:Sect1">
      <xsl:choose>
        <xsl:when test="element-available('mxslt:value-of')">
	  <xsl:variable name="display">
	    <mxslt:value-of select="$GET[sect]" />
	  </xsl:variable>
	  
	  <xsl:choose>
	    <xsl:when test="$display = ''">
	      <xsl:call-template name="display">
	        <xsl:with-param name="last" 
			select="count($realDocument/db:sect|$realDocument/db:Sect|$realDocument/db:Sect1)" />
	        <xsl:with-param name="display">1</xsl:with-param>
              </xsl:call-template>
	    </xsl:when>

	    <xsl:when test="$display = 'all' or $display = '0'">
	      <xsl:apply-imports />
	      <!-- <xsl:apply-templates select="$realDocument" /> -->
	    </xsl:when>

	    <xsl:otherwise>
	      <xsl:call-template name="display">
	        <xsl:with-param name="last" 
			select="count($realDocument/db:sect|$realDocument/db:Sect|$realDocument/db:Sect1)" />
	        <xsl:with-param name="display" select="$display" />
              </xsl:call-template>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:apply-imports />
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>

    <xsl:otherwise>
      <xsl:apply-imports />
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Inizio index destra -->

<xsl:template name="sideindex">
<!--  <table width="100%" border="0" height="10"><tr><td></td></tr></table>
  <table width="95%" align="right" border="0" bgcolor="#000000" cellspacing="0" cellpadding="1">
    <tr><td>
      <table width="100%" border="0" bgcolor="#FFFFFF" cellspacing="0" cellpadding="0">
        <tr><td>
          <table width="95%" align="center" border="0" cellspacing="0" cellpadding="0">
            <tr><td> -->
	    <hr />
              <xsl:apply-templates select="$realDocument/db:sect|$realDocument/db:Sect|$realDocument/db:Sect1" mode="index" />
<!--            </td></tr>
         </table>
       </td></tr>
      </table>
    </td></tr>
  </table> -->
</xsl:template>

<xsl:template name="index-link">
<table width="100%" border="0" cellpadding="0" cellspacing="0">
<tr><td width="1" valign="top">
  <a>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:attribute name="href">
      <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:number level="single" count="/db:article/db:sect|/db:Article/db:sect|/db:article/db:Sect|db:article/db:Sect1|/db:Article/db:Sect|/db:Article/db:Sect1" />#<xsl:number level="multiple" count="db:sect|db:Sect|db:Sect1" /></xsl:attribute>
    <b><xsl:number level="multiple" count="db:sect|db:Sect|db:Sect1" />&#160;</b>
  </a></td>
  <td valign="top"><a>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:attribute name="href">
      <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:number level="single" count="/db:article/db:sect|/db:Article/db:sect|/db:article/db:Sect|db:article/db:Sect1|/db:Article/db:Sect|/db:Article/db:Sect1 " />#<xsl:number level="multiple" count="sect" /></xsl:attribute>
    <xsl:value-of select="db:title/.|db:Title/." />
  </a>
</td></tr>
</table>
</xsl:template>

<xsl:template match="db:sect|db:Sect|db:Sect1" mode="index">
  <xsl:param name="class">sdope-index<xsl:value-of select="count(ancestor-or-self::db:sect|ancestor-or-self::db:Sect|ancestor-or-self::db:Sect1)" /></xsl:param>

  <table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:call-template name="index-link">
      <xsl:with-param name="class" select="$class" />
    </xsl:call-template>
    
    <xsl:if test="db:sect|db:Sect|db:Sect1">
      <table width="90%" align="right" border="0" cellspacing="0" cellpadding="0"><tr><td>
        <xsl:apply-templates select="db:sect|db:Sect|db:Sect1" mode="index" />
      </td></tr></table>
    </xsl:if>
  </td></tr></table>
</xsl:template>
<!-- Fine index destra -->


<xsl:template name="navbar">
  <xsl:if test="$realDocument/db:sect[position()=$display - 1] or $realDocument/db:Sect[position()=$display - 1] or $realDocument/db:Sect1[position()=$display - 1]">
    <a class="sdope-first">
      <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?sect=1</xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.first" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/first.gif</xsl:attribute>
      </img>
    </a>

    <a class="sdope-prev">
      <xsl:attribute name="href"> 
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$display - 1" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.prev" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/prev.gif</xsl:attribute>
      </img>
    </a>
  </xsl:if>

  <xsl:if test="$realDocument/db:sect[position()=$display + 1] or $realDocument/db:Sect[position()=$display + 1] or $realDocument/db:Sect1[position()=$display + 1]">
    <a class="sdope-next">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$display + 1" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.next" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/next.gif</xsl:attribute></img>
    </a>
    <a class="sdope-last">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$last" /></xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.last" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/last.gif</xsl:attribute></img>
    </a>
  </xsl:if>
</xsl:template>

<xsl:template name="display">
  <table width="100%" height="100%" border="0" cellspacing="0" cellpadding="0">
    <tr><td class="sdope" valign="top">
      <xsl:apply-templates select="$realDocument/db:articleinfo|$realDocument/db:ArticleInfo|$realDocument/db:ArtHeader|$realDocument/db:artheader" />
    </td></tr>

    <tr><td valign="top" align="right">
      <xsl:call-template name="navbar">
        <xsl:with-param name="display" select="$display" />
        <xsl:with-param name="last" select="$last" />
      </xsl:call-template>
    </td></tr>

    <tr><td height="100%" valign="top">
      <xsl:apply-templates select="$realDocument/db:sect[position()=$display]|$realDocument/db:Sect1[position()=$display]|$realDocument/db:Sect[position()=$display]" />
    </td></tr>

    <tr><td valign="bottom" align="right">
      <xsl:call-template name="navbar">
        <xsl:with-param name="display" select="$display" />
        <xsl:with-param name="last" select="$last" />
      </xsl:call-template>
    </td></tr> 
    <tr><td height="10"></td></tr>
  </table>
</xsl:template>

<xsl:template match="s:right">
  <xsl:if test="($realDocument/db:sect or $realDocument/db:Sect or $realDocument/db:Sect1) and element-available('mxslt:value-of')">
    <tr><td>
      <xsl:call-template name="sideindex" />
    </td></tr>
  </xsl:if>

  <xsl:apply-imports />
</xsl:template>

</xsl:stylesheet>
