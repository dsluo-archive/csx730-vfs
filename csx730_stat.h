#ifndef CSX730_STAT_H
#define CSX730_STAT_H

#include <stdbool.h>
#include <stdlib.h>

typedef long double val_t;

typedef struct {
  bool   unbiased; /**< Whether the estimators are restricted to be unbiased. */
  size_t n;        /**< The number of values accepted. */
  val_t  sum;      /**< The sum of the accepted values. */
  val_t  sumSq;    /**< The sum of the squares of the accepted values. */
  val_t  min;      /**< The minimum value accepted. */
  val_t  max;      /**< The maximum value accepted. */
} stat_t;

/**
 * Initializes the @c stat_t object pointed to by @p stat.
 * Objects initialized by this function will use unbiased
 * estimators.
 *
 * @param stat    pointer to the @c stat_t object
 */
void csx730_stat_init(stat_t * stat);

/**
 * Initializes the @c stat_t object pointed to by @p stat.
 *
 * @param stat    pointer to the @c stat_t object
 * @param biased  whether or not the statistic is biased
 */
void csx730_stat_init_biased(stat_t * stat, bool biased);

/**
 * Accept another value into the statistic information.
 *
 * @param stat  pointer to the @c stat_t object
 * @param value the value to accept
 */
void csx730_stat_accept(stat_t * stat, val_t value);

/**
 * Returns the mean of the accepted values.
 *
 * @param stat  pointer to the @c stat_t object
 */
val_t csx730_stat_mean(stat_t * stat);

/**
 * Returns the population variance of the accepted values.
 *
 * @param stat  pointer to the @c stat_t object
 */
val_t csx730_stat_variance(stat_t * stat);

/**
 * Returns the standard deviation of the accepted values.
 * @param stat  pointer to the @c stat_t object
 */
val_t csx730_stat_stddev(stat_t * stat);

#endif // CSX730_STAT_H
