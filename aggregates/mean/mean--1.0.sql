DROP FUNCTION IF EXISTS mean_sfunc(internal,anynonarray);

CREATE OR REPLACE FUNCTION mean_sfunc(internal,anynonarray)
RETURNS internal AS
'/extensions/aggregates/mean/mean','mean_sfunc'
LANGUAGE C IMMUTABLE;

DROP FUNCTION IF EXISTS mean_ffunc(internal,anynonarray);

CREATE OR REPLACE FUNCTION mean_ffunc(internal,anynonarray)
RETURNS integer AS
'/extensions/aggregates/mean/mean','mean_ffunc'
LANGUAGE C IMMUTABLE;

DROP AGGREGATE IF EXISTS mean(anynonarray);

CREATE OR REPLACE AGGREGATE mean(anynonarray)(
        SFUNC=mean_sfunc,
        STYPE=internal,
        FINALFUNC=mean_ffunc,
        FINALFUNC_EXTRA
);