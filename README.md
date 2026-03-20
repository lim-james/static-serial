# Static Serial

`C++26` `Reflections` `GCC16`

Zero-overhead binary serialization w/ C++26 Reflection support

## Motivation

I have been hearing a lot about C++26's reflections and wanted to check it out.
But I was primarily inspired by [Barry Revzin's Practical Reflection at CppCon25](https://www.youtube.com/watch?v=ZX_z6wzEOG0) and his challenge to see how we can use reflections to solve problems.

## Resources
- [P2996R12 Paper](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r12.html#selecting-members) 
- GCC 16's header files

> As of writing this project, I couldn't find any available resources but that's
> fine with me because I want to attempt a new form of learning.

## C++26 features explored

1. `template for` expansion statements, complie-time iteration
2. `std::meta::nonstatic_data_members_of` extract all nonstatic members of given
   type
3. `^^` reflection operator - compute reflection value
4. `[: refl :]` splicers - produce grammatical elements from reflections (copied
   from P2996
5. `std::define_static_array` - takes a range and materialize a span for compile
   time (consteval). Needed this for `template for` 
6. `std::meta::size_of` why this over `sizeof`? Simply because sizeof
   accomodates for padded bytes whilst meta::size_of is raw number of bytes

## Difficulties faced

In this section, I will be documenting problems I have faced whilst attempting
to learn.

1. Compiler & P-Paper interface mismatch. p-paper describes `member_of(info
   r)` whilst the compiler takes in an additional parameter `access_context`
   - Overcomed by referrencing header files directly
