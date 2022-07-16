#ifndef S_SPMACRO_H
#define S_SPMACRO_H


#ifdef SP_EXPORT_PLUGIN
#      define PLUGINEXPORT   __declspec(dllexport)
#    else
#      define PLUGINEXPORT   __declspec(dllimport)
#    endif

#ifdef EXPORT_CORE
#      define COREEXPORT   __declspec(dllexport)
#    else
#      define COREEXPORT   __declspec(dllimport)
#    endif

#ifdef EXPORT_FILE
#      define FILEEXPORT   __declspec(dllexport)
#    else
#      define FILEEXPORT   __declspec(dllimport)
#    endif

#ifdef EXPORT_RENDER
#      define RENDEREXPORT   __declspec(dllexport)
#    else
#      define RENDEREXPORT   __declspec(dllimport)
#    endif


#if SP_COMPILER == SP_COMPILER_MSVC
#	define SP_NORETURN __declspec(noreturn)
#elif SP_COMPILER == SP_COMPILER_GNUC || SP_COMPILER == SP_COMPILER_CLANG
#	define SP_NORETURN __attribute__((noreturn))
#else
#	define SP_NORETURN
#endif

#define SPAssert(a, b) if (!(a)){}

//
//#if SP_ASSERT_MODE == 1
//#   if SP_DEBUG_MODE
//#       define SPAssert( a, b ) assert( (a) && (b) )
//#   else
//#       define SPAssert( a, b ) if( !(a) ) SP_EXCEPT_2( SPException::ExceptionCodes::ERR_RT_ASSERTION_FAILED, (b) )
//#   endif
//
//#elif SP_ASSERT_MODE == 2
//#   define SPAssert( a, b ) if( !(a) ) SP_EXCEPT_2( SPException::ExceptionCodes::ERR_RT_ASSERTION_FAILED, (b) )
//#else
//#   define SPAssert( expr, mesg ) assert( (expr) && (mesg) )
//#endif
//
//#if SP_DEBUG_MODE
//#   define SPAssertDbg( a, b ) SPAssert( a, b )
//#else
//#   define SPAssertDbg( expr, mesg )
//#endif


#ifndef SP_EXCEPT
#define SP_EXCEPT_3(code, desc, src) SPExceptionFactory::Throw_Exception(code, desc, src, __FILE__, __LINE__)
#define SP_EXCEPT_2(code, desc)      SPExceptionFactory::Throw_Exception(code, desc, __FUNCTION__, __FILE__, __LINE__)
#define SP_EXCEPT_CHOOSER(arg1, arg2, arg3, arg4, ...) arg4
#define SP_EXPAND(x) x // MSVC workaround
#define SP_EXCEPT(...) SP_EXPAND(SP_EXCEPT_CHOOSER(__VA_ARGS__, SP_EXCEPT_3, SP_EXCEPT_2)(__VA_ARGS__))
#endif



#define SP_CLASS_DELETE_COPY(Class) \
	Class(const Class& v) = delete; \
	Class& operator=(const Class& v) = delete;

#define SP_CLASS_DELETE_MOVE(Class) \
	Class(Class&& v) = delete; \
	Class& operator=(Class&& v) = delete;

#define SP_CLASS_MEMBER(Class, Type, Member) \
public: \
	Type& get_##Member()\
	{\
		return Member##_;\
	}\
	void set_##Member(const Type& Member) \
	{\
		this->Member##_ = Member;\
	}\
protected:\
	Type Member##_;\

#define SP_CLASS_MEMBER_DEF(Class, Type, Member) \
public: \
	Type& get_##Member();\
	void set_##Member(const Type& Member);\
protected:\
	Type Member##_;

#define SP_CLASS_MEMBER_IMP_SET(Class, Type, Member) \
	void Class::set_##Member(const Type& Member)

#define SP_DELETE_ARRAY_PTR(arrayPtr) \
	if(arrayPtr) \
	{\
		delete[] arrayPtr;\
		arrayPtr = nullptr;\
	}

#define SP_DELETE_OBJECT_PTR(objectPtr) \
	if(objectPtr) \
	{\
		delete objectPtr;\
		objectPtr = nullptr;\
	}

#define SP_CREATE_OBJECT_PTR(objectPtr, Class, ...) \
	if(objectPtr == nullptr) \
	{\
		objectPtr = new  Class(##__VA_ARGS__);\
		objectPtr->init();\
	}


#define BACKSLASH "/"

#define SLASH "\\" 


#define SP_SINGLETON_PUBLIC_DEF(Class) \
protected:\
	Class();\
	~Class() = default;\
public:\
	static Class* Get(void);\
	void init();\
	void destroy();\


#define SP_SINGLETON_IMPL(Class) \
	template<> Class* SPSingleton<Class>::Instance_ = 0;\
	Class* Class::Get(void){ if(!Instance_) Instance_ = new Class(); return Instance_;}

#endif
