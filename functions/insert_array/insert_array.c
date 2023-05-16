#include "/usr/include/postgresql/14/server/postgres.h"
#include "/usr/include/postgresql/14/server/fmgr.h"
#include "/usr/include/postgresql/14/server/utils/lsyscache.h"
#include "/usr/include/postgresql/14/server/utils/array.h"
#include "/usr/include/postgresql/14/server/catalog/pg_type.h"
#include "stdio.h"

PG_MODULE_MAGIC;

Datum insert_array(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(insert_array);

Datum
insert_array(PG_FUNCTION_ARGS)
{
    ArrayType *input_array = PG_GETARG_ARRAYTYPE_P(0);
    int new_element = PG_GETARG_INT32(1);
    int position = PG_GETARG_INT32(2);

    Oid elementype = ARR_ELEMTYPE(input_array);
    int16 elementwidth;
    bool elementtypebyval;
    char elementalignmentcode;
    Datum *elements;
    bool *nulls;
    int count;

    int *arr;
    int i,replaced_element;
    int found=0;

    ArrayType *output_array;

    get_typlenbyvalalign(elementype,&elementwidth,&elementtypebyval,&elementalignmentcode);

    deconstruct_array(input_array,elementype,elementwidth,elementtypebyval,elementalignmentcode,&elements,&nulls,&count);

    arr = malloc(sizeof(int *)*count+1);

    for (i=0;i<count;i++)
        arr[i]=elements[i];

    for (i=0;i<count;i++)
    {
        if (i==position)
        {
            replaced_element = arr[i];
            arr[i] = new_element;
            count++;
            found = 1;
            continue;
        }
        if (found==1)
        {
            arr[i]= arr[i]+replaced_element;
            replaced_element = arr[i]-replaced_element;
            arr[i]=arr[i]-replaced_element;
        }
    }

    elements = repalloc(elements,count);

    for (i=0;i<count;i++)
        elements[i]=Int64GetDatum(arr[i]);

    output_array = construct_array(elements,count,INT8OID,8,true,'d');

    PG_RETURN_ARRAYTYPE_P(output_array);

}