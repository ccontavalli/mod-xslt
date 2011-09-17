<?xml version="1.0" encoding="ISO-8859-1"?>



<xsl:stylesheet version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:faq="http://www.masobit.net/ns/faq/1.0"
	xmlns="http://www.w3.org/TR/rec-html40">

  <xsl:output 
    omit-xml-declaration="yes" encoding="ISO-8859-1" 
    method="html" indent="yes" 
    doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" />  

  <xsl:include href="./config.xsl" />

  <xsl:strip-space elements="*" /> 

  <xsl:template match="/">
  <html>
    <head>
      <title><xsl:value-of select="@module" /> FAQ - <xsl:value-of select="@version" /></title>
      <link href="./css/faq.css" rel="stylesheet" type="text/css" />
    </head>

    <body class="faq">
      <xsl:apply-templates />
    </body>
  </html>
  </xsl:template>

  <xsl:template match="faq:faq">
    <span class="faq">
      <a name="index" />
      <h1 class="faq-title"><xsl:value-of select="@module" />&#160;FAQ</h1>
      <b class="faq-version">version&#160;<xsl:value-of select="@version" />&#160;</b>
      <b class="faq-released">released&#160;<xsl:value-of select="@released" /></b>
      <p class="faq-body">
        <xsl:apply-templates select="faq:copyright" />
        <xsl:apply-templates mode="index" select="faq:content" />
        <xsl:apply-templates mode="text" select="faq:content" />
      </p>
    </span>
  </xsl:template>

  <xsl:template mode="index" match="faq:content">
    <table class="faq-index"><tr class="faq-index"><td class="faq-index">
      <xsl:apply-templates mode="index" select="faq:sect|faq:entry" />
    </td></tr></table>
  </xsl:template>

  <xsl:template mode="index" match="faq:entry">
    <xsl:param name="class">faq-index-entry<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
    <table>
      <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <xsl:apply-templates mode="index" select="faq:question" />
    </table> 
  </xsl:template>

  <xsl:template name="faq:link-question">
      <xsl:param name="class">faq-index-entry<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
      <xsl:param name="value">(ERROR)</xsl:param>
      <a>
        <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
        <xsl:attribute name="href">#<xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute>
        <xsl:value-of select="$value" />
      </a>
  </xsl:template>



  <xsl:template mode="index" match="faq:question">
    <xsl:param name="class">faq-index-question<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
    <tr>
      <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <td>
      <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <xsl:call-template name="faq:link-question">
        <xsl:with-param name="class"><xsl:value-of select="$class" /></xsl:with-param>
        <xsl:with-param name="value"><xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:with-param>
      </xsl:call-template>
    </td>
    <td>
      <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <xsl:call-template name="faq:link-question">
        <xsl:with-param name="class"><xsl:value-of select="$class" /></xsl:with-param>
        <xsl:with-param name="value"><xsl:apply-templates /></xsl:with-param>
      </xsl:call-template>
    </td>
    </tr>
  </xsl:template>


  <xsl:template mode="index" match="faq:sect">
    <xsl:param name="class">faq-index-sect<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
    <table>
      <tr><td>
        <table>
          <tr>
            <td>
              <a>
	        <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
                <xsl:attribute name="href">#<xsl:number level="multiple" count="faq:sect" /></xsl:attribute>
                <xsl:number level="multiple" count="faq:sect" />
              </a>
            </td>
            <td>
              <a>
	        <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
                <xsl:attribute name="href">#<xsl:number level="multiple" count="faq:sect" /></xsl:attribute>
                <xsl:value-of select="@title" />
              </a>
            </td>
          </tr>
        </table>
      </td></tr>
      <tr><td>
        <table>
          <tr>
            <td>
	      <xsl:attribute name="width"><xsl:value-of select="$faq.index.indent" /></xsl:attribute>&#160;</td>
            <td><xsl:apply-templates mode="index" select="faq:sect|faq:entry" /></td>
          </tr>
        </table>
      </td></tr>
    </table>
  </xsl:template>
  
  <xsl:template mode="text" match="faq:content">
    <table class="faq-text"><tr class="faq-text"><td class="faq-text">
      <xsl:apply-templates mode="text" select="faq:sect|faq:entry" />
    </td></tr></table>
  </xsl:template>

  <xsl:template mode="text" match="faq:sect">
    <xsl:param name="class">faq-text-sect<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
    <table>
      <xsl:attribute name="class"><xsl:value-of select="$class" />E</xsl:attribute>
      <tr><td> 
        <table>
	  <xsl:attribute name="class"><xsl:value-of select="$class" />T</xsl:attribute>
          <tr>
            <td>
              <a href="#index">
                <xsl:attribute name="name">
                  <xsl:number level="multiple" count="faq:sect" />
                </xsl:attribute>
                <xsl:number level="multiple" count="faq:sect" />
              </a>
            </td>
            <td>
              <a href="#index">
                <xsl:value-of select="@title" />
              </a>
            </td>
          </tr>
        </table>
      </td></tr>

      <tr><td>
        <table>
	  <xsl:attribute name="class"><xsl:value-of select="$class" />B</xsl:attribute>
          <tr>
            <td>
	      <xsl:attribute name="width"><xsl:value-of select="$faq.index.indent" /></xsl:attribute>&#160;</td>
            <td><xsl:apply-templates mode="text" select="faq:sect|faq:entry" /></td>
          </tr>
        </table>
      </td></tr>
    </table>
  </xsl:template>


  <xsl:template mode="text" match="faq:question">
    <xsl:param name="class">faq-text-question<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
    <table>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <tr>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <td>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <a href="#index">
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
        <xsl:attribute name="name"><xsl:number level="multiple" count="faq:sect|faq:entry" /></xsl:attribute>
        <xsl:number level="multiple" count="faq:sect|faq:entry" />
      </a>
    </td>
    <td>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <a href="#index">
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
    <xsl:apply-templates /></a>
    </td>
    </tr></table>
  </xsl:template>


  <xsl:template mode="text" match="faq:answer">
    <xsl:param name="class">faq-text-answer<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>

    <table>
    <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>

    <tr>
      <td><xsl:attribute name="width"><xsl:value-of select="$faq.text.indent" /></xsl:attribute>&#160;</td>
      <td><xsl:apply-templates /></td>
      <td><xsl:attribute name="width"><xsl:value-of select="$faq.text.indent" /></xsl:attribute>&#160;</td>
    </tr></table>
  </xsl:template>


  <xsl:template mode="text" match="faq:entry">
    <xsl:param name="class">faq-text-entry<xsl:value-of select="count(ancestor::faq:sect)" /></xsl:param>
    <table>
      <xsl:attribute name="class"><xsl:value-of select="$class" /></xsl:attribute>
      <tr><td>
        <xsl:apply-templates mode="text" select="faq:question" />
        <xsl:apply-templates mode="text" select="faq:answer" />
        <xsl:apply-templates mode="text" select="faq:contributed" />
      </td></tr>
    </table> 
  </xsl:template>



<!--

     <xsl:if test="child::*/question">

     <tr><td>

       <table><tr><td width="10">&#160;</td><td>

             <xsl:call-template name="questions" />

       </td></tr></table>

     </td></tr>

     </xsl:if>



     <xsl:if test="sect">

     <tr><td>

       <table><tr><td width="20">&#160;</td><td><table>

           <xsl:apply-templates mode="text" /></table></td></tr>

       </table>

     </td></tr>

     </xsl:if>



     </table></td></tr>

  </xsl:template>

  

  <xsl:template name="indexquestions">

    <xsl:if test="child::*/question">

      <table>

        <xsl:for-each select="child::*/question">



    <tr class="index">



    <td width="10">&#160;</td>

    <td>

      <a>

     <xsl:attribute name="href">#<xsl:number level="multiple" count="sect|entry" /></xsl:attribute>

        <xsl:number level="multiple" count="sect|entry" />

   </a>

    </td>

    <td>

      <a>

     <xsl:attribute name="href">#<xsl:number level="multiple" count="sect|entry" /></xsl:attribute>

        <xsl:apply-templates />

      </a>

    </td>

    </tr>

    </xsl:for-each>

    </table>

    </xsl:if>

  </xsl:template>

  <xsl:template name="questions">

    <table>

    <xsl:for-each select="child::*/question">

      <tr><td>

      <table>

        <tr class="question">

    <td>

      <a href="#index">

        <xsl:attribute name="name">

          <xsl:number level="multiple" count="sect|entry" />

        </xsl:attribute>

              <xsl:number level="multiple" count="sect|entry" />

      </a>

    </td>

    <td>

      <a href="#index">

              <xsl:apply-templates />

      </a>

    </td>

  </tr>

      </table>

      </td></tr>

      <tr><td>

      <table>

        <tr class="answer"><td>

         <xsl:apply-templates select="../answer" />

  </td></tr>

  <xsl:apply-templates select="../contributed" />

      </table>

      </td></tr>

    </xsl:for-each>

    </table>

  </xsl:template>

  -->

  <xsl:template match="faq:author">
    <b>Author:</b> <xsl:value-of select="faq:name" />
    (email: <xsl:value-of select="faq:email" />)<br/>
  </xsl:template>


  <xsl:template match="faq:copyright">
    <p class="faq-copyright">
    <b>Copyright:</b> <xsl:apply-templates select="faq:notice" /><br />
      <xsl:apply-templates select="faq:text" />
    </p>
  </xsl:template>


  <xsl:template match="faq:text">
    <p><xsl:apply-templates /></p>
  </xsl:template>


  <xsl:template match="faq:notice">
    <xsl:value-of select="." />;
  </xsl:template>


  <xsl:template mode="text" match="faq:contributed">
    <table class="faq-contributed"><tr class="faq-contributed">
      <td>
        <b>Contributed by:</b>&#160;<xsl:value-of select="faq:name" /> 
        &#160;&lt;<xsl:value-of select="faq:email" />&gt;
      </td>
    </tr></table>
  </xsl:template>


  <xsl:template match="faq:itemize">
    <ul><xsl:apply-templates /></ul>
  </xsl:template>


  <xsl:template match="faq:url">
    <a>
      <xsl:attribute name="href"><xsl:value-of select="@url" /></xsl:attribute>
      <xsl:value-of select="@url" />
    </a>
  </xsl:template>


  <xsl:template match="faq:item">
    <li><xsl:apply-templates /></li>
  </xsl:template>


  <xsl:template match="faq:bf">
    <b><xsl:apply-templates /></b>
  </xsl:template>


  <xsl:template match="faq:answer/faq:p">
    <p class="faq-answer"><xsl:apply-templates /></p>
  </xsl:template>


  <xsl:template match="faq:question/faq:output">
    <samp class="faq-index">
      <xsl:apply-templates />
    </samp>
  </xsl:template>


  <xsl:template match="faq:code">
    <br />
    <code>
      <xsl:apply-templates />
    </code>
  </xsl:template>


  <xsl:template match="faq:output">
    <br />
    <samp>
      <xsl:apply-templates />
    </samp>
  </xsl:template>

</xsl:stylesheet>

