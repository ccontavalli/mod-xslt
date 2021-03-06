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

#ifndef MXSLT_DEBUG_H
# define MXSLT_DEBUG_H

# if ! defined(mxslt_debug_string) && ! defined(__GLIBC__)
#  define mxslt_debug_string(string) ((string) ? (string) : "(null)")
# else 
#  ifndef mxslt_debug_string
#    define mxslt_debug_string(string) (string)
#  endif
# endif

# define mxslt_debug_compare(dbglevel, levels) \
		(((dbglevel) & (levels)) == (levels))

# define mxslt_doc_debug_level(document, levels) \
		mxslt_debug_compare((document)->dbglevel, levels)

# define mxslt_doc_debug_print(doc, level, ...) \
		do { 										\
		  if(mxslt_doc_debug_level(doc, level)) 					\
		    (doc)->dbghdlr((doc)->dbgctx, (level), (doc)->dbglevel, __VA_ARGS__); 	\
		} while(0)
# define mxslt_doc_debug_write(doc, level, ...) \
		do { 										\
		  assert(doc->dbghdlr);								\
		  (doc)->dbghdlr((doc)->dbgctx, (level), (doc)->dbglevel, __VA_ARGS__); 	\
		} while(0)

# define mxslt_debug_print(state, level, ...) \
		do { 										\
		  if(state->document) {								\
		    mxslt_doc_debug_print(state->document, level, __VA_ARGS__);			\
		  } else {									\
		    mxslt_doc_debug_print(state, level, __VA_ARGS__);				\
		  }										\
		} while(0)

/* IMPORTANT: if you add any value here, you need to update modxslt-debug.c */
typedef enum mxslt_debug_e {
  MXSLT_DBG_ERROR=BIT(0),  /* INTERNAL - do not mark messages with this bit */
  MXSLT_DBG_CONFIG=BIT(1), /* Reading/parsing server configurations */
  MXSLT_DBG_DEBUG=BIT(2),  /* No other purposes than debugging */
  MXSLT_DBG_FLAGS=BIT(3),  /* Add info about debug masks and flags */
  MXSLT_DBG_LIBXML=BIT(4), /* Libxml related messages */
  MXSLT_DBG_PARSER=BIT(5), /* modxslt expression parsing messages (lex/yacc) */
  MXSLT_DBG_PROTO=BIT(6),  /* http protocol related */
  MXSLT_DBG_RULES=BIT(7),  /* modxslt xslt rules and similar */
  MXSLT_DBG_SAPI=BIT(8),   /* server API messages related */
  MXSLT_DBG_VARIABLES=BIT(9), /* variable expansion */
  MXSLT_DBG_INTERNAL=BIT(10), /* internal workings of modxslt */

  MXSLT_DBG_VERBOSE0=BIT(15), /* spammy messages */
  MXSLT_DBG_VERBOSE1=BIT(16), /* spammier messages */
  MXSLT_DBG_VERBOSE2=BIT(17), /* spammiest messages */

  MXSLT_DBG_ALL=0xffff - MXSLT_DBG_ERROR
} mxslt_debug_e;

  /** Enables libmodxslt1 debugging for a particular document.
   * @param doc Initialized pointer to a struct mxslt_doc_t.
   * @param level Desired debugging level. Should be a logical or of bits
   *   defined in mxslt_debug_e.
   * @param hdlr Pointer to a function able to output debugging messages.
   * @param ctx Passed blindly and trasparently as the first argument of hdlr */
extern unsigned int mxslt_doc_debug_enable(
    mxslt_doc_t * doc, unsigned int level, mxslt_debug_hdlr_f hdlr, void * ctx);

  /** Enables libmodxslt1 global debugging features.
   * @param state Initialized pointer to a struct mxslt_state_t.
   * @param level Desired debugging level. Should be a logical or of bits
   *   defined in mxslt_debug_e.
   * @param hdlr Pointer to a function able to output debugging messages.
   * @param ctx Passed blindly and trasparently as the first argument of hdlr */
extern unsigned int mxslt_debug_enable(
    mxslt_shoot_t * shoot, unsigned int level, mxslt_debug_hdlr_f hdlr,
    void * ctx);

  /** When initializing libmodxslt1 debugging, provides user desired debugging
   * level. This function reads from the user configurations (an environment
   * variable, but in the future it may become a configuration file) the user
   * desired debugging level for the mxslt_ library. This is most useful when
   * initializing the library, in those softwares that need to initialize the
   * library before parsing user configurations or configuration files. */
extern unsigned int mxslt_debug_firstlevel(void);

  /** Returns the bitmask associated with a given debug level.
   * For example, by calling mxslt_debug_string("sapi"), the return value would
   * be MXSLT_DBG_SAPI. This function is most useful when transforming user
   * configuration lines into bitmasks.
   * @param str A string representing a given debugging level.
   * @param len The lenght of the string (strlen(str)).
   * @return The bit corresponding to the specified string,
   *   or MXSLT_DBG_ERROR, if the string is not recognized
   *   or str is null or empty (""). */
extern unsigned int mxslt_debug_flag(const char * str, unsigned int len);

  /** Returns the bitmask associated with a given debug string.
   * This function expects a comma separated list of debug flags to be enabled.
   * It returns the bitmask associated.
   * @param str The user supplied list of flags.
   * @return The bitmask corresponding to the specified string, or
   *   MXSLT_DBG_ERROR if any of the flags is invalid. */
extern unsigned int mxslt_debug_parse(char * str);
#endif 
