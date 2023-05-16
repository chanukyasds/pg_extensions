#include "/usr/include/postgresql/14/server/postgres.h"
#include "/usr/include/postgresql/14/server/fmgr.h"
#include "/usr/include/postgresql/14/server/executor/executor.h"
#include "/usr/include/postgresql/14/server/utils/lsyscache.h"
#include "/usr/include/postgresql/14/server/utils/array.h"
#include "/usr/include/postgresql/14/server/utils/builtins.h"
#include "/usr/include/postgresql/14/server/catalog/pg_type.h"
#include "/usr/include/postgresql/14/server/access/tupmacs.h"
#include "/usr/include/postgresql/14/server/funcapi.h"
#include "stdio.h"
#include "stddef.h"

PG_MODULE_MAGIC;

Datum return_records(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(return_records);

Datum return_records(PG_FUNCTION_ARGS)
{
    ArrayType *input_array = PG_GETARG_ARRAYTYPE_P(0);
    Oid elementType = ARR_ELEMTYPE(input_array);

    int16 elementWidth;
    bool elementTypeByVal;
    char elementAlignmentCode;
    bool *nulls;
    Datum *elements;
    int count;

    FuncCallContext *funcctx;
    TupleDesc tupdesc;
    Datum each_row;

    get_typlenbyvalalign(elementType, &elementWidth, &elementTypeByVal, &elementAlignmentCode);
    deconstruct_array(input_array, elementType, elementWidth, elementTypeByVal, elementAlignmentCode, &elements, &nulls, &count);

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldcontext;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /* switch to memory context appropriate for multiple function calls */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* Build a tuple descriptor for our result type */
        tupdesc = CreateTemplateTupleDesc(1); // creating template
        TupleDescInitEntry(tupdesc, (AttrNumber)1, "a1", INT4OID, -1, 0);
        tupdesc = BlessTupleDesc(tupdesc);

        /* allocate required values for funcctx */
        funcctx->max_calls = count;
        funcctx->call_cntr = 0;
        funcctx->tuple_desc = tupdesc;

        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();

    if (funcctx->call_cntr < funcctx->max_calls)
    {
        /* index is passed as funcctx->call_cntr because its starts from 0 and
        increments everytime by SRF_RETURN_NEXT */
        each_row = elements[funcctx->call_cntr];

        SRF_RETURN_NEXT(funcctx, each_row);
    }

    SRF_RETURN_DONE(funcctx);
}