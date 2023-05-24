/*
*   Primary source file statistical distribustion functuions

    functions with prefix "cdf" is for cumulative distribution
    functions with prefix "rdf" is for random distribution

    Available Distributions:

        1. F-Distribution
        2. Gaussian Distribution
        3. Unit Gaussian Distribution
        4. Gaussian Tail Distribution
        5. Bivariate Gaussian Distribution
        6. Exponential Distribution
        7. Laplace Distribution
        8. Exponential Power Distribution
        9. Cauchy Distribution
       10. Rayleigh Distribution
       11. Rayleigh Tail Distribution
       12. Landau Distribution
       13. Gamma Distribution
       14. Flat (Uniform) Distribution
       15. Lognormal Distribution
       16. Chi-squared Distribution
       17. T-Distribution
       18. Beta Distribution
       19. Logistic Distribution
       20. Pareto Distribution
       21. Weibull Distribution
       22. Type-1 Gumbel Distribution
       23. Type-2 Gumbel Distribution
       24. Poisson Distribution
       25. Bernoulli Distribution
       26. Binomial Distribution
       27. Negative Binomial Distribution
       28. Pascal Distribution
       29. Geometric Distribution
       30. Hypergeometric Distribution
       31. Logarithmic Distribution

*/

#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <postgres.h>
#include <fmgr.h>
#include <math.h>

PG_MODULE_MAGIC;

/* F Distribution Function Prototypes */
Datum rdf_fdist(PG_FUNCTION_ARGS);
Datum cdf_fdist_p(PG_FUNCTION_ARGS);
Datum cdf_fdist_q(PG_FUNCTION_ARGS);
Datum cdf_fdist_pinv(PG_FUNCTION_ARGS);
Datum cdf_fdist_qinv(PG_FUNCTION_ARGS);

/* Gaussian Distribution Function Prototypes */
Datum rdf_gaussian(PG_FUNCTION_ARGS);
Datum cdf_gaussian_p(PG_FUNCTION_ARGS);
Datum cdf_gaussian_q(PG_FUNCTION_ARGS);
Datum cdf_gaussian_pinv(PG_FUNCTION_ARGS);
Datum cdf_gaussian_qinv(PG_FUNCTION_ARGS);

/* Unit Gaussian Distribution Function Prototypes */
Datum rdf_unit_gaussian(PG_FUNCTION_ARGS);
Datum cdf_unit_gaussian_p(PG_FUNCTION_ARGS);
Datum cdf_unit_gaussian_q(PG_FUNCTION_ARGS);
Datum cdf_unit_gaussian_pinv(PG_FUNCTION_ARGS);
Datum cdf_unit_gaussian_qinv(PG_FUNCTION_ARGS);

/* Gaussian Tail Distribution Function Prototypes */
Datum rdf_gaussian_tail(PG_FUNCTION_ARGS);
Datum rdf_unit_gaussian_tail(PG_FUNCTION_ARGS);

/* Bivariate Gaussian Distribution Function ProtoTypes */
Datum rdf_bivariate_gaussian(PG_FUNCTION_ARGS);

/* Exponential Distribution Function Prototypes */
Datum rdf_expodist(PG_FUNCTION_ARGS);
Datum cdf_expodist_p(PG_FUNCTION_ARGS);
Datum cdf_expodist_q(PG_FUNCTION_ARGS);
Datum cdf_expodist_pinv(PG_FUNCTION_ARGS);
Datum cdf_expodist_qinv(PG_FUNCTION_ARGS);

/* Version 1 Calling */

PG_FUNCTION_INFO_V1(rdf_fdist);
PG_FUNCTION_INFO_V1(cdf_fdist_p);
PG_FUNCTION_INFO_V1(cdf_fdist_q);
PG_FUNCTION_INFO_V1(cdf_fdist_pinv);
PG_FUNCTION_INFO_V1(cdf_fdist_qinv);
PG_FUNCTION_INFO_V1(rdf_gaussian);
PG_FUNCTION_INFO_V1(cdf_gaussian_p);
PG_FUNCTION_INFO_V1(cdf_gaussian_q);
PG_FUNCTION_INFO_V1(cdf_gaussian_pinv);
PG_FUNCTION_INFO_V1(cdf_gaussian_qinv);
PG_FUNCTION_INFO_V1(rdf_unit_gaussian);
PG_FUNCTION_INFO_V1(cdf_unit_gaussian_p);
PG_FUNCTION_INFO_V1(cdf_unit_gaussian_q);
PG_FUNCTION_INFO_V1(cdf_unit_gaussian_pinv);
PG_FUNCTION_INFO_V1(cdf_unit_gaussian_qinv);
PG_FUNCTION_INFO_V1(rdf_gaussian_tail);
PG_FUNCTION_INFO_V1(rdf_unit_gaussian_tail);
PG_FUNCTION_INFO_V1(rdf_bivariate_gaussian);

PG_FUNCTION_INFO_V1(rdf_expodist);
PG_FUNCTION_INFO_V1(cdf_expodist_p);
PG_FUNCTION_INFO_V1(cdf_expodist_q);
PG_FUNCTION_INFO_V1(cdf_expodist_pinv);
PG_FUNCTION_INFO_V1(cdf_expodist_qinv);

/* F Distribution Function Definitions */

Datum rdf_fdist(PG_FUNCTION_ARGS)
{
    const double alpha = PG_GETARG_FLOAT8(0);
    const double df1 = PG_GETARG_FLOAT8(1);
    const double df2 = PG_GETARG_FLOAT8(2);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_fdist_pdf(alpha, df1, df2);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_fdist(alpha,df1,df2)."),
                           errhint("Check input values for rdf_fdist function.")));
}

Datum cdf_fdist_p(PG_FUNCTION_ARGS)
{
    const double alpha = PG_GETARG_FLOAT8(0);
    const double df1 = PG_GETARG_FLOAT8(1);
    const double df2 = PG_GETARG_FLOAT8(2);
    float pval;

    gsl_set_error_handler_off();

    pval = gsl_cdf_fdist_P(alpha, df1, df2);

    if (gsl_finite(pval))
        PG_RETURN_FLOAT8(pval);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_fdist_p(alpha,df1,df2)."),
                           errhint("Check input values for cdf_fdist_p function.")));
}

Datum cdf_fdist_q(PG_FUNCTION_ARGS)
{
    const double alpha = PG_GETARG_FLOAT8(0);
    const double df1 = PG_GETARG_FLOAT8(1);
    const double df2 = PG_GETARG_FLOAT8(2);
    float qval;

    gsl_set_error_handler_off();

    qval = gsl_cdf_fdist_Q(alpha, df1, df2);

    if (gsl_finite(qval))
        PG_RETURN_FLOAT8(qval);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_fdist_q(alpha,df1,df2)."),
                           errhint("Check input values for cdf_fdist_q function.")));
}

Datum cdf_fdist_pinv(PG_FUNCTION_ARGS)
{
    const double alpha = PG_GETARG_FLOAT8(0);
    const double df1 = PG_GETARG_FLOAT8(1);
    const double df2 = PG_GETARG_FLOAT8(2);
    float pinv;

    gsl_set_error_handler_off();

    pinv = gsl_cdf_fdist_Pinv(alpha, df1, df2);

    if (gsl_finite(pinv))
        PG_RETURN_FLOAT8(pinv);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_fdist_pinv(alpha,df1,df2)."),
                           errhint("Check input values for cdf_fdist_pinv function.")));
}

Datum cdf_fdist_qinv(PG_FUNCTION_ARGS)
{
    const double alpha = PG_GETARG_FLOAT8(0);
    const double df1 = PG_GETARG_FLOAT8(1);
    const double df2 = PG_GETARG_FLOAT8(2);
    float qinv;

    gsl_set_error_handler_off();

    qinv = gsl_cdf_fdist_Qinv(alpha, df1, df2);

    if (gsl_finite(qinv))
        PG_RETURN_FLOAT8(qinv);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_fdist_qinv(alpha,df1,df2)."),
                           errhint("Check input values for cdf_fdist_qinv function.")));
}

/* Gaussian Distribution Function Definitions */

Datum rdf_gaussian(PG_FUNCTION_ARGS)
{
    double x = PG_GETARG_FLOAT8(0);
    double sigma = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_gaussian_pdf(x, sigma);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_gaussian(x,sigma)."),
                           errhint("Check input values for rdf_gaussian function.")));
}

Datum cdf_gaussian_p(PG_FUNCTION_ARGS)
{
    double x = PG_GETARG_FLOAT8(0);
    double sigma = PG_GETARG_FLOAT8(1);
    float pval;

    gsl_set_error_handler_off();

    pval = gsl_cdf_gaussian_P(x, sigma);

    if (gsl_finite(pval))
        PG_RETURN_FLOAT8(pval);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_gaussian_p(x,sigma)."),
                           errhint("Check input values for cdf_gaussian_p function.")));
}

Datum cdf_gaussian_q(PG_FUNCTION_ARGS)
{
    double x = PG_GETARG_FLOAT8(0);
    double sigma = PG_GETARG_FLOAT8(1);
    float qval;

    gsl_set_error_handler_off();

    qval = gsl_cdf_gaussian_Q(x, sigma);

    if (gsl_finite(qval))
        PG_RETURN_FLOAT8(qval);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_gaussian_q(x,sigma)."),
                           errhint("Check input values for cdf_gaussian_q function.")));
}

Datum cdf_gaussian_pinv(PG_FUNCTION_ARGS)
{
    double P = PG_GETARG_FLOAT8(0);
    double sigma = PG_GETARG_FLOAT8(1);
    float pinv;

    gsl_set_error_handler_off();

    pinv = gsl_cdf_gaussian_Pinv(P, sigma);

    if (gsl_finite(pinv))
        PG_RETURN_FLOAT8(pinv);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_gaussian_pinv(P,sigma)."),
                           errhint("Check input values for cdf_gaussian_pinv function.")));
}

Datum cdf_gaussian_qinv(PG_FUNCTION_ARGS)
{
    double Q = PG_GETARG_FLOAT8(0);
    double sigma = PG_GETARG_FLOAT8(1);
    float qinv;

    gsl_set_error_handler_off();

    qinv = gsl_cdf_gaussian_P(Q, sigma);

    if (gsl_finite(qinv))
        PG_RETURN_FLOAT8(qinv);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_gaussian_qinv(Q,sigma)."),
                           errhint("Check input values for cdf_gaussian_qinv function.")));
}

/* Unit Gaussian Distribution Function Definitions */

Datum rdf_unit_gaussian(PG_FUNCTION_ARGS)
{
    double x = PG_GETARG_FLOAT8(0);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_ugaussian_pdf(x);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_unit_gaussian(x)."),
                           errhint("Check input values for rdf_unit_gaussian function.")));
}

Datum cdf_unit_gaussian_p(PG_FUNCTION_ARGS)
{
    double x = PG_GETARG_FLOAT8(0);
    float pval;

    gsl_set_error_handler_off();

    pval = gsl_cdf_ugaussian_P(x);

    if (gsl_finite(pval))
        PG_RETURN_FLOAT8(pval);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_unit_gaussian_p(x)."),
                           errhint("Check input values for cdf_unit_gaussian_p function.")));
}

Datum cdf_unit_gaussian_q(PG_FUNCTION_ARGS)
{
    double x = PG_GETARG_FLOAT8(0);
    float qval;

    gsl_set_error_handler_off();

    qval = gsl_cdf_ugaussian_Q(x);

    if (gsl_finite(qval))
        PG_RETURN_FLOAT8(qval);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_unit_gaussian_q(x)."),
                           errhint("Check input values for cdf_unit_gaussian_q function.")));
}

Datum cdf_unit_gaussian_pinv(PG_FUNCTION_ARGS)
{
    double P = PG_GETARG_FLOAT8(0);
    float pinv;

    gsl_set_error_handler_off();

    pinv = gsl_cdf_ugaussian_Pinv(P);

    if (gsl_finite(pinv))
        PG_RETURN_FLOAT8(pinv);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_unit_gaussian_pinv(P)."),
                           errhint("Check input values for cdf_unit_gaussian_pinv function.")));
}

Datum cdf_unit_gaussian_qinv(PG_FUNCTION_ARGS)
{
    double Q = PG_GETARG_FLOAT8(0);
    float qinv;

    gsl_set_error_handler_off();

    qinv = gsl_cdf_ugaussian_P(Q);

    if (gsl_finite(qinv))
        PG_RETURN_FLOAT8(qinv);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_unit_gaussian_qinv(Q)."),
                           errhint("Check input values for cdf_unit_gaussian_qinv function.")));
}

/* Gaussian Tail Distribution Function Definitions */

Datum rdf_gaussian_tail(PG_FUNCTION_ARGS)
{
    const double x = PG_GETARG_FLOAT8(0);
    const double a = PG_GETARG_FLOAT8(1);
    const double sigma = PG_GETARG_FLOAT8(2);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_gaussian_tail_pdf(x, a, sigma);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_gaussian_tail(x,a,sigma)."),
                           errhint("Check input values for rdf_gaussian_tail function.")));
}

Datum rdf_unit_gaussian_tail(PG_FUNCTION_ARGS)
{
    const double x = PG_GETARG_FLOAT8(0);
    const double a = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_ugaussian_tail_pdf(x, a);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_unit_gaussian_tail(x,a)."),
                           errhint("Check input values for rdf_unit_gaussian_tail function.")));
}

/* Bivariate Gaussian Distribution Function Definitions */

Datum rdf_bivariate_gaussian(PG_FUNCTION_ARGS)
{
    const double x = PG_GETARG_FLOAT8(0);
    const double y = PG_GETARG_FLOAT8(1);
    const double sigma_x = PG_GETARG_FLOAT8(2);
    const double sigma_y = PG_GETARG_FLOAT8(3);
    const double rho = PG_GETARG_FLOAT8(4);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_bivariate_gaussian_pdf(x, y, sigma_x, sigma_y, rho);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_bivariate_gaussian(x,y,sigma_x,sigma_y,rho)."),
                           errhint("Check input values for rdf_bivariate_gaussian function.")));
}

/* Exponential Distribution Function Definitions */

Datum rdf_expodist(PG_FUNCTION_ARGS)
{

    const double x = PG_GETARG_FLOAT8(0);
    const double mu = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_ran_exponential_pdf(x, mu);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate rdf_expodist(x,mu)."),
                           errhint("Check input values for rdf_expodist function.")));
}

Datum cdf_expodist_p(PG_FUNCTION_ARGS)
{
    const double x = PG_GETARG_FLOAT8(0);
    const double mu = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_cdf_exponential_P(x, mu);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_expodist_p(x,mu)."),
                           errhint("Check input values for cdf_expodist_p function.")));
}

Datum cdf_expodist_q(PG_FUNCTION_ARGS)
{
    const double x = PG_GETARG_FLOAT8(0);
    const double mu = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_cdf_exponential_Q(x, mu);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_expodist_q(x,mu)."),
                           errhint("Check input values for cdf_expodist_q function.")));
}

Datum cdf_expodist_pinv(PG_FUNCTION_ARGS)
{
    const double P = PG_GETARG_FLOAT8(0);
    const double mu = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_cdf_exponential_Pinv(P, mu);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_expodist_pinv(P,mu)."),
                           errhint("Check input values for cdf_expodist_pinv function.")));
}

Datum cdf_expodist_qinv(PG_FUNCTION_ARGS)
{
    const double Q = PG_GETARG_FLOAT8(0);
    const double mu = PG_GETARG_FLOAT8(1);
    float res;

    gsl_set_error_handler_off();

    res = gsl_cdf_exponential_Qinv(Q, mu);

    if (gsl_finite(res))
        PG_RETURN_FLOAT8(res);
    else
        ereport(ERROR, (
                           errmsg("Unable to calculate cdf_expodist_qinv(Q,mu)."),
                           errhint("Check input values for cdf_expodist_qinv function.")));
}

