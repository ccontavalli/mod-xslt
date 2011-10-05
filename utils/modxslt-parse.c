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


#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <modxslt1/modxslt.h>

void mxslt_shell_error(void * null, const char * msg, ...) {
  va_list args;

  va_start(args, msg);
  vfprintf(stderr, msg, args);

  if(msg[strlen(msg)-1] != '\n')
    fputc(' ', stderr);
  va_end(args);

  return;
}

void mxslt_shell_header(char * name, char * value, void * null) {
  return;
}

int mxslt_shell_writer(void * ctx, const char * buffer, int blen) {
  FILE * fout = (FILE *)ctx;
  int ret;

  ret=(int)fwrite(buffer, blen, 1, fout);

  return ret*blen;
}

int mxslt_shell_closer(void * ctx) {
  return 0;
}

mxslt_callback_t mxslt_shell_callback = {
  mxslt_shell_writer,
  mxslt_shell_closer
};

int main(int argc, char ** argv) {
  mxslt_recursion_t recursion = MXSLT_RECURSION_INIT;
  xmlParserInputBufferPtr input;
  mxslt_doc_t document;
  FILE * foutput;
  char * ninput;
  mxslt_shoot_t mxslt_state;
  int status;

  foutput=stdout;
  switch(argc) {
    case 3:
      foutput=fopen(argv[2], "w"); 
      if(foutput == NULL) {
	perror("cannot open output file");
	return 3;
      }

    case 2:
      ninput=argv[1];
      break;

    default:
      fprintf(stderr, "Usage: xslt input-file [output-file]\n");
      return 1;
  }

    /* Initialize library */
  mxslt_xml_load();

    /* Initialize parsing */
  mxslt_xml_init(&mxslt_state, NULL, NULL);
  mxslt_doc_init(&document, APOS("shell"), &mxslt_state, &recursion, mxslt_shell_error, NULL, NULL);

    /* Prepare input buffer */
  input=mxslt_create_input_file(&document, (char *)ninput);
  if(input == NULL) {
    mxslt_error(&document, "fatal - could not open input file: %s, %s\n", ninput, strerror(errno));
    mxslt_doc_done(&document, &mxslt_state);

    return 2;
  }

    /* Load file in buffer */
  status=mxslt_doc_load(&document, input, ninput, NULL, 0, NULL);
  if(status != MXSLT_OK) {
    mxslt_error(&document, "fatal - could not load input file: %s\n", ninput);
    mxslt_doc_done(&document, &mxslt_state);
    return 3;
  }

    /* Parse Processing Instructions found in document */
  status=mxslt_doc_parse_pi(&document);
  switch(status) {
    case MXSLT_FAILURE:
      mxslt_error(&document, "fatal - error while parsing PI\n");
      mxslt_doc_done(&document, &mxslt_state);
      return 4;

    case MXSLT_NONE:
      mxslt_error(&document, "fatal - no <?xml-stylesheet or <?modxslt-stylesheet found in %s\n", ninput);
      mxslt_doc_done(&document, &mxslt_state);
      return 7;

    case MXSLT_UNLOADABLE:
      mxslt_error(&document, "fatal - couldn't find suitable stylesheet for: %s\n", ninput);
      mxslt_doc_done(&document, &mxslt_state);
      return 5;
  }

    /* parse */
  status=mxslt_doc_parse(&document, mxslt_shell_header, NULL);
  if(status == MXSLT_FAILURE) {
    mxslt_error(&document, "fatal - couldn't apply stylesheet to: %s\n", ninput);
    mxslt_doc_done(&document, &mxslt_state);

    return 6;
  }

  mxslt_doc_send(&document, &mxslt_shell_callback, foutput);
  mxslt_doc_done(&document, &mxslt_state);

  return 0;
}
