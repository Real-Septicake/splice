#pragma once

#include <meta>

#include "splice/detail/meta_utils.hpp"

namespace splice
{
  /// @brief Internal helper that performs a pointer-to-member dereference and returns a true lvalue  reference. Exists
  /// solely to work around a GCC 16 bug where the name mangler ICEs on any function template whose signature contains a
  /// `std::meta::info` NTTP, directly or via an alias.  By moving the actual member access into a plain
  /// (non-reflection) template, the mangler only ever sees ordinary types (`MemberT`, `ClassT`) and never chokes.
  namespace detail
  {
    template<typename MemberT, typename ClassT>
    [[nodiscard]] MemberT &shadow_get_ref(ClassT *instance, MemberT ClassT::*pm)
    {
      return instance->*pm;
    }
  }


  /// @brief Helper alias that resolves the type of a reflected member.
  template<std::meta::info Member>
  using MemberType = [:std::meta::type_of(Member):];

  /// @brief Returns a reference to the member reflected by @p Member on the given instance.
  ///
  /// @tparam Member A reflection of a direct member of @p T.
  /// @tparam T      The type pointed to by @p instance. May be `const`-qualified; the returned
  ///                reference propagates that const-ness automatically.
  /// @param  instance Pointer to the object to read from.
  /// @returns An lvalue reference to the reflected member (`MemberType<Member>&` or
  ///          `const MemberType<Member>&` depending on the const-ness of `T`).
  ///
  /// @note @p Member must be a direct member of @p T (ignoring const); a mismatch is a
  ///       compile-time error.
  ///
  /// @note **GCC 16 workaround:** GCC 16's name mangler ICEs when a `std::meta::info` NTTP
  ///       appears anywhere in a function template's mangled signature. To avoid this, the
  ///       splice expression `&[:Member:]` is evaluated inside the function body (where it is
  ///       legal), and the actual dereference is delegated to `detail::shadow_get_ref` — a
  ///       plain template with no reflection in its signature. `decltype(auto)` preserves the
  ///       returned `MemberT&` without stripping the reference. Revisit once the bug is fixed.
  ///
  /// @par Example
  /// @code
  /// // Read
  /// float seed = splice::shadow_get<^^GameWorld::m_seed>(&world);
  /// // Write through the returned reference
  /// splice::shadow_get<^^GameWorld::m_seed>(&world) = 42.0f;
  /// @endcode
  template<std::meta::info Member, typename T>
  [[nodiscard]] decltype(auto) shadow_get(T *instance)
  {
    static_assert(member_belongs_to<std::remove_const_t<T>, Member>(),
        "shadow_get: Member does not belong to T. "
        "Check that the reflected member is a direct member of the "
        "type of the pointer you are passing.");

    constexpr MemberType<Member> std::remove_const_t<T>::*pm = &[:Member:];
    return detail::shadow_get_ref(instance, pm);
  }

  /// @brief Sets the member reflected by @p Member on the given instance to @p value.
  ///
  /// Accepts both lvalue and rvalue references and moves when possible.
  ///
  /// @tparam Member A reflection of a direct member of @p T.
  /// @tparam T      The type pointed to by @p instance.
  /// @tparam U      The type of the value being assigned; must be assignable to `MemberType<Member>`.
  /// @param  instance Pointer to the object to modify.
  /// @param  value    The value to assign; forwarded into the member.
  ///
  /// @note @p Member must be a direct member of @p T; a mismatch is a compile-time error.
  ///
  /// @par Example
  /// @code
  /// splice::shadow_set<^^GameWorld::m_seed>(&world, 42.0f);
  /// splice::shadow_set<^^GameWorld::m_name>(&world, std::string("foo"));
  /// @endcode
  template<std::meta::info Member, typename T, typename U>
  void shadow_set(T *instance, U &&value)
  {
    static_assert(member_belongs_to<T, Member>(), "shadow_set: Member does not belong to T. "
                                                  "Check that the reflected member is a direct member of the "
                                                  "type of the pointer you are passing.");

    static_assert(std::is_assignable_v<MemberType<Member> &, U &&>,
        "shadow_set: value type is not assignable to the member type.");

    instance->[:Member:] = std::forward<U>(value);
  }

} // namespace splice
