DROP FUNCTION IF EXISTS insert_array(integer[],integer,integer);

CREATE OR REPLACE FUNCTION insert_array(integer[],integer,integer)
RETURNS integer[]
AS '/extensions/insert_array/insert_array','insert_array'
LANGUAGE C STRICT;