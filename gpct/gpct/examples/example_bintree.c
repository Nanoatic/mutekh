// example_bintree.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gpct/cont_bintree.h>
#include <gpct/cont_pbintree.h>

#define CONT BINTREE

struct myitem
{
  CONTAINER_ENTRY_TYPE(CONT)        tree_entry;
  unsigned int                         data;
};

CONTAINER_TYPE    (mytree, CONT, struct myitem, tree_entry, 32);
CONTAINER_KEY_TYPE(mytree, PTR, SCALAR, data);

CONTAINER_FUNC    (mytree, CONT, static, myfunc, data);

/********************************************************* Tree display hack */

int tree_height(mytree_entry_t *e)
{
  if (!e)
    return 0;

  int l = tree_height(e->child[0]);
  int r = tree_height(e->child[1]);

  return l > r ? l + 1 : r + 1;
}

void tree_get_level_(mytree_entry_t **t, int c, mytree_entry_t *e, int level, int l)
{
  if (e)
    tree_get_level_(t, (c << 1) | 0, e->child[0], level, l + 1);
  if (level == l)
    t[c] = e;
  if (e)
    tree_get_level_(t, (c << 1) | 1, e->child[1], level, l + 1);
}

int tree_get_level(mytree_entry_t **t, mytree_root_t *root, int level)
{
  tree_get_level_(t, 0, root->root, level, 0);
}

void disp_tree(mytree_root_t *root)
{
  int h = tree_height(root->root);
  int l;

  for (l = 0; l < h; l++)
    {
      mytree_entry_t *t[1 << l];
      int s = (1 << (h - l));
      int i, j;
      memset(t, 0, sizeof(t));
      tree_get_level(t, root, l);

      putchar('\n');

      for (j = 0; j < (1 << l); j++)
	{
	  if (t[j])
	    {
	      char *data[10];
	      int sl = sprintf(data, "%i", ((mytree_item_t)t[j])->data);

	      for (i = 0; i < s - sl / 2; i++) putchar(' ');
	      printf("%s", data);
	      for (i = 0; i < s - sl / 2 - (sl & 1); i++) putchar(' ');
	    }
	  else
	    {
	      for (i = 0; i < s; i++) putchar(' ');
	      putchar('.');
	      for (i = 0; i < s - 1; i++) putchar(' ');
	    }
	}

      putchar('\n');
    }
}

/**********************************************************/

struct myitem *new(unsigned int data)
{
  struct myitem *i = malloc(sizeof(*i));

  i->data = data;

  return i;
}

int main()
{
  mytree_root_t tree;
  int i;

  srand(time(0));

  myfunc_init(&tree);

  for (i = 0; i < 10; i++)
    {
      unsigned int r = rand()%100;

      printf("insert %i\n", r);
      myfunc_push(&tree, new(r));
    }

  disp_tree(&tree);

  assert(!myfunc_check(&tree));

  puts("FOREACH");

  CONTAINER_FOREACH(mytree, CONT, &tree, {
    printf("%i\n", item->data);
  });

  puts("FOREACH REVERSE");

  CONTAINER_FOREACH_REVERSE(mytree, CONT, &tree, {
    printf("%i\n", item->data);
  });

  puts("FOREACH UNORDERED");

  CONTAINER_FOREACH_UNORDERED(mytree, CONT, &tree, {
    printf("%i\n", item->data);
  });

  return 0;
}

