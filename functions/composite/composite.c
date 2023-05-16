#include "/usr/include/postgresql/14/server/postgres.h"
#include "/usr/include/postgresql/14/server/fmgr.h"
#include "/usr/include/postgresql/14/server/executor/executor.h"
#include "/usr/include/postgresql/14/server/utils/lsyscache.h"
#include "/usr/include/postgresql/14/server/utils/array.h"
#include "/usr/include/postgresql/14/server/catalog/pg_type.h"
#include "/usr/include/postgresql/14/server/access/tupmacs.h"
#include "/usr/include/postgresql/14/server/funcapi.h"
#include "stdio.h"
#include "stddef.h"

PG_MODULE_MAGIC;

Datum composite(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(composite);

Datum composite(PG_FUNCTION_ARGS)
{
    ArrayType *input_array = PG_GETARG_ARRAYTYPE_P(0);
    Oid elementtype = ARR_ELEMTYPE(input_array);
    int16 elementwidth;
    bool elementtypebyval;
    char elementalignmentcode;
    Datum *elements;
    bool *nulls;
    int count;

    int i, j;
    int atts_count = 4;
    TupleDesc resultTupleDesc;
    HeapTupleHeader row_header;
    Datum *output_datums;
    Datum *result_datums;
    ArrayType *output_array;
    ArrayType *inner_arry;
    HeapTuple ret_tuple;
    int *arr;
    int sum;
    int in_arr_len;

    // getting array info
    get_typlenbyvalalign(elementtype, &elementwidth, &elementtypebyval, &elementalignmentcode);

    // deconstructing the array to get datums and count
    deconstruct_array(input_array, elementtype, elementwidth, elementtypebyval, elementalignmentcode, &elements, &nulls, &count);

    // building the TupleDesc manually
    // So i avoided using obsolete function TypeGetTupleDesc
    // atts information can be found in pg_attribute and pg_type
    // future we can manage this automatically by fetching query records with OID on pg_attribute view
    resultTupleDesc = CreateTemplateTupleDesc(atts_count); // creating template
    TupleDescInitEntry(resultTupleDesc, (AttrNumber)1, "a1", INT4OID, -1, 0);
    TupleDescInitEntry(resultTupleDesc, (AttrNumber)2, "a2", TEXTOID, -1, 0);
    TupleDescInitEntry(resultTupleDesc, (AttrNumber)3, "a3", INT4ARRAYOID, -1, 0);
    TupleDescInitEntry(resultTupleDesc, (AttrNumber)4, "a4", INT4OID, -1, 0);
    resultTupleDesc = BlessTupleDesc(resultTupleDesc); // for datums it is required to Bless

    // allocating memory to result_datums
    // we have to return same no of records so count is used
    result_datums = palloc(sizeof(Datum) * count);

    for (i = 0; i < count; i++)
    {

        bool isnull1, isnull2, isnull3, innerarrnull, isnull_tup;
        sum = 0;

        // getting header of each record in array
        row_header = DatumGetHeapTupleHeader(elements[i]);

        // third one is array so forming an integer array
        inner_arry = DatumGetArrayTypeP(GetAttributeByName(row_header, "a3", &innerarrnull));
        arr = (int *)ARR_DATA_PTR(inner_arry);

        // calculating internal array length
        in_arr_len = ARR_DIMS(inner_arry)[0];

        // calculating sum of internal array
        for (j = 0; j < in_arr_len; j++)
            sum = sum + arr[j];

        // allocating memory for datums based on atts inside each_record
        output_datums = palloc(sizeof(Datum *) * atts_count);

        // allocating individual datum values to first 3 atts and calculated sum to 4th att
        output_datums[0] = GetAttributeByName(row_header, "a1", &isnull1);
        output_datums[1] = GetAttributeByName(row_header, "a2", &isnull2);
        output_datums[2] = GetAttributeByName(row_header, "a3", &isnull3);
        output_datums[3] = (Datum)sum;

        // Building a record / tuple with the datums array
        ret_tuple = heap_form_tuple(resultTupleDesc, output_datums, &isnull_tup);

        // getting datum from tuple and adding to result_datums
        result_datums[i] = HeapTupleGetDatum(ret_tuple);

        // every time releasing memory to avoid memory leaks
        pfree(output_datums);
    }

    // constructing array with the result_datums
    output_array = construct_array(result_datums, count, elementtype, elementwidth, elementtypebyval, elementalignmentcode);

    PG_RETURN_ARRAYTYPE_P(output_array);
}
