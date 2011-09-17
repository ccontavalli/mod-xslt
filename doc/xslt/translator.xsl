<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>

<!-- <!DOCTYPE xslt SYSTEM "${dtdRoot}/lat1.dtd"> -->

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" extension-element-prefixes="mxslt"
    xmlns:mxslt="http://www.mod-xslt2.com/ns/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:s="http://www.masobit.net/ns/site/1.0"
    xmlns="http://www.w3.org/TR/rec-html40">

<xsl:import href="site/sdope.xsl" /> 
<xsl:import href="site/faq.xsl" /> 
<xsl:import href="site/repository.xsl" /> 

<xsl:output method="html" 
      omit-xml-declaration="yes" 
      encoding="ISO-8859-1" 
      indent="yes" 
      doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" />

<xsl:strip-space elements="*" />

<xsl:include href="config.xsl" />

<xsl:param name="realDocument" select="child::*" />

<xsl:template match="/">
  <xsl:choose>

    <xsl:when test="element-available('mxslt:value-of')">
      <xsl:apply-templates select="document('skel.xml')/s:page">
      </xsl:apply-templates>
    </xsl:when>

    <xsl:otherwise>
      <xsl:apply-templates select="document('skel.xml')/s:page" />
    </xsl:otherwise>

  </xsl:choose>
</xsl:template>

<xsl:template match="s:right">
  <table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">
    <tr><td height="1%" align="left" valign="top">
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr><td align="left">
          <xsl:apply-templates select="s:menu[@position='up']" />
        </td></tr>

        <xsl:apply-imports />
      </table>
    </td></tr>
    <tr><td height="1%" align="center" valign="bottom">
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr><td align="left">
          <xsl:apply-templates select="s:menu[@position='down']" />
        </td></tr>
	<tr><td align="left" height="5"> </td></tr>
      </table>
    </td></tr>
  </table> 
</xsl:template>

<xsl:template match="s:left">
  <xsl:apply-imports />

  <table width="127" height="100%" border="0" cellpadding="0" cellspacing="0">
    <tr><td height="1%" align="center" valign="top">
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr><td align="right">
          <xsl:apply-templates select="s:menu[@position='up']" />
        </td></tr>
      </table>
    </td></tr>
    <tr><td height="1%" align="center" valign="bottom">
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr><td align="right">
          <xsl:apply-templates select="s:menu[@position='down']" />
        </td></tr>
      </table>
    </td></tr>
  </table> 
</xsl:template>

<xsl:template match="s:section">
  <html>
    <head>
      <title><xsl:value-of select="@title" /></title>
      <link rel="stylesheet" type="text/css">
        <xsl:attribute name="href"><xsl:value-of select="$httpRoot" />/css/modules.css</xsl:attribute>
      </link>
    </head>

    <body class="site"> 
      <table width="760" height="38" border="0" cellpadding="0" cellspacing="0">
          <!-- First row, containing mod-xslt logo -->
        <tr><td class="site-banner" /></tr>

          <!-- White spacer, between first row and body -->
        <tr><td height="2" /></tr>

        <tr><td>
          <table width="760" height="500" border="0" cellpadding="0" cellspacing="0">

             <!-- black higher bar -->
            <tr><td>
              <table width="760" height="25%" border="0" cellpadding="0" cellspacing="0">
                <tr valign="middle"> 
                  <td height="20" colspan="3" bgcolor="#000000"> 
                    <table width="100%" border="0" cellspacing="0" cellpadding="0">
                      <tr>
                        <td width="18%" height="20" align="left">
                          <xsl:apply-templates select="s:nav[@label='section']" />
                        </td>

                        <td>
                          <xsl:apply-templates select="s:nav[@label='up']" />
                        </td>
                      </tr>
                    </table> 
                  </td>
	        </tr>
              </table>
            </td></tr>

             <!-- Main body -->
            <tr><td>
              <table width="760" height="460" border="0" cellpadding="0" cellspacing="0">
                <tr>
                  <td width="127" valign="top">
		    <xsl:apply-templates select="s:left" />
		  </td>
                  
		    <!-- Left Blue line -->
                  <td height="100%" width="5" align="right" valign="bottom">
		    <xsl:if test="$graphic.usebars = 1">
		      <img border="0" height="100%" width="1">
		        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/spazblu.gif</xsl:attribute>
		      </img>
		    </xsl:if>
                  </td>

		    <!-- Main Body -->
                  <td valign="top">
		    <table width="100%" height="100%">
		      <tr><td class="site-main">
		        <xsl:apply-templates select="$realDocument" />
                        <!-- <xsl:call-template name="body" /> -->
		      </td></tr>
		    </table>
                  </td>

                    <!-- Right Blue Line -->
                  <td height="100%" width="5" align="left" valign="bottom">
		    <xsl:if test="$graphic.usebars = 1">
		      <img border="0" height="100%" width="1">
		        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/spazblu.gif</xsl:attribute>
		      </img>
		    </xsl:if>
                  </td>

                    <!-- Right column -->
                  <td width="155" valign="top">
		    <xsl:apply-templates select="s:right" />
                  </td>
                </tr>
              </table>
            </td></tr>

              <!-- Black lower bar -->
            <tr><td>
              <table width="760" height="25%" border="0" cellpadding="0" cellspacing="0">
                <tr valign="middle">
		<td height="20" colspan="2" bgcolor="#000000" class="site-whitelinks">
                      <xsl:apply-templates select="s:nav[@label='down']" />
                </td></tr>
              </table>
            </td></tr>
          </table>
        </td></tr>
      </table>
    </body>
  </html>
</xsl:template>

<xsl:template match="s:nav[@label='section']">
  <div class="site-red">&#160;<xsl:value-of select="@title" /></div>
</xsl:template>

<xsl:template name="mklink">
  <xsl:param name="class">site-whitelinks</xsl:param>

  <a>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:choose>
      <xsl:when test="starts-with($href, 'http://')">
        <xsl:attribute name="href"><xsl:value-of select="$href" /></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="href"><xsl:value-of select="$httpRoot" />/<xsl:value-of select="$href" /></xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:value-of select="$title" />
  </a>
</xsl:template>

<xsl:template match="s:link">
  <xsl:call-template name="mklink">
    <xsl:with-param name="class">site-whitelinks</xsl:with-param>
    <xsl:with-param name="href" select="@href" />
    <xsl:with-param name="title" select="@title" />
  </xsl:call-template>&#160;|&#160;
</xsl:template>

<xsl:template match="s:copyright">
  &#160;&#160;©&#160;<xsl:value-of select="." />&#160;|&#160; 
</xsl:template>

<xsl:template match="s:nav[@label='up']">
        <xsl:apply-templates select="s:link" />
</xsl:template>

<xsl:template match="s:nav[@label='down']">
        <xsl:apply-templates select="s:copyright" />

        <xsl:apply-templates select="s:link" />
</xsl:template>

<xsl:template match="s:submenu">
  <tr align="left" width="100%">
    <td class="site-submenu" valign="top">&#187;</td>
    <td align="left" width="100%">
      <xsl:call-template name="mklink">
        <xsl:with-param name="class">site-submenu</xsl:with-param>
        <xsl:with-param name="href" select="@href" />
	<xsl:with-param name="href">
	  <xsl:value-of select="@href" />
	</xsl:with-param>
	<xsl:with-param name="title" select="@title" />
      </xsl:call-template>
    </td>
  </tr>
</xsl:template>

<xsl:template match="s:menu">
  <table width="123" border="0" cellspacing="0" cellpadding="0" >
    <tr><td height="1">
      <img border="0">
        <xsl:attribute name="src">
	  <xsl:value-of select="$httpRoot" />/images/<xsl:value-of select="@label" />.gif
	</xsl:attribute>
        <xsl:attribute name="alt"><xsl:value-of select="@title" /></xsl:attribute>
      </img>
    </td></tr>
  </table>
</xsl:template>

<xsl:template match="s:menu[@type='text']">
  <table width="123" border="0" cellspacing="0" cellpadding="0" >
    <tr colspan="2">
      <xsl:attribute name="height"><xsl:value-of select="$menu.vspace" /></xsl:attribute>
    </tr>
    <tr>
      <td valign="top" bgcolor="#808988" width="1">
        <xsl:if test="$menu.arrow = 0">
        <img border="0" alt="  - ">
	  <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/optionsimb.gif</xsl:attribute> 
	</img>
	</xsl:if>
      </td>

      <td width="100%" height="14" align="left" class="site-whitelinks" bgcolor="#8899A6">
             <strong><xsl:value-of select="@title" /></strong>
      </td>
    </tr>
    <tr><td colspan="2">
      <table width="100%" border="0" cellspacing="0" bgcolor="#808988">
        <xsl:attribute name="cellpadding"><xsl:value-of select="$menu.border" /></xsl:attribute>
        <tr><td> 
          <table width="100%" border="0" align="left" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
            <tr><td>
              <table border="0" cellspacing="0" cellpadding="1" width="100%">
    	        <xsl:apply-templates select="s:submenu" />
              </table>    
            </td></tr>
          </table>
        </td></tr>
      </table>
    </td></tr>
  </table>
</xsl:template>

</xsl:stylesheet>
