<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" extension-element-prefixes="mxslt"
	xmlns:mxslt="http://www.mod-xslt2.com/ns/1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:rep="http://www.masobit.net/ns/repository/1.0"
	xmlns:s="http://www.masobit.net/ns/site/1.0"
	xmlns="http://www.w3.org/TR/rec-html40">

<xsl:import href="../rep/repository.xsl" />

<xsl:template name="rep:navbar">
  <xsl:param name="current">1</xsl:param>
  <xsl:param name="first">1</xsl:param>
  <xsl:param name="last"><xsl:value-of select="count(//rep:release)" /></xsl:param>

  <!-- DEBUG - first: <xsl:value-of select="$first" />, current: <xsl:value-of select="$current" />, last: <xsl:value-of select="$last" /><br />
  -->

  <xsl:if test="$current &gt; $first">
    <a class="rep-first">
      <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?version=<xsl:value-of select="$first" /></xsl:attribute>

      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.first" /></xsl:attribute>
	<xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/first.gif</xsl:attribute>
      </img>
    </a>

    <a class="rep-prev">
      <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?version=<xsl:value-of select="$current - 1" /></xsl:attribute>

      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.prev" /></xsl:attribute>
	<xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/prev.gif</xsl:attribute>
      </img>
    </a>
  </xsl:if>

  <xsl:if test="$current &lt; $last">
    <a class="rep-next">
      <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?version=<xsl:value-of select="$current + 1" /></xsl:attribute>

      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.next" /></xsl:attribute>
	<xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/next.gif</xsl:attribute>
      </img>
    </a>

    <a class="rep-last">
      <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?version=<xsl:value-of select="$last" /></xsl:attribute>

      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.last" /></xsl:attribute>
	<xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/last.gif</xsl:attribute>
      </img>
    </a>

  </xsl:if>
  
</xsl:template>

<xsl:template name="rep:display"> 
  <xsl:param name="display">1</xsl:param>

  <table width="100%">
    <tr><td align="right">
  <xsl:call-template name="rep:navbar">
    <xsl:with-param name="current"><xsl:value-of select="$display" /></xsl:with-param>
  </xsl:call-template>
  </td></tr>

  <tr><td align="center">
  <table width="95%" align="center" border="0" cellspacing="0" cellpadding="0">
    <tr><td>
      <xsl:apply-templates select="rep:release[position()=$display]" />
    </td></tr>
  </table>
  </td></tr>

  <tr><td align="right">
  <xsl:call-template name="rep:navbar">
    <xsl:with-param name="current"><xsl:value-of select="$display" /></xsl:with-param>
  </xsl:call-template>
  </td></tr></table>
</xsl:template>

<xsl:template match="rep:repository">
  <xsl:choose>
    <xsl:when test="element-available('mxslt:value-of')">
      <xsl:variable name="display">
        <mxslt:value-of select="$GET[version]" />
      </xsl:variable>

      <xsl:choose>
        <xsl:when test="$display = ''">
	  <xsl:call-template name="rep:display">
	    <xsl:with-param name="display">1</xsl:with-param>
	  </xsl:call-template>
	</xsl:when>

	<xsl:when test="$display = 'all' or $display = '0'">
	  <xsl:apply-imports />
	</xsl:when>

	<xsl:otherwise>
	  <xsl:call-template name="rep:display">
	    <xsl:with-param name="display"><xsl:value-of select="$display" /></xsl:with-param>
	  </xsl:call-template>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>

    <xsl:otherwise>
      <xsl:apply-imports />
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="rep:release" mode="index">
  <xsl:if test="position() &lt;= $rep.max.right">
  <tr><td>
  <a class="rep-sidelink">
    <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?version=<xsl:value-of select="count(preceding-sibling::rep:release) + 1" /></xsl:attribute>
    <div><xsl:value-of select="ancestor::rep:repository/rep:description/rep:name" />&#160;
    <xsl:apply-templates select="descendant::rep:version[@type='mmp']" /></div>
     &#160; &#160; <xsl:apply-templates select="descendant::rep:version[@type='ymd']" />
  </a>
  </td></tr>
  </xsl:if>

  <xsl:if test="position() = $rep.max.right + 1">
  <tr><td>
    <div class="rep-sidelink">[...]</div>
  </td></tr>
  </xsl:if>
</xsl:template>

<xsl:template match="s:right">
  <xsl:if test="element-available('mxslt:value-of')">
    <xsl:if test="$realDocument//rep:release">
    <tr><td>
    <hr />
      <table cellspacing="0" cellpadding="0">
      <xsl:apply-templates select="$realDocument//rep:release" mode="index" />
      </table>
    </td></tr>
    </xsl:if>
  </xsl:if>

  <xsl:apply-imports />
</xsl:template>

<xsl:template match="rep:repository/rep:description" /> 

</xsl:stylesheet>
