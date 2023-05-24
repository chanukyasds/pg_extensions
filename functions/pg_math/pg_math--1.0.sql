
DROP SCHEMA IF EXISTS "math";

CREATE SCHEMA "math";

SET search_path = 'math';

/* F Distribution Functions */ 

DROP FUNCTION IF EXISTS rdf_fdist(float8,float8,float8);

CREATE OR REPLACE FUNCTION rdf_fdist(float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_fdist'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_fdist_p(float8,float8,float8);

CREATE OR REPLACE FUNCTION cdf_fdist_p(float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_fdist_p'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_fdist_q(float8,float8,float8);

CREATE OR REPLACE FUNCTION cdf_fdist_q(float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_fdist_q'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_fdist_pinv(float8,float8,float8);

CREATE OR REPLACE FUNCTION cdf_fdist_pinv(float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_fdist_pinv'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_fdist_qinv(float8,float8,float8);

CREATE OR REPLACE FUNCTION cdf_fdist_qinv(float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_fdist_qinv'
LANGUAGE C STRICT;

/* Gaussian Distribution Functions */ 

DROP FUNCTION IF EXISTS rdf_gaussian(float8,float8);

CREATE OR REPLACE FUNCTION rdf_gaussian(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_gaussian'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_gaussian_p(float8,float8);

CREATE OR REPLACE FUNCTION cdf_gaussian_p(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_gaussian_p'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_gaussian_q(float8,float8);

CREATE OR REPLACE FUNCTION cdf_gaussian_q(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_gaussian_q'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_gaussian_pinv(float8,float8);

CREATE OR REPLACE FUNCTION cdf_gaussian_pinv(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_gaussian_pinv'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_gaussian_qinv(float8,float8);

CREATE OR REPLACE FUNCTION cdf_gaussian_qinv(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_gaussian_qinv'
LANGUAGE C STRICT;

/* Unit Gaussian Distribution Functions */ 

DROP FUNCTION IF EXISTS rdf_unit_gaussian(float8);

CREATE OR REPLACE FUNCTION rdf_unit_gaussian(float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_unit_gaussian'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_unit_gaussian_p(float8);

CREATE OR REPLACE FUNCTION cdf_unit_gaussian_p(float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_unit_gaussian_p'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_unit_gaussian_q(float8);

CREATE OR REPLACE FUNCTION cdf_unit_gaussian_q(float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_unit_gaussian_q'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_unit_gaussian_pinv(float8);

CREATE OR REPLACE FUNCTION cdf_unit_gaussian_pinv(float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_unit_gaussian_pinv'
LANGUAGE C STRICT;


DROP FUNCTION IF EXISTS cdf_unit_gaussian_qinv(float8);

CREATE OR REPLACE FUNCTION cdf_unit_gaussian_qinv(float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_unit_gaussian_qinv'
LANGUAGE C STRICT;

/* Gaussian Tail Distribution Functions */ 

DROP FUNCTION IF EXISTS rdf_gaussian_tail(float8,float8,float8);

CREATE OR REPLACE FUNCTION rdf_gaussian_tail(float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_gaussian_tail'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS rdf_unit_gaussian_tail(float8,float8);

CREATE OR REPLACE FUNCTION rdf_unit_gaussian_tail(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_unit_gaussian_tail'
LANGUAGE C STRICT;

/* Bivariate Gaussian Distribution Functions */

DROP FUNCTION IF EXISTS rdf_bivariate_gaussian(float8,float8,float8,float8,float8);

CREATE OR REPLACE FUNCTION rdf_bivariate_gaussian(float8,float8,float8,float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_bivariate_gaussian'
LANGUAGE C STRICT;

/* Exponential Distribution Functions */

DROP FUNCTION IF EXISTS rdf_expodist(float8,float8);

CREATE OR REPLACE FUNCTION rdf_expodist(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','rdf_expodist'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_expodist_p(float8,float8);

CREATE OR REPLACE FUNCTION cdf_expodist_p(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_expodist_p'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_expodist_q(float8,float8);

CREATE OR REPLACE FUNCTION cdf_expodist_q(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_expodist_q'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_expodist_pinv(float8,float8);

CREATE OR REPLACE FUNCTION cdf_expodist_pinv(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_expodist_pinv'
LANGUAGE C STRICT;

DROP FUNCTION IF EXISTS cdf_expodist_qinv(float8,float8);

CREATE OR REPLACE FUNCTION cdf_expodist_qinv(float8,float8)
RETURNS float8
AS '/extensions/pg_math/pg_math','cdf_expodist_qinv'
LANGUAGE C STRICT;

