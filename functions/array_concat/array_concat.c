#include "/usr/include/postgresql/14/server/postgres.h"
#include "/usr/include/postgresql/14/server/fmgr.h"
#include "/usr/include/postgresql/14/server/funcapi.h"
#include "/usr/include/postgresql/14/server/utils/array.h"
#include "/usr/include/postgresql/14/server/utils/lsyscache.h"

PG_MODULE_MAGIC;

Datum array_concat(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(array_concat);

Datum array_concat(PG_FUNCTION_ARGS)
{
    ArrayType *input_array = PG_GETARG_ARRAYTYPE_P(0);
    Oid elementType = ARR_ELEMTYPE(input_array);
    int16 elementWidth;
    bool elementTypeByval;
    char elementAlignmentCode;
    Datum *elements;
    bool *nulls;
    int count;

    int i;
    text *result;
    int size = 0;
    int run_size = 0;

    size_t each_size;

    get_typlenbyvalalign(elementType, &elementWidth, &elementTypeByval, &elementAlignmentCode);
    deconstruct_array(input_array, elementType, elementWidth, elementTypeByval, elementAlignmentCode, &elements, &nulls, &count);

    for (i = 0; i < count; i++)
        size = size + VARSIZE_ANY_EXHDR(DatumGetCString(elements[i]));

    size = size + VARHDRSZ;

    result = (text *)palloc(size);

    SET_VARSIZE(result, size);

    for (i = 0; i < count; i++)
    {
        each_size = VARSIZE_ANY_EXHDR(DatumGetCString(elements[i]));
        memcpy(VARDATA(result) + run_size, VARDATA_ANY(elements[i]), each_size);
        run_size = run_size + each_size;
    }

    PG_RETURN_TEXT_P(result);

    
}
