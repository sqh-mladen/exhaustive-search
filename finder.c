#include <stdio.h>
#include <gmp.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_combination.h>
#include <gsl/gsl_multiset.h>

#define MAX_POWER 10
#define MAX_LENGTH 4
#define MAX_PRIME 7

mpz_t * primes;

void es_util_init_primes(mpz_t start, unsigned int l) {
	size_t i;
	mpz_t prime;
	
	mpz_init_set(prime, start);
	
	free(primes);
	primes = malloc(l * sizeof(mpz_t));
		
	for (i = 0; i < l; i++) {
		mpz_init(primes[i]);
		mpz_set(primes[i], prime);
		mpz_nextprime(prime, prime);
	}
	mpz_clear(prime);
}

/**
 * Compute the order of the group
 * @order: the computed order
 * @c: multiset map for prime powers
 */
void es_util_compute_order(mpz_t order, gsl_multiset * c) {
	size_t i, l = gsl_multiset_k(c);	
	mpz_t power;

	mpz_init(power);
	mpz_set_ui(order, 1);

	for (i = 0; i < l; i++) {
		mpz_pow_ui(power, primes[i], gsl_multiset_get(c, (l - i - 1)));
		mpz_mul(order, order, power);
	}

	mpz_clear(power);
}

void es_util_print_solution(gsl_multiset *c, gsl_vector *v) {
	size_t i, l = gsl_multiset_k(c);
	unsigned long n;
	
	printf("G: ");
	for (i = 0; i < l; i++) {
		n = (unsigned int)gsl_multiset_get(c, (l - i - 1));
		if (n > 0) {
			gmp_printf("%Zd^%d", primes[i], n);
			if (l - i > 1) printf("*");
		}
	}
	printf(", S: ");
	for(i = 0, l = v->size; i < l; i++) {
		n = (unsigned int)gsl_vector_get(v, (l - i -1));
		if (n > 0) {
			gmp_printf("%Zd^%d", primes[i], n);
			if (l - i > 1) printf("*");
		}
	}
	printf("\n");
}

/**
 * Process found solution.
 */
void es_found(gsl_multiset *c, gsl_vector *v, mpz_t order, mpz_t divisor) {
	es_util_print_solution(c, v);
}

/**
 * Check for a solution.
 */
int es_check(mpz_t order, mpz_t divisor) {
	// Check if trivial solution.
	if (mpz_cmp(order, divisor) <= 0) {
		goto end;
	}
	
	mpz_t t, r, d;
	
	mpz_inits(t, r, d, NULL);
	
	mpz_add_ui(d, divisor, 1);
	mpz_mul(t, divisor, d);
	mpz_mod(r, t, order);
	
	// Check if test product is divisible by combination.
	if (mpz_cmp_ui(r, 0) == 0) {
			mpz_divexact(t, t, order);
			mpz_mod_ui(r, t, 2);
			// Check if test product is even.
			if (mpz_cmp_ui(r, 0) == 0) {
				// Check if test product is a perfect power.
				if (mpz_perfect_power_p(t) != 0) {
						// Found a solution.
						return 1;
				}
			}
	}
	end:
	return 0;
}

/**
 * Callback for the computation.
 */
void es_callback(gsl_multiset *c, mpz_t order, gsl_vector * v) {
	size_t i, l;
	mpz_t prime_power, product;

	mpz_inits(prime_power, product, NULL);
	mpz_set_ui(product, 1);
	
	for (i = 0, l = v->size; i < l; i++) {
		mpz_pow_ui(prime_power, primes[i], (unsigned int)gsl_vector_get(v, (l - i - 1)));
		mpz_mul(product, product, prime_power);
	}

	if (es_check(order, product)) {
		es_found(c, v, order, product);
	}
	
	//gmp_printf(": %Zd\n", product);
	mpz_clear(prime_power);
	mpz_clear(product);
}

void es_run_combinations(gsl_multiset *ms, gsl_combination * c[], size_t size, mpz_t order, void (*callback)(gsl_multiset *, mpz_t, gsl_vector *)) {
	size_t max = size - 1;

	gsl_vector * v = gsl_vector_alloc(size);

	void combinations(gsl_vector * v, size_t pos) {
		size_t j, k;
		for (j = 0, k = gsl_combination_n(c[pos]); j < k; j++) {
			gsl_vector_set(v, pos, gsl_combination_get(c[pos], j));

			if (pos == max) {
				callback(ms, order, v);
			} else {
				combinations(v, pos + 1);
			}
		}
	}

	combinations(v, 0);
}


int es_fork(gsl_multiset * c) {
	size_t i, j, l = gsl_multiset_k(c);
	mpz_t order;
	
	mpz_init(order);
	
	es_util_compute_order(order, c);

	gsl_combination * list[l];
	for (i = 0; i < l; i++) {
		j = gsl_multiset_get(c, i);
		if (j == 0) goto exit;
		list[i] = gsl_combination_calloc((j+1), (j+1));
	}
	
	//printf ("{");
    //gsl_multiset_fprintf (stdout, c, " %u");
    //printf (" }\n");
	
	
	es_run_combinations(c, list, l, order, es_callback);
	
	exit:
	return 0;
}

/**
 * Start the computation.
 */
void es_start(void) {
	size_t i, j;
	mpz_t prime;
	gsl_multiset * c;

	mpz_init_set_ui(prime, 2);

	/* Run through all combinations of powers.
	 */
	for (i = 1; i <= MAX_PRIME; i++) {
		// Initialisations.
		es_util_init_primes(prime, MAX_LENGTH);
		mpz_nextprime(prime, prime);
		
		for (j = 1; j <= MAX_LENGTH; j++) {
			// Allocate a multiset of j-th order.
			c = gsl_multiset_calloc(MAX_POWER, j);
			do {
				// Fork on a combination.
				es_fork(c);
			} while (gsl_multiset_next(c) == GSL_SUCCESS);
			gsl_multiset_free(c);
		}
	}
}

int main(void) {
	/* Get going.
	 */
	es_start();
}
