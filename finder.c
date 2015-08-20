/**
 * Exhaustive Search
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gmp.h"
#include "gsl/gsl_vector.h"
#include "gsl/gsl_combination.h"
#include "gsl/gsl_multiset.h"

#define MAX_POWER 10

#define MIN_LENGTH 1
#define MAX_LENGTH 5

#define START_PRIME 3
#define END_PRIME 50

mpz_t * primes;

/**
 * 
 */
struct es_settings {
	unsigned int start_prime;
	unsigned int end_prime;
	unsigned int min_length;
	unsigned int max_length;
};

/**
 * es_util_init_primes:
 * Initialise the prime sequence.
 * 
 * @start: Starting prime number.
 * @l: Length of the sequence.
 */
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
 * es_util_compute_order:
 * Compute the order of the group
 * 
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

void es_util_log_openfile() {
	
}

/**
 * 
 */
 
void es_util_log_progress() {
	
}

void es_util_log_status(mpz_t prime, size_t pnumber, unsigned long l) {
	gmp_printf("Finished batch: starting prime %Zd (#%d) in seq of length %d.\n", prime, pnumber, l);
}

/**
 * es_util_log_summary:
 * Log summary of a settings and scheduled progress.
 * 
 * @s: Settings used for the batch.
 */
void es_util_log_summary(struct es_settings s) {
	printf("Batch settings: starting prime %d, length of prime seq %d, "
		   "min length %d and max length %d.\n", 
		   s.start_prime, s.end_prime, s.max_length, s.min_length);
}

/**
 * es_util_print_combination:
 * Print a formatted combination from multiset.
 * 
 * @c: 
 */
void es_util_print_combination(gsl_multiset *c) {
	printf ("<");
    gsl_multiset_fprintf (stdout, c, " %u");
    printf (" >\n");
}

/**
 * es_util_print_solution:
 * Print formatted solution to standard output.
 * 
 */
void es_util_print_solution(gsl_multiset *c, gsl_vector *v) {
	size_t i, l = gsl_multiset_k(c);
	unsigned long n;
	
	printf("Group: ");
	for (i = 0; i < l; i++) {
		n = (unsigned int)gsl_multiset_get(c, (l - i - 1));
		if (n > 0) {
			gmp_printf("%Zd^%d", primes[i], n);
			if (l - i > 1) printf("*");
		}
	}
	printf(", Subgroup: ");
	for(i = 0, l = v->size; i < l; i++) {
		n = (unsigned int)gsl_vector_get(v, (l - i - 1));
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
			// Check if test product is even.
			if (mpz_even_p(t) != 0) {
				// Check if test product is a perfect power.
				if (mpz_cmp_ui(t, 2) == 0 || mpz_perfect_power_p(t) != 0) {
						// Found a solution.
						return 1;
				}
			}
	}
	
	// Free memory of temporary variables.
	mpz_clears(t, r, d, NULL);
	
	// Nothing to do here.
	end:
	return 0;
}

/**
 * es_callback:
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
	
	mpz_clears(prime_power, product, NULL);
}

/**
 * es_run_combination:
 * Run through all possibilities for a combination.
 * 
 */
void es_run_combination(gsl_multiset *ms, gsl_combination * c[], size_t size, mpz_t order, void (*callback)(gsl_multiset *, mpz_t, gsl_vector *)) {
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
	// Compute recursively.
	combinations(v, 0);
	// Free memory.
	gsl_vector_free(v);
}

/**
 * es_fork:
 * Fork process on a combination.
 * 
 * @c: The combination to process.
 */
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
	
	
	es_run_combination(c, list, l, order, es_callback);
	
	exit:
	mpz_clear(order);
	return 0;
}

/**
 * es_start:
 * Start the computation for a partition of primes.
 * 
 * @s: 
 */
void es_start(struct es_settings s) {
	size_t i, j;
	mpz_t prime;
	gsl_multiset * c;

	mpz_init_set_ui(prime, s.start_prime);

	/* Run through all combinations of powers.
	 */
	for (i = 1; i <= s.end_prime; i++) {
		// Prepare the sequence of primes.
		es_util_init_primes(prime, MAX_LENGTH);
		
		for (j = s.min_length; j <= s.max_length; j++) {
			// Allocate a combination of j-th order.
			c = gsl_multiset_calloc(MAX_POWER, j);
			do {
				// Fork on a combination.
				es_fork(c);
			} while (gsl_multiset_next(c) == GSL_SUCCESS);
			gsl_multiset_free(c);
		}
		
		es_util_log_status(prime, i, s.max_length);
		
		mpz_nextprime(prime, prime);
	}
	
	mpz_clear(prime);
}

int main(int argc, char ** argv) {
	int c, verbose = 0;
	struct es_settings settings;
	
	// Set the defaults.
	settings = (struct es_settings) {
		.start_prime = START_PRIME,
		.end_prime = END_PRIME,
		.max_length = MAX_LENGTH,
		.min_length = MIN_LENGTH
	};
	
	// Process the arguments.
	while ((c = getopt (argc, argv, "s:e:m:n:v")) != -1) {
		switch (c) {
			case 's': {
				settings.start_prime = atoi(optarg);
				break;
			}
			case 'e': {
				settings.end_prime = atoi(optarg);
				break;
			}
			case 'm': {
				settings.max_length = atoi(optarg);
				break;
			}
			case 'n': {
				settings.min_length = atoi(optarg);
				break;
			}
			case 'v': {
				verbose = 1;
				break;
			}
			case '?': {
				if (optopt == 'c')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			}
			default: {
				abort();
			}
		}
	}
	
	/* Get going.
	 */
	// Log batch.
	es_util_log_summary(settings);
	// Start given the settings.
	es_start(settings);
	// Log batch.
	es_util_log_summary(settings);
	
	return 0;
}
