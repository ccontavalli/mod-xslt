<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:rep="http://www.masobit.net/ns/repository/1.0"
  xmlns="http://www.w3.org/TR/rec-html40">

<xsl:output method="html" 
            omit-xml-declaration="yes" 
	    encoding="ISO-8859-1" 
	    indent="yes" 
	    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" />
 
<xsl:template match="/">
  <html>
    <head>
      <title> 
        <xsl:value-of select="/rep:repository/rep:description/rep:name" /> --
        <xsl:value-of select="/rep:repository/rep:description/rep:home" />
      </title>

      <link href="./css/repository.css" rel="stylesheet" type="text/css"/>
    </head>

    <body topMargin="0" leftMargin="0" marginwidth="0" marginheight="0">
      <xsl:apply-templates select="rep:repository" />
    </body>
  </html>  
</xsl:template>

<xsl:template match="rep:repository">
  <table width="95%" align="center" border="0" cellspacing="0" cellpadding="0">
    <tr><td>
      <xsl:apply-templates />
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="rep:description/rep:link">
  <tr>
    <td class="rep-desc" width="1%"><xsl:value-of select="." />:</td>
    <td>
      <a class="rep-desc">
        <xsl:attribute name="href"><xsl:value-of select="@href" /></xsl:attribute>
        <xsl:value-of select="@href" />
      </a>
    </td>
  </tr> 
</xsl:template>

<xsl:template match="rep:description">
  <table width="100%" bgcolor="#000000" align="center" border="0" cellspacing="0" cellpadding="1">
  <tr><td>
  <table width="100%" bgcolor="#FFFFFF" align="center" border="0" cellspacing="0" cellpadding="2">
    <tr>
      <td class="rep-desc" width="1%">Name: </td>
      <td class="rep-desc"><xsl:value-of select="rep:name" /></td>
    </tr>
    <tr>
      <td class="rep-desc" width="1%">Maintainer: </td>
      <td class="rep-desc"><xsl:value-of select="rep:maintainer" /></td>
    </tr>

    <xsl:apply-templates select="rep:link" />
  </table>
  </td></tr></table>
</xsl:template>

<xsl:template match="rep:version[@type='mmp']">
  <!--<xsl:value-of select="rep:label" />&#160; -->
  <xsl:value-of select="rep:major" />.<xsl:value-of select="rep:minor" />.<xsl:value-of select="rep:patchlevel" />
</xsl:template>

<xsl:template match="rep:version[@type='ymd']">
  (<xsl:value-of select="rep:label" />&#160;<xsl:value-of select="rep:ymd" />)
</xsl:template>

<xsl:template match="rep:format">
  <xsl:param name="format"><xsl:value-of select="@type" /></xsl:param>

  <tr><td class="rep-format">
    <a class="rep-format">
      <xsl:attribute name="href"><xsl:value-of select="ancestor::rep:release/rep:description/rep:base" />/<xsl:value-of select="@name" /></xsl:attribute>
      <xsl:value-of select="@name" />
    </a>
    <div class="rep-format">
    &#160;&#160;<xsl:value-of select="document('description.xml')/rep:formats/rep:download/rep:type[@label=$format]/rep:description" />
    </div>
  </td>
  <td align="right" class="rep-arch">
    <xsl:choose>
      <xsl:when test="@arch">
        <xsl:value-of select="@arch" /> 
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="document('description.xml')/rep:formats/rep:download/rep:type[@label=$format]/rep:arch" />
      </xsl:otherwise>
    </xsl:choose>
  </td>
  </tr>

  <tr><td colspan="2"><table border="0" cellspacing="0" cellpadding="0">
    <xsl:apply-templates />
  </table>
  <xsl:if test="following-sibling::rep:format">
    <hr />
  </xsl:if>
  </td></tr>
</xsl:template>

<xsl:template match="rep:url">
  <tr><td height="16" bgcolor="#757575">
    <a class="rep-white">
      <xsl:attribute name="href"><xsl:value-of select="@href" /></xsl:attribute>
      <xsl:value-of select="." />
    </a>
  </td></tr>

  <xsl:if test="../rep:checksum">
  <tr width="100%"><td><table cellspacing="0" cellpadding="0" width="100%">
    <xsl:apply-templates select="../rep:checksum" />
  </table></td></tr>
  </xsl:if>
</xsl:template>

<xsl:template match="rep:checksum">
  <xsl:param name="type"><xsl:value-of select="@type" /></xsl:param>
  <xsl:param name="class">rep-chk<xsl:value-of select="@type" /></xsl:param>

  <tr width="100%"><td width="10">&#160;</td><td>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:value-of select="document('description.xml')/rep:formats/rep:checksum/rep:type[@label=$type]/rep:name" />: 
    <xsl:choose>
      <xsl:when test="document('description.xml')/rep:formats/rep:checksum/rep:type[@label=$type]/@format = 'link'">
        <a>
	  <xsl:attribute name="class"><xsl:value-of select="$class" />-value</xsl:attribute>
	  <xsl:attribute name="href"><xsl:value-of select="ancestor::rep:release/rep:description/rep:base" />/<xsl:value-of select="." /></xsl:attribute>
	  <xsl:value-of select="." />
	</a>
      </xsl:when>

      <xsl:otherwise>
        <span><xsl:attribute name="class"><xsl:value-of select="$class" />-value</xsl:attribute><xsl:value-of select="." /></span>
      </xsl:otherwise>
    </xsl:choose>
  </td></tr>
</xsl:template>

<xsl:template match="rep:checksum[@href]">
<tr width="100%"><td width="10">&#160;</td><td>
    <a>
      <xsl:attribute name="class">rep-chk<xsl:value-of select="@type" /></xsl:attribute>
      <xsl:attribute name="href"><xsl:value-of select="@href" /></xsl:attribute>
      <xsl:value-of select="." />
    </a>
  </td></tr>
</xsl:template>

<xsl:template match="rep:download">
  <table width="95%" align="center" border="0" cellspacing="0" cellpadding="0">
      <xsl:apply-templates select="rep:format" />
  </table>
</xsl:template>

<xsl:template match="rep:release/rep:description">
  <table width="100%" align="center" border="0" bgcolor="#393939" cellspacing="0" cellpadding="0">
  <tr>
   <td height="20" class="rep-white"><xsl:value-of select="ancestor::rep:repository/rep:description/rep:name" /></td>
   <td width="1%" class="rep-white"><xsl:apply-templates select="rep:version[@type='mmp']" />&#160;</td>
   <td class="rep-white"><xsl:apply-templates select="rep:version[@type='ymd']" /></td>
   <td class="rep-white" align="right">
     <a class="rep-white">
       <xsl:attribute name="href">
         <!-- <xsl:value-of select="ancestor::rep:release/rep:description/rep:base" />/--><xsl:value-of select="rep:data/@href" /></xsl:attribute>
       <xsl:value-of select="rep:data/." />
     </a>
   </td>
  </tr></table>
</xsl:template>

<xsl:template match="rep:release">
  <table width="100%" bgcolor="#000000" align="center" border="0" cellspacing="0" cellpadding="1">
  <tr><td>

  <table class="rep-desc" width="100%" bgcolor="#FFFFFF" border="0" cellspacing="0" cellpadding="0">
    <tr><td>
      <xsl:apply-templates select="rep:description" />
      <table width="95%" class="rep-desc" cellpadding="5" cellspacing="0"><tr>
        
        <td>
	  <xsl:apply-templates select="rep:download" />
        </td>
      </tr></table>
    </td></tr>

    <xsl:if test="rep:update">
      <tr><td class="rep-update">
      <hr />
      <font color="green">Update:</font> <xsl:apply-templates select="rep:update" />
      </td></tr>
    </xsl:if>
  </table>
  </td></tr>
  </table>

  <xsl:apply-templates select="rep:changes" />
</xsl:template>

<xsl:template match="rep:entry"> 
  <li class="rep-change">
    <xsl:if test="rep:date">
      <p class="rep-change-date"><xsl:value-of select="rep:date" /></p>
    </xsl:if>
    <p class="rep-change-log"><xsl:value-of select="rep:log" /></p>
    <xsl:if test="rep:author">
      <p class="rep-change-author">(<xsl:value-of select="rep:author" />)</p>
    </xsl:if>
  </li>
</xsl:template>

<xsl:template match="rep:changes">
  <p class="rep-change-title">Changes in current version:</p>
  <ul class="rep-change">
    <xsl:apply-templates select="rep:entry" />
  </ul>
</xsl:template>

</xsl:stylesheet>
