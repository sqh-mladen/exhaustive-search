#include <stdio.h>
#include <gmp.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_combination.h>
#include <gsl/gsl_multiset.h>

#define MAX_POWER 10
#define MAX_PRIME 4

int primes[] = { 2, 3, 5, 7, 11 };

/**
 * Compute the order of the group
 * @order: the computed order
 * @c: multiset map for prime powers
 */
void es_util_compute_order(mpz_t order, gsl_multiset * c) {
	size_t i, l = gsl_multiset_k(c);	
	mpz_t power, prime;

	mpz_inits(power, prime, NULL);

	mpz_set_ui(order, 1);
	mpz_set_ui(prime, 2);

	for (i = 0; i < l; i++) {
		mpz_pow_ui(power, prime, gsl_multiset_get(c, i));
		mpz_nextprime(prime, prime);
		mpz_mul(order, order, power);
	}

	mpz_clears(power, prime, NULL);
}

void es_check(mpz_t divisor, mpz_t order) {

} 

void es_callback(mpz_t order, gsl_vector * v) {
	size_t i;
	mpz_t prime_power, product;

	mpz_init(prime_power);
	mpz_init(product);
	mpz_set_ui(product, 1);
	for (i = 0; i < v->size; i++) {
		mpz_ui_pow_ui(prime_power, (unsigned int)primes[i], (unsigned int)gsl_vector_get(v, i));
		mpz_mul(product, product, prime_power);
	}

	gmp_printf(": %Zd\n", product);
	mpz_clear(prime_power);
	mpz_clear(product);
}

void es_run_combinations(gsl_combination * c[], size_t size, mpz_t order, void (*callback)(gsl_vector *)) {
	size_t max = size - 1;

	gsl_vector * v = gsl_vector_alloc(size);

	void combinations(gsl_vector * v, size_t pos) {
		size_t j, k;
		for (j = 0, k = gsl_combination_n(c[pos]); j < k; j++) {
			gsl_vector_set(v, pos, gsl_combination_get(c[pos], j));

			if (pos == max) {
				callback(order, v);
			} else {
				combinations(v, pos + 1);
			}
		}
	}

	combinations(v, 0);
}


void es_fork(gsl_multiset * c) {
	size_t i, j, l = gsl_multiset_k(c);

	mpz_t order;
	mpz_init(order);
	es_util_compute_order(order, c);
	gmp_printf("Order: %Zd\n", order);

	gsl_combination * list[l];
	for (i = 0; i < l; i++) {
		j = gsl_multiset_get(c, i);
		list[i] = gsl_combination_calloc((j+1), (j+1));
	}
	printf ("{");
        gsl_multiset_fprintf (stdout, c, " %u");
        printf (" }\n");

	es_run_combinations(list, l, order, es_callback);
}

void es_start(void) {
	size_t i;
	gsl_multiset * c;

	/* Run through all combinations of powers.
	 */
	for (i = 1; i <= MAX_PRIME; i++) {
		c = gsl_multiset_calloc(MAX_POWER, i);
		do {
			// fork on a combination
			es_fork(c);
		} while (gsl_multiset_next(c) == GSL_SUCCESS);
		gsl_multiset_free(c);
	}
}

int main(void) {
	/* Get going.
         */
	es_start();
}
