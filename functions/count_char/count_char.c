#include "postgresql/14/server/postgres.h"
#include <string.h>
#include "fmgr.h"
#include "postgresql/14/server/utils/geo_decls.h"
#include <stdio.h>

PG_MODULE_MAGIC;

Datum count_char(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(count_char);

Datum
count_char(PG_FUNCTION_ARGS)
{
    text        *in_string = PG_GETARG_TEXT_PP(0);
    text        *search_char = PG_GETARG_TEXT_PP(1);
    int32       i;
    int64       count=0,length;
    char        *string_array =in_string->vl_dat;
    char        *char_var = search_char->vl_dat;

    if (strlen(char_var)>1)
        elog(ERROR, "only single characters allowed to count");

    length = strlen(string_array);

    for(i=0;i<length;i++)
        {
            if(string_array[i]==char_var[0])
                count++;
        }

    elog(INFO, "count_value %ld",count);
  
    PG_RETURN_INT64(count);

    return 0;
 }