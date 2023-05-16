DROP FUNCTION IF EXISTS split_array(stud_type[]);

CREATE OR REPLACE FUNCTION split_array(stud_type[])
RETURNS SETOF stud_type
AS '/extensions/split_array/split_array','split_array'
LANGUAGE C STRICT;