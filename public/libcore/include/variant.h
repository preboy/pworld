#pragma once

enum class var_type : uint8
{
    vt_none,
    vt_uint8,
    vt_uint16,
    vt_uint32,
    vt_uint64,

	vt_int8,
	vt_int16,
	vt_int32,
	vt_int64,

    vt_string,
};


template<typename T>
class TypeInfo
{
public:
    static const bool is_valid = false;
};


#define DEF_AVAILABLE_TYPE(TYPE, ETYPE) \
template<> \
class TypeInfo<TYPE> \
{\
public:\
    static const bool is_valid = true; \
    static const var_type type = ETYPE; \
};

DEF_AVAILABLE_TYPE(uint8,  var_type::vt_uint8)
DEF_AVAILABLE_TYPE(uint16, var_type::vt_uint16)
DEF_AVAILABLE_TYPE(uint32, var_type::vt_uint32)
DEF_AVAILABLE_TYPE(uint64, var_type::vt_uint64)

DEF_AVAILABLE_TYPE(int8,  var_type::vt_int8)
DEF_AVAILABLE_TYPE(int16, var_type::vt_int16)
DEF_AVAILABLE_TYPE(int32, var_type::vt_int32)
DEF_AVAILABLE_TYPE(int64, var_type::vt_int64)

DEF_AVAILABLE_TYPE(std::string, var_type::vt_string)

#undef DEF_AVAILABLE_TYPE


class CVariant
{
public:
    CVariant(){}
    ~CVariant(){}

    CVariant(const CVariant&) = delete;
    CVariant& operator = (const CVariant&) = delete;

public:
    template<typename T>
    CVariant(const T& t)
    {
        _type = var_type::vt_none;
        static_assert(TypeInfo<T>::is_valid, "CVariant Error: unsupport type!");
        SetValue<T>(t);
	}

    template<typename T>
    void SetValue(const T& t)
    {
        _type = TypeInfo<T>::type;
        switch (_type)
        {
        case var_type::vt_uint8:
            _var_uint8 = (uint8)(t);
            break;
        case var_type::vt_uint16:
            _var_uint16 = (uint16)(t);
            break;
        case var_type::vt_uint32:
            _var_uint32 = (uint32)(t);
            break;
        case var_type::vt_uint64:
            _var_uint64 = (uint64)(t);
            break;
        case var_type::vt_int8:
            _var_int8 = (int8)(t);
            break;
        case var_type::vt_int16:
            _var_int16 = (int16)(t);
            break;
        case var_type::vt_int32:
            _var_int32 = (int32)(t);
            break;
        case var_type::vt_int64:
            _var_int64 = (int64)(t);
            break;
        case var_type::vt_string:
            _var_string = std::string(t);
            break;
        default:
            static_assert(TypeInfo<T>::is_valid, "CVariant Error: unsupport type!");
        }
    }

	inline var_type GetType() { return _type; }

	template<typename T>
	T GetValue()
	{
		std::assert(_type == TypeInfo<T>::type);
		switch (_type)
		{
		case var_type::vt_uint8:
			return _var_uint8;
			break;
		case var_type::vt_uint16:
			return _var_uint16;
			break;
		case var_type::vt_uint32:
			return _var_uint32;
			break;
		case var_type::vt_uint64:
			return _var_uint64;
			break;
		case var_type::vt_int8:
			return _var_int8;
			break;
		case var_type::vt_int16:
			return _var_int16;
			break;
		case var_type::vt_int32:
			return _var_int32;
			break;
		case var_type::vt_int64:
			return _var_int64;
			break;
        case var_type::vt_string:
            return _var_string;
            break;
		default:
			static_assert(TypeInfo<T>::is_valid, "CVariant Error: unsupport type!");
			break;
		}
	}

private:
    var_type _type;
    union
    {
        uint8           _var_uint8;
        uint16          _var_uint16;
        uint32          _var_uint32;
        uint64          _var_uint64;
        int8            _var_int8;
        int16           _var_int16;
        int32           _var_int32;
        int64           _var_int64;
        std::string     _var_string;
	};

};

