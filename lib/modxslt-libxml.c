/*
 *  mod-xslt -- Copyright (C) 2002-2008 
 *   		 Carlo Contavalli 
 *   		 <ccontavalli at inscatolati.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include "modxslt0/modxslt.h"
#include "modxslt0/modxslt-internal.h"
#include <assert.h>

#ifndef XRAISE
# define XRAISE exit(1); return NULL;
#endif

static void mxslt_doc_dump_ctx(xmlParserCtxt * ctx, mxslt_doc_t * doc) {
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx: %08x\n", (unsigned int)ctx);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->sax: %08x\n", (unsigned int)ctx->sax);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->myDoc: %08x\n", (unsigned int)ctx->myDoc);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->wellFormed: %08x\n", (unsigned int)ctx->wellFormed);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->replaceEntities: %08x\n", (unsigned int)ctx->replaceEntities);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->version: %s\n", (char *)ctx->version);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->encoding: %s\n", (char *)ctx->encoding);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->standalone: %08x\n", (unsigned int)ctx->standalone);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->html: %08x\n", (unsigned int)ctx->html);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->input: %08x\n", (unsigned int)ctx->input);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->inputNr: %08x\n", (unsigned int)ctx->inputNr);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->inputMax: %08x\n", (unsigned int)ctx->inputMax);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->inputTab: %08x\n", (unsigned int)ctx->inputTab);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->node: %08x\n", (unsigned int)ctx->node);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->nodeNr: %08x\n", (unsigned int)ctx->nodeNr);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->nodeMax: %08x\n", (unsigned int)ctx->nodeMax);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->nodeTab: %08x\n", (unsigned int)ctx->nodeTab);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->record_info: %08x\n", (unsigned int)ctx->record_info);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->node_seq: %08x\n", (unsigned int)&(ctx->node_seq));
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->errNo: %08x\n", (unsigned int)ctx->errNo);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->hasExternalSubset: %08x\n", (unsigned int)ctx->hasExternalSubset);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->hasPErefs: %08x\n", (unsigned int)ctx->hasPErefs);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->external: %08x\n", (unsigned int)ctx->external);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->valid: %08x\n", (unsigned int)ctx->valid);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->validate: %08x\n", (unsigned int)ctx->validate);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->vctxt: %08x\n", (unsigned int)&(ctx->vctxt));
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->instate: %08x\n", (unsigned int)ctx->instate);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->token: %08x\n", (unsigned int)ctx->token);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->directory: %s\n", (char *)ctx->directory);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->nbChars: %08lx\n", (unsigned long int)ctx->nbChars);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->checkIndex: %08lx\n", (unsigned long int)ctx->checkIndex);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->keepBlanks: %08x\n", (unsigned int)ctx->keepBlanks);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->disableSAX: %08x\n", (unsigned int)ctx->disableSAX);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->inSubset: %08x\n", (unsigned int)ctx->disableSAX);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->intSubName: %s\n", (char *)ctx->intSubName);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->extSubURI: %s\n", (char *)ctx->extSubURI);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->extSubSystem: %s\n", (char *)ctx->extSubSystem);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->space: %08x\n", (unsigned int)ctx->space);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->spaceNr: %08x\n", (unsigned int)ctx->spaceNr);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->spaceMax: %08x\n", (unsigned int)ctx->spaceMax);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->spaceTab: %08x\n", (unsigned int)ctx->spaceTab);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->depth: %08x\n", (unsigned int)ctx->depth);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->entity: %08x\n", (unsigned int)ctx->entity);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->charset: %08x\n", (unsigned int)ctx->charset);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->nodelen: %08x\n", (unsigned int)ctx->nodelen);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->nodemem: %08x\n", (unsigned int)ctx->nodemem);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->pedantic: %08x\n", (unsigned int)ctx->pedantic);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->_private: %08x\n", (unsigned int)ctx->_private);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->loadsubset: %08x\n", (unsigned int)ctx->loadsubset);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->linenumbers: %08x\n", (unsigned int)ctx->linenumbers);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->catalogs: %08x\n", (unsigned int)ctx->catalogs);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->recovery: %08x\n", (unsigned int)ctx->recovery);
  mxslt_debug_write(doc, MXSLT_DBG_DEBUG | MXSLT_DBG_LIBXML, "ctx->progressive: %08x\n", (unsigned int)ctx->progressive);

  return;
}

	    /* tie up xmlNode and style_pi */
# define XML_LINK_PI(document, pinode, pi) \
	do { \
	  (pi)->node=pinode; \
	  (pinode)->_private=(pi); \
	  (pi)->next=NULL; \
	  if((document)->pi_last) \
	    (((document)->pi_last)->next)=(mxslt_pi_t *)(pi); \
	  if(!((document)->pi_first)) \
	    ((document)->pi_first)=(mxslt_pi_t *)(pi); \
	  ((document)->pi_last)=(mxslt_pi_t *)(pi); \
	} while(0)


  /* This function replaces processingInstruction */
void mxslt_sax_processing_instruction(void * ctx, const xmlChar * target, const xmlChar * data) {
  xmlParserCtxtPtr ctxt=(xmlParserCtxtPtr)ctx;
  mxslt_doc_t * document=(mxslt_get_state()->document);
  mxslt_pi_style_t * style_pi;
  mxslt_pi_param_t * param_pi;
  mxslt_pi_base_t * base_pi;
  char * type = NULL, * tmp;
  xmlNodePtr pi;
  int status;

    /* WARNING: don't change this array! Look below! */
  static const mxslt_attr_search_t xml_stylesheet[] = {
    { (xmlChar *)"href", mxslt_sizeof_str("href") },
    { (xmlChar *)"media", mxslt_sizeof_str("media") },
    { (xmlChar *)"type", mxslt_sizeof_str("type") }
  };

    /* WARNING: don't change this array! Look below! */
  static const mxslt_attr_search_t mxslt_param[] = {
    { (xmlChar *)"name", mxslt_sizeof_str("name") },
    { (xmlChar *)"value", mxslt_sizeof_str("value") }
  };

    /* WARNING: don't change this array! Look below! */
  static const mxslt_attr_search_t mxslt_base[] = {
    { (xmlChar *)"value", mxslt_sizeof_str("value") }
  };

    /* This array _must_ have the same number of elements
     * as the array  xml_stylesheet */
  char * xml_stylesheet_values[] = { NULL, NULL, NULL };
  enum xml_stylesheet_types {
    t_href=0,
    t_media,
    t_type
  };

    /* This array _must_ have the same number of elements
     * as the array  mxslt_param */
  char * mxslt_param_values[] = { NULL, NULL };
  enum mxslt_param_types {
    t_name=0,
    t_select
  };

    /* This array _must_ have the same number of elements 
     * as the array mxslt_base */
  char * mxslt_base_values[] = { NULL };
  enum mxslt_base_types {
    t_value=0
  };

  mxslt_debug_print(document, MXSLT_DBG_LIBXML, "processing instruction \"%s\", attributes |%s|\n", target, data);

    /* Ignore pi when we're parsing the stylesheet */
  if(document->flags & MXSLT_STYLE)
    return;

    /* Create a new pi node */
  pi=xmlNewPI(target, data);
  if(!pi)
    return;

    /* Check if target is a known pi */
  switch(target[0]) {
    case 'x':
      if(!xmlStrEqual(target+1, (xmlChar *)"xml-stylesheet"+1))
        break;

        /* Get href, type and media from attribute list */
      status=mxslt_get_static_attr((char *)data, xml_stylesheet, xml_stylesheet_values, mxslt_sizeof_array(xml_stylesheet));
      if(status != MXSLT_OK) {
	if(xml_stylesheet_values[0])
	  xfree(xml_stylesheet_values[0]);
	if(xml_stylesheet_values[1])
	  xfree(xml_stylesheet_values[1]);
	if(xml_stylesheet_values[2])
	  xfree(xml_stylesheet_values[2]);
	goto error;
      }

      if(xml_stylesheet_values[t_type] && xml_stylesheet_values[t_href]) {
	  /* Decode type */
	type=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)xml_stylesheet_values[t_type], 
					     XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);

          /* Skip blanks before type */
        for(tmp=type; MXSLT_IS_BLANK(*tmp); tmp++)
          ;

          /* compare type string */
        if(!strncmp(tmp, "text/xml", mxslt_sizeof_str("text/xml")) || 
           !strncmp(tmp, "text/xsl", mxslt_sizeof_str("text/xsl"))) {

            /* trailing spaces are allowed */
          for(tmp+=mxslt_sizeof_str("text/x?l"); MXSLT_IS_BLANK(*tmp); tmp++)
            ;

	    /* Check there's nothing else beside them */
          if(*tmp != '\0') {
	    xmlFree(type);
  	    break;
	  }
	    
	    /* Type is ok, add parameter to processing 
	     * instruction */
	  style_pi=(mxslt_pi_style_t *)xmalloc(sizeof(mxslt_pi_style_t));
	  style_pi->type=MXSLT_PI_STDST;
	  style_pi->ctype=type;
	  style_pi->href=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)xml_stylesheet_values[t_href], 
					       XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);
	  if(xml_stylesheet_values[t_media])
	    style_pi->media=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)xml_stylesheet_values[t_media],
					        XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);
	  else
	    style_pi->media=NULL;

	    /* tie up xmlNode and style_pi */
	  XML_LINK_PI(document, pi, style_pi);
	} else {
	  mxslt_error(document, "warning - <xml-stylesheet type=\"... unknown: '%s'!\n", type);
	  xmlFree(type);
	}
      } else {
	if(!xml_stylesheet_values[t_href])
	  mxslt_error(document, "warning - <xml-stylesheet href=\"... is missing, skipping PI!\n");
	if(!xml_stylesheet_values[t_type])
	  mxslt_error(document, "warning - <xml-stylesheet type=\"... is missing, skipping PI!\n");
      }

      if(xml_stylesheet_values[t_media])
        xfree(xml_stylesheet_values[t_media]);
      if(xml_stylesheet_values[t_href])
        xfree(xml_stylesheet_values[t_href]);
      if(xml_stylesheet_values[t_type])
        xfree(xml_stylesheet_values[t_type]);
      break;

    case 'm':
      if(target[1] != 'o' || target[2] != 'd' || 
         target[3] != 'x' || target[4] != 's' || 
         target[5] != 'l' || target[6] != 't' ||
	 target[7] != '-') 
        break;
          
      switch(target[8]) {
	case 'b':
	  if(!xmlStrEqual(target+8, (xmlChar *)"modxslt-base"+8)) {
            mxslt_error(document, "warning - unknown modxslt PI: %s in %s\n", target, document->localfile);
            break;
	  }

	    /* Read modxslt-base */
          status=mxslt_get_static_attr((char *)data, mxslt_base, mxslt_base_values, mxslt_sizeof_array(mxslt_base));
	  if(status != MXSLT_OK) {
            if(mxslt_base_values[0])
              xfree(mxslt_base_values[0]);
	    goto error;
	  }

	    /* Verify we got the base */
	  if(!mxslt_base_values[0]) {
            mxslt_error(document, "warning - <modxslt-base ... base=\" is missing, skipping PI\n", target, document->localfile);
	    break;
	  }

	    /* Remember the pi we found */
	  base_pi=(mxslt_pi_base_t *)(xmalloc(sizeof(mxslt_pi_base_t)));
	  base_pi->type=MXSLT_PI_UBASE;
	  base_pi->file=(char *)xmlCanonicPath((xmlChar *)mxslt_base_values[0]);
	  base_pi->directory=xmlParserGetDirectory(mxslt_base_values[0]);
	  xfree(mxslt_base_values[0]);

	    /* Switch base in the current context */
	  if(ctxt->input->filename)
	    xmlFree((char *)ctxt->input->filename);
	  if(ctxt->directory)
	    xmlFree(ctxt->directory);

	    /* Remember those information */
	  ctxt->input->filename=(char *)xmlStrdup((xmlChar *)base_pi->file);
	  ctxt->directory=(char *)xmlStrdup((xmlChar *)base_pi->directory);

	    /* tie up xmlNode and style_pi */
	  XML_LINK_PI(document, pi, base_pi);
	  break;

        case 'p':
          if(!xmlStrEqual(target+8, (xmlChar *)"modxslt-param"+8)) {
            mxslt_error(document, "warning - unknown modxslt PI: %s in %s\n", target, document->localfile);
            break;
	  }

          status=mxslt_get_static_attr((char *)data, mxslt_param, mxslt_param_values, mxslt_sizeof_array(mxslt_param));
	  if(status != MXSLT_OK) {
	    if(mxslt_param_values[0])
	      xfree(mxslt_param_values[0]);
	    if(mxslt_param_values[1])
	      xfree(mxslt_param_values[1]);

	    goto error;
	  }

          if(mxslt_param_values[t_name]) {
	      /* Allocate and create pi */
	    param_pi=(mxslt_pi_param_t *)xmalloc(sizeof(mxslt_pi_param_t));
	    param_pi->type=MXSLT_PI_PARAM;
	    param_pi->param=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)mxslt_param_values[t_name],
					        XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);
	    param_pi->value=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)mxslt_param_values[t_select],
					        XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);

	      /* Link pi to list of pis */
	    XML_LINK_PI(document, pi, param_pi);
	  } else {
    	    mxslt_error(document, "warning - modxslt-param specified without ``name'' attribute in `%s'\n", document->localfile);
	  }

	    /* Free memory up */
          if(mxslt_param_values[t_select])
            xfree(mxslt_param_values[t_select]);
          if(mxslt_param_values[t_name])
            xfree(mxslt_param_values[t_name]);
          break;
    
        case 's':
          if(!xmlStrEqual(target+8, (xmlChar *)"modxslt-stylesheet"+8)) {
            mxslt_error(document, "warning - unknown modxslt PI: %s in %s\n", target, document->localfile);
            break;
	  }
      
          status=mxslt_get_static_attr((char *)data, xml_stylesheet, xml_stylesheet_values, mxslt_sizeof_array(xml_stylesheet));
	  if(status != MXSLT_OK) {
	    if(xml_stylesheet_values[0])
	      xfree(xml_stylesheet_values[0]);
	    if(xml_stylesheet_values[1])
	      xfree(xml_stylesheet_values[1]);
	    if(xml_stylesheet_values[2])
	      xfree(xml_stylesheet_values[2]);

	    goto error;
	  }

          if(xml_stylesheet_values[t_type]) {
	      /* Decode type */
  	    type=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)xml_stylesheet_values[t_type], 
					         XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);

              /* Skip blanks before type */
            for(tmp=type; MXSLT_IS_BLANK(*tmp); tmp++)
              ;

              /* compare type string */
            if(!strncmp(tmp, "text/xml", mxslt_sizeof_str("text/xml")) || 
               !strncmp(tmp, "text/xsl", mxslt_sizeof_str("text/xsl"))) {

                /* trailing spaces are allowed */
              for(tmp+=mxslt_sizeof_str("text/x?l"); MXSLT_IS_BLANK(*tmp); tmp++)
                ;

	        /* Check there's nothing else beside them */
              if(*tmp != '\0') {
	        mxslt_error(document, "warning - <modxslt-stylesheet type=\"... trailing junk: '%s'!\n", type);
        	xmlFree(type);
  	        break;
	      }
	    
	        /* Type is ok, add parameter to processing 
	         * instruction */
    	      style_pi=(mxslt_pi_style_t *)xmalloc(sizeof(mxslt_pi_style_t));
	      style_pi->type=MXSLT_PI_MODST;
	      style_pi->ctype=type;
	      if(xml_stylesheet_values[t_href])
	        style_pi->href=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)xml_stylesheet_values[t_href], 
				    	        XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);
	      else 
		style_pi->href=NULL;
     	      if(xml_stylesheet_values[t_media])
	        style_pi->media=(char *)xmlStringDecodeEntities(ctxt, (xmlChar *)xml_stylesheet_values[t_media],
					        XML_SUBSTITUTE_REF|XML_SUBSTITUTE_PEREF, 0, 0, 0);
	      else
	        style_pi->media=NULL;

	        /* Link to nodes chain */
	      XML_LINK_PI(document, pi, style_pi);
            } else {
	      mxslt_error(document, "warning - <modxslt-stylesheet type=\"... unknown: '%s'!\n", type);
	      xmlFree(type);
	    }
	  }

          if(xml_stylesheet_values[t_media])
            xfree(xml_stylesheet_values[t_media]);
          if(xml_stylesheet_values[t_href])
            xfree(xml_stylesheet_values[t_href]);
          if(xml_stylesheet_values[t_type])
            xfree(xml_stylesheet_values[t_type]);
          break;

        default:
          mxslt_error(document, "warning - unknown modxslt PI: %s in %s\n", target, document->localfile);
          break;
      }
      break;
  }      

    /* Taken from `processingInstruction' */
  switch(ctxt->inSubset) {
    case 1:
      xmlAddChild((xmlNodePtr)ctxt->myDoc->intSubset, pi);
      return;

    case 2:
      xmlAddChild((xmlNodePtr)ctxt->myDoc->extSubset, pi);
      return;
  }

  if(!ctxt->myDoc->children || !ctxt->node) {
    xmlAddChild((xmlNodePtr)ctxt->myDoc, pi);
    return;
  }

  if(ctxt->node->type == XML_ELEMENT_NODE)
    xmlAddChild(ctxt->node, pi);
  else 
    xmlAddSibling(ctxt->node, pi);

  return;

error:
   mxslt_error(document, "warning - weird data while processing PI: %s in %s\n", target, document->localfile);
   xmlFreeNode(pi);

   return;
}

  /* This function replaces resolveEntity 
   * Is this still needed? */
xmlParserInputPtr mxslt_sax_resolve_entity(void * ctx, const xmlChar * publicId, const xmlChar * systemId) {
  xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
  mxslt_doc_t * document=(mxslt_get_state()->document);
  xmlParserInputPtr ret;
  xmlChar *URI;
  const char *base = NULL;
  char * id;

    /* If standalone, do not read any
     * external DTD */
  if(ctxt->standalone == 1)
    return NULL;

  if(ctxt->input)
    base=ctxt->input->filename;

  if(!base)
    base=ctxt->directory;

     /* In resolve entity */
  id=mxslt_yy_str_parse(document, (char *)systemId, strlen((char *)systemId));
  mxslt_debug_print(document, MXSLT_DBG_LIBXML, "%s needs entity publicId: %s, systemId: %s", 
	base, mxslt_debug_string(publicId), mxslt_debug_string(systemId));
  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG, "base: %s, filename: %s, directory: %s",
	base, mxslt_debug_string(ctxt->input->filename), mxslt_debug_string(ctxt->directory));

  if(mxslt_debug_level(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE1)) 
    mxslt_doc_dump_ctx(ctxt, document);

  URI=xmlBuildURI((xmlChar *)id, (const xmlChar *)base);

  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_VERBOSE0, 
		"loading entity: %s, publicid: %s\n", mxslt_debug_string(URI), mxslt_debug_string(publicId));
  ret = xmlLoadExternalEntity((const char *) URI, (const char *) publicId, ctxt);

  if(URI != NULL)
    xmlFree(URI);
  xfree(id);

  return(ret);
}

  /* To calculate name of other files loaded, libxml2 uses
   * ctx->input->filename, if defined, or ctxt->directory */
  /* Taken from libxml2, xmlSAXParseMemoryWithData */
xmlDocPtr mxslt_doc_xml_parse(mxslt_doc_t * document, xmlParserInputBufferPtr buf, char * localfile) {
  xmlParserCtxtPtr ctx;
  xmlParserInputPtr input;
  xmlDocPtr retval;

  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0,
		"xml_parse/xmlCreateMemoryParserCtxt -- replacing entities: %08x\n", xmlSubstituteEntitiesDefaultValue);

  if(buf == NULL)
    return NULL;

    /* SNIPPET: This is a good mix&shake of 
     * xmlCreateMemoryParserCtxt, xmlCreateFileParserCtxt */
  ctx=xmlNewParserCtxt();
  if(ctx == NULL) {
    xmlFreeParserInputBuffer(buf);
    return NULL;
  }

#if LIBXML_VERSION >= 20600
  xmlCtxtUseOptions(ctx, MXSLT_XSLT_OPTIONS);
#endif

    /* Default are no longer changed... since 2.6.0 they
     * are completely ignored, leading to broken modxslt :|| */
  if(ctx->sax) {
    ctx->sax->resolveEntity=mxslt_sax_resolve_entity;
    ctx->sax->processingInstruction=mxslt_sax_processing_instruction;
  }

    /* Remember which document we are parsing
     * in this context */
/*  ctx->_private=document; */

  input=xmlNewInputStream(ctx);
  if(input == NULL) {
    xmlFreeParserInputBuffer(buf);
    xmlFreeParserCtxt(ctx);

    return NULL;
  }

  input->filename=(char *)xmlCanonicPath((xmlChar *)localfile);
  if(input->filename == NULL) {
    xmlFreeParserCtxt(ctx);
    xmlFreeParserInputBuffer(buf);
    xmlFreeInputStream(input);

    return NULL;
  }

  input->buf=buf;
  input->base=input->buf->buffer->content;
  input->cur=input->buf->buffer->content;
  input->end=&input->buf->buffer->content[input->buf->buffer->use];

  inputPush(ctx, input);

  if(ctx->directory == NULL)
    ctx->directory=xmlParserGetDirectory(localfile);
    /* END SNIPPET */

  if(mxslt_debug_level(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE2)) 
    mxslt_doc_dump_ctx(ctx, document);

    /* Parse document */
  xmlParseDocument(ctx);

  if(ctx->wellFormed)
    retval=ctx->myDoc;
  else {
    retval=NULL;
    xmlFreeDoc(ctx->myDoc);
    ctx->myDoc=NULL;
  }
  xmlFreeParserCtxt(ctx);
  /* xmlFreeParserInputBuffer(buf); */

  return retval;
}

#ifndef MXSLT_DISABLE_EXTENSIONS
void mxslt_transform_header(xsltTransformContextPtr ctxt, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp) {
  mxslt_doc_t * document=mxslt_get_state()->document;
  char * header, * value;
  char * cur, * store;

#ifdef MXSLT_FALLBACK_WRAPAROUND 
  xmlNodePtr tmp, tofree;
#endif

  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0, 
		    "%s: <modxslt:header-set called\n", mxslt_debug_string(document->localfile));

    /* Check input parameters */
  if(ctxt == NULL || node == NULL || inst == NULL)
    return; 

    /* Check type is correct */
  if(inst->type != XML_ELEMENT_NODE)
    return;

    /* Verify we have some attribute */
  if(inst->properties == NULL || inst->properties->type != XML_ATTRIBUTE_NODE ||
     !inst->properties->children || inst->properties->children->type != XML_TEXT_NODE ||
     !inst->properties->children->content) {
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:header-set name='... no valid attribute specifyed!");
    return;
  }

    /* Get attribute value */
  if(!xmlStrEqual(inst->properties->name, (xmlChar *)"name")) {
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:header-set %s='... unknown attribute!", inst->properties->name);
    return;
  }

    /* Warn if we were provided more
     * attributes than expected */
  if(inst->properties->next) 
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:header-set %s='... unknown additional attribute!", 
		       inst->properties->next->name);

  header=(char *)inst->properties->children->content;

#ifdef MXSLT_FALLBACK_WRAPAROUND 
    /* Ugly hack: is there a better way to do this? */
    /* Problem: for some reason, if there is a `fallback' node
     *   xsltApplyOneTemplate reports an error. I suspect the fact that
     *   precompiled info is missing for this node (precompilation
     *   stops at unknown node modxslt:header)... However, couldn't i
     *   find a better way to handle this. I believe it is a libxml
     *   bug (didn't get any answer when reported). */
  for(tmp=inst->children; tmp != NULL;) {
    if(tmp->name && xmlStrEqual(tmp->name, (xmlChar *)"fallback")) {
      tofree=tmp;
      tmp=tmp->next;
      xmlUnlinkNode(tofree);
      xmlFreeNode(tofree); 
    } else {
      tmp=tmp->next;
    }
  }
#endif 

    /* Look for text value */
  value=(char *)xsltEvalTemplateString(ctxt, node, inst);
  mxslt_debug_print(document, MXSLT_DBG_LIBXML, "requesting header '%s' to be set to value '%s'\n", 
		    mxslt_debug_string(header), mxslt_debug_string(value));

    /* Shrink spaces in value, only if needed */
  if(!value)
    value="";
  else {
    if(xsltFindElemSpaceHandling(ctxt, inst) == 1) {
        /* Remove \n, \r, \t from output - probably the uglyest
	 * construct in the whole source */
      for(cur=store=value; *cur != '\0'; cur++)
        switch(*cur) { 
    	  case '\n':
          case '\r':
	  case '\t':
	  case ' ':
	    for(; *cur == ' ' || *cur == '\t' || *cur == '\n' || *cur == '\r'; cur++)
	      ;
	    *store++=' ';

	  default:
	    *store++=*cur;
	    break;
        }
      *store='\0';
    }
  }
  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG, "reply header '%s' has been shrinked to '%s'\n", 
		    mxslt_debug_string(header), mxslt_debug_string(value));

    /* Now, let's do it... */
    /* XXX: do we need to call xmlStringDecodeEntities? */
  document->header_set(header, value, document->header_data);

  xmlFree(value);

  return;
}

void mxslt_transform_value_of(xsltTransformContextPtr ctxt, xmlNodePtr node, xmlNodePtr inst, xsltElemPreCompPtr comp) {
  mxslt_doc_t * document=mxslt_get_state()->document;
  char * var, * value;

  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0, 
		    "%s: <modxslt:value-of called\n", mxslt_debug_string(document->localfile));

    /* Check input parameters */
  if(ctxt == NULL || node == NULL || inst == NULL)
    return; 

    /* Check type is correct */
  if(inst->type != XML_ELEMENT_NODE)
    return;

    /* Verify we have some attribute */
  if(inst->properties == NULL || inst->properties->type != XML_ATTRIBUTE_NODE ||
     !inst->properties->children || inst->properties->children->type != XML_TEXT_NODE ||
     !inst->properties->children->content) {
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:value-of select='... no valid attribute specifyed!");
    return;
  }

    /* Get attribute value */
  if(!xmlStrEqual(inst->properties->name, (xmlChar *)"select")) {
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:value-of %s='... unknown attribute!", inst->properties->name);
    return;
  }

    /* Warn if we were provided more
     * attributes than expected */
  if(inst->properties->next) 
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:value-of %s='... unknown additional attribute!", 
		       inst->properties->next->name);

  var=(char *)inst->properties->children->content;

    /* Now, let's do it... */
  value=mxslt_yy_str_parse(document, var, strlen(var));
  if(!value)
    value="";

  mxslt_debug_print(document, MXSLT_DBG_LIBXML, "setting variable '%s' to value '%s'\n", 
		    mxslt_debug_string(var), value);

    /* XXX: uhm... how do I know I always need escaping? what 
     * if escaping was disabled? */
  var=(char *)xsltCopyTextString(ctxt, ctxt->insert, (xmlChar *)value, 0); 
  xfree(value);

  if(!var)
    xsltTransformError(ctxt, ctxt->style, inst, "<modxslt:value-of %s='...xsltCopyTextString failed!", 
		       inst->properties->next->name);

  return;
}
#endif /* MXSLT_DISABLE_EXTENSIONS */

  /* Wrapper for xsltApplyStylesheet */
xmlDocPtr mxslt_doc_xml_apply_stylesheet(mxslt_doc_t * mxslt_doc, xsltStylesheetPtr style, 
					 xmlDocPtr doc, const char ** params) {
  xmlDocPtr retval;
  xsltTransformContextPtr userCtxt;

  userCtxt=xsltNewTransformContext(style, doc);
  if(userCtxt == NULL)
    return NULL;

  xsltSetCtxtParseOptions(userCtxt, MXSLT_XSLT_OPTIONS);

    /* XXX I don't like this much :| ... I consider this an hack */
  /* userCtxt->_private=(void *)mxslt_doc; */
  userCtxt->outputFile=mxslt_doc->localfile;

#ifndef MXSLT_DISABLE_EXTENSIONS
  xsltRegisterExtElement(userCtxt, (xmlChar *)"header-set", (xmlChar *)MXSLT_NS_URI, mxslt_transform_header);
  xsltRegisterExtElement(userCtxt, (xmlChar *)"value-of", (xmlChar *)MXSLT_NS_URI, mxslt_transform_value_of);
#endif

  retval=xsltApplyStylesheetUser(style, doc, params, NULL, NULL, userCtxt);

  xsltFreeTransformContext(userCtxt);

  return retval;
}

  /* Taken from libxml2, xmlSAXParseMemoryWithData */
xmlDocPtr mxslt_doc_xml_load_entity(mxslt_doc_t * document, char * localfile) {
  xmlParserCtxtPtr ctx;
  xmlParserInputPtr input;
  xmlDocPtr retval;
  xmlChar * filename;

  mxslt_debug_print(document, MXSLT_DBG_LIBXML | MXSLT_DBG_DEBUG | MXSLT_DBG_VERBOSE0,
		"load_entity/xmlCreateMemoryParserCtxt -- replacing entities: %08x\n", xmlSubstituteEntitiesDefaultValue);

    /* SNIPPET: This is a good mix&shake of 
     * xmlCreateMemoryParserCtxt, xmlCreateFileParserCtxt */
  ctx=xmlNewParserCtxt();
  if(ctx == NULL)
    return NULL;

#if LIBXML_VERSION >= 20600
  xmlCtxtUseOptions(ctx, MXSLT_XSLT_OPTIONS);
#endif

    /* Remember which document we are parsing
     * in this context */
  /* ctx->_private=document; */

  filename=xmlCanonicPath((xmlChar *)localfile);
  if(filename == NULL) {
    xmlFreeParserCtxt(ctx);
    return NULL;
  }
  
  input=xmlLoadExternalEntity((char *)filename, NULL, ctx);
  xmlFree(filename);
  if(input == NULL) {
    xmlFreeParserCtxt(ctx);
    return NULL;
  }

  inputPush(ctx, input);

  if(ctx->directory == NULL)
    ctx->directory=xmlParserGetDirectory(localfile);
    /* END SNIPPET */

  /* MXSLT_DUMP_CTX(ctx); */

    /* Parse document */
  xmlParseDocument(ctx);

  if(ctx->wellFormed)
    retval=ctx->myDoc;
  else {
    retval=NULL;
    xmlFreeDoc(ctx->myDoc);
    ctx->myDoc=NULL;
  }
  xmlFreeParserCtxt(ctx);

  return retval;
}

#if !defined(HAVE_LIBXML_HACK) && defined(HAVE_LIBXML_THREADS)
/* Problem:
 *   - xmlSetGlobalState may need to either ``copy'' memory
 *     from one place to another or ``change a pointer''.
 *
 *   - changing a pointer requires:
 *       an ** where to save the old pointer
 *       a new * to set instead of the old one
 *   - copying memory requires 
 *       a memory area where to save the old state
 *       a new memory area to copy over the current state
 *   
 *   In either cases, the SetGlobalState must be transparent
 *   to the user 
 *
 */
int xmlSetGlobalState(xmlGlobalStatePtr new_state, xmlGlobalStatePtr * old_state) {
  xmlGlobalStatePtr global;
  int retval=1;
  
    /* Get a pointer to the current 
     * memory area */
  global=xmlGetGlobalState();
    /* If the user wants to
     * save the current state somewhere */
  if(old_state) {
    *old_state=(xmlGlobalStatePtr)xmlMalloc(sizeof(xmlGlobalState));
    if(*old_state)
      memcpy(*old_state, global, sizeof(xmlGlobalState));
    else
      retval=0;
  }

    /* If the user provides a new_state state 
     * and the copy was succesfull */
  if(new_state && (!old_state || *old_state)) {
    memcpy(global, new_state, sizeof(xmlGlobalState));
    xmlFree(new_state);
  }

    /* Return status */
  return retval;
}

void xmlFreeGlobalState(xmlGlobalStatePtr state) {
  xmlFree(state);

  return;
}

xmlGlobalStatePtr xmlNewGlobalState(void) {
  xmlGlobalStatePtr retval;

  retval=(xmlGlobalStatePtr)(xmlMalloc(sizeof(xmlGlobalState)));
  xmlInitializeGlobalState(retval);

  return retval;
}

#endif
