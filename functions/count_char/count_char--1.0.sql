DROP FUNCTION IF EXISTS count_char(text,text);

CREATE OR REPLACE FUNCTION count_char(text,text)
RETURNS bigint
AS '/home/chanukyasds/code/postgres_dev/count_char/count_char','count_char'
LANGUAGE C STRICT;

