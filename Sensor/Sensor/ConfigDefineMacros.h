#pragma once
#define DEFINE_PROPERTY_BASE(type,name,GET,REF,prefix) \
	public:  \
		type& REF##name(void) { return prefix##name; } \
		type GET##name(void) const { return prefix##name; } \
	private: \
		type prefix##name

#define DEFINE_PROPERTY(type,name,prefix) DEFINE_PROPERTY_BASE(type,name,Get,Ref,prefix)

#define DEFINE_PROPERTY_POINTER(type,name) DEFINE_PROPERTY(type,name,m_p)
#define DEFINE_PROPERTY_ULONG(name) DEFINE_PROPERTY(unsigned long,name,m_u)
#define DEFINE_PROPERTY_USHORT(name) DEFINE_PROPERTY(unsigned short,name,m_u)
#define DEFINE_PROPERTY_DWORD(name) DEFINE_PROPERTY(DWORD,name,m_dw)
#define DEFINE_PROPERTY_BOOLEAN(name) DEFINE_PROPERTY_BASE(bool,name,Is,Ref,m_b)
#define DEFINE_PROPERTY_TIME(name)	DEFINE_PROPERTY(time_t,name,m_t)
#define DEFINE_PROPERTY_STRING(name) DEFINE_PROPERTY(std::wstring,name,m_str)
#define DEFINE_PROPERTY_ASTRING(name) DEFINE_PROPERTY(std::string,name,m_str)
#define DEFINE_PROPERTY_TSTRING(name) DEFINE_PROPERTY(std::tstring,name,m_str)

#define ForEach(name,m_vec) template<typename F>\
	void ForEach##name(F& f)const \
	{\
	if(m_vec) for_each(m_vec->begin(),m_vec->end(),f);return;\
	}

#define ForEachRef(name,m_vec) template<typename F>\
	void ForEachRef##name(F& f)const \
	{\
	for_each(m_vec.begin(),m_vec.end(),f);return;\
	}



#define Operator2UnaryFunction(OP,FUNC_NAME,PARAM_TYPE,RETURN_TYPE,RETURN) \
struct FUNC_NAME:public std::unary_function<PARAM_TYPE,RETURN_TYPE> \
{ \
	result_type operator()(argument_type param) \
	{ \
	RETURN OP param; \
	} \
}

namespace std
{
	// C++11 already has one
	//template<typename InputIterator,typename OutputIterator,typename Predicate>
	//OutputIterator copy_if(InputIterator begin,InputIterator end,OutputIterator destBegin,Predicate p) 
	//{
	//	while (begin != end) 
	//	{
	//		if (p(*begin))*destBegin++ = *begin;
	//		++begin;
	//	}
	//	return destBegin;
	//}

	template<typename InputIterator,typename OutputIterator,typename Predicate>
	OutputIterator copy_if_rtti(InputIterator begin,InputIterator end,OutputIterator destBegin,Predicate p) 
	{
		while (begin != end) 
		{
			typedef OutputIterator::container_type::value_type VALUE_TYPE;
			if(VALUE_TYPE _value = dynamic_cast<VALUE_TYPE>(*begin))
			{
				if (p(_value))*destBegin++ = _value;
			}
			++begin;
		}
		return destBegin;
	}

	//T_pattern could be T_Container::value_type, or user defined overload operator==(T_Container::value_type,T_pattern)
	template<typename T_Container,typename T_pattern>
	bool IsIn(const T_Container* pContainer,const T_pattern val){return find(pContainer->begin(),pContainer->end(),val) != pContainer->end();}
	
	template<typename TYPE>
	inline bool PositivePredication(TYPE val){return true;}
}