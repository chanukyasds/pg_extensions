DROP FUNCTION IF EXISTS split_composite(custom_type[]);

CREATE OR REPLACE FUNCTION split_composite(custom_type[])
RETURNS SETOF custom_type
AS '/extensions/split_composite/split_composite','split_composite'
LANGUAGE C STRICT;