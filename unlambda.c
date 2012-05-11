#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <glib.h>

#include "object.h"



gint main(gint argc, gchar* argv[]){
  char* hw = "`r```````````.H.e.l.l.o. .w.o.r.l.di";
  char* fib =  "```s``s``sii`ki\n"
"`k.*``s``s`ks\n"
"``s`k`s`ks``s``s`ks``s`k`s`kr``s`k`sikk\n"
"`k``s`ksk";
  /*
   * http://www.madore.org/~david/programs/unlambda/
   */
  GOptionContext *context;
  GError *error = NULL;

  context = g_option_context_new("This line is appeared after Uagese's command line");
  g_option_context_set_summary(context, "This line is appeared before options");
  g_option_context_set_description(context, "This line is appeared after options");
  if(!g_option_context_parse(context, &argc, &argv, &error)){
    g_printerr("option parsing failed: %s\n", error->message);
    return 1;
  }

  
  start_mem();

  printf("%s\n", fib);
  //eval(hw);
  eval(fib);
  //printf("%s\n", hw);
  stop_mem();
  return 0;
}

