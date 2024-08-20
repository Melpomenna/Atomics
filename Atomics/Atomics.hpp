#pragma once
#include <pch.h>

#define ATOMICS_EXPORT __declspec(dllexport)
#define ATOMICS_WIN_API __stdcall

namespace atomics
{

	template<class T>
	class Atomic final
	{
	public:
		using value_type = T;
	public:
		ATOMICS_EXPORT Atomic();
		ATOMICS_EXPORT explicit Atomic(int value);
		ATOMICS_EXPORT ~Atomic() = default;

		ATOMICS_EXPORT Atomic(const Atomic&);
		ATOMICS_EXPORT Atomic& ATOMICS_WIN_API operator=(const Atomic& other);

		ATOMICS_EXPORT Atomic(Atomic&&) noexcept;
		ATOMICS_EXPORT Atomic& ATOMICS_WIN_API operator=(Atomic&&) noexcept;

		ATOMICS_EXPORT void ATOMICS_WIN_API add(value_type) noexcept;
		ATOMICS_EXPORT void ATOMICS_WIN_API sub(value_type) noexcept;
		ATOMICS_EXPORT void ATOMICS_WIN_API store(value_type value) noexcept;

		ATOMICS_EXPORT value_type ATOMICS_WIN_API load() const noexcept;
		ATOMICS_EXPORT value_type ATOMICS_WIN_API operator*() const noexcept { return  load(); }
	private:
		ATOMICS_EXPORT value_type* ATOMICS_WIN_API ValueFieldAddr() const noexcept
		{
			return const_cast<value_type*>(&value_);
		}
	private:
		value_type value_;
	};

	template<class T>
	Atomic<T>::Atomic() : value_(value_type())
	{
		static_assert(sizeof(value_type) < 8);
	}

	template<class T>
	Atomic<T>::Atomic(int value) : Atomic()
	{
		store(value);
	}

	template<class T>
	Atomic<T>::Atomic(const Atomic& other)
	{
		store(other.load());
	}

	template<class T>
	Atomic<T>& ATOMICS_WIN_API Atomic<T>::operator=(const Atomic& other)
	{
		if (this == &other) return *this;
		store(other.load());
		return *this;
	}

	template<class T>
	Atomic<T>::Atomic(Atomic&& other) noexcept
	{
		store(other.load());
		other.store(value_type());
	}

	template<class T>
	Atomic<T>& ATOMICS_WIN_API Atomic<T>::operator=(Atomic&& other) noexcept
	{
		if (this == &other) return *this;

		store(other.load());
		other.store(value_type());

		return *this;
	}

	template<class T>
	void  ATOMICS_WIN_API Atomic<T>::add(value_type value) noexcept
	{
		if  constexpr (sizeof(value_type) == 4)
		{
			value_type* addr = ValueFieldAddr();
			__asm {
				mov ebx, addr
				mov eax, value
				lock add[ebx], eax
			}
		}
	}

	template<class T>
	void  ATOMICS_WIN_API Atomic<T>::sub(value_type value) noexcept
	{
		if  constexpr (sizeof(value_type) == 4)
		{
			value_type* addr = ValueFieldAddr();
			__asm {
				mov ebx, addr
				mov eax, value
				lock sub[ebx], eax
			}
		}
	}

	template<class T>
	void ATOMICS_WIN_API Atomic<T>::store(value_type value) noexcept
	{
		value_type* addr = ValueFieldAddr();
		if constexpr (sizeof(value_type) == 4)
		{
			__asm {
				mov ebx, addr
				mov eax, value
				mov[ebx], 0
				lock add[ebx], eax
			}
		}
		else if constexpr (sizeof(value_type) == 1) {
			__asm {
				mov ebx, addr
				mov ah, value
				mov[ebx], 0
				lock add[ebx], ah
			}
		}
	}

	template<class T>
	typename Atomic<T>::value_type ATOMICS_WIN_API Atomic<T>::load() const noexcept
	{
		volatile value_type value = *ValueFieldAddr();
		if  constexpr (sizeof(value_type) == 4)
		{
			__asm {
				mov edi, value
				xchg eax, edi
			}
		}
		else if constexpr (sizeof(value_type) == 1)
		{
			__asm {
				mov ch, value
				xchg ah, ch
			}
		}
	}


}

using AtomicInt = atomics::Atomic<int>;
using AtomicBool = atomics::Atomic<bool>;
