#pragma once

//////////////////////////////////////////////////
enum class ETag : UINT32
{
	None = 0,
	CameraAttachable
};

inline ETag operator|(ETag _lhs, ETag _rhs);
inline ETag operator&(ETag _lhs, ETag _rhs);
inline ETag operator^(ETag _lhs, ETag _rhs);
inline ETag operator~(ETag _tag);

//////////////////////////////////////////////////
class C_Tags
{
	public:


		explicit C_Tags(ETag _initial_tags = ETag::None);
		~C_Tags() = default;


		void AddTag(ETag _tag);
		void RemoveTag(ETag _tag);
		bool HasTag(ETag _tag) const;

	private:

		ETag m_Tags;
};