#include "..\public\particles\particles.h"

void CUtlBinaryBlock::Set2(const void* pValue, int nLen)
{
	Assert(!m_Memory.IsReadOnly());

	if (!pValue)
	{
		nLen = 0;
	}

	SetLength(nLen);

	if (m_nActualLength > 0)
	{
		if (((const char*)m_Memory.Base()) >= ((const char*)pValue) + nLen ||
			((const char*)m_Memory.Base()) + m_nActualLength <= ((const char*)pValue))
		{
			memcpy(m_Memory.Base(), pValue, m_nActualLength);
		}
		else
		{
			memmove(m_Memory.Base(), pValue, m_nActualLength);
		}
	}
}

void CUtlString::Set2(const char* pValue)
{
	Assert(!m_Storage.IsReadOnly());
	int nLen = pValue ? Q_strlen(pValue) + 1 : 0;
	m_Storage.Set2(pValue, nLen);
}

CUtlString& CUtlString::operator-=(const char* src)
{
	Assert(!m_Storage.IsReadOnly());
	Set2(src);
	return *this;
}

//-----------------------------------------------------------------------------
// Method to unpack data into a structure
//-----------------------------------------------------------------------------
void CDmxElement::UnpackIntoStructure2(void* pData, const DmxElementUnpackStructure_t* pUnpack) const
{
	for (; pUnpack->m_AttributeType != AT_UNKNOWN; ++pUnpack)
	{
		char* pDest = (char*)pData + pUnpack->m_nOffset;

		// Recurse?
		if (pUnpack->m_pSub)
		{
			UnpackIntoStructure2((void*)pDest, pUnpack->m_pSub);
			continue;
		}

		if (IsArrayType(pUnpack->m_AttributeType))
		{
			// NOTE: This does not work with string/bitfield array data at the moment
			if ((pUnpack->m_AttributeType == AT_STRING_ARRAY) || (pUnpack->m_nBitOffset != NO_BIT_OFFSET))
			{
				AssertMsg(0, ("CDmxElement::UnpackIntoStructure: String and bitfield array attribute types not currently supported!\n"));
				continue;
			}
		}

		if ((pUnpack->m_AttributeType == AT_VOID) || (pUnpack->m_AttributeType == AT_VOID_ARRAY))
		{
			AssertMsg(0, ("CDmxElement::UnpackIntoStructure: Binary blob attribute types not currently supported!\n"));
			continue;
		}

		CDmxAttribute temp(NULL);
		const CDmxAttribute* pAttribute = GetAttribute(pUnpack->m_pAttributeName);
		if (!pAttribute)
		{
			if (!pUnpack->m_pDefaultString)
				continue;

			temp.AllocateDataMemory_AndConstruct(pUnpack->m_AttributeType);
			if (!IsArrayType(pUnpack->m_AttributeType))
			{
				// Convert the default string into the target (array types do this inside GetArrayValue below)
				temp.SetValueFromString(pUnpack->m_pDefaultString);
			}
			pAttribute = &temp;
		}

		if (pUnpack->m_AttributeType != pAttribute->GetType())
		{
			Warning("CDmxElement::UnpackIntoStructure: Mismatched attribute type in attribute \"%s\"!\n", pUnpack->m_pAttributeName);
			continue;
		}

		if (pAttribute->GetType() == AT_STRING)
		{
			if (pUnpack->m_nSize == UTL_STRING_SIZE)  // the string is a UtlString.
			{
				*(CUtlString*)pDest -= pAttribute->GetValueString();
			}
			else  // the string is a preallocated char array.
			{
				// Strings get special treatment: they are stored as in-line arrays of chars
				Q_strncpy(pDest, pAttribute->GetValueString(), pUnpack->m_nSize);
			}
			continue;
		}

		// Get the basic type, if the attribute is an array:
		DmAttributeType_t basicType = CDmxAttribute::ArrayAttributeBasicType(pAttribute->GetType());

		// Special case - if data type is float, but dest size == 16, we are unpacking into simd by replication
		if ((basicType == AT_FLOAT) && (pUnpack->m_nSize == sizeof(fltx4)))
		{
			if (IsArrayType(pUnpack->m_AttributeType))
			{
				// Copy from the attribute into a fixed-size array:
				float* pfDest = (float*)pDest;
				const CUtlVector< float >& floatVector = pAttribute->GetArray< float >();
				for (int i = 0; i < pUnpack->m_nArrayLength; i++)
				{
					for (int j = 0; j < 4; j++) memcpy(pfDest++, &floatVector[i], sizeof(float));
				}
			}
			else
			{
				memcpy(pDest + 0 * sizeof(float), pAttribute->m_pData, sizeof(float));
				memcpy(pDest + 1 * sizeof(float), pAttribute->m_pData, sizeof(float));
				memcpy(pDest + 2 * sizeof(float), pAttribute->m_pData, sizeof(float));
				memcpy(pDest + 3 * sizeof(float), pAttribute->m_pData, sizeof(float));
			}
		}
		else
		{
			int nDataTypeSize = pUnpack->m_nSize;
			if (basicType == AT_INT)
			{
				if (pUnpack->m_nBitOffset == NO_BIT_OFFSET) // This test is not for bitfields
				{
					AssertMsg(nDataTypeSize <= CDmxAttribute::AttributeDataSize(basicType),
						("CDmxElement::UnpackIntoStructure: Incorrect size to unpack data into in attribute \"%s\"!\n", pUnpack->m_pAttributeName));
				}
			}
			else
			{
				AssertMsg(nDataTypeSize == CDmxAttribute::AttributeDataSize(basicType),
					("CDmxElement::UnpackIntoStructure: Incorrect size to unpack data into in attribute \"%s\"!\n", pUnpack->m_pAttributeName));
			}

			if (IsArrayType(pUnpack->m_AttributeType))
			{
				// Copy from the attribute into a fixed-size array (padding with the default value if need be):
				pAttribute->GetArrayValue(pUnpack->m_AttributeType, pDest, nDataTypeSize, pUnpack->m_nArrayLength, pUnpack->m_pDefaultString);
			}
			else if (pUnpack->m_nBitOffset == NO_BIT_OFFSET)
			{
				memcpy(pDest, pAttribute->m_pData, pUnpack->m_nSize);
			}
			else
			{
				if (pAttribute->GetType() == AT_INT)
				{
					// Int attribute types are used for char/short/int.
					switch (pUnpack->m_BitfieldType)
					{
					case BITFIELD_TYPE_BOOL:
						// Note: unsigned char handles bools as bitfields.
						UnpackBitfield((unsigned char*)pDest, pUnpack, pAttribute);
						break;
					case BITFIELD_TYPE_CHAR:
						UnpackBitfield((char*)pDest, pUnpack, pAttribute);
						break;
					case BITFIELD_TYPE_UNSIGNED_CHAR:
						UnpackBitfield((unsigned char*)pDest, pUnpack, pAttribute);
						break;
					case BITFIELD_TYPE_SHORT:
						UnpackBitfield((short*)pDest, pUnpack, pAttribute);
						break;
					case BITFIELD_TYPE_UNSIGNED_SHORT:
						UnpackBitfield((unsigned short*)pDest, pUnpack, pAttribute);
						break;
					case BITFIELD_TYPE_INT:
						UnpackBitfield((int*)pDest, pUnpack, pAttribute);
						break;
					case BITFIELD_TYPE_UNSIGNED_INT:
						UnpackBitfield((unsigned int*)pDest, pUnpack, pAttribute);
						break;
					default:
						Assert(0);
						break;
					};
				}
				else
				{
					UnpackBitfield((char*)pDest, pUnpack, pAttribute);
				}
			}
		}
	}
}