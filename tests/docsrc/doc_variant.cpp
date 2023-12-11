// metapp library
// 
// Copyright (C) 2022 Wang Qi (wqking)
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "testutil.h"

#include "metapp/allmetatypes.h"

/*desc
# Variant reference

## Overview

`metapp::Variant` is a light weight class that allows to store data of any type and convert between types.  
Variant holds a single value of any type at the same time.
The type can be any C++ type, such as int, class, std::string, std::vector, function pointer, etc, any type.  

For example,  

```c++
metapp::Variant v1 = 5;
metapp::Variant v2 = std::vector<std::string>();
metapp::Variant v3 = MyClass();
```

In above code, the type held by v1 is metapp::tkInt, v2 is metapp::tkVector,
v3 is metapp::tkObject unless MyClass is registered with another type kind.

## Header
desc*/

//code
#include "metapp/variant.h"
//code

/*desc
## Constructors and assignment operators

#### Default constructor

```c++
Variant() noexcept;
```

Construct an empty Variant of type `tkVoid`.

#### Construct from a value

```c++
template <typename T>
Variant(T && value);
```

Construct a Variant of type T, copy value into Variant if value is l-value, or move value into Variant if it's r-value.  
Note T is always treated as value type, even if it's reference. That's to say, we can't construct Variant of reference
using this constructor, we need to use `Variant::reference` to create reference.  
If `value` is not copyable nor movable, exception `metapp::NotConstructibleException` is raised.  

#### Construct from a type and a value

```c++
Variant(const MetaType * metaType, const void * copyFrom); // #1
Variant(const MetaType * metaType, const void * copyFrom, const CopyStrategy copyStrategy); // #2
```

Construct a Variant of type `metaType`, and initialize with the object pointed by `copyFrom`.  
If `copyFrom` is nullptr, the object in the Variant is default constructed.  
If `copyFrom` is not nullptr, the object in the Variant is copied from the object pointed by `copyFrom`. In such case,
`copyFrom` must point to an object of the exact same type of `metaType`. The constructor doesn't and can't validate `copyFrom`.

For #1 form,  
If `metaType` is copyable, `copyFrom` will be copied into Variant.  
If `metaType` is not copyable, `copyFrom` will be moved into Variant.  
If `metaType` is not copyable nor movable, exception `metapp::NotConstructibleException` is raised.  

For #2 form, how `copyFrom` is copied is determined by `CopyStrategy`.  

```c++
enum class CopyStrategy
{
	autoDetect,
	copy,
	move
};
```

`CopyStrategy::autoDetect`: `copyFrom` is copied in the same way as #1 form.  
`CopyStrategy::copy`: `copyFrom` is copied, if `metaType` is not copyable, exception `metapp::NotConstructibleException` is raised.  
`CopyStrategy::move`: `copyFrom` is moved, if `metaType` is not movable, exception `metapp::NotConstructibleException` is raised.  

Note: `Variant(metaType)` will create a Variant that holds a pointer to a MetaType (tkMetaType), it's different with
`Variant(metaType, nullptr)`.

#### Copy and move constructors

```c++
Variant(const Variant & other) noexcept;
Variant(Variant && other) noexcept;
```

Copy and move constructors.

#### Assign from value
```c++
template <typename T>
Variant & operator = (T && value) noexcept;
```

Assign to the Variant with `value`.  
The previous value held by the variant is destroyed after assigned with the new value.  
`value` will be copied or moved to `this` Variant, depending on whether it's a l-value or r-value.  

#### Copy and move assignment
```c++
Variant & operator = (const Variant & other) noexcept;
Variant & operator = (Variant && other) noexcept;
```
Copy and move assignment.  
The previous value held by the variant is destroyed after assigned with the new variant.  
Example code,  
desc*/

ExampleFunc
{
	//code
	metapp::Variant t(5);
	ASSERT(t.getMetaType()->equal(metapp::getMetaType<int>())); // t is int
	ASSERT(t.get<int>() == 5);
	metapp::Variant u(38.2);
	ASSERT(u.getMetaType()->equal(metapp::getMetaType<double>())); // u is double

	t = u;
	ASSERT(t.getMetaType()->equal(metapp::getMetaType<double>())); // t is double
	ASSERT(t.get<double>() == 38.2);
	//code
}

/*desc
#### create
```c++
template <typename T>
static Variant create(const typename std::remove_reference<T>::type & value); // #1
template <typename T>
static Variant create(typename std::remove_reference<T>::type && value); // #2
```
Construct a Variant of type T and copy value into Variant, then return the Variant.  
If T is value type (not reference), #1 form will copy the value into the Variant, #2 form will move the value into the Variant.  
This is "named constructor" that's equivalent to `template <typename T> Variant(T && value)`,
the `create` function allows to specify T explicitly.  
If T is metapp::Variant or reference to metapp::Variant, value is returned directly.  
T must be specified explicitly, type deduction is disabled on purpose.  

**Example**
desc*/

ExampleFunc
{
	//code
	// This doesn't compile, we must specify T explicitly.
	//metapp::Variant v = metapp::Variant::create(5);

	int n = 5;

	// The type held by v1 is tkReference
	metapp::Variant v1 = metapp::Variant::create<int &>(n);
	ASSERT(v1.getMetaType()->isReference());

	// The type held by v2 is const int
	metapp::Variant v2 = metapp::Variant::create<const int>(n);
	ASSERT(v2.getMetaType()->isConst());

	// On contrary, if we don't use create, the meta type is different.
	// The type held by v3 is tkInt, not tkReference
	metapp::Variant v3(n);
	ASSERT(! v3.getMetaType()->isReference());

	// The type held by v4 is int, not const
	metapp::Variant v4 = n;
	ASSERT(! v4.getMetaType()->isConst());
	//code
}

/*desc
#### reference
```c++
template <typename T>
static Variant reference(T && value);
```

Create a Variant of reference to `value`. `value` can be either lvalue or rvalue reference.  
This is similar to `Variant::create<T &>(value)`, but in `reference` the template argument can be deduced,
and if `T` is `Variant`, it creates reference to `Variant`.  

**Example**
desc*/

ExampleFunc
{
	//code
	int n = 5;
	// The type held by v is tkReference
	metapp::Variant v = metapp::Variant::reference(n);
	//code
}

/*desc
#### retype
```c++
static Variant retype(const MetaType * metaType, const Variant & var);
```

Return a Variant which data is the data in `var`, type is `metaType`.  
This is only useful if you are 100% sure the data in `var` can be in type `metaType`,
otherwise, you should cast the Variant instead of retyping it.  

#### takeFrom
```c++
static Variant takeFrom(const MetaType * metaType, void * instance);
```

Return a Variant which data is the object pointed by `instance`, type is `metaType`.  
`instance` is a pointer that points to an object allocated on the heap,
the constructed Variant will take and manage the ownership of `instance`, so `instance` should not be freed any elsewhere.  
`metaType` should be the type that `instance` points to, not the pointer type.
The returned `Variant` is a value object, not a pointer.

**Example**  
desc*/

ExampleFunc
{
	//code
	std::string * instance = new std::string("Hello");
	metapp::Variant v = metapp::Variant::takeFrom(metapp::getMetaType<std::string>(), instance);
	// v is a value, so we should get as value or reference, but not pointer
	ASSERT(v.get<const std::string &>() == "Hello");
	// Now v will free instance when v is destroyed
	//code
}

/*desc
#### takeFrom another Variant
```c++
static Variant takeFrom(const Variant & var);
```

Return a Variant which data is the pointer in `var`, type is the type in `var`.  
`var` must hold a pointer. It must not hold the object direct.  
This function is not useful in most cases. The only useful case is to use it on the return value when invoking a meta constructor.

**Example**  
desc*/

ExampleFunc
{
	//code
	// This is wrong. In this case, var hold the ownership of std::string
	// takeFrom can't take the ownership from var
	//metapp::Variant var(std::string());

	// This works, var hold the pointer, it doesn't hold the ownership of std::string
	// Note: if we don't call takeFrom later, var will leak the object
	// because it doesn't hold the ownership.
	metapp::Variant var(new std::string());
	metapp::Variant v(metapp::Variant::takeFrom(var));
	//code
}

/*desc
## Member functions

#### getMetaType
```c++
const MetaType * getMetaType() const noexcept;
```
Return the meta type held by the variant. The result is always valid pointer.
Any variant, including the default constructed, always contains a meta type.

#### canGet
```c++
template <typename T>
bool canGet() const; // #1

bool canGet(const MetaType * toMetaType) const; // #2
```
Return true if `myVariant.get<T>()` can be called to get the underlying value.  
#1 form is equivalent to `canGet(metapp::getMetaType<T>())`. 

The rules to determine `canGet`, assume the underlying value has type V,  
1. If T is Variant or reference to Variant, `canGet` returns true. Otherwise if V is Variant or reference to Variant, returns `this->get<const Variant &>().canGet<T>()`.
2. If both T and V are references, `canGet` returns true.  
3. If both T and V are pointers after reference is removed, `canGet` returns true.  
4. If both T and V are C array after reference is removed, `canGet` returns true.  
5. If either T or V is reference, the other one is not reference,
`canGet` returns true only if the referred type is same as the non-reference type.  
6. If neither T and V are reference or pointer, `canGet` returns true only if T is same as V.  

The pseudo code for the rules in canGet, "thisType" is the type in `this` Variant.

```c++
bool canGet(toMetaType) {
	if(toMetaType is Variant or reference to Variant) {
		return true;
	}
	if(thisType is Variant or reference to Variant) {
		return this->get<const Variant &>().canGet(toMetaType);
	}
	if(toMetaType is reference and thisType is reference) {
		return true;
	}
	if(toMetaType is pointer and thisType is pointer) {
		return true;
	}
	if(toMetaType is array and thisType is array) {
		return true;
	}
	return false;
}
```

`canGet` and `get` expect either T is same as underlying type V, or T and V are reference are pointer.  
If You need to get the underlying value as different type, use `canCast` and `cast`.  

**Example**  
desc*/

ExampleFunc
{
	//code
	int n = 5;
	metapp::Variant v1(metapp::Variant::create<int &>(n)); // reference
	ASSERT(v1.canGet<int>()); // rule 3
	ASSERT(v1.canGet<int &>()); // rule 1
	ASSERT(v1.canGet<const int &>()); // rule 1
	ASSERT(v1.get<int>() == 5);
	ASSERT(v1.get<int &>() == 5);
	ASSERT(v1.get<const int &>() == 5);
	ASSERT(v1.canGet<long &>()); // rule 1
	v1.get<long &>(); // this may return wrong value, because long & is not int &

	metapp::Variant v2(38); // int
	ASSERT(v2.canGet<int>()); // rule 4
	ASSERT(v2.canGet<int &>()); // rule 3
	ASSERT(! v2.canGet<long>()); // rule 4

	int m = 9;
	metapp::Variant v3(&m); // pointer
	ASSERT(v3.canGet<int *>()); // rule 2
	ASSERT(v3.canGet<const int *>()); // rule 2
	ASSERT(! v3.canGet<int>()); // rule 2
	//code
}

/*desc
#### get
```c++
template <typename T>
T get() const;
```
Returns the underlying value as T.  
If `canGet<T>()` returns false, it throws exception `metapp::BadCastException`.  
If T is array such as int[3], the return type is the reference to the array, e.g, int(&)[3].
If T is function type, the return type is function pointer.  
If T is Variant or reference to Variant, then, if `this` type is Variant or reference to Variant, returns the underlying Variant,
otherwise returns `*this`.

Note: `get` doesn't check if `canGet()` returns true for performance reason. It's the caller's responsibility to be sure
`T` matches the underlying value type. For checked get, use function `checkedGet`. `get` does `assert(canGet<T>())`, that's
only available in debug mode for debug purpose.

T can be reference of the underlying type. For example, if the a Variant `v` holds a std::string,
we can call `v.get<std::string &>()`, or `v.get<const std::string &>()` to get a reference
instead of copy the value. That helps to improve the performance.  
We should always getting as reference to avoid copying, unless you do want to copy the value.  

The pseudo code for the rules in get, "thisType" is the type in `this` Variant.

```c++
template <typename T>
ReturnType get() {
	if(T is Variant or reference to Variant) {
		if(thisType is Variant or reference to Variant) {
			return underlying Variant;
		}
		return *this;
	}
	if(thisType is Variant or reference to Variant) {
		return this->get<const Variant &>().get<T>();
	}
	return underlying value;
}
```

Note: `canGet` and `get` are not type safe when either types (T and `this` type inside the Variant)
are pointer and reference. The reason is `canGet` and `get` assume the user have knowledge on the type
held by the Variant and they provide a fast way to access the underlying pointer and reference.  
If you pursuit better type safety, use `canCast` and `cast`.  
If you want to `get` with strong type safety when there are pointers or reference, you can compare
the types before `get`, for example,  
desc*/

ExampleFunc
{
	//code
	metapp::Variant v;
	if(metapp::getNonReferenceMetaType(v)->equal(metapp::getMetaType<int *>())) {
		// We are sure v holds a pointer to int, such as `int *`, or `const int *`, etc.
		int * p = v.get<int *>(); // safe
		*p = 5;
	}
	// Below check is not safe, `canGet` returns true even if v holds a `double *`.
	if(v.canGet<int *>()) {
	}
	//code
}

/*desc
#### get
```c++
template <typename T>
T checkedGet() const;
```

If `canGet<T>()` returns true, `checkedGet()` returns the value from `get()`.  
If `canGet<T>()` returns false, it throws exception `metapp::BadCastException`.  

#### getAddress
```c++
void * getAddress() const;
```
Return the address of underlying value. This is the same semantic as the address-of operator & in C++.  
If the Variant holds a reference, returns the address of the value it refers to.  
Otherwise, return the address of the underlying value.  

**Example**  
desc*/

ExampleFunc
{
	//code
	metapp::Variant v1(5);
	ASSERT(v1.get<int>() == 5);

	// equivalent native C++: *&v1 = 38;
	*(int *)v1.getAddress() = 38;
	ASSERT(v1.get<int>() == 38);
	int n1 = 8;
	int n2 = 9;
	// pointer, equivalent native C++: int * v2 = &n1;
	metapp::Variant v2(&n1);
	ASSERT(* v2.get<int *>() == 8);
	// equivalent native C++: *&v2 = &n2;
	*(int **)v2.getAddress() = &n2;
	ASSERT(* v2.get<int *>() == 9);

	int m = 10;
	// reference, equivalent native C++: int & v3 = m;
	metapp::Variant v3(metapp::Variant::reference(m));
	ASSERT(m == 10);
	// equivalent native C++: *&v3 = 15;
	*(int *)v3.getAddress() = 15;
	ASSERT(m == 15);
	//code
}

/*desc
#### canCast
```c++
bool canCast(const MetaType * toMetaType) const;

// This template form is same as
// canCast(metapp::getMetaType<T>());
template <typename T>
bool canCast() const;
```

Return true if `myVariant.cast(toMetaType)` can be called to cast the underlying value to `toMetaType`.   
Note: if both the Variant and `toMetaType` are class pointers or references,
the function will use the global MetaRepoList returned by `metapp::getMetaRepoList()` to cast between base and derived classes.  
Below table shows the rules to determine `canCast`, assume the underlying value has meta type `from`,
and we want to cast it to type `to` (which is `toMetaType`), `F` and `T` are value type, they are not reference, not pointer.  

| from | to  | canCast returns                                                                                            |
|------|-----|------------------------------------------------------------------------------------------------------------|
| F &  | T & | If both F and T are registered class, return the result of canCast on F and T.<br/>If F == T, return referent to T. Otherwise, return cast F to T, which is not a reference any more. |
| F    | T & | the result of canCast on F and T                                                                           |
| F &  | T   | the result of canCast on F and T                                                                           |
| F *  | T * | If both F and T are registered class, return the result of canCast on F and T.<br/>Otherwise, return F == T |
| F *  | T   | false                                                                                                      |
| F    | T * | false                                                                                                      |
| F    | T   | determined by canCast                                                                                      |

Note: `void *` can cast to any pointer, and any pointer can cast to `void *`.

#### cast
```c++
Variant cast(const MetaType * toMetaType) const;

// This template form is same as
// cast(metapp::getMetaType<T>());
template <typename T>
Variant cast() const;
```

If `canCast` returns true, `cast` returns the casted variant which type matches `toMetaType` or `T`.  
If `canCast<T>()` returns false, it throws exception `metapp::BadCastException`.  
To get the casted value, call `get` on the returned variant. For example, `int castedValue = v.cast<int>().get<int>()`.  

Note: The casted Variant may have different reference or CV qualifiers with T. For example, if we cast to `const T &`, the meta type
in the returned Variant can be `T &`, or `T`, both the `const` and reference can be discarded. If we cast to `T`, the return type
can be `T &`, the reference is added. This is not an issue, because we can always call `var.get<const T &>()`
or `var.get<T &>()` on the casted Variant, there is no any difference.  
In brief, the returned type doesn't guarantee the reference or CV is same as the type we cast to, but the returned type
can guarantee as below code (assume we cast to T, the returned type is U),  
```c++
metapp::getNonReferenceMetaType(metapp::getMetaType<U>())->equal(metapp::getNonReferenceMetaType(metapp::getMetaType<T>()));
castedVariant.canGet<U>();
castedVariant.canGet<U &>();
castedVariant.canGet<std::remove_reference<U>::type>();
```

#### castSilently
```c++
Variant castSilently(const MetaType * toMetaType) const;
template <typename T>
Variant castSilently() const;
```

Similar to `cast`, the only difference is that if `canCast<T>()` returns false,
`castSilently` doesn't throw exception, and returns an empty Variant (which isEmpty() is true).  

**Hint**: when to use `canCast`, `cast`, and `castSilently`   
If you only want to check if it's cast-able, but don't need to perform the cast, use `canCast`.  
If you want a variant must be casted, and throw exception if it can't be casted,
use `cast` without checking `canCast` explicitly because `cast` will check `canCast`, and `canCast` is almost
as expensive on performance as `cast`.  
If you want a variant be casted, and allow the cast fail, use `castSilently`, then check if the result is empty.  

#### isEmpty
```c++
bool isEmpty() const noexcept;
```
Return true if the variant holds `tkVoid`. A default constructed variant holds `tkVoid`.
Such a variant can't be got value, and can't be casted.  

#### clone
```c++
Variant clone() const;
```

Clone the underlying object and return a Variant that holds the cloned object.  
To understand how `clone` works, please see the section "Memory management in Variant".  

#### assign

```c++
Variant & assign(const Variant & other);
```

Assign `other` to `this`.  
Firstly the function casts `other` to the meta type in `this`, then copy the data in the casted Variant to the data in `this`.  
If `this` is a Variant of reference, the referred-to object is modified. Otherwise, the object contained by the Variant is modified.
This function is particular useful to set value to the referred-to object referred by a reference.  

This function is complete different with the `Variant & operator = (const Variant & other) noexcept`.  
The `operator =` is class semantic. That's to say, when using `operator =`, `this` is a fresh new Variant that's copied from `other`.  
Function `assign` is C++ value assignment semantic. That's to say, it's similar to do the expression `v = u`.  
Let's see examples, first let's see how C++ assignment works.  

```c++
T t;
U u;
t = u;
```
In above code, `u` is casted to type `T`, then assign to `t`. After the assignment, `t` still has type `T`. If `t` is reference,  

```c++
T n;
T & t = n;
U u;
t = u;
```
In above code, after the assignment, `n` will receive the new value of `u`.  

Now let's see how Variant `assign` works.  
desc*/

ExampleFunc
{
	{
		//code
		// Assign to value.
		metapp::Variant t(5);
		ASSERT(t.getMetaType()->equal(metapp::getMetaType<int>())); // t is int
		ASSERT(t.get<int>() == 5);
		metapp::Variant u(38.2);
		ASSERT(u.getMetaType()->equal(metapp::getMetaType<double>())); // u is double

		t.assign(u);
		ASSERT(t.getMetaType()->equal(metapp::getMetaType<int>())); // t is still int
		ASSERT(t.get<int>() == 38); // t receives new value (int)38.2, that's 38
		//code
	}

	{
		//code
		// Assign to reference.
		int n = 5;
		metapp::Variant t = metapp::Variant::reference(n);
		ASSERT(t.getMetaType()->equal(metapp::getMetaType<int &>())); // t is int &
		ASSERT(t.get<int>() == 5);
		metapp::Variant u(38.2);
		ASSERT(u.getMetaType()->equal(metapp::getMetaType<double>())); // u is double

		t.assign(u);
		ASSERT(t.getMetaType()->equal(metapp::getMetaType<int &>())); // t is still int &
		ASSERT(t.get<int>() == 38); // t receives new value (int)38.2, that's 38
		ASSERT(n == 38); // n is also modified
		//code
	}
}

/*desc
#### swap
```c++
void swap(Variant & other) noexcept;
```

Swap with another variant.

## Free functions

#### getTypeKind
```c++
TypeKind getTypeKind(const Variant & v);
```

Get the TypeKind held by the variant. This is a shortcut function for `v.getMetaType()->getTypeKind()`.

#### Streaming operators
```c++
std::istream & operator >> (std::istream & stream, Variant & v);
std::ostream & operator << (std::ostream & stream, const Variant & v);
```

Variant supports input and output stream if the underlying value supports the stream.  
If the underlying value doesn't support streaming, invoking the I/O streaming operators wll throw `metapp::UnsupportedException`.

#### swap
```c++
void swap(Variant & a, Variant & b) noexcept;

```
Swap two variants.


## Memory management in Variant

#### The data storage in Variant is similar to native C++

If the underlying value is pointer or reference, Variant only stores the pointer or reference,
it doesn't store the data pointed by the pointer or reference.  
If the underlying value is C array, the array is copied to the internal memory.  
If the underlying value is function, it's decayed to function pointer.  
If the underlying value is not a pointer or reference, Variant copies the value to the internal memory,
and destroy the value (call the destructor if the value is an object) when the Variant is destroyed, or assigned with another value.  

#### Copying variants is different from native C++

For value which is fundamental types such as int, long, or pointer, or any POD struct
which size is smaller enough (the max size is 8 or 16 bytes, depending on the platform and the compiler),
the value is stored in Variant directly. That means when the Variant is copied, the value is copied too.  
For value which size is not small, or not POD data, the value is stored on the heap using a `std::shared_ptr`
that's managed by Variant. That's to say, when the Variant is copied, the value is not copied.
If you want the value be copied, use `Variant::clone`.  
Copying Variant is always trivial, there is not any memory allocation.  

desc*/
