#include "postgresql/14/server/postgres.h"
#include <string.h>
#include "fmgr.h"
#include "postgresql/14/server/utils/geo_decls.h"
#include <stdio.h>
#include "postgresql/14/server/utils/array.h"
#include "postgresql/14/server/utils/memutils.h"
#include "postgresql/14/server/utils/lsyscache.h"
#include "/usr/include/postgresql/14/server/common/int.h"
#include "/usr/include/postgresql/14/server/catalog/pg_type.h"


PG_MODULE_MAGIC;

Datum array_rotate_left(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(array_rotate_left);

Datum array_rotate_left(PG_FUNCTION_ARGS)
{
    ArrayType *a = PG_GETARG_ARRAYTYPE_P(0);
    int16 elemWidth;
    Oid elemType = ARR_ELEMTYPE(a);
    ArrayType *b;
    Datum *datums;
   

    int rotation = PG_GETARG_INT32(1);
    int i, j;
    int temp;
    int count;    
    bool elemTypeByVal;
    char elemAlignmentCode;
    bool *nulls;
    

    get_typlenbyvalalign(elemType, &elemWidth, &elemTypeByVal, &elemAlignmentCode);
    deconstruct_array(a, elemType, elemWidth, elemTypeByVal, elemAlignmentCode, &datums, &nulls, &count);

    for (i=0;i<rotation;i++)
    {    
        temp=datums[0]; 
        for(j=0;j<count;j++)
            datums[j] = datums[j+1];
        datums[count-1]=temp; 
    }

    b = construct_array(datums,count,INT8OID,8,true,'d');

    PG_RETURN_ARRAYTYPE_P(b);

    
}