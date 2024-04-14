#pragma once

//////////////////////////////////////////////////
enum class ETag
{
	None = 0,
	Camera_Attachable = 1 << 0,
	Physics_Static = 1 << 1,
	Physics_Dynamic = 1 << 2,
	Drawable = 1 << 3,
		Drawable_Background = 1 << 4,
		Drawable_Entity = 1 << 5,
		Drawable_Foreground = 1 << 6
};

inline ETag operator|(ETag _lhs, ETag _rhs);
inline ETag operator&(ETag _lhs, ETag _rhs);
inline ETag operator^(ETag _lhs, ETag _rhs);
inline ETag operator~(ETag _tag);

//////////////////////////////////////////////////
class C_Tags : public IComponent
{
	public:

		explicit C_Tags(GameObject* _owner, ETag _initial_tags = ETag::None);
		~C_Tags() = default;


		void AddTag(ETag _tag);
		void RemoveTag(ETag _tag);
		bool HasTag(ETag _tag) const;

		void Update([[maybe_unused]] float _dt) override {};
		void LateUpdate([[maybe_unused]] float _dt) override {};

	private:

		ETag m_Tags;
};