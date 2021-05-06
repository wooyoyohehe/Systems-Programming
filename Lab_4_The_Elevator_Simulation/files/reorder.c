#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"
#include "jrb.h"
#include "dllist.h"

#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))

int main()
{
  IS is;
  JRB lines, tmp;
  Dllist l, ltmp;
  double t;

  lines = make_jrb();
  is = new_inputstruct(NULL);

  while (get_line(is) > 0) {
    sscanf(is->fields[0], "%lf", &t);
    tmp = jrb_find_dbl(lines, t);
    if (tmp == NULL) tmp = jrb_insert_dbl(lines, t, new_jval_v((void *) new_dllist()));
    l = (Dllist) tmp->val.v;
    dll_append(l, new_jval_s(strdup(is->text1)));
  }
  jrb_traverse(tmp, lines) {
    l = (Dllist) tmp->val.v;
    dll_traverse(ltmp, l) printf("%s", ltmp->val.s);
  }
  exit(0);
}



