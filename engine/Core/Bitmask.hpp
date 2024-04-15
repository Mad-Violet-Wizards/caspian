#pragma once

#include <stdint.h>
#include <type_traits>

template<typename T>
concept BitmaskAcceptableType = std::is_unsigned<T>::value &&
																(std::is_same_v<T, uint8_t> ||
																 std::is_same_v<T, uint16_t> ||
																 std::is_same_v<T, uint32_t> ||
																 std::is_same_v<T, uint64_t>);

////////////////////////////////////////////////////////////
template<BitmaskAcceptableType T>
class Bitmask
{
public:

	Bitmask();
	Bitmask(T mask);
	// Copy constructor & copy assignment operator?
	void CopyMask(const Bitmask<T>& other);

	T GetMask() const;

	bool GetBit(size_t pos) const;
	void SetBit(size_t pos, bool on);

	void SetBit(size_t pos);
	void ClearBit(size_t pos);
	void Clear();

private:

	T m_Mask;
};
////////////////////////////////////////////////////////////

template<BitmaskAcceptableType T>
Bitmask<T>::Bitmask()
	: m_Mask(0)
{
}

template<BitmaskAcceptableType T>
Bitmask<T>::Bitmask(T mask)
	: m_Mask(mask)
{

}

template<BitmaskAcceptableType T>
void Bitmask<T>::CopyMask(const Bitmask<T>& other)
{
	m_Mask = other.GetMask();
}

template<BitmaskAcceptableType T>
T Bitmask<T>::GetMask() const
{
	return m_Mask;
}

template<BitmaskAcceptableType T>
bool Bitmask<T>::GetBit(size_t pos) const
{
	return (m_Mask & (1 << pos)) != 0;
}

template<BitmaskAcceptableType T>
void Bitmask<T>::SetBit(size_t pos, bool on)
{
	on ? SetBit(pos) : ClearBit(pos);
}

template<BitmaskAcceptableType T>
void Bitmask<T>::SetBit(size_t pos)
{
	m_Mask = m_Mask | (1 << pos);
}

template<BitmaskAcceptableType T>
void Bitmask<T>::ClearBit(size_t pos)
{
	m_Mask = m_Mask & ~(1 << pos);
}

template<BitmaskAcceptableType T>
void Bitmask<T>::Clear()
{
	m_Mask = 0;
}
