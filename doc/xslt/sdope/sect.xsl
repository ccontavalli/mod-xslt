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
 
<xsl:template match="db:sect|db:Sect|db:Sect1|db:Sect2|db:Sect3|db:Sect4|db:Sect5">
  <xsl:param name="depth"><xsl:value-of select="count(ancestor-or-self::db:sect|ancestor-or-self::db:Sect|ancestor-or-self::db:Sect1|ancestor-or-self::db:Sect2|ancestor-or-self::db:Sect3|ancestor-or-self::db:Sect4|ancestor-or-self::db:Sect5)" /></xsl:param>
  <div>
    <xsl:attribute name="class">sdope-sect<xsl:value-of select="$depth" /></xsl:attribute>

    <xsl:number level="multiple" count="db:sect|db:Sect|db:Sect1|db:Sect2|db:Sect3|db:Sect4|db:Sect5" />&#160;
    <a>
      <xsl:attribute name="class">sdope-sect<xsl:value-of select="$depth" /></xsl:attribute>
      <xsl:attribute name="name"><xsl:number level="multiple" count="db:sect|db:Sect|db:Sect1|db:Sect2|db:Sect3|db:Sect4|db:Sect5" /></xsl:attribute>
      <xsl:value-of select="db:title/.|db:Title/." />
    </a>
  </div>

  <div>
    <xsl:attribute name="class">sdope-text<xsl:value-of select="$depth" /></xsl:attribute>
    <xsl:apply-templates select="db:para|db:Para|db:Sect|db:Sect1|db:Sect2|db:Sect3|db:Sect4|db:Sect5" />
  </div>
</xsl:template>

 <!-- Old stuff
<xsl:template match="sect">
  <br /><br />

  <table width="100%" border="0" cellspacing="0" cellpadding="1" bgcolor="#4A7Ac9">
    <tr><td>
      <table width="100%" border="0" cellspacing="0" cellpadding="5" bgcolor="#CEDEFA" class="sdope">
        <tr><td>
          <font size="2"><b>
	    <xsl:number level="multiple" count="sect" />&#160;
            <a>
	      <xsl:attribute name="name"><xsl:value-of select="@id" /></xsl:attribute>
              <xsl:attribute name="name"><xsl:number level="multiple" count="sect" /></xsl:attribute>
              <xsl:value-of select="title/." />
	    </a>
          </b></font>
	  <br />
        </td></tr>
      </table>
    </td></tr>
  </table>

  <br />
  <xsl:apply-templates />
</xsl:template>

<xsl:template match="sect/title">
</xsl:template>


<xsl:template match="sect/sect">
  <br /><br />
  <h5>
    <xsl:number level="multiple" count="sect" />&#160;
    <a>
      <xsl:attribute name="name"><xsl:value-of select="@id" /></xsl:attribute>
      <xsl:attribute name="name"><xsl:number level="multiple" count="sect" /></xsl:attribute>
      <xsl:value-of select="title/." />
    </a>
  </h5>

  <xsl:apply-templates />
</xsl:template>

<xsl:template match="sect/sect/sect">
  <br /><br />
  <i><h5>
    <xsl:number level="multiple" count="sect" />&#160;
    <a> 
      <xsl:attribute name="name"><xsl:value-of select="@id" /></xsl:attribute>
      <xsl:attribute name="name"><xsl:number level="multiple" count="sect" /></xsl:attribute>
      <xsl:value-of select="title/." />
    </a>
  </h5></i>

  <xsl:apply-templates />
</xsl:template>
-->

</xsl:stylesheet>
