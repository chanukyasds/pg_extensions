#include "/usr/include/postgresql/14/server/postgres.h"
#include "/usr/include/postgresql/14/server/fmgr.h"
#include "/usr/include/postgresql/14/server/funcapi.h"
#include "/usr/include/postgresql/14/server/executor/executor.h"
#include "/usr/include/postgresql/14/server/executor/spi.h"
#include "/usr/include/postgresql/14/server/utils/builtins.h"
#include "/usr/include/postgresql/14/server/utils/lsyscache.h"
#include "/usr/include/postgresql/14/server/utils/array.h"
#include "/usr/include/postgresql/14/server/catalog/pg_type.h"
#include "/usr/include/postgresql/14/server/access/tupmacs.h"

PG_MODULE_MAGIC;

Datum split_composite(PG_FUNCTION_ARGS);
TupleDesc generate_tupledesc(int elementType);

PG_FUNCTION_INFO_V1(split_composite);

Datum split_composite(PG_FUNCTION_ARGS)
{
    ArrayType *input_array = PG_GETARG_ARRAYTYPE_P(0);
    Oid elementType = ARR_ELEMTYPE(input_array);

    int16 elementWidth;
    bool elementTypeByVal;
    char elementAlignmentCode;
    Datum *elements;
    bool *nulls;
    int count;

    int i;
    FuncCallContext *funcctx;
    TupleDesc tupDesc;
    HeapTupleHeader tupHeader;
    Datum *fields;
    HeapTuple tuple;
    Datum each_row;

    get_typlenbyvalalign(elementType, &elementWidth, &elementTypeByVal, &elementAlignmentCode);

    deconstruct_array(input_array, elementType, elementWidth, elementTypeByVal, elementAlignmentCode, &elements, &nulls, &count);

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldcontext;

        funcctx = SRF_FIRSTCALL_INIT();

        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        tupDesc = generate_tupledesc(elementType);

        tupDesc = BlessTupleDesc(tupDesc);

        funcctx->max_calls = count;
        funcctx->call_cntr = 0;
        funcctx->tuple_desc = tupDesc;

        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();

    if (funcctx->call_cntr < funcctx->max_calls)
    {
        fields = palloc(sizeof(Datum *) * funcctx->tuple_desc->natts);

        bool isnull1, isnull2, isnull3, isnull4, tupnull;

        i = (int)funcctx->call_cntr;

        tupHeader = DatumGetHeapTupleHeader(elements[i]);

        fields[0] = GetAttributeByName(tupHeader, "a1", &isnull1);
        fields[1] = GetAttributeByName(tupHeader, "a2", &isnull2);
        fields[2] = GetAttributeByName(tupHeader, "a3", &isnull3);
        fields[3] = GetAttributeByName(tupHeader, "a4", &isnull4);

        tuple = heap_form_tuple(funcctx->tuple_desc, fields, &tupnull);

        each_row = HeapTupleGetDatum(tuple);

        SRF_RETURN_NEXT(funcctx, each_row);
    }

    SRF_RETURN_DONE(funcctx);
}

TupleDesc generate_tupledesc(int elementType)
{
    char query[500];
    int total_rows;

    TupleDesc tupdesc;
    HeapTuple tuple;
    SPITupleTable *tuptable;

    TupleDesc resTupleDesc;
    AttrNumber attnum;
    const char *attname;
    Oid oidtypeid;
    int32 typmod;
    int attdim;

    int i;

    pg_sprintf(query, "SELECT typs.attnum,typs.attname,typs.atttypid as OidTypeid,pt.typtypmod as typmod,0 as attdim "
                      "FROM pg_type pt JOIN (SELECT attname,typname,atttypid,attnum "
                      "FROM pg_type typ JOIN pg_attribute att ON att.attrelid = typ.typrelid "
                      "WHERE att.attname IN ( SELECT attname FROM pg_attribute "
                      "WHERE attrelid IN (SELECT typrelid FROM pg_type WHERE oid = %d)))typs  "
                      "ON typs.atttypid = pt.oid ORDER BY 1; ",
               elementType);

    SPI_connect();
    SPI_exec(query, 0);
    total_rows = (int)SPI_processed;

    tuptable = SPI_tuptable;
    tupdesc = tuptable->tupdesc;

    /*handling cols for future req*/

    resTupleDesc = CreateTemplateTupleDesc(total_rows);
    for (i = 0; i < total_rows; i++)
    {
        bool isnull1, isnull2, isnull3, isnull4;
        tuple = tuptable->vals[i];
        
        /*Finding Entries to create Tuple Desc*/
        attnum = SPI_getbinval(tuple, tupdesc, 1, &isnull1);
        attname = SPI_getvalue(tuple, tupdesc, 2);
        oidtypeid = SPI_getbinval(tuple, tupdesc, 3, &isnull2);
        typmod = SPI_getbinval(tuple, tupdesc, 4, &isnull3);
        attdim = SPI_getbinval(tuple, tupdesc, 5, &isnull4);

        TupleDescInitEntry(resTupleDesc, attnum, attname, oidtypeid, typmod, attdim);
    }

    SPI_finish();

    return resTupleDesc;
}