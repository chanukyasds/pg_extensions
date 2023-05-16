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

Datum min_to_max_sfunc(PG_FUNCTION_ARGS);
Datum min_to_max_ffunc(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(min_to_max_sfunc);
PG_FUNCTION_INFO_V1(min_to_max_ffunc);

typedef union pgnum
{
	int16 i16;
	int32 i32;
	int64 i64;
	float4 f4;
	float8 f8;
} pgnum;

static text *
array_to_text_internal(FunctionCallInfo fcinfo, ArrayType *v, const char *fldsep, const char *null_string)
{
	text *result;
	int nitems,
		*dims,
		ndims;
	Oid element_type;
	int typlen;
	bool typbyval;
	char typalign;
	StringInfoData buf;
	bool printed = false;
	char *p;
	bits8 *bitmap;
	int bitmask;
	int i;
	ArrayMetaState *my_extra;

	ndims = ARR_NDIM(v);
	dims = ARR_DIMS(v);
	nitems = ArrayGetNItems(ndims, dims);

	if (nitems == 0)
		return cstring_to_text_with_len("", 0);

	element_type = ARR_ELEMTYPE(v);
	
	initStringInfo(&buf);

	my_extra = (ArrayMetaState *)fcinfo->flinfo->fn_extra;

	if (my_extra == NULL)
	{

		fcinfo->flinfo->fn_extra = MemoryContextAlloc(fcinfo->flinfo->fn_mcxt, sizeof(ArrayMetaState));
		my_extra = (ArrayMetaState *)fcinfo->flinfo->fn_extra;
		my_extra->element_type = ~element_type;
	}

	if (my_extra->element_type != element_type)
	{

		get_type_io_data(element_type, IOFunc_output, &my_extra->typlen, &my_extra->typbyval, &my_extra->typalign, &my_extra->typdelim, &my_extra->typioparam, &my_extra->typiofunc);

		fmgr_info_cxt(my_extra->typiofunc, &my_extra->proc, fcinfo->flinfo->fn_mcxt);

		my_extra->element_type = element_type;
	}
	typlen = my_extra->typlen;
	typbyval = my_extra->typbyval;
	typalign = my_extra->typalign;

	p = ARR_DATA_PTR(v);
	bitmap = ARR_NULLBITMAP(v);
	bitmask = 1;

	for (i = 0; i < nitems; i++)
	{
		Datum itemvalue;
		char *value;

		if (bitmap && (*bitmap & bitmask) == 0)
		{
			if (null_string != NULL)
			{
				if (printed)
					appendStringInfo(&buf, "%s%s", fldsep, null_string);
				else
					appendStringInfoString(&buf, null_string);
				printed = true;
			}
		}
		else
		{
			itemvalue = fetch_att(p, typbyval, typlen);

			value = OutputFunctionCall(&my_extra->proc, itemvalue);

			if (printed)
				appendStringInfo(&buf, "%s%s", fldsep, value);
			else
				appendStringInfoString(&buf, value);
			printed = true;

			p = att_addlength_pointer(p, typlen, p);
			p = (char *)att_align_nominal(p, typalign);
		}

		if (bitmap)
		{
			bitmask <<= 1;
			if (bitmask == 0x100)
			{
				bitmap++;
				bitmask = 1;
			}
		}
	}

	result = cstring_to_text_with_len(buf.data, buf.len);
	pfree(buf.data);

	return result;
}

Datum min_to_max_sfunc(PG_FUNCTION_ARGS)
{
	Oid arg_type = get_fn_expr_argtype(fcinfo->flinfo, 1);
	MemoryContext aggcontext;
	ArrayBuildState *state;
	Datum data;

	if (arg_type == InvalidOid)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), errmsg("Invalid Parameter Value")));

	if (!AggCheckCallContext(fcinfo, &aggcontext))
		elog(ERROR, "min_to_max_sfunc called in non-aggregate context");

	if (PG_ARGISNULL(0))
		state = initArrayResult(arg_type, aggcontext, false); // initially first argument is Null so we initArrayResult to return empty ArrayBuildState
	else
		state = (ArrayBuildState *)PG_GETARG_POINTER(0); // from next time we directly get ArrayBuildState

	// fetching the row data , if its null return 0 or fetch the row value
	data = PG_ARGISNULL(1) ? (Datum)0 : PG_GETARG_DATUM(1);

	// make an arrayBuildstate with each data value fetched
	state = accumArrayResult(state, data, PG_ARGISNULL(1), arg_type, aggcontext);

	// returning the state with the rows processed to first argument internal
	PG_RETURN_POINTER(state);
}

Datum min_to_max_ffunc(PG_FUNCTION_ARGS)
{

	ArrayBuildState *state;
	int dims[1], lbs[1], ndims, valsLength, i;
	ArrayType *vals;
	Oid valsType;
	int16 valsTypeWidth, retTypeWidth;
	bool valsTypeByValue, retTypeByValue, resultIsNull = true, *valsNullFlags;
	char valsTypeAlignmentCode, retTypeAlignmentCode;
	Datum *valsContent, retContent[2];
	pgnum minV, maxV;
	bool retNulls[2] = {true, true};
	ArrayType *retArray;

	Assert(AggCheckCallContext(fcinfo, NULL));

	// fetching the data processed by state func from first argument which is internal
	// all the rows will be in this state and later we make an array out of it
	state = PG_ARGISNULL(0) ? NULL : (ArrayBuildState *)PG_GETARG_POINTER(0);

	if (state == NULL)
		PG_RETURN_NULL();

	dims[0] = state->nelems;
	lbs[0] = 1;

	// building array with ArrayBuildstate using makeMdArrayResult and typecast to ArrayType
	vals = (ArrayType *)makeMdArrayResult(state, 1, dims, lbs, CurrentMemoryContext, false);

	ndims = vals->ndim;

	if (ndims > 1)
		ereport(ERROR, (errmsg("Not received one dimensional array ")));

	valsType = ARR_ELEMTYPE(vals);

	if (valsType != INT2OID &&
		valsType != INT4OID &&
		valsType != INT8OID &&
		valsType != FLOAT4OID &&
		valsType != FLOAT8OID)
	{
		ereport(ERROR, (errmsg("Supported Datatypes are SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION.")));
	}

	// getting the width , typebyval and typealignment code
	get_typlenbyvalalign(valsType, &valsTypeWidth, &valsTypeByValue, &valsTypeAlignmentCode);

	// deconstructing the array
	deconstruct_array(vals, valsType, valsTypeWidth, valsTypeByValue, valsTypeAlignmentCode,
					  &valsContent, &valsNullFlags, &valsLength);

	switch (valsType)
	{
	case INT2OID:
		for (i = 0; i < valsLength; i++)
		{
			if (valsNullFlags[i])
			{
				continue;
			}
			else if (resultIsNull)
			{
				minV.i16 = DatumGetInt16(valsContent[i]);
				maxV.i16 = DatumGetInt16(valsContent[i]);
				resultIsNull = false;
			}
			else
			{
				if (DatumGetInt16(valsContent[i]) < minV.i16)
					minV.i16 = DatumGetInt16(valsContent[i]);
				if (DatumGetInt16(valsContent[i]) > maxV.i16)
					maxV.i16 = DatumGetInt16(valsContent[i]);
			}
		}
		retContent[0] = Int16GetDatum(minV.i16);
		retContent[1] = Int16GetDatum(maxV.i16);
		get_typlenbyvalalign(INT2OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
		break;
	case INT4OID:
		for (i = 0; i < valsLength; i++)
		{
			if (valsNullFlags[i])
			{
				continue;
			}
			else if (resultIsNull)
			{
				minV.i32 = DatumGetInt32(valsContent[i]);
				maxV.i32 = DatumGetInt32(valsContent[i]);
				resultIsNull = false;
			}
			else
			{
				if (DatumGetInt32(valsContent[i]) < minV.i32)
					minV.i32 = DatumGetInt32(valsContent[i]);
				if (DatumGetInt32(valsContent[i]) > maxV.i32)
					maxV.i32 = DatumGetInt32(valsContent[i]);
			}
		}
		retContent[0] = Int32GetDatum(minV.i32);
		retContent[1] = Int32GetDatum(maxV.i32);
		get_typlenbyvalalign(INT4OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
		break;
	case INT8OID:
		for (i = 0; i < valsLength; i++)
		{
			if (valsNullFlags[i])
			{
				continue;
			}
			else if (resultIsNull)
			{
				minV.i64 = DatumGetInt64(valsContent[i]);
				maxV.i64 = DatumGetInt64(valsContent[i]);
				resultIsNull = false;
			}
			else
			{
				if (DatumGetInt64(valsContent[i]) < minV.i64)
					minV.i64 = DatumGetInt64(valsContent[i]);
				if (DatumGetInt64(valsContent[i]) > maxV.i64)
					maxV.i64 = DatumGetInt64(valsContent[i]);
			}
		}
		retContent[0] = Int64GetDatum(minV.i64);
		retContent[1] = Int64GetDatum(maxV.i64);
		get_typlenbyvalalign(INT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
		break;
	case FLOAT4OID:
		for (i = 0; i < valsLength; i++)
		{
			if (valsNullFlags[i])
			{
				continue;
			}
			else if (resultIsNull)
			{
				minV.f4 = DatumGetFloat4(valsContent[i]);
				maxV.f4 = DatumGetFloat4(valsContent[i]);
				resultIsNull = false;
			}
			else
			{
				if (DatumGetFloat4(valsContent[i]) < minV.f4)
					minV.f4 = DatumGetFloat4(valsContent[i]);
				if (DatumGetFloat4(valsContent[i]) > maxV.f4)
					maxV.f4 = DatumGetFloat4(valsContent[i]);
			}
		}
		retContent[0] = Float4GetDatum(minV.f4);
		retContent[1] = Float4GetDatum(maxV.f4);
		get_typlenbyvalalign(FLOAT4OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
		break;
	case FLOAT8OID:
		for (i = 0; i < valsLength; i++)
		{
			if (valsNullFlags[i])
			{
				continue;
			}
			else if (resultIsNull)
			{
				minV.f8 = DatumGetFloat8(valsContent[i]);
				maxV.f8 = DatumGetFloat8(valsContent[i]);
				resultIsNull = false;
			}
			else
			{
				if (DatumGetFloat8(valsContent[i]) < minV.f8)
					minV.f8 = DatumGetFloat8(valsContent[i]);
				if (DatumGetFloat8(valsContent[i]) > maxV.f8)
					maxV.f8 = DatumGetFloat8(valsContent[i]);
			}
		}
		retContent[0] = Float8GetDatum(minV.f8);
		retContent[1] = Float8GetDatum(maxV.f8);
		get_typlenbyvalalign(FLOAT8OID, &retTypeWidth, &retTypeByValue, &retTypeAlignmentCode);
		break;
	default:
		ereport(ERROR, (errmsg("Supported Datatypes are SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION.")));
	}

	lbs[0] = 1;
	dims[0] = 2;
	if (!resultIsNull)
	{
		retNulls[0] = false;
		retNulls[1] = false;
	}
	retArray = construct_md_array(retContent, retNulls, 1, dims, lbs, valsType, retTypeWidth, retTypeByValue, retTypeAlignmentCode);

	// PG_RETURN_TEXT_P(return_result(retArray, "->"));
	PG_RETURN_TEXT_P(array_to_text_internal(fcinfo, retArray, "->", NULL));
}
