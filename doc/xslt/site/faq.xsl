<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
   mod-xslt - Copyright (C) 2002-2008 
   		Marco Merlin <mmerlin at masobit.net> and
		Carlo Contavalli <ccontavalli at inscatolati.net> -->

<xsl:stylesheet version="1.0" extension-element-prefixes="mxslt"
	xmlns:mxslt="http://www.mod-xslt2.com/ns/1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:s="http://www.masobit.net/ns/site/1.0"
	xmlns:faq="http://www.masobit.net/ns/faq/1.0"
	xmlns="http://www.w3.org/TR/rec-html40">

  <xsl:import href="../faq/faq.xsl" />

  <!-- override copyright notice -->
<xsl:template match="faq:copyright" />

<xsl:template name="faq:link-question">
  <a>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?question=<xsl:value-of select="count(ancestor::*/preceding-sibling::*/descendant-or-self::faq:entry)+1" />&amp;sect=<xsl:value-of select="count(ancestor::faq:sect/preceding-sibling::faq:sect[parent::faq:content])+1" /></xsl:attribute>
    <xsl:value-of select="$value" />
  </a>
</xsl:template>


  <!-- override default body -->
<xsl:template match="faq:faq">
  <span class="faq">
    <a name="index" />
    <table width="100%">
      <tr>
        <td align="left" class="faq"><h1 class="faq-title"><xsl:value-of select="@module" />&#160;FAQ</h1></td>
	<td align="right" valign="top">
	  <b class="faq-version">version&#160;<xsl:value-of select="@version" /></b>
	</td>
      </tr>
    </table>

    <p class="faq-body">
        <!-- check if mod-xslt extensions are available -->
      <xsl:choose>
        <xsl:when test="element-available('mxslt:value-of')">
	  <xsl:variable name="sect">
	    <mxslt:value-of select="$GET[sect]" />
	  </xsl:variable>
	  <xsl:variable name="question">
	    <mxslt:value-of select="$GET[question]" />
	  </xsl:variable>

	  <xsl:choose>
	    <xsl:when test="($question = 'all' or $question = '0') and $sect > 0">
	      <xsl:call-template name="showsect">
	        <xsl:with-param name="sect" select="0" />
              </xsl:call-template>
	      <xsl:apply-templates mode="text" select="faq:content/faq:sect[position()=$sect]/faq:entry" />
	    </xsl:when>

	    <xsl:when test="$question != '' and $question > 0 and $question != 'all'">
	      <xsl:call-template name="showquestion">
               <xsl:with-param name="question" select="$question" />
             </xsl:call-template>

	    </xsl:when>

	    <xsl:when test="$sect = ''">
	      <xsl:call-template name="showsect">
                <xsl:with-param name="sect">1</xsl:with-param>
              </xsl:call-template>  
	    </xsl:when>

	    <xsl:when test="$sect = '0' or $sect = 'all'"  >
	      <xsl:apply-templates mode="index" select="faq:content" />
	      <xsl:apply-templates mode="text" select="faq:content" />
	    </xsl:when>

	    <xsl:otherwise>
	      <xsl:call-template name="showsect">
	        <xsl:with-param name="sect" select="$sect" />
	      </xsl:call-template>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:apply-templates mode="index" select="faq:content" />
	  <xsl:apply-templates mode="text" select="faq:content" />
	</xsl:otherwise>
      </xsl:choose>
    </p>
  </span>
</xsl:template>


<xsl:template name="navbarsect">
  <xsl:if test="$realDocument/faq:content/faq:sect[position()=$sect - 1]">
    <a class="faq-first">
      <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?sect=1</xsl:attribute>
      <img border="0"> 
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/first.gif</xsl:attribute>
        <xsl:attribute name="alt"><xsl:value-of select="$text.first" /></xsl:attribute>
      </img>
    </a>

    <a class="faq-prev">
      <xsl:attribute name="href"> 
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$sect - 1" />
      </xsl:attribute>
      <img border="0"> 
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/prev.gif</xsl:attribute>
        <xsl:attribute name="alt"><xsl:value-of select="$text.prev" /></xsl:attribute>
      </img>
    </a>
  </xsl:if>

  <xsl:if test="$realDocument/faq:content/faq:sect[position()=$sect + 1]">
    <a class="faq-next">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$sect + 1" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/next.gif</xsl:attribute>
        <xsl:attribute name="alt"><xsl:value-of select="$text.next" /></xsl:attribute>
      </img>
    </a>
    <a class="faq-last">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$last" /></xsl:attribute>
      <img border="0">
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/last.gif</xsl:attribute>
        <xsl:attribute name="alt"><xsl:value-of select="$text.last" /></xsl:attribute>
      </img>
    </a>
  </xsl:if>
</xsl:template>

<xsl:template name="navbarquestion">
<!--   First: <xsl:value-of select="$first" /><br />
   Last: <xsl:value-of select="$last" /><br />  -->
  <xsl:if test="$first &lt; $question"> 
    <a class="faq-index">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$sect" />&amp;question=<xsl:value-of select="$first" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.first" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/first.gif</xsl:attribute>
      </img>
    </a>

    <a class="faq-index">
      <xsl:attribute name="href"> 
        <mxslt:value-of select="$MODXSLT[httpurl]" />?question=<xsl:value-of select="$question - 1" />&amp;sect=<xsl:value-of select="$sect" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.prev" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/prev.gif</xsl:attribute>
      </img>
    </a>
  </xsl:if> 

  <a class="faq-index">
    <xsl:attribute name="href">
      <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$sect" />
    </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.up" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/up.gif</xsl:attribute>
      </img>
  </a>

  <xsl:if test="$question &lt; $last">
    <a class="faq-index">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?question=<xsl:value-of select="$question + 1" />&amp;sect=<xsl:value-of select="$sect" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.next" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/next.gif</xsl:attribute>
      </img>
    </a>
    <a class="faq-index">
      <xsl:attribute name="href">
        <mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:value-of select="$sect" />&amp;question=<xsl:value-of select="$last" />
      </xsl:attribute>
      <img border="0">
        <xsl:attribute name="alt"><xsl:value-of select="$text.last" /></xsl:attribute>
        <xsl:attribute name="src"><xsl:value-of select="$httpRoot" />/images/last.gif</xsl:attribute>
      </img>
    </a>
  </xsl:if>
</xsl:template>



<xsl:template name="showsect">
  <tr><td align="right" valign="top">

  <xsl:call-template name="navbarsect">
    <xsl:with-param name="sect" select="$sect" />
    <xsl:with-param name="last" select="count($realDocument/faq:content/faq:sect)" />
  </xsl:call-template>

  </td></tr>
  <tr><td height="100%" valign="top">

  <xsl:apply-templates mode="index" select="faq:content/faq:sect[position()=$sect]" />

  </td></tr>

  <tr><td align="right" valign="bottom">

  <xsl:call-template name="navbarsect">
    <xsl:with-param name="sect" select="$sect" />
    <xsl:with-param name="last" select="count($realDocument/faq:content/faq:sect)" />
  </xsl:call-template>

  </td></tr>
  <tr><td height="10"></td></tr>
</xsl:template>


<xsl:template name="showquestion">
  <xsl:param name="sect"><mxslt:value-of select="$GET[sect]" /></xsl:param>

  <xsl:param name="first" select="$question - count(/descendant::faq:entry[position()=$question]/preceding-sibling::*)" />
  <xsl:param name="last" select="$question + count(/descendant::faq:entry[position()=$question]/following-sibling::*)" />

  <tr><td align="right" valign="top">

  <xsl:call-template name="navbarquestion">
    <xsl:with-param name="sect" select="$sect" />
    <xsl:with-param name="question" select="$question" />
    <xsl:with-param name="first" select="$first" />
    <xsl:with-param name="last" select="$last" />
  </xsl:call-template>

  </td></tr>
  <tr><td height="100%" valign="top">

  <xsl:apply-templates mode="text" select="/descendant::faq:entry[position()=$question]" />

  </td></tr>
  <tr><td align="right" valign="bottom">

  <xsl:call-template name="navbarquestion">
    <xsl:with-param name="sect" select="$sect" />
    <xsl:with-param name="question" select="$question" />
    <xsl:with-param name="first" select="$first" />
    <xsl:with-param name="last" select="$last" />
  </xsl:call-template>
</td></tr>

<tr><td height="10"></td></tr>

</xsl:template>



<xsl:template match="s:right">
  <xsl:param name="class">faq-index<xsl:value-of select="count(ancestor-or-self::faq:sect)" /></xsl:param>

  <xsl:if test="element-available('mxslt:value-of') and $realDocument/faq:content">
    <tr><td>
      <hr /> 
      <table width="100%" cellspacing="0" cellpadding="0" border="0">
        <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>

        <xsl:apply-templates mode="sideindex" select="$realDocument/faq:content" />
      </table>
    </td></tr>
  </xsl:if>

  <xsl:apply-imports /> 
</xsl:template>

<xsl:template match="faq:sect" mode="sideindex">

    <xsl:param name="class">faq-index<xsl:value-of select="count(ancestor-or-self::faq:sect)" /></xsl:param>

    <tr>
      <td width="1" valign="top">
     <a>
       <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
       <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:number count="/faq:faq/faq:content/faq:sect" level="single" /></xsl:attribute>
       <b><xsl:number level="multiple" count="faq:sect" />&#160;</b>
     </a>
      </td>
      <td>
     <a>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
     
       <xsl:attribute name="href"><mxslt:value-of select="$MODXSLT[httpurl]" />?sect=<xsl:number count="/faq:faq/faq:content/faq:sect" level="single" /></xsl:attribute>
       <xsl:value-of select="@title" />
</a>

    </td></tr>
    <!--
    <xsl:if test=".//faq:sect">
      <tr><td><table>
        <tr><td>
	  <xsl:attribute name="width"><xsl:value-of select="$faq.index.indent" /></xsl:attribute>&#160;
	</td>
        <td><table><xsl:apply-templates mode="sideindex" select=".//faq:sect" /></table></td></tr>
      </table></td></tr>
    </xsl:if> 
    -->
</xsl:template>



</xsl:stylesheet>
