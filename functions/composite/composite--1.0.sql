DROP TYPE IF EXISTS custom_type;

CREATE TYPE custom_type AS (a1 integer,a2 text,a3 integer[],a4 bigint);

DROP FUNCTION IF EXISTS composite(custom_type[]);

CREATE OR REPLACE FUNCTION composite(custom_type[])
RETURNS custom_type[]
AS '/extensions/composite/composite','composite'
LANGUAGE C STRICT;

