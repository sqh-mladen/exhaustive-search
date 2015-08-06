#include <stdio.h>
#include <gsl/gsl_multiset.h>

int
main (void)
{
  gsl_multiset * c, * copy;
  size_t i, j;

  printf ("All multisets of {0,1,2,3} by size:\n") ;
  for (i = 1; i <= 4; i++)
    {
      c = gsl_multiset_calloc (10, i);
      copy = gsl_multiset_alloc(10, i);
      do
        {
		printf ("{");
          gsl_multiset_fprintf (stdout, c, " %u");
          printf (" }\n");


		gsl_multiset_memcpy(copy, c);
		do {

			for (j = 0; j < i; j++) {
				if (gsl_multiset_get(copy, j) > gsl_multiset_get(c, j))
					goto next;
			}

			gsl_multiset_fprintf(stdout, copy, "%u ");
			next:
			printf("\n");

		} while (gsl_multiset_prev(copy) == GSL_SUCCESS);

        }
      while (gsl_multiset_next (c) == GSL_SUCCESS);
      gsl_multiset_free (c);
      gsl_multiset_free(copy);
    }

  return 0;
}
