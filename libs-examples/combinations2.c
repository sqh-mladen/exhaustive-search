#include <stdio.h>
#include <gsl/gsl_combination.h>
#include <gsl/gsl_vector.h>


/**
 * qwe
 */

void gsl_all_combinations(gsl_combination * c[], size_t size) {
	size_t max = size - 1;

	gsl_vector * v = gsl_vector_alloc(size);

	void combinations(gsl_vector * v, size_t pos) {
		//printf("Called: %d.\n", (int)pos);
		size_t j, k;
		for (j = 0, k = gsl_combination_n(c[pos]); j < k; j++) {
			//printf("%d\n", (int)j);
			gsl_vector_set(v, pos, (double)gsl_combination_get(c[pos], j));

			if (pos == max) {
				size_t i;
				for (i = 0; i < size; i++)
					printf("%g ", gsl_vector_get(v, i));
				printf("\n");
			} else {
				//gsl_combination_next(c[pos]);
				combinations(v, pos + 1);
			}
		}
	}

	combinations(v, 0);
}

int
main (void)
{
  gsl_combination * c[4];
  size_t i, j, k, l;


  for (j = 0; j < 4; j++) {
	c[j] = gsl_combination_calloc(j+1, j+1);
  }
  gsl_all_combinations(c, 4);

  for (j = 0; j < 4; j++) {
        gsl_combination_free(c[j]);
  }



/*

  for (i = 0, j = 0; j < 4;) {

	for (k = 0; k <= j; k++) {
		do {
			for (l = 0; l < 4; l++) {
				gsl_combination_fprintf(stdout, c[l], "%u ");
			}
			printf("\n");
		} while (gsl_combination_next(c[k]) == GSL_SUCCESS);
	}

	gsl_combination_free(c[j]);
	c[j] = gsl_combination_calloc(4, 1);

	gsl_combination_next(c[(++i % 4)]);
  }

  for (i = 0; i <= 4; i++)
    {
      c = gsl_combination_calloc (4, i);
      do
        {
          printf ("{");
          gsl_combination_fprintf (stdout, c, " %u");
          printf (" }\n");
        }
      while (gsl_combination_next (c) == GSL_SUCCESS);
      gsl_combination_free (c);
    }
*/
  return 0;
}
