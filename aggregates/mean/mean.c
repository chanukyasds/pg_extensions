#include <postgres.h>
#include <fmgr.h>
#include <utils/array.h>
#include <catalog/pg_type.h>
#include <utils/lsyscache.h>
#include <math.h>
#include <string.h>
#include <common/int.h>
#include <utils/builtins.h>
#include <utils/typcache.h>
#include <funcapi.h>

PG_MODULE_MAGIC;

Datum mean_sfunc(PG_FUNCTION_ARGS);
Datum mean_ffunc(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(mean_sfunc);
PG_FUNCTION_INFO_V1(mean_ffunc);

Datum mean_sfunc(PG_FUNCTION_ARGS)
{
    Oid arg_type = get_fn_expr_argtype(fcinfo->flinfo,1);
    MemoryContext aggcontext;
    ArrayBuildState *state;
    Datum value;

    if (arg_type == InvalidOid)
        ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),errmsg("Invalid Parameter Value")));
    
    if (!AggCheckCallContext(fcinfo,&aggcontext))
        elog(ERROR, "mean_sfunc called in non-aggregate context");

    if(PG_ARGISNULL(0)) // initally internal will be null so we did initArrayResult
        state = initArrayResult(arg_type,aggcontext,false);
    else                // from next run we do arraybuildstate with internal value returned by previous run and accumulate each row
        state = (ArrayBuildState *)PG_GETARG_POINTER(0);

    // we read value one by one if its null we pass (Datum)0.
    value = PG_ARGISNULL(1)? (Datum) 0 : PG_GETARG_DATUM(1);

    // everytime we make accumArrayResult with the value 
    state = accumArrayResult(state,value,PG_ARGISNULL(1),arg_type,aggcontext);

    // each iteration will have accumArrayResult with value of rows processed
    PG_RETURN_POINTER(state);
}

// so the build state is having all the values in first arg which is of type internal
// now we build the arraybuildstate using the internal value returned by the sfunc

Datum mean_ffunc(PG_FUNCTION_ARGS)
{
    ArrayType *array;
    Oid elementType;
    int16 elementWidth;
    bool elementTypeByVal;
    char elementAlignmentCode;
    Datum *values;
    bool *nulls;
    int count;
    int i;
    int sum=0;
    int nullcount=0;

    ArrayBuildState *state;
    int ndims;
    int dims[1];
    int lbs[1];

    Datum result;

    Assert(AggCheckCallContext(fcinfo,NULL));

    state = PG_ARGISNULL(0) ? NULL : (ArrayBuildState *) PG_GETARG_POINTER(0);

    if (state==NULL)
        PG_RETURN_NULL();

    ndims = 1;
    dims[0] = state->nelems;
    lbs[0] = 1;

    array = (ArrayType *)makeMdArrayResult(state,ndims,dims,lbs,CurrentMemoryContext,false);

    elementType =  ARR_ELEMTYPE(array);
    
    get_typlenbyvalalign(elementType,&elementWidth,&elementTypeByVal,&elementAlignmentCode);

    deconstruct_array(array,elementType,elementWidth,elementTypeByVal,elementAlignmentCode,&values,&nulls,&count);

    for (i=0;i<count;i++)
    {
        if(nulls[i])
        {
            nullcount++;
            continue;
        }
        sum = sum+DatumGetInt64(values[i]);
    }
    
    count = count - nullcount ;

    result = Int64GetDatum(sum/count);

    PG_RETURN_DATUM(result);
}