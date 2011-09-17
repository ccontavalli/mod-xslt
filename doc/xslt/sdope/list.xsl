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

<xsl:template match="db:itemizedlist|db:ItemizedList">
  <xsl:param name="class">sdope-itemizedlist</xsl:param>

  <ul>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:apply-templates match="db:listitem|db:ListItem" />
  </ul>
</xsl:template>

<!--
<xsl:template match="variablelist">
  <ul><xsl:apply-templates /></ul>
</xsl:template>

<xsl:template match="varlistentry">
  <li><xsl:apply-templates /></li>
</xsl:template>
-->

<xsl:template match="db:listitem|db:ListItem">
  <xsl:param name="class">sdope-listitem</xsl:param>

  <li> 
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:apply-templates />
  </li>
</xsl:template>

<xsl:template match="db:orderedlist|db:OrderedList">
  <ul><xsl:apply-templates /></ul>
</xsl:template>

<xsl:template match="db:orderedlist/db:listitem|db:OrderedList/db:listitem|db:orderedlist/db:ListItem|db:OrderedList/db:ListItem">
  <li class="sdope-orderedlist"><!-- <b><xsl:number level="single" count="db:listitem|db:ListItem" /></b>&#160; -->
    <xsl:apply-templates />
  </li>
</xsl:template>

<xsl:template match="db:variablelist|db:VariableList">
  <table width="100%"><tr><td class="sdope-varlistentry">
    <xsl:apply-templates select="db:VarListEntry|db:varlistentry" />
  </td></tr></table>
</xsl:template>

<xsl:template match="db:varlistentry|db:VarListEntry">
  <table width="100%">
    <tr><td class="sdope-term">
      <xsl:apply-templates select="db:term|db:Term" />
    </td></tr>
  </table>
  <table>
    <tr>
      <td width="10">&#160;</td>
      <td class="sdope-listitem">
        <xsl:apply-templates select="db:listitem|db:ListItem" />
      </td>
    </tr>
  </table>
</xsl:template>

<xsl:template match="db:varlistentry/db:listitem|db:varlistentry/db:ListItem|db:VarListEntry/db:ListItem|db:VarListEntry/db:listitem">
  <xsl:apply-templates />
</xsl:template>

</xsl:stylesheet> 
