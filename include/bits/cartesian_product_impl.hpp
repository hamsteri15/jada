//This file is directly copied from https://github.com/TartanLlama/ranges which provides support for the c++23 ranges

#ifndef TL_RANGES_CARTESIAN_PRODUCT_HPP
#define TL_RANGES_CARTESIAN_PRODUCT_HPP

#include <iterator>
#include <ranges>
#include <type_traits>
#include <tuple>
#include <utility>

//This chunk is from tl_ranges: meta.hpp file
namespace tl {


   template <class Tuple>
   constexpr inline std::size_t tuple_size = std::tuple_size_v<std::remove_cvref_t<Tuple>>;

   template <std::size_t N>
   using index_constant = std::integral_constant<std::size_t, N>;

   namespace meta {
      //Partially-apply the given template with the given arguments
      template <template <class...> class T, class... Args>
      struct partial {
         template <class... MoreArgs>
         struct apply {
            using type = T<Args..., MoreArgs...>;
         };
      };

      namespace detail {
         template <class T, template<class...> class Into, std::size_t... Idx>
         constexpr auto repeat_into_impl(std::index_sequence<Idx...>)
            ->Into < typename decltype((Idx, std::type_identity<T>{}))::type... > ;
      }

      //Repeat the given type T into the template Into N times
      template <class T, std::size_t N, template <class...> class Into>
      using repeat_into = decltype(tl::meta::detail::repeat_into_impl<T,Into>(std::make_index_sequence<N>{}));
   }
}

//This is the tl_ranges tuple_utility.hpp file
namespace tl {
   //If the size of Ts is 2, returns pair<Ts...>, otherwise returns tuple<Ts...>
   namespace detail {
      template<class... Ts>
      struct tuple_or_pair_impl : std::type_identity<std::tuple<Ts...>> {};
      template<class Fst, class Snd>
      struct tuple_or_pair_impl<Fst, Snd> : std::type_identity<std::pair<Fst, Snd>> {};
   }
   template<class... Ts>
   using tuple_or_pair = detail::tuple_or_pair_impl<Ts...>::type;

    
   template <class Tuple>
   constexpr auto min_tuple(Tuple&& tuple) {
      return std::apply([](auto... sizes) {
         return std::ranges::min({
           std::common_type_t<decltype(sizes)...>(sizes)...
            });
         }, std::forward<Tuple>(tuple));
   }

   template <class Tuple>
   constexpr auto max_tuple(Tuple&& tuple) {
      return std::apply([](auto... sizes) {
         return std::ranges::max({
           std::common_type_t<decltype(sizes)...>(sizes)...
            });
         }, std::forward<Tuple>(tuple));
   }
   

   //Call f on every element of the tuple, returning a new one
   template<class F, class... Tuples>
   constexpr auto tuple_transform(F&& f, Tuples&&... tuples)
   {
      if constexpr (sizeof...(Tuples) > 1) {
         auto call_at_index = []<std::size_t Idx, class Fu, class... Ts>
            (tl::index_constant<Idx>, Fu ff, Ts&&... ttuples) {
            return ff(std::get<Idx>(std::forward<Ts>(ttuples))...);
         };

         constexpr auto min_size = tl::min_tuple(std::tuple(tl::tuple_size<Tuples>...));

         return[&] <std::size_t... Idx>(std::index_sequence<Idx...>) {
            return tuple_or_pair < std::decay_t<decltype(call_at_index(tl::index_constant<Idx>{}, std::move(f), std::forward<Tuples>(tuples)...)) > ... >
               (call_at_index(tl::index_constant<Idx>{}, std::move(f), std::forward<Tuples>(tuples)...)...);
         }(std::make_index_sequence<min_size>{});
      }
      else if constexpr (sizeof...(Tuples) == 1) {
         return std::apply([&]<class... Ts>(Ts&&... elements) {
            return tuple_or_pair<std::invoke_result_t<F&, Ts>...>(
               std::invoke(f, std::forward<Ts>(elements))...
               );
         }, std::forward<Tuples>(tuples)...);
      }
      else {
         return std::tuple{};
      }
   }

    
   //Call f on every element of the tuple
   template<class F, class Tuple>
   constexpr auto tuple_for_each(F&& f, Tuple&& tuple)
   {
      return std::apply([&]<class... Ts>(Ts&&... elements) {
         (std::invoke(f, std::forward<Ts>(elements)), ...);
      }, std::forward<Tuple>(tuple));
   }
   

   template <class Tuple>
   constexpr auto tuple_pop_front(Tuple&& tuple) {
      return std::apply([](auto&& head, auto&&... tail) {
         return std::pair(std::forward<decltype(head)>(head), std::tuple(std::forward<decltype(tail)>(tail)...));
         }, std::forward<Tuple>(tuple));
   }

   namespace detail {
      template <class F, class V>
      constexpr auto tuple_fold_impl(F, V v) {
         return v;
      }
      template <class F, class V, class Arg, class... Args>
      constexpr auto tuple_fold_impl(F f, V v, Arg arg, Args&&... args) {
         return tl::detail::tuple_fold_impl(f, 
            std::invoke(f, std::move(v), std::move(arg)), 
            std::forward<Args>(args)...);
      }
   }

   template <class F, class T, class Tuple>
   constexpr auto tuple_fold(Tuple tuple, T t, F f) {
      return std::apply([&](auto&&... args) {
         return tl::detail::tuple_fold_impl(std::move(f), std::move(t), std::forward<decltype(args)>(args)...);
         }, std::forward<Tuple>(tuple));
   }

   template<class... Tuples>
   constexpr auto tuple_zip(Tuples&&... tuples) {
      auto zip_at_index = []<std::size_t Idx, class... Ts>
         (tl::index_constant<Idx>, Ts&&... ttuples) {
         return tuple_or_pair<std::decay_t<decltype(std::get<Idx>(std::forward<Ts>(ttuples)))>...>(std::get<Idx>(std::forward<Ts>(ttuples))...);
      };

      constexpr auto min_size = tl::min_tuple(std::tuple(tl::tuple_size<Tuples>...));

      return[&] <std::size_t... Idx>(std::index_sequence<Idx...>) {
         return tuple_or_pair<std::decay_t<decltype(zip_at_index(tl::index_constant<Idx>{}, std::forward<Tuples>(tuples)...))>... >
            (zip_at_index(tl::index_constant<Idx>{}, std::forward<Tuples>(tuples)...)...);
      }(std::make_index_sequence<min_size>{});
   }
}















//This is from tl_ranges common.hpp
namespace tl {
   namespace detail {
      template <class I>
      concept single_pass_iterator = std::input_or_output_iterator<I> && !std::forward_iterator<I>;

      template <typename... V>
      constexpr auto common_iterator_category() {
         if constexpr ((std::ranges::random_access_range<V> && ...))
            return std::random_access_iterator_tag{};
         else if constexpr ((std::ranges::bidirectional_range<V> && ...))
            return std::bidirectional_iterator_tag{};
         else if constexpr ((std::ranges::forward_range<V> && ...))
            return std::forward_iterator_tag{};
         else if constexpr ((std::ranges::input_range<V> && ...))
            return std::input_iterator_tag{};
         else
            return std::output_iterator_tag{};
      }
   }

   template <class... V>
   using common_iterator_category = decltype(detail::common_iterator_category<V...>());

   template <class R>
   concept simple_view = std::ranges::view<R> && std::ranges::range<const R> &&
      std::same_as<std::ranges::iterator_t<R>, std::ranges::iterator_t<const R>> &&
      std::same_as<std::ranges::sentinel_t<R>,
      std::ranges::sentinel_t<const R>>;

   struct as_sentinel_t {};
   constexpr inline as_sentinel_t as_sentinel;

   template <bool Const, class T>
   using maybe_const = std::conditional_t<Const, const T, T>;
}




//This is the tl_ranges basic_iterator.hpp file
namespace tl {
   template <std::destructible T>
   class basic_mixin : protected T {
   public:
      constexpr basic_mixin()
         noexcept(std::is_nothrow_default_constructible<T>::value)
         requires std::default_initializable<T> :
         T() {}
      constexpr basic_mixin(const T& t)
         noexcept(std::is_nothrow_copy_constructible<T>::value)
         requires std::copy_constructible<T> :
         T(t) {}
      constexpr basic_mixin(T&& t)
         noexcept(std::is_nothrow_move_constructible<T>::value)
         requires std::move_constructible<T> :
         T(std::move(t)) {}


      constexpr T& get() & noexcept { return *static_cast<T*>(this); }
      constexpr const T& get() const& noexcept { return *static_cast<T const*>(this); }
      constexpr T&& get() && noexcept { return std::move(*static_cast<T*>(this)); }
      constexpr const T&& get() const&& noexcept { return std::move(*static_cast<T const*>(this)); }
   };

   namespace cursor {
      namespace detail {
         template <class C>
         struct tags {
            static constexpr auto single_pass() requires requires { { C::single_pass } -> std::convertible_to<bool>; } {
               return C::single_pass;
            }
            static constexpr auto single_pass() { return false; }

            static constexpr auto contiguous() requires requires { { C::contiguous } -> std::convertible_to<bool>; } {
               return C::contiguous;
            }
            static constexpr auto contiguous() { return false; }
         };
      }
      template <class C>
      constexpr bool single_pass = detail::tags<C>::single_pass();

      template <class C>
      constexpr bool tagged_contiguous = detail::tags<C>::contiguous();

      namespace detail {
         template <class C>
         struct deduced_mixin_t {
            template <class T> static auto deduce(int)->T::mixin;
            template <class T> static auto deduce(...)->tl::basic_mixin<T>;
            using type = decltype(deduce<C>(0));
         };
      }

      template <class C>
      using mixin_t = detail::deduced_mixin_t<C>::type;

      template <class C>
      requires
         requires(const C& c) { c.read(); }
      using reference_t = decltype(std::declval<const C&>().read());

      namespace detail {
         template <class C>
         struct deduced_value_t {
            template<class T> static auto deduce(int)->T::value_type;
            template<class T> static auto deduce(...)->std::decay_t<reference_t<T>>;

            using type = decltype(deduce<C>(0));
         };
      }

      template <class C>
      requires std::same_as<typename detail::deduced_value_t<C>::type, std::decay_t<typename detail::deduced_value_t<C>::type>>
         using value_type_t = detail::deduced_value_t<C>::type;

      namespace detail {
         template <class C>
         struct deduced_difference_t {
            template <class T> static auto deduce(int)->T::difference_type;
            template <class T>
            static auto deduce(long)->decltype(std::declval<const T&>().distance_to(std::declval<const T&>()));
            template <class T>
            static auto deduce(...)->std::ptrdiff_t;

            using type = decltype(deduce<C>(0));
         };
      }

      template <class C>
      using difference_type_t = detail::deduced_difference_t<C>::type;

      template <class C>
      concept cursor = std::semiregular<std::remove_cv_t<C>>
         && std::semiregular<mixin_t<std::remove_cv_t<C>>>
         && requires {typename difference_type_t<C>; };

      template <class C>
      concept readable = cursor<C> && requires(const C & c) {
         c.read();
         typename reference_t<C>;
         typename value_type_t<C>;
      };

      template <class C>
      concept arrow = readable<C>
         && requires(const C & c) { c.arrow(); };

      template <class C, class T>
      concept writable = cursor<C>
         && requires(C & c, T && t) { c.write(std::forward<T>(t)); };

      template <class S, class C>
      concept sentinel_for = cursor<C> && std::semiregular<S>
         && requires(const C & c, const S & s) { {c.equal(s)} -> std::same_as<bool>; };

      template <class S, class C>
      concept sized_sentinel_for = sentinel_for<S, C> &&
         requires(const C & c, const S & s) {
            {c.distance_to(s)} -> std::same_as<difference_type_t<C>>;
      };

      template <class C>
      concept next = cursor<C> && requires(C & c) { c.next(); };

      template <class C>
      concept prev = cursor<C> && requires(C & c) { c.prev(); };

      template <class C>
      concept advance = cursor<C>
         && requires(C & c, difference_type_t<C> n) { c.advance(n); };

      template <class C>
      concept indirect_move = readable<C>
         && requires(const C & c) { c.indirect_move(); };

      template <class C, class O>
      concept indirect_swap = readable<C> && readable<O>
         && requires(const C & c, const O & o) {
         c.indirect_swap(o);
         o.indirect_swap(c);
      };

      template <class C>
      concept input = readable<C> && next<C>;
      template <class C>
      concept forward = input<C> && sentinel_for<C, C> && !single_pass<C>;
      template <class C>
      concept bidirectional = forward<C> && prev<C>;
      template <class C>
      concept random_access = bidirectional<C> && advance<C> && sized_sentinel_for<C, C>;
      template <class C>
      concept contiguous = random_access<C> && tagged_contiguous<C> && std::is_reference_v<reference_t<C>>;

      template <class C>
      constexpr auto cpp20_iterator_category() {
         if constexpr (contiguous<C>)
            return std::contiguous_iterator_tag{};
         else if constexpr (random_access<C>)
            return std::random_access_iterator_tag{};
         else if constexpr (bidirectional<C>)
            return std::bidirectional_iterator_tag{};
         else if constexpr (forward<C>)
            return std::forward_iterator_tag{};
         else
            return std::input_iterator_tag{};
      }
      template <class C>
      using cpp20_iterator_category_t = decltype(cpp20_iterator_category<C>());

      //There were a few changes in requirements on iterators between C++17 and C++20
      //See https://wg21.link/p2259 for discussion
      //- C++17 didn't have contiguous iterators
      //- C++17 input iterators required *it++ to be valid
      //- C++17 forward iterators required the reference type to be exactly value_type&/value_type const& (i.e. not a proxy)
      struct not_a_cpp17_iterator {};

      template <class C>
      concept reference_is_value_type_ref =
         (std::same_as<reference_t<C>, value_type_t<C>&> || std::same_as<reference_t<C>, value_type_t<C> const&>);

      template <class C>
      concept can_create_postincrement_proxy =
         (std::move_constructible<value_type_t<C>> && std::constructible_from<value_type_t<C>, reference_t<C>>);

      template <class C>
      constexpr auto cpp17_iterator_category() {
         if constexpr (random_access<C> && reference_is_value_type_ref<C>)
            return std::random_access_iterator_tag{};
         else if constexpr (bidirectional<C> && reference_is_value_type_ref<C>)
            return std::bidirectional_iterator_tag{};
         else if constexpr (forward<C> && reference_is_value_type_ref<C>)
            return std::forward_iterator_tag{};
         else if constexpr (can_create_postincrement_proxy<C>)
            return std::input_iterator_tag{};
         else
            return not_a_cpp17_iterator{};
      }
      template <class C>
      using cpp17_iterator_category_t = decltype(cpp17_iterator_category<C>());

      //iterator_concept and iterator_category are tricky; this abstracts them out.
      //Since the rules for iterator categories changed between C++17 and C++20
      //a C++20 iterator may have a weaker category in C++17,
      //or it might not be a valid C++17 iterator at all.
      //iterator_concept will be the C++20 iterator category.
      //iterator_category will be the C++17 iterator category, or it will not exist
      //in the case that the iterator is not a valid C++17 iterator.
      template <cursor C, class category = cpp17_iterator_category_t<C>>
      struct associated_types_category_base {
         using iterator_category = category;
      };
      template <cursor C>
      struct associated_types_category_base<C, not_a_cpp17_iterator> {};

      template <cursor C>
      struct associated_types : associated_types_category_base<C> {
         using iterator_concept = cpp20_iterator_category_t<C>;
         using value_type = cursor::value_type_t<C>;
         using difference_type = cursor::difference_type_t<C>;
         using reference = cursor::reference_t<C>;
      };

      namespace detail {
         // We assume a cursor is writeable if it's either not readable
         // or it is writeable with the same type it reads to
         template <class C>
         struct is_writable_cursor {
            template <readable T>
            requires requires (C c) {
               c.write(c.read());
            }
            static auto deduce()->std::true_type;

            template <readable T>
            static auto deduce()->std::false_type;

            template <class T>
            static auto deduce()->std::true_type;

            static constexpr bool value = decltype(deduce<C>())::value;
         };
      }
   }

   namespace detail {
      template <class T>
      struct post_increment_proxy {
      private:
         T cache_;

      public:
         template<typename U>
         constexpr post_increment_proxy(U&& t)
            : cache_(std::forward<U>(t))
         {}
         constexpr T const& operator*() const noexcept
         {
            return cache_;
         }
      };
   }


   template <cursor::input C>
   class basic_iterator :
      public cursor::mixin_t<C>
   {
   private:
      using mixin = cursor::mixin_t<C>;

      constexpr auto& cursor() noexcept { return this->mixin::get(); }
      constexpr auto const& cursor() const noexcept { return this->mixin::get(); }

      template <cursor::input>
      friend class basic_iterator;

      //TODO these need to change to support output iterators
      using reference_t = decltype(std::declval<C>().read());
      using const_reference_t = reference_t;

   public:
      using mixin::get;

      using value_type = cursor::value_type_t<C>;
      using difference_type = cursor::difference_type_t<C>;
      using reference = cursor::reference_t<C>;

      basic_iterator() = default;

      using mixin::mixin;

      constexpr explicit basic_iterator(C&& c)
         noexcept(std::is_nothrow_constructible_v<mixin, C&&>) :
         mixin(std::move(c)) {}


      constexpr explicit basic_iterator(C const& c)
         noexcept(std::is_nothrow_constructible_v<mixin, C const&>) :
         mixin(c) {}

      template <std::convertible_to<C> O>
      constexpr basic_iterator(basic_iterator<O>&& that)
         noexcept(std::is_nothrow_constructible<mixin, O&&>::value) :
         mixin(that.cursor()) {}

      template <std::convertible_to<C> O>
      constexpr basic_iterator(const basic_iterator<O>& that)
         noexcept(std::is_nothrow_constructible<mixin, const O&>::value) :
         mixin(std::move(that.cursor())) {}

      template <std::convertible_to<C> O>
      constexpr basic_iterator& operator=(basic_iterator<O>&& that) &
         noexcept(std::is_nothrow_assignable<C&, O&&>::value) {
         cursor() = std::move(that.cursor());
         return *this;
      }

      template <std::convertible_to<C> O>
      constexpr basic_iterator& operator=(const basic_iterator<O>& that) &
         noexcept(std::is_nothrow_assignable<C&, const O&>::value) {
         cursor() = that.cursor();
         return *this;
      }

      template <class T>
      requires
         (!std::same_as<std::decay_t<T>, basic_iterator> &&
            !cursor::next<C>&&
            cursor::writable<C, T>)
         constexpr basic_iterator& operator=(T&& t) &
         noexcept(noexcept(std::declval<C&>().write(static_cast<T&&>(t)))) {
         cursor() = std::forward<T>(t);
         return *this;
      }

      friend constexpr decltype(auto) iter_move(const basic_iterator& i)
         noexcept(noexcept(i.cursor().indirect_move()))
         requires cursor::indirect_move<C> {
         return i.cursor().indirect_move();
      }

      template <class O>
      requires cursor::indirect_swap<C, O>
         friend constexpr void iter_swap(
            const basic_iterator& x, const basic_iterator<O>& y)
         noexcept(noexcept((void)x.indirect_swap(y))) {
         x.indirect_swap(y);
      }

      //Input iterator
      constexpr decltype(auto) operator*() const
         noexcept(noexcept(std::declval<const C&>().read()))
         requires (cursor::readable<C> && !cursor::detail::is_writable_cursor<C>::value) {
         return cursor().read();
      }

      //Output iterator
      constexpr decltype(auto) operator*()
         noexcept(noexcept(reference_t{ cursor() }))
         requires (cursor::next<C>&& cursor::detail::is_writable_cursor<C>::value) {
         return reference_t{ cursor() };
      }

      //Output iterator
      constexpr decltype(auto) operator*() const
         noexcept(noexcept(
            const_reference_t{ cursor() }))
         requires (cursor::next<C>&& cursor::detail::is_writable_cursor<C>::value) {
         return const_reference_t{ cursor() };
      }

      constexpr basic_iterator& operator*() noexcept
         requires (!cursor::next<C>) {
         return *this;
      }

      // operator->: "Manual" deduction override,
      constexpr decltype(auto) operator->() const
         noexcept(noexcept(cursor().arrow()))
         requires cursor::arrow<C> {
         return cursor().arrow();
      }
      // operator->: Otherwise, if reference_t is an lvalue reference,
      constexpr decltype(auto) operator->() const
         noexcept(noexcept(*std::declval<const basic_iterator&>()))
         requires (cursor::readable<C> && !cursor::arrow<C>)
         && std::is_lvalue_reference<const_reference_t>::value{
         return std::addressof(**this);
      }

         // modifiers
         constexpr basic_iterator& operator++() & noexcept {
         return *this;
      }
      constexpr basic_iterator& operator++() &
         noexcept(noexcept(cursor().next()))
         requires cursor::next<C> {
         cursor().next();
         return *this;
      }

      //C++17 required that *it++ was valid.
      //For input iterators, we can't copy *this, so we need to create a proxy reference.
      constexpr auto operator++(int) &
         noexcept(noexcept(++std::declval<basic_iterator&>()) &&
            std::is_nothrow_move_constructible_v<value_type>&&
            std::is_nothrow_constructible_v<value_type, reference>)
         requires (cursor::single_pass<C>&&
            std::move_constructible<value_type>&&
            std::constructible_from<value_type, reference>) {
         detail::post_increment_proxy<value_type> p(**this);
         ++* this;
         return p;
      }

      //If we can't create a proxy reference, it++ is going to return void
      constexpr void operator++(int) &
         noexcept(noexcept(++std::declval<basic_iterator&>()))
         requires (cursor::single_pass<C> && !(std::move_constructible<value_type>&&
            std::constructible_from<value_type, reference>)) {
         (void)(++(*this));
      }

      //If C is a forward cursor then copying it is fine
      constexpr basic_iterator operator++(int) &
         noexcept(std::is_nothrow_copy_constructible_v<C>&&
            std::is_nothrow_move_constructible_v<C> &&
            noexcept(++std::declval<basic_iterator&>()))
         requires (!cursor::single_pass<C>) {
         auto temp = *this;
         ++* this;
         return temp;
      }

      constexpr basic_iterator& operator--() &
         noexcept(noexcept(cursor().prev()))
         requires cursor::bidirectional<C> {
         cursor().prev();
         return *this;
      }

      //Postfix decrement doesn't have the same issue as postfix increment
      //because bidirectional requires the cursor to be a forward cursor anyway
      //so copying it is fine.
      constexpr basic_iterator operator--(int) &
         noexcept(std::is_nothrow_copy_constructible<basic_iterator>::value&&
            std::is_nothrow_move_constructible<basic_iterator>::value &&
            noexcept(--std::declval<basic_iterator&>()))
         requires cursor::bidirectional<C> {
         auto tmp = *this;
         --* this;
         return tmp;
      }

      constexpr basic_iterator& operator+=(difference_type n) &
         noexcept(noexcept(cursor().advance(n)))
         requires cursor::random_access<C> {
         cursor().advance(n);
         return *this;
      }

      constexpr basic_iterator& operator-=(difference_type n) &
         noexcept(noexcept(cursor().advance(-n)))
         requires cursor::random_access<C> {
         cursor().advance(-n);
         return *this;
      }

      constexpr decltype(auto) operator[](difference_type n) const
         noexcept(noexcept(*(std::declval<basic_iterator&>() + n)))
         requires cursor::random_access<C> {
         return *(*this + n);
      }

      // non-template type-symmetric ops to enable implicit conversions
      friend constexpr difference_type operator-(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y.cursor().distance_to(x.cursor())))
         requires cursor::sized_sentinel_for<C, C> {
         return y.cursor().distance_to(x.cursor());
      }
      friend constexpr bool operator==(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(x.cursor().equal(y.cursor())))
         requires cursor::sentinel_for<C, C> {
         return x.cursor().equal(y.cursor());
      }
      friend constexpr bool operator!=(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(!(x == y)))
         requires cursor::sentinel_for<C, C> {
         return !(x == y);
      }
      friend constexpr bool operator<(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 < (y - x);
      }
      friend constexpr bool operator>(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 > (y - x);
      }
      friend constexpr bool operator<=(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 <= (y - x);
      }
      friend constexpr bool operator>=(
         const basic_iterator& x, const basic_iterator& y)
         noexcept(noexcept(y - x))
         requires cursor::sized_sentinel_for<C, C> {
         return 0 >= (y - x);
      }
   };

   namespace detail {
      template <class C>
      struct is_basic_iterator {
         template <class T>
         static auto deduce(basic_iterator<T> const&)->std::true_type;
         template <class T>
         static auto deduce(...)->std::false_type;
         static constexpr inline bool value = decltype(deduce(std::declval<C>()))::value;
      };
   }

   // basic_iterator nonmember functions
   template <class C>
   constexpr basic_iterator<C> operator+(
      const basic_iterator<C>& i, cursor::difference_type_t<C> n)
      noexcept(std::is_nothrow_copy_constructible<basic_iterator<C>>::value&&
         std::is_nothrow_move_constructible<basic_iterator<C>>::value &&
         noexcept(std::declval<basic_iterator<C>&>() += n))
      requires cursor::random_access<C> {
      auto tmp = i;
      tmp += n;
      return tmp;
   }
   template <class C>
   constexpr basic_iterator<C> operator+(
      cursor::difference_type_t<C> n, const basic_iterator<C>& i)
      noexcept(noexcept(i + n))
      requires cursor::random_access<C> {
      return i + n;
   }

   template <class C>
   constexpr basic_iterator<C> operator-(
      const basic_iterator<C>& i, cursor::difference_type_t<C> n)
      noexcept(noexcept(i + (-n)))
      requires cursor::random_access<C> {
      return i + (-n);
   }
   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr cursor::difference_type_t<C2> operator-(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(
         rhs.get().distance_to(lhs.get()))) {
      return rhs.get().distance_to(lhs.get());
   }
   template <class C, class S>
   requires cursor::sized_sentinel_for<S, C>
      constexpr cursor::difference_type_t<C> operator-(
         const S& lhs, const basic_iterator<C>& rhs)
      noexcept(noexcept(rhs.get().distance_to(lhs))) {
      return rhs.get().distance_to(lhs);
   }
   template <class C, class S>
   requires cursor::sized_sentinel_for<S, C>
      constexpr cursor::difference_type_t<C> operator-(
         const basic_iterator<C>& lhs, const S& rhs)
      noexcept(noexcept(-(rhs - lhs))) {
      return -(rhs - lhs);
   }

   template <class C1, class C2>
   requires cursor::sentinel_for<C2, C1>
      constexpr bool operator==(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(lhs.get().equal(rhs.get()))) {
      return lhs.get().equal(rhs.get());
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator==(
         const basic_iterator<C>& lhs, const S& rhs)
      noexcept(noexcept(lhs.get().equal(rhs))) {
      return lhs.get().equal(rhs);
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator==(
         const S& lhs, const basic_iterator<C>& rhs)
      noexcept(noexcept(rhs == lhs)) {
      return rhs == lhs;
   }

   template <class C1, class C2>
   requires cursor::sentinel_for<C2, C1>
      constexpr bool operator!=(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(!(lhs == rhs))) {
      return !(lhs == rhs);
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator!=(
         const basic_iterator<C>& lhs, const S& rhs)
      noexcept(noexcept(!lhs.get().equal(rhs))) {
      return !lhs.get().equal(rhs);
   }
   template <class C, class S>
   requires cursor::sentinel_for<S, C>
      constexpr bool operator!=(
         const S& lhs, const basic_iterator<C>& rhs)
      noexcept(noexcept(!rhs.get().equal(lhs))) {
      return !rhs.get().equal(lhs);
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator<(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept(lhs - rhs < 0)) {
      return (lhs - rhs) < 0;
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator>(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept((lhs - rhs) > 0)) {
      return (lhs - rhs) > 0;
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator<=(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept((lhs - rhs) <= 0)) {
      return (lhs - rhs) <= 0;
   }

   template <class C1, class C2>
   requires cursor::sized_sentinel_for<C1, C2>
      constexpr bool operator>=(
         const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
      noexcept(noexcept((lhs - rhs) >= 0)) {
      return (lhs - rhs) >= 0;
   }

   template <class V, bool Const>
   class basic_sentinel {
      using Base = std::conditional_t<Const, const V, V>;

   public:
      std::ranges::sentinel_t<Base> end_{};
      basic_sentinel() = default;
      constexpr explicit basic_sentinel(std::ranges::sentinel_t<Base> end)
         : end_{ std::move(end) } {}

      constexpr basic_sentinel(basic_sentinel<V, !Const> other) requires Const&& std::
         convertible_to<std::ranges::sentinel_t<V>,
         std::ranges::sentinel_t<Base>>
         : end_{ std::move(other.end_) } {}

      constexpr auto end() const {
         return end_;
      }

      friend class basic_sentinel<V, !Const>;
   };
}

namespace std {
   template <class C>
   struct iterator_traits<tl::basic_iterator<C>> : tl::cursor::associated_types<C> {};
}














//This is the tl_ranges cartesian_product.hpp file
namespace tl {
   template <std::ranges::forward_range... Vs>
   requires (std::ranges::view<Vs> && ...) class cartesian_product_view
      : public std::ranges::view_interface<cartesian_product_view<Vs...>> {

      //random access + sized is allowed because we can jump all iterators to the end
      template <class... Ts>
      static constexpr bool am_common = (std::ranges::common_range<Ts> && ...)
         || ((std::ranges::random_access_range<Ts> && ...) && (std::ranges::sized_range<Ts> && ...));

      template <class... Ts>
      static constexpr bool am_sized = (std::ranges::sized_range<Ts> && ...);

      //requires common because we need to be able to cycle the iterators from begin to end in O(n)
      template <class... Ts>
      static constexpr bool am_bidirectional =
         ((std::ranges::bidirectional_range<Ts> && ...) && (std::ranges::common_range<Ts> && ...));

      //requires sized because we need to calculate new positions for iterators using arithmetic modulo the range size
      template <class... Ts>
      static constexpr bool am_random_access =
         ((std::ranges::random_access_range<Ts> && ...) &&
            (std::ranges::sized_range<Ts> && ...));

      template <class... Ts>
      static constexpr bool am_distanceable =
         ((std::sized_sentinel_for<std::ranges::iterator_t<Ts>, std::ranges::iterator_t<Ts>>) && ...)
         && am_sized<Ts...>;

      std::tuple<Vs...> bases_;

      template <bool Const>
      class cursor;

      //Wraps the end iterator for the 0th range.
      //This is all that's required because the cursor will only ever set the 0th iterator to end when
      //the cartesian product operation has completed.
      template <bool Const>
      class sentinel {
         using parent = std::conditional_t<Const, const cartesian_product_view, cartesian_product_view>;
         using first_base = decltype(std::get<0>(std::declval<parent>().bases_));
         std::ranges::sentinel_t<first_base> end_;

      public:
         sentinel() = default;
         sentinel(std::ranges::sentinel_t<first_base> end) : end_(std::move(end)) {}

         //const-converting constructor
         constexpr sentinel(sentinel<!Const> other) requires Const &&
            (std::convertible_to<std::ranges::sentinel_t<first_base>, std::ranges::sentinel_t<const first_base>>)
            : end_(std::move(other.end_)) {
         }

         template <bool>
         friend class cursor;
      };

      template <bool Const>
      class cursor {
         template<class T>
         using constify = std::conditional_t<Const, const T, T>;

         constify<std::tuple<Vs...>>* bases_;
         std::tuple<std::ranges::iterator_t<constify<Vs>>...> currents_{};

      public:
         using reference =
            std::tuple<std::ranges::range_reference_t<constify<Vs>>...>;
         using value_type =
            std::tuple<std::ranges::range_value_t<constify<Vs>>...>;

         using difference_type = std::ptrdiff_t;

         cursor() = default;
         constexpr explicit cursor(constify<std::tuple<Vs...>>* bases)
            : bases_{ bases }, currents_( tl::tuple_transform(std::ranges::begin, *bases) )
         {}

         //If the underlying ranges are common, we can get to the end by assigning from end
         constexpr explicit cursor(as_sentinel_t, constify<std::tuple<Vs...>>* bases)
            requires(std::ranges::common_range<Vs> && ...)
            : cursor{ bases }
         {
            std::get<0>(currents_) = std::ranges::end(std::get<0>(*bases_));
         }

         //If the underlying ranges are sized and random access, we can get to the end by moving it forward by size
         constexpr explicit cursor(as_sentinel_t, constify<std::tuple<Vs...>>* bases)
            requires(!(std::ranges::common_range<Vs> && ...) && (std::ranges::random_access_range<Vs> && ...) && (std::ranges::sized_range<Vs> && ...))
            : cursor{ bases }
         {
            std::get<0>(currents_) += std::ranges::size(std::get<0>(*bases_));
         }

         //const-converting constructor
         constexpr cursor(cursor<!Const> i) requires Const && (std::convertible_to<
            std::ranges::iterator_t<Vs>,
            std::ranges::iterator_t<constify<Vs>>> && ...)
            : bases_{ std::move(i.bases_) }, currents_{ std::move(i.currents_) } {}


         constexpr decltype(auto) read() const {
            return tuple_transform([](auto& i) -> decltype(auto) { return *i; }, currents_);
         }

         //Increment the iterator at std::get<N>(currents_)
         //If that iterator hits its end, recurse to std::get<N-1>
         template <std::size_t N = (sizeof...(Vs) - 1)>
         void next() {
            auto& it = std::get<N>(currents_);
            ++it;
            if (it == std::ranges::end(std::get<N>(*bases_))) {
               if constexpr (N > 0) {
                  it = std::ranges::begin(std::get<N>(*bases_));
                  next<N - 1>();
               }
            }
         }

         //Decrement the iterator at std::get<N>(currents_)
         //If that iterator was at its begin, cycle it to end and recurse to std::get<N-1>
         template <std::size_t N = (sizeof...(Vs) - 1)>
         void prev() requires (am_bidirectional<constify<Vs>...>) {
            auto& it = std::get<N>(currents_);
            if (it == std::ranges::begin(std::get<N>(*bases_))) {
               std::ranges::advance(it, std::ranges::end(std::get<N>(*bases_)));
               if constexpr (N > 0) {
                  prev<N - 1>();
               }
            }
            --it;
         }

         template <std::size_t N = (sizeof...(Vs) - 1)>
         void advance(difference_type n) requires (am_random_access<constify<Vs>...>) {
            auto& it = std::get<N>(currents_);
            auto& base = std::get<N>(*bases_);
            auto begin = std::ranges::begin(base);
            auto end = std::ranges::end(base);
            auto size = end - begin;

            auto distance_from_begin = it - begin;

            //Calculate where in the iterator cycle we should end up
            auto offset = (distance_from_begin + n) % size;

            //Calculate how many times incrementing this iterator would cause it to cycle round
            //This will be negative if we cycled by decrementing
            auto times_cycled = (distance_from_begin + n) / size - (offset < 0 ? 1 : 0);

            //Set the iterator to the correct new position
            it = begin + static_cast<difference_type>(offset < 0 ? offset + size : offset);

            if constexpr (N > 0) {
               //If this iterator cycled, then we need to advance the N-1th iterator
               //by the number of times it cycled
               if (times_cycled != 0) {
                  advance<N - 1>(times_cycled);
               }
            }
            else {
               //If we're the 0th iterator, then cycling should set the iterator to the end
               if (times_cycled > 0) {
                  std::ranges::advance(it, end);
               }
            }
         }

         constexpr bool equal(const cursor& rhs) const
            requires (std::equality_comparable<std::ranges::iterator_t<constify<Vs>>> && ...) {
            return currents_ == rhs.currents_;
         }

         constexpr bool equal(const sentinel<Const>& s) const {
            return std::get<0>(currents_) == s.end_;
         }

         template <std::size_t N = (sizeof...(Vs) - 1)>
         constexpr auto distance_to(cursor const& other) const
            requires (am_distanceable<constify<Vs>...>) {
            if constexpr (N == 0) {
               return std::ranges::distance(std::get<0>(currents_), std::get<0>(other.currents_));
            }
            else {
               auto distance = distance_to<N - 1>(other);
               auto scale = std::ranges::distance(std::get<N>(*bases_));
               auto diff = std::ranges::distance(std::get<N>(currents_), std::get<N>(other.currents_));
               return difference_type{ distance * scale + diff };
            }
         }

         friend class cursor<!Const>;
      };

   public:
      cartesian_product_view() = default;
      explicit cartesian_product_view(Vs... bases) : bases_(std::move(bases)...) {}

      constexpr auto begin() requires (!(simple_view<Vs> && ...)) {
         return basic_iterator{ cursor<false>(std::addressof(bases_)) };
      }
      constexpr auto begin() const requires (std::ranges::range<const Vs> && ...) {
         return basic_iterator{ cursor<true>(std::addressof(bases_)) };
      }

      constexpr auto end() requires (!(simple_view<Vs> && ...) && am_common<Vs...>) {
         return basic_iterator{ cursor<false>(as_sentinel, std::addressof(bases_)) };
      }

      constexpr auto end() const requires (am_common<const Vs...>) {
         return basic_iterator{ cursor<true>(as_sentinel, std::addressof(bases_)) };
      }

      constexpr auto end() requires(!(simple_view<Vs> && ...) && !am_common<Vs...>) {
         return sentinel<false>(std::ranges::end(std::get<0>(bases_)));
      }

      constexpr auto end() const requires ((std::ranges::range<const Vs> && ...) && !am_common<const Vs...>) {
         return sentinel<true>(std::ranges::end(std::get<0>(bases_)));
      }

      constexpr auto size() requires (am_sized<Vs...>) {
         //Multiply all the sizes together, returning the common type of all of them
         return std::apply([](auto&&... bases) {
            using size_type = std::common_type_t<std::ranges::range_size_t<decltype(bases)>...>;
            return (static_cast<size_type>(std::ranges::size(bases)) * ...);
            }, bases_);
      }

      constexpr auto size() const requires (am_sized<const Vs...>) {
         return std::apply([](auto&&... bases) {
            using size_type = std::common_type_t<std::ranges::range_size_t<decltype(bases)>...>;
            return (static_cast<size_type>(std::ranges::size(bases)) * ...);
            }, bases_);
      }
   };

   template <class... Rs>
   cartesian_product_view(Rs&&...)->cartesian_product_view<std::views::all_t<Rs>...>;

   namespace views {
      namespace detail {
         class cartesian_product_fn {
         public:
            constexpr std::ranges::empty_view<std::tuple<>> operator()() const noexcept {
               return {};
            }
            template <std::ranges::viewable_range... V>
            requires ((std::ranges::forward_range<V> && ...) && (sizeof...(V) != 0))
               constexpr auto operator()(V&&... vs) const {
               return tl::cartesian_product_view{ std::views::all(std::forward<V>(vs))... };
            }
         };
      }  // namespace detail

      inline constexpr detail::cartesian_product_fn cartesian_product;
   }  // namespace views

}  // namespace tl

#endif