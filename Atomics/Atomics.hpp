#pragma once
#include <pch.h>

namespace atomics
{

	template<class T>
	class Atomic final
	{
	public:
		using value_type = T;
	public:
		__declspec(dllexport) Atomic();
		__declspec(dllexport) explicit Atomic(int value);
		__declspec(dllexport) ~Atomic() = default;

		__declspec(dllexport) Atomic(const Atomic&);
		__declspec(dllexport) Atomic& operator=(const Atomic& other);

		__declspec(dllexport) Atomic(Atomic&&) noexcept;
		__declspec(dllexport) Atomic& operator=(Atomic&&) noexcept;

		__declspec(dllexport) void add(value_type) noexcept;
		__declspec(dllexport) void sub(value_type) noexcept;
		__declspec(dllexport) void store(value_type value) noexcept;

		__declspec(dllexport) value_type load() const noexcept;
		__declspec(dllexport) value_type operator*() const noexcept { return  load(); }
	private:
		__declspec(dllexport) value_type* ValueFieldAddr() const noexcept
		{
			return const_cast<value_type*>(&value_);
		}
	private:
		value_type value_;
	};

	template<class T>
	Atomic<T>::Atomic() : value_(value_type()) {}

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
	Atomic<T>&  Atomic<T>::operator=(const Atomic& other)
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
	Atomic<T>&  Atomic<T>::operator=(Atomic&& other) noexcept
	{
		if (this == &other) return *this;

		store(other.load());
		other.store(value_type());

		return *this;
	}

	template<class T>
	void  Atomic<T>::add(value_type value) noexcept
	{
		if  constexpr (sizeof(value_type) >= 4)
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
	void  Atomic<T>::sub(value_type value) noexcept
	{
		if  constexpr (sizeof(value_type) >= 4)
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
	void  Atomic<T>::store(value_type value) noexcept
	{
		value_type* addr = ValueFieldAddr();
		if constexpr (sizeof(value_type) >= 4)
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
	typename Atomic<T>::value_type  Atomic<T>::load() const noexcept
	{
		volatile value_type value = *ValueFieldAddr();
		if  constexpr (sizeof(value_type) >= 4)
		{
			__asm {
				mov edi, value
				xchg eax, edi
			}
		}
		else if constexpr (sizeof(value_type) == 1)
		{
			__asm {
				mov edi, value
				xchg ah, edi
			}
		}
	}


}

using AtomicInt = atomics::Atomic<int>;
using AtomicBool = atomics::Atomic<bool>;
using AtomicSize_t = atomics::Atomic<unsigned long>;
