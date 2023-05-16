DROP FUNCTION IF EXISTS array_rotate_left(integer[],integer);

CREATE OR REPLACE FUNCTION array_rotate_left(integer[],integer)
RETURNS integer[]
AS '/extensions/rotate_array/rotate_array','array_rotate_left'
LANGUAGE C STRICT;