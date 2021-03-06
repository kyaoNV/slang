#include "slang-string.h"
#include "text-io.h"

namespace Slang
{
    // OSString

    OSString::OSString()
        : beginData(0)
        , endData(0)
    {}

    OSString::OSString(wchar_t* begin, wchar_t* end)
        : beginData(begin)
        , endData(end)
    {}

    OSString::~OSString()
    {
        if (beginData)
        {
            delete[] beginData;
        }
    }

    static const wchar_t kEmptyOSString[] = { 0 };

    wchar_t const* OSString::begin() const
    {
        return beginData ? beginData : kEmptyOSString;
    }

    wchar_t const* OSString::end() const
    {
        return endData ? endData : kEmptyOSString;
    }

    // StringSlice

    StringSlice::StringSlice()
        : representation(0)
        , beginIndex(0)
        , endIndex(0)
    {}

    StringSlice::StringSlice(String const& str, UInt beginIndex, UInt endIndex)
        : representation(str.buffer)
        , beginIndex(beginIndex)
        , endIndex(endIndex)
    {}


    //

	_EndLine EndLine;

    String operator+(const char * op1, const String & op2)
	{
        String result(op1);
        result.append(op2);
        return result;
	}

	String operator+(const String & op1, const char * op2)
	{
        String result(op1);
        result.append(op2);
        return result;
	}

	String operator+(const String & op1, const String & op2)
	{
        String result(op1);
        result.append(op2);
        return result;
	}

	int StringToInt(const String & str, int radix)
	{
		if (str.StartsWith("0x"))
			return (int)strtoll(str.Buffer(), NULL, 16);
		else
			return (int)strtoll(str.Buffer(), NULL, radix);
	}
	unsigned int StringToUInt(const String & str, int radix)
	{
		if (str.StartsWith("0x"))
			return (unsigned int)strtoull(str.Buffer(), NULL, 16);
		else
			return (unsigned int)strtoull(str.Buffer(), NULL, radix);
	}
	double StringToDouble(const String & str)
	{
		return (double)strtod(str.Buffer(), NULL);
	}
	float StringToFloat(const String & str)
	{
		return strtof(str.Buffer(), NULL);
	}

#if 0
	String String::ReplaceAll(String src, String dst) const
	{
		String rs = *this;
		int index = 0;
		int srcLen = src.length;
		int len = rs.length;
		while ((index = rs.IndexOf(src, index)) != -1)
		{
			rs = rs.SubString(0, index) + dst + rs.SubString(index + srcLen, len - index - srcLen);
			len = rs.length;
		}
		return rs;
	}
#endif

	String String::FromWString(const wchar_t * wstr)
	{
#ifdef _WIN32
		return Slang::Encoding::UTF16->ToString((const char*)wstr, (int)(wcslen(wstr) * sizeof(wchar_t)));
#else
		return Slang::Encoding::UTF32->ToString((const char*)wstr, (int)(wcslen(wstr) * sizeof(wchar_t)));
#endif
	}

	String String::FromWString(const wchar_t * wstr, const wchar_t * wend)
	{
#ifdef _WIN32
		return Slang::Encoding::UTF16->ToString((const char*)wstr, (int)((wend - wstr) * sizeof(wchar_t)));
#else
		return Slang::Encoding::UTF32->ToString((const char*)wstr, (int)((wend - wstr) * sizeof(wchar_t)));
#endif
	}

	String String::FromWChar(const wchar_t ch)
	{
#ifdef _WIN32
		return Slang::Encoding::UTF16->ToString((const char*)&ch, (int)(sizeof(wchar_t)));
#else
		return Slang::Encoding::UTF32->ToString((const char*)&ch, (int)(sizeof(wchar_t)));
#endif
	}

	String String::FromUnicodePoint(unsigned int codePoint)
	{
		char buf[6];
		int len = Slang::EncodeUnicodePointToUTF8(buf, (int)codePoint);
		buf[len] = 0;
		return String(buf);
	}

	OSString String::ToWString(UInt* outLength) const
	{
		if (!buffer)
		{
            return OSString();
		}
		else
		{
			List<char> buf;
			Slang::Encoding::UTF16->GetBytes(buf, *this);

            auto length = buf.Count() / sizeof(wchar_t);
			if (outLength)
				*outLength = length;

			buf.Add(0);
			buf.Add(0);

            wchar_t* beginData = (wchar_t*)buf.Buffer();
            wchar_t* endData = beginData + length;

			buf.ReleaseBuffer();

            return OSString(beginData, endData);
		}
	}

    //

    void String::ensureUniqueStorageWithCapacity(UInt requiredCapacity)
    {
        if (buffer && buffer->isUniquelyReferenced() && buffer->capacity >= requiredCapacity)
            return;

        UInt newCapacity = buffer ? 2*buffer->capacity : 16;
        if (newCapacity < requiredCapacity)
        {
            newCapacity = requiredCapacity;
        }

        UInt length = getLength();
        StringRepresentation* newRepresentation = StringRepresentation::createWithCapacityAndLength(newCapacity, length);

        if (buffer)
        {
            memcpy(newRepresentation->getData(), buffer->getData(), length + 1);
        }

        buffer = newRepresentation;
    }

    char* String::prepareForAppend(UInt count)
    {
        auto oldLength = getLength();
        auto newLength = oldLength + count;
        ensureUniqueStorageWithCapacity(newLength);
        return getData() + oldLength;
    }


    void String::append(const char* textBegin, char const* textEnd)
    {
        auto oldLength = getLength();
        auto textLength = textEnd - textBegin;

        auto newLength = oldLength + textLength;

        ensureUniqueStorageWithCapacity(newLength);

        memcpy(getData() + oldLength, textBegin, textLength);
        getData()[newLength] = 0;
        buffer->length = newLength;
    }

    void String::append(char const* str)
    {
        if (str)
        {
            append(str, str + strlen(str));
        }
    }

    void String::append(char chr)
    {
        append(&chr, &chr + 1);
    }

    void String::append(String const& str)
    {
        if (!buffer)
        {
            buffer = str.buffer;
            return;
        }

        append(str.begin(), str.end());
    }

    void String::append(StringSlice const& slice)
    {
        append(slice.begin(), slice.end());
    }

    void String::append(int value, int radix)
    {
        enum { kCount = 33 };
        char* data = prepareForAppend(kCount);
        auto count = IntToAscii(data, value, radix);
        ReverseInternalAscii(data, count);
        buffer->length += count;
    }

    void String::append(unsigned int value, int radix)
    {
        enum { kCount = 33 };
        char* data = prepareForAppend(kCount);
        auto count = IntToAscii(data, value, radix);
        ReverseInternalAscii(data, count);
        buffer->length += count;
    }

    void String::append(long long value, int radix)
    {
        enum { kCount = 65 };
        char* data = prepareForAppend(kCount);
        auto count = IntToAscii(data, value, radix);
        ReverseInternalAscii(data, count);
        buffer->length += count;
    }


    void String::append(float val, const char * format)
    {
        enum { kCount = 128 };
        char* data = prepareForAppend(kCount);
        sprintf_s(data, kCount, format, val);
        buffer->length += strnlen_s(data, kCount);
    }

    void String::append(double val, const char * format)
    {
        enum { kCount = 128 };
        char* data = prepareForAppend(kCount);
        sprintf_s(data, kCount, format, val);
        buffer->length += strnlen_s(data, kCount);
    }
}
