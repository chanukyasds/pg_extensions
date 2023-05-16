# pg_extensions

This repo contains multiple extensions releated to PostgreSQL.

All written in C language with the help of PostgreSQL Header files, Macros and data types.

# Aggregates

1. min_to_max - extension to find min_to_max and return as text with seperator (min->max)
2. mean - extension to find mean of column values (NULLS handled)

# Functions

1. array_concat - extension to contact array of texts 
2. composite -  extension to handle array of composite type paramter and modify its data and return arrray of composite type.
3. count_char - extension to count a character in given string
4. insert_array - extension to insert a new element in array at specific position
5. rotate_array - extension to rotate array elements
6. split_composite - extension to split the array of composite type into mutiple rows of composite type

In near future , I will develop statistic aggregates which may contain multi column aggregates for postgresql.



chanukya sds
