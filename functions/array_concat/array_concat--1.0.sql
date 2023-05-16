DROP FUNCTION IF EXISTS array_concat(text[]);

CREATE OR REPLACE FUNCTION array_concat(text[]) RETURNS text 
AS '/extensions/array_concat/array_concat','array_concat'
LANGUAGE C STRICT;