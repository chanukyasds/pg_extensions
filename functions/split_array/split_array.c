#include "/usr/include/postgresql/14/server/postgres.h"
#include "/usr/include/postgresql/14/server/fmgr.h"
#include "/usr/include/postgresql/14/server/funcapi.h"
#include "/usr/include/postgresql/14/server/executor/executor.h"
#include "/usr/include/postgresql/14/server/executor/spi.h"
#include "/usr/include/postgresql/14/server/utils/array.h"
#include "/usr/include/postgresql/14/server/utils/lsyscache.h"
//#include "/usr/include/postgresql/14/server/utils/builtins.h"
//#include "/usr/include/postgresql/14/server/catalog/pg_type.h"
//#include "/usr/include/postgresql/14/server/access/tupmacs.h"

PG_MODULE_MAGIC;

Datum split_array(PG_FUNCTION_ARGS);
TupleDesc get_tupledesc(int elementType);

PG_FUNCTION_INFO_V1(split_array);

Datum split_array(PG_FUNCTION_ARGS)
{
    ArrayType *input_array = PG_GETARG_ARRAYTYPE_P(0);
    Oid elementType = ARR_ELEMTYPE(input_array);
    int16 elementWidth;
    bool elementTypeByVal;
    char elementAlignmentCode;
    Datum *elements;
    bool *nulls;
    int count;

    FuncCallContext *funcctx;
    TupleDesc tupDesc;
    HeapTupleHeader tupleHeader;
    HeapTuple tuple;
    Datum *fields;
    Datum each_row;
    int i;


    get_typlenbyvalalign(elementType, &elementWidth, &elementTypeByVal, &elementAlignmentCode);
    deconstruct_array(input_array, elementType, elementWidth, elementTypeByVal, elementAlignmentCode, &elements, &nulls, &count);

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldcontext;

        funcctx = SRF_FIRSTCALL_INIT();

        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        tupDesc = get_tupledesc(elementType);

        tupDesc = BlessTupleDesc(tupDesc);

        funcctx->max_calls = count;
        funcctx->call_cntr = 0;
        funcctx->tuple_desc = tupDesc;

        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();

    if(funcctx->call_cntr < funcctx->max_calls)
    {
        fields = palloc(sizeof(Datum *)*funcctx->tuple_desc->natts);

        i = (int)funcctx->call_cntr;

        tupleHeader = DatumGetHeapTupleHeader(elements[i]);

        bool isnull1,isnull2,tup_null;

        fields[0] = GetAttributeByName(tupleHeader,"a1",&isnull1);
        fields[1] = GetAttributeByName(tupleHeader,"a2",&isnull2);

        tuple = heap_form_tuple(funcctx->tuple_desc,fields,&tup_null);

        each_row = HeapTupleGetDatum(tuple);

        SRF_RETURN_NEXT(funcctx,each_row);
    }

    SRF_RETURN_DONE(funcctx);
}


TupleDesc get_tupledesc(int elementType)
{
    char query[500];
    int total_rows;
    SPITupleTable *tupTable;
    TupleDesc tupDesc;
    TupleDesc resultTupleDesc;
    HeapTuple tuple;
    int i;

    AttrNumber attnum;
    const char *attname;
    Oid oidtypeid;
    int32 typmod;
    int attdim;

    pg_sprintf(query, "SELECT  distinct ON (1,2) typs.attnum,typs.attname,typs.atttypid as OidTypeid,pt.typtypmod as typmod,0 as attdim "
                      "FROM pg_type pt JOIN (SELECT attname,typname,atttypid,attnum "
                      "FROM pg_type typ JOIN pg_attribute att ON att.attrelid = typ.typrelid "
                      "WHERE att.attname IN ( SELECT attname FROM pg_attribute "
                      "WHERE attrelid IN (SELECT typrelid FROM pg_type WHERE oid = %d)))typs  "
                      "ON typs.atttypid = pt.oid ORDER BY 1; ",
               elementType);
    SPI_connect();
    SPI_exec(query, 0);
    total_rows = (int)SPI_processed;

    tupTable = SPI_tuptable;
    tupDesc = tupTable->tupdesc;

    resultTupleDesc = CreateTemplateTupleDesc(total_rows);
    for (i = 0; i < total_rows; i++)
    {
        bool isnull1, isnull2, isnull3, isnull4;
        tuple = tupTable->vals[i];

        attnum = SPI_getbinval(tuple,tupDesc, 1, &isnull1);
        attname = SPI_getvalue(tuple,tupDesc, 2);
        oidtypeid = SPI_getbinval(tuple,tupDesc, 3, &isnull2);
        typmod = SPI_getbinval(tuple,tupDesc, 4, &isnull3);
        attdim = SPI_getbinval(tuple,tupDesc, 5, &isnull4);

        TupleDescInitEntry(resultTupleDesc, attnum, attname, oidtypeid, typmod, attdim);
    }

    SPI_finish();

    return resultTupleDesc;
}

