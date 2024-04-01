#include "engine/pch.hpp"

#include "C_Tags.hpp"

C_Tags::C_Tags(GameObject* _owner, ETag _initial_tags)
	: IComponent(_owner)
	, m_Tags(_initial_tags)
{

}

void C_Tags::AddTag(ETag _tag)
{
	m_Tags = m_Tags | _tag;
}

void C_Tags::RemoveTag(ETag _tag)
{
	m_Tags = m_Tags & ~_tag;
}

bool C_Tags::HasTag(ETag _tag) const
{
	return (m_Tags & _tag) == _tag;
}

#define ENUM_FLAG_OPERATOR(_op, _lhs, _rhs) \
inline ETag operator##_op(ETag _lhs, ETag _rhs) \
{ \
	return static_cast<ETag>(static_cast<UINT32>(_lhs) _op static_cast<UINT32>(_rhs)); \
}

ENUM_FLAG_OPERATOR(| , _lhs, _rhs);
ENUM_FLAG_OPERATOR(&, _lhs, _rhs);
ENUM_FLAG_OPERATOR(^, _lhs, _rhs);

ETag operator~(ETag _tag)
{
	return static_cast<ETag>(~static_cast<UINT32>(_tag));
}
