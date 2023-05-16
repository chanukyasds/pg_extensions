/*
	Dropping and Re creating sfunc: min_to_max_sfunc for aggregate 
	Accepts ANYNONARRAY TYPE
	Returns INTERNAL TYPE
*/

DROP FUNCTION IF EXISTS min_to_max_sfunc(internal,anynonarray);

CREATE OR REPLACE FUNCTION 
min_to_max_sfunc(internal,anynonarray)
RETURNS internal
AS '/extensions/aggregates/min_to_max/min_to_max', 'min_to_max_sfunc'
LANGUAGE c IMMUTABLE;

/*
	Dropping and Re creating ffunc: min_to_max_ffunc for aggregate 
	Accepts INTERNAL TYPE
	Returns TEXT TYPE
*/

DROP FUNCTION IF EXISTS min_to_max_ffunc(internal,anynonarray);

CREATE OR REPLACE FUNCTION 
min_to_max_ffunc(internal,anynonarray)
RETURNS text
AS '/extensions/aggregates/min_to_max/min_to_max', 'min_to_max_ffunc'
LANGUAGE c IMMUTABLE;

/*
	Dropping and Re creating AGGREGATE min_to_max 
*/

DROP AGGREGATE IF EXISTS min_to_max(anynonarray);

CREATE OR REPLACE AGGREGATE min_to_max(anynonarray)(
        SFUNC=min_to_max_sfunc,
        STYPE=internal,
        FINALFUNC=min_to_max_ffunc,
        FINALFUNC_EXTRA
);