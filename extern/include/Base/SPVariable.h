#ifndef S_SPVARIABLE_H_
#define S_SPVARIABLE_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#ifndef IMATH_HALF_NO_LOOKUP_TABLE
	#define IMATH_HALF_NO_LOOKUP_TABLE
#endif
#include <Imath/half.h>

#include <string>
#include <bitset>

#pragma warning(push)
#pragma warning(disable: _VCRUNTIME_DISABLED_WARNINGS)


typedef signed char        sp_int_8;
typedef short              sp_int_16;
typedef int                sp_int_32;
typedef long long          sp_int_64;

typedef unsigned char      sp_uint_8;
typedef unsigned short     sp_uint_16;
typedef unsigned int       sp_uint_32;
typedef unsigned long long sp_uint_64;

typedef half   sp_decimal_h;
typedef float  sp_decimal_f;
typedef double sp_decimal_d;

typedef bool		  sp_bool;
typedef std::string	  sp_string;
typedef std::wstring	  sp_wstring;
typedef char	  sp_char;
typedef wchar_t	  sp_wchar;
typedef unsigned char sp_uchar;
typedef std::bitset<128> sp_uuid;

typedef Eigen::RowVector<sp_int_8,  2>  sp_ivec2_8;
typedef Eigen::RowVector<sp_int_8,  3>  sp_ivec3_8;
typedef Eigen::RowVector<sp_int_8,  4>  sp_ivec4_8;

typedef Eigen::RowVector<sp_int_16, 2>  sp_ivec2_16;
typedef Eigen::RowVector<sp_int_16, 3>  sp_ivec3_16;
typedef Eigen::RowVector<sp_int_16, 4>  sp_ivec4_16;

typedef Eigen::RowVector<sp_int_32, 2>  sp_ivec2_32;
typedef Eigen::RowVector<sp_int_32, 3>  sp_ivec3_32;
typedef Eigen::RowVector<sp_int_32, 4>  sp_ivec4_32;

typedef Eigen::RowVector<sp_int_64, 2>  sp_ivec2_64;
typedef Eigen::RowVector<sp_int_64, 3>  sp_ivec3_64;
typedef Eigen::RowVector<sp_int_64, 4>  sp_ivec4_64;

typedef Eigen::RowVector<sp_uint_8,  2>  sp_uivec2_8;
typedef Eigen::RowVector<sp_uint_8,  3>  sp_uivec3_8;
typedef Eigen::RowVector<sp_uint_8,  4>  sp_uivec4_8;

typedef Eigen::RowVector<sp_uint_16, 2>  sp_uivec2_16;
typedef Eigen::RowVector<sp_uint_16, 3>  sp_uivec3_16;
typedef Eigen::RowVector<sp_uint_16, 4>  sp_uivec4_16;

typedef Eigen::RowVector<sp_uint_32, 2>  sp_uivec2_32;
typedef Eigen::RowVector<sp_uint_32, 3>  sp_uivec3_32;
typedef Eigen::RowVector<sp_uint_32, 4>  sp_uivec4_32;

typedef Eigen::RowVector<sp_uint_64, 2>  sp_uivec2_64;
typedef Eigen::RowVector<sp_uint_64, 3>  sp_uivec3_64;
typedef Eigen::RowVector<sp_uint_64, 4>  sp_uivec4_64;


typedef Eigen::RowVector<sp_decimal_h, 2>  sp_vec2_h;
typedef Eigen::RowVector<sp_decimal_h, 3>  sp_vec3_h;
typedef Eigen::RowVector<sp_decimal_h, 4>  sp_vec4_h;

typedef Eigen::RowVector<sp_decimal_f, 2>  sp_vec2_f;
typedef Eigen::RowVector<sp_decimal_f, 3>  sp_vec3_f;
typedef Eigen::RowVector<sp_decimal_f, 4>  sp_vec4_f;

typedef Eigen::RowVector<sp_decimal_d, 2>  sp_vec2_d;
typedef Eigen::RowVector<sp_decimal_d, 3>  sp_vec3_d;
typedef Eigen::RowVector<sp_decimal_d, 4>  sp_vec4_d;


typedef Eigen::Matrix <sp_int_8,  2, 2> sp_imat2_8;
typedef Eigen::Matrix <sp_int_8,  3, 3> sp_imat3_8;
typedef Eigen::Matrix <sp_int_8,  4, 4> sp_imat4_8;
typedef Eigen::MatrixX<sp_int_8>        sp_imatn_8;


typedef Eigen::Matrix <sp_int_16, 2, 2> sp_imat2_16;
typedef Eigen::Matrix <sp_int_16, 3, 3> sp_imat3_16;
typedef Eigen::Matrix <sp_int_16, 4, 4> sp_imat4_16;
typedef Eigen::MatrixX<sp_int_16>       sp_imatn_16;

typedef Eigen::Matrix <sp_int_32, 2, 2> sp_imat2_32;
typedef Eigen::Matrix <sp_int_32, 3, 3> sp_imat3_32;
typedef Eigen::Matrix <sp_int_32, 4, 4> sp_imat4_32;
typedef Eigen::MatrixX<sp_int_32>       sp_imatn_32;

typedef Eigen::Matrix <sp_int_64, 2, 2> sp_imat2_64;
typedef Eigen::Matrix <sp_int_64, 3, 3> sp_imat3_64;
typedef Eigen::Matrix <sp_int_64, 4, 4> sp_imat4_64;
typedef Eigen::MatrixX<sp_int_64>       sp_imatn_64;

typedef Eigen::Matrix <sp_uint_8,  2, 2> sp_uimat2_8;
typedef Eigen::Matrix <sp_uint_8,  3, 3> sp_uimat3_8;
typedef Eigen::Matrix <sp_uint_8,  4, 4> sp_uimat4_8;
typedef Eigen::MatrixX<sp_uint_8>        sp_uimatn_8;

typedef Eigen::Matrix <sp_uint_16, 2, 2> sp_uimat2_16;
typedef Eigen::Matrix <sp_uint_16, 3, 3> sp_uimat3_16;
typedef Eigen::Matrix <sp_uint_16, 4, 4> sp_uimat4_16;
typedef Eigen::MatrixX<sp_uint_16>       sp_uimatn_16;

typedef Eigen::Matrix <sp_uint_32, 2, 2> sp_uimat2_32;
typedef Eigen::Matrix <sp_uint_32, 3, 3> sp_uimat3_32;
typedef Eigen::Matrix <sp_uint_32, 4, 4> sp_uimat4_32;
typedef Eigen::MatrixX<sp_uint_32>       sp_uimatn_32;

typedef Eigen::Matrix <sp_uint_64, 2, 2> sp_uimat2_64;
typedef Eigen::Matrix <sp_uint_64, 3, 3> sp_uimat3_64;
typedef Eigen::Matrix <sp_uint_64, 4, 4> sp_uimat4_64;
typedef Eigen::MatrixX<sp_uint_64>       sp_uimatn_64;

typedef Eigen::Matrix <sp_decimal_h, 2, 2> sp_dmat2_h;
typedef Eigen::Matrix <sp_decimal_h, 3, 3> sp_dmat3_h;
typedef Eigen::Matrix <sp_decimal_h, 4, 4> sp_dmat4_h;
typedef Eigen::MatrixX<sp_decimal_h>       sp_dmatn_h;

typedef Eigen::Matrix <sp_decimal_f, 2, 2> sp_dmat2_f;
typedef Eigen::Matrix <sp_decimal_f, 3, 3> sp_dmat3_f;
typedef Eigen::Matrix <sp_decimal_f, 4, 4> sp_dmat4_f;
typedef Eigen::MatrixX<sp_decimal_f>       sp_dmatn_f;

typedef Eigen::Matrix <sp_decimal_d, 2, 2> sp_dmat2_d;
typedef Eigen::Matrix <sp_decimal_d, 3, 3> sp_dmat3_d;
typedef Eigen::Matrix <sp_decimal_d, 4, 4> sp_dmat4_d;
typedef Eigen::MatrixX<sp_decimal_d>       sp_dmatn_d;


#ifdef S_HIGH_PRECISION

typedef sp_int_64    spi_int;
typedef sp_uint_64   spi_uint;
typedef sp_decimal_d spd_decimal;

typedef sp_ivec2_64 spv_ivec2;
typedef sp_ivec3_64 spv_ivec3;
typedef sp_ivec4_64 spv_ivec4;

typedef sp_uivec2_64 spv_uivec2;
typedef sp_uivec3_64 spv_uivec3;
typedef sp_uivec4_64 spv_uivec4;

typedef sp_vec2_d spv_vec2;
typedef sp_vec3_d spv_vec3;
typedef sp_vec4_d spv_vec4;

typedef sp_imat2_64 spi_imat2;
typedef sp_imat3_64 spi_imat3;
typedef sp_imat4_64 spi_imat4;
typedef sp_imatn_64 spi_imatn;

typedef sp_uimat2_64 spi_uimat2;
typedef sp_uimat3_64 spi_uimat3;
typedef sp_uimat4_64 spi_uimat4;
typedef sp_uimatn_64 spi_uimatn;

typedef sp_dmat2_d  spm_dmat2;
typedef sp_dmat3_d  spm_dmat3;
typedef sp_dmat4_d  spm_dmat4;
typedef sp_dmatn_d  spm_dmatn;

#elif defined(S_LOW_PRECISION)

typedef sp_int_16    spi_int;
typedef sp_uint_16   spi_uint;
typedef sp_decimal_h spd_decimal;

typedef sp_ivec2_16 spv_ivec2;
typedef sp_ivec3_16 spv_ivec3;
typedef sp_ivec4_16 spv_ivec4;

typedef sp_uivec2_16 spv_uivec2;
typedef sp_uivec3_16 spv_uivec3;
typedef sp_uivec4_16 spv_uivec4;

typedef sp_vec2_h spv_vec2;
typedef sp_vec3_h spv_vec3;
typedef sp_vec4_h spv_vec4;

typedef sp_imat2_16 spi_imat2;
typedef sp_imat3_16 spi_imat3;
typedef sp_imat4_16 spi_imat4;
typedef sp_imatn_16 spi_imatn;

typedef sp_uimat2_16 spi_uimat2;
typedef sp_uimat3_16 spi_uimat3;
typedef sp_uimat4_16 spi_uimat4;
typedef sp_uimatn_16 spi_uimatn;

typedef sp_dmat2_h  spm_dmat2;
typedef sp_dmat3_h  spm_dmat3;
typedef sp_dmat4_h  spm_dmat4;
typedef sp_dmatn_h  spm_dmatn;

#else 

#ifndef S_MEDIUM_PRECISION
#define S_MEDIUM_PRECISION
#endif


typedef sp_int_32    spi_int;
typedef sp_uint_32   spi_uint;
typedef sp_decimal_f spd_decimal;

typedef sp_ivec2_32 spv_ivec2;
typedef sp_ivec3_32 spv_ivec3;
typedef sp_ivec4_32 spv_ivec4;

typedef sp_uivec2_32 spv_uivec2;
typedef sp_uivec3_32 spv_uivec3;
typedef sp_uivec4_32 spv_uivec4;

typedef sp_vec2_f spv_vec2;
typedef sp_vec3_f spv_vec3;
typedef sp_vec4_f spv_vec4;

typedef sp_imat2_32 spi_imat2;
typedef sp_imat3_32 spi_imat3;
typedef sp_imat4_32 spi_imat4;
typedef sp_imatn_32 spi_imatn;

typedef sp_uimat2_32 spi_uimat2;
typedef sp_uimat3_32 spi_uimat3;
typedef sp_uimat4_32 spi_uimat4;
typedef sp_uimatn_32 spi_uimatn;

typedef sp_dmat2_f  spm_dmat2;
typedef sp_dmat3_f  spm_dmat3;
typedef sp_dmat4_f  spm_dmat4;
typedef sp_dmatn_f  spm_dmatn;

#endif

enum class Precision : sp_uint_8
{
	MEDIUM = 0,
	HIGH   = 1,
	LOW    = 2
};


struct SPVariant
{
	enum class VarientType : sp_uint_8
	{
		NONE    = 0,
		BOOL    = 1,
		INT     = 2,
		UINT    = 3,
		DECIMAL = 4,
		VEC2    = 5,
		VEC3    = 6,
		VEC4    = 7,
		IVEC2   = 8,
		IVEC3   = 9,
		IVEC4   = 10,
		UIVEC2  = 11,
		UIVEC3  = 12,
		UIVEC4  = 13,
		STR     = 14,
		ID      = 15
	};

	VarientType type;
	sp_bool used;

	union
	{
		spd_decimal  d;
		spi_int	     i;
		spi_uint	 ui;
		spv_vec2     vec2;
		spv_vec3     vec3;
		spv_vec4     vec4;
		spv_ivec2    ivec2;
		spv_ivec3    ivec3;
		spv_ivec4    ivec4;
		spv_uivec2   uivec2;
		spv_uivec3   uivec3;
		spv_uivec4   uivec4;
		sp_uuid		 id;
	};
	sp_string str;
	

	SPVariant()
		: type(VarientType::NONE)
		, vec4({ 0.0, 0.0, 0.0, 0.0 })
		, str("")
		, used(false)
	{

	}
	~SPVariant() = default;
	SPVariant(const sp_uuid& v)
		: type(VarientType::ID)
		, id(v)
		, str("")
		, used(true)
	{

	}
	SPVariant(const spi_int& v)
		: type(VarientType::INT)
		, i(v)
		, str("")
		, used(true)
	{

	}
	SPVariant(const spi_uint& v)
		: type(VarientType::UINT)
		, ui(v)
		, str("")
		, used(true)
	{

	}
	SPVariant(const spd_decimal& v)
		: type(VarientType::DECIMAL)
		, d(v)
		, str("")
		, used(true)
	{

	}
	SPVariant(const sp_bool& v)
		: type(VarientType::BOOL)
		, i(v)
		, str("")
		, used(true)
	{

	}
	SPVariant(const char* v)
		: type(VarientType::STR)
		, vec4({ 0.0, 0.0, 0.0, 0.0 })
		, str(v)
		, used(true)
	{

	}
	SPVariant(const std::string& v)
		: type(VarientType::STR)
		, vec4({ 0.0, 0.0, 0.0, 0.0 })
		, str(v)
		, used(true)
	{

	}

	SPVariant(const spd_decimal& x, const spd_decimal& y)
		: type(VarientType::VEC2)
		, vec2({x, y})
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_vec2& v)
		: type(VarientType::VEC2)
		, vec2(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const spi_int& x, const spi_int& y)
		: type(VarientType::IVEC2)
		, ivec2({x, y })
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_ivec2& v)
		: type(VarientType::IVEC2)
		, ivec2(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const spi_uint& x, const spi_uint& y)
		: type(VarientType::UIVEC2)
		, uivec2({x, y})
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_uivec2& v)
		: type(VarientType::UIVEC2)
		, uivec2(v)
		, str("")
		, used(true)
	{

	}


	SPVariant(const spd_decimal& x, const spd_decimal& y, const spd_decimal& z)
		: type(VarientType::VEC3)
		, vec3({x, y, z})
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_vec3& v)
		: type(VarientType::VEC3)
		, vec3(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const spi_int& x, const spi_int& y, const spi_int& z)
		: type(VarientType::IVEC3)
		, ivec3({ x, y, z })
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_ivec3& v)
		: type(VarientType::IVEC3)
		, ivec3(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const spi_uint& x, const spi_uint& y, const spi_uint& z)
		: type(VarientType::UIVEC3)
		, uivec3({ x, y, z })
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_uivec3& v)
		: type(VarientType::UIVEC3)
		, uivec3(v)
		, str("")
		, used(true)
	{

	}


	SPVariant(const spd_decimal& x, const spd_decimal& y, const spd_decimal& z, const spd_decimal& w)
		: type(VarientType::VEC4)
		, vec4({ x, y, z, w })
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_vec4& v)
		: type(VarientType::VEC4)
		, vec4(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const spi_int& x, const spi_int& y, const spi_int& z, const spi_int& w)
		: type(VarientType::IVEC4)
		, ivec4({ x, y, z, w })
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_ivec4& v)
		: type(VarientType::IVEC4)
		, ivec4(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const spi_uint& x, const spi_uint& y, const spi_uint& z, const spi_uint& w)
		: type(VarientType::UIVEC4)
		, uivec4({x, y, z, w })
		, str("")
		, used(true)
	{

	}
	SPVariant(const spv_uivec4& v)
		: type(VarientType::UIVEC4)
		, uivec4(v)
		, str("")
		, used(true)
	{

	}

	SPVariant(const SPVariant& v) 
	{
		this->type = v.type;
		this->str = v.str;
		this->used = v.used;
		switch (v.type)
		{
		case VarientType::BOOL:
		case VarientType::INT:     this->i = v.i;           break;
		case VarientType::UINT:    this->ui = v.ui;         break;
		case VarientType::DECIMAL: this->d = v.d;           break;
		case VarientType::VEC2:    this->vec2 = v.vec2;     break;
		case VarientType::VEC3:    this->vec3 = v.vec3;     break;
		case VarientType::VEC4:    this->vec4 = v.vec4;     break;
		case VarientType::IVEC2:   this->ivec2 = v.ivec2;   break;
		case VarientType::UIVEC2:  this->uivec2 = v.uivec2; break;
		case VarientType::IVEC3:   this->ivec3 = v.ivec3;   break;
		case VarientType::UIVEC3:  this->uivec3 = v.uivec3; break;
		case VarientType::IVEC4:   this->ivec4 = v.ivec4;   break;
		case VarientType::UIVEC4:  this->uivec4 = v.uivec4; break;
		case VarientType::ID:      this->id = v.id;         break;
		default:
			break;
		}
	}
	SPVariant& operator=(const SPVariant& v) 
	{
		this->type = v.type;
		this->str = v.str;
		this->used = v.used;
		switch (v.type)
		{
		case VarientType::BOOL:
		case VarientType::INT:     this->i = v.i;           break;
		case VarientType::UINT:    this->ui = v.ui;         break;
		case VarientType::DECIMAL: this->d = v.d;           break;
		case VarientType::VEC2:    this->vec2 = v.vec2;     break;
		case VarientType::VEC3:    this->vec3 = v.vec3;     break;
		case VarientType::VEC4:    this->vec4 = v.vec4;     break;
		case VarientType::IVEC2:   this->ivec2 = v.ivec2;   break;
		case VarientType::UIVEC2:  this->uivec2 = v.uivec2; break;
		case VarientType::IVEC3:   this->ivec3 = v.ivec3;   break;
		case VarientType::UIVEC3:  this->uivec3 = v.uivec3; break;
		case VarientType::IVEC4:   this->ivec4 = v.ivec4;   break;
		case VarientType::UIVEC4:  this->uivec4 = v.uivec4; break;
		case VarientType::ID:      this->id = v.id;         break;
		default:
			break;
		}
		return *this;
	
	}
	SPVariant(SPVariant&& v) noexcept
	{
		this->type = v.type;
		this->str = v.str;
		this->used = v.used;
		switch (v.type)
		{
		case VarientType::BOOL:
		case VarientType::INT:     this->i = v.i;           break;
		case VarientType::UINT:    this->ui = v.ui;         break;
		case VarientType::DECIMAL: this->d = v.d;           break;
		case VarientType::VEC2:    this->vec2 = v.vec2;     break;
		case VarientType::VEC3:    this->vec3 = v.vec3;     break;
		case VarientType::VEC4:    this->vec4 = v.vec4;     break;
		case VarientType::IVEC2:   this->ivec2 = v.ivec2;   break;
		case VarientType::UIVEC2:  this->uivec2 = v.uivec2; break;
		case VarientType::IVEC3:   this->ivec3 = v.ivec3;   break;
		case VarientType::UIVEC3:  this->uivec3 = v.uivec3; break;
		case VarientType::IVEC4:   this->ivec4 = v.ivec4;   break;
		case VarientType::UIVEC4:  this->uivec4 = v.uivec4; break;
		case VarientType::ID:      this->id = v.id;         break;
		default:
			break;
		}
	}
	SPVariant& operator=(SPVariant&& v)noexcept
	{
		this->type = v.type;
		this->str = v.str;
		this->used = v.used;
		switch (v.type)
		{
		case VarientType::BOOL:
		case VarientType::INT:     this->i = v.i;           break;
		case VarientType::UINT:    this->ui = v.ui;         break;
		case VarientType::DECIMAL: this->d = v.d;           break;
		case VarientType::VEC2:    this->vec2 = v.vec2;     break;
		case VarientType::VEC3:    this->vec3 = v.vec3;     break;
		case VarientType::VEC4:    this->vec4 = v.vec4;     break;
		case VarientType::IVEC2:   this->ivec2 = v.ivec2;   break;
		case VarientType::UIVEC2:  this->uivec2 = v.uivec2; break;
		case VarientType::IVEC3:   this->ivec3 = v.ivec3;   break;
		case VarientType::UIVEC3:  this->uivec3 = v.uivec3; break;
		case VarientType::IVEC4:   this->ivec4 = v.ivec4;   break;
		case VarientType::UIVEC4:  this->uivec4 = v.uivec4; break;
		case VarientType::ID:      this->id = v.id;         break;
		default:
			break;
		}
		return *this;
	}

};
using SPVariantDataMap = std::unordered_map<sp_string, SPVariant>;
typedef std::basic_stringstream<char, std::char_traits<char>, std::allocator<char> > SPStringStream;

#pragma warning(pop) 

#endif  
