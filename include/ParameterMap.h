/***************************************************************
 * Author: Quirijn Bouts (www.qbouts.com | github.com/qbouts)  *
 * LICENSE: MIT                                                *
 ***************************************************************/

#ifndef PARAMETER_MAP_H
#define PARAMETER_MAP_H

#include <array>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace qbouts {
namespace detail {
template <class F, class Tuple>
constexpr decltype(auto) apply_optionals(F &&f, Tuple &&t);

template <int First, int Last, typename Lambda>
inline void static_for(Lambda const &f);
}  // namespace detail

/////////////////////////////////////////////////////////////
//////////////////     ParameterMap     /////////////////////
/////////////////////////////////////////////////////////////

/**
 *  @brief Represents a set of named parameters and facilitates calling functions with the stored parameters.
 *
 *  A parameter map be thought of as a set of {name, value} pairs. The ParameterMap class allows
 *  users to build up a parameter map and submit the stored parameters to a function.
 *
 *  \par Creating a ParameterMap
 *  The map is constructed with a set of parameter types supplied as template arguments and names for each parameter.
 *  Once constructed parameters can be stored as well as retrieved using \a set and \a get functions.
 *  Parameters can be identified using the names supplied upon constructing the map as well as using their index.
 *
 *  \par Calling a function with the stored parameters
 *  Using the \a submit member function the parameters can be 'submitted' to a supplied function: The function will be
 *  called with the stored parameters.
 *
 *  \par Performance
 *  Care has been taken to avoid making unnecessary copies of parameters or string comparisons.
 *  When using a ParameterMap of in a performance sensitive part of your code be aware of the following:
 *  - Any operations where parameters are identified by their name ( \a set, \a get, \a is_set) will compute an
 *    std::hash of the given \a name, which takes linear time in the length of the name. Where possible,
 *    prefer the use of the index based variants of these functions or build the parameter map outside of
 *    the performance critical section of your code.
 *  - For most functions the overhead of using \a submit compared to calling the function directly will be negligible.
 *    There is however one small exception to be aware of: function calls made using \a submit do not benefit from move
 *    semantics. To be specific: For functions that accept parameters by rvalue reference (e.g. int&&), a copy of the
 *    stored parameter will be made. This is required as the parameter map may not be modified by the \a submit call.
 *
 */
template <typename... PARAMETERS>
class ParameterMap {
public:
	/**
	 *  @brief Constructor.
	 *  @param names The names of the parameters represented by the parameter map.
	 *
	 *  Constructor initializes the ParameterMap given a set of parameter names.
	 *  Exactly one name should be supplied per parameter, compilation will fail otherwise.
	 */
	template <typename... PARAM_NAMES>
	explicit ParameterMap(PARAM_NAMES &&... names) requires(sizeof...(PARAMETERS) == sizeof...(PARAM_NAMES));


	/****************************************************************************/
	/*********************************** Set ************************************/
	/****************************************************************************/

	/**
	 *  @brief Sets the value of the parameter identified by \a name.
	 *  @param name Name of the parameter to which the @a value should be assigned.
	 *  @param value The value which should be stored for this parameter.
	 *  @throw  std::invalid_argument if no parameters match @a name.
	 */
	template <typename T>
	void set(const std::string_view &name, T &&value);

	/**
	 *  @brief Sets the value of the parameter identified by \a index.
	 *  @param index The index of the parameter to which the @a value should be assigned.
	 *  @param value The value which should be stored for this parameter (starting at 0).
	 *  @throw  std::out_of_range if @a index is an invalid index.
	 */
	template <typename T>
	void set(size_t index, T &&value);

	/**
	 *  @brief Sets the value of the parameter identified by \a INDEX.
	 *  @tparam INDEX The index of the parameter to which the @a value should be assigned (starting at 0).
	 *  @param value The value which should be stored for this parameter.
	 *
	 *  @note This function is noexcept if the assignment operation of the parameter is noexcept.
	 */
	template <size_t INDEX, typename T>
	void set(T &&value);

	/****************************************************************************/
	/*********************************** Get ************************************/
	/****************************************************************************/

	/**
	 *  @brief Gets the value of the parameter identified by \a name as a const (read-only) reference.
	 *  @tparam T The type of the parameter to be retrieved.
	 *  @param name Name of the parameter to be retrieved.
	 *  @return The value of the parameter as a const reference to @a T.
	 *  @throw  std::invalid_argument if no parameters match @a name or if the parameter type is incompatible to @a T.
	 *  @throw  std::runtime_error if no value is stored for the parameter.
	 */
	template <typename T>
	[[nodiscard]] const std::remove_cv_t<std::remove_reference_t<T>> &get(const std::string_view &name) const;

	/**
	 *  @brief Gets the value of the parameter identified by \a index as a const (read-only) reference.
	 *  @tparam T The type of the parameter to be retrieved.
	 *  @param index The index of the parameter to be retrieved (starting at 0).
	 *  @return The value of the parameter as a const reference to @a T.
	 *  @throw  std::out_of_range if @a index is an invalid index.
	 *  @throw  std::runtime_error if no value is stored for the parameter.
	 */
	template <typename T>
	[[nodiscard]] const std::remove_cv_t<std::remove_reference_t<T>> &get(size_t index) const;

	/**
	 *  @brief Gets the value of the parameter identified by \a INDEX as a const (read-only) reference.
	 *  @tparam INDEX The index of the parameter to be retrieved.
	 *  @return The value of the parameter as a const reference to its type (deduced automatically).
	 *  @throw  std::runtime_error if no value is stored for the parameter.
	 */
	template <size_t INDEX>
	[[nodiscard]] auto get() const requires(INDEX < sizeof...(PARAMETERS));


	/****************************************************************************/
	/********************************* is_set ***********************************/
	/****************************************************************************/

	/**
	 *  @brief Returns whether a value is set for the parameter identified by \a name.
	 *  @param name Name of the parameter to check.
	 *  @return True if a value is stored for the parameter, false otherwise.
	 *  @throw  std::invalid_argument if no parameters match @a name.
	 */
	[[nodiscard]] bool is_set(const std::string_view &name);

	/**
	 *  @brief Returns whether a value is set for the parameter identified by \a index.
	 *  @param index Index of the parameter to check.
	 *  @return True if a value is stored for the parameter, false otherwise.
	 *  @throw  std::out_of_range if @a index is an invalid index.
	 */
	[[nodiscard]] bool is_set(size_t index);
	/**
	 *  @brief Returns whether a value is set for the parameter identified by \a INDEX.
	 *  @tparam INDEX Index of the parameter to check.
	 *  @return True if a value is stored for the parameter, false otherwise.
	 */
	template <size_t INDEX>
	[[nodiscard]] bool is_set() const noexcept requires(INDEX < sizeof...(PARAMETERS));

	/****************************************************************************/
	/********************************** clear ***********************************/
	/****************************************************************************/

	/**
	 *  @brief Clears (destroys) all values stored for parameters.
	 */
	void clear() noexcept;

	/****************************************************************************/
	/********************************** size ************************************/
	/****************************************************************************/

	/**
	 *  @brief Returns the number of parameters in the ParameterMap.
	 *  @return The number of parameters in the ParameterMap.
	 */
	static constexpr size_t size() noexcept { return n_parameters; }

	/****************************************************************************/
	/********************************* submit ***********************************/
	/****************************************************************************/

	/**
	 *  @brief Calls the function with parameters previously stored using \a set.
	 *  @tparam FUNCTION The function to be called.
	 *  @return The return value of returned by the call to the supplied function.
	 *  @throw Throws a std::runtime_error if not all parameters have values stored
	 *
	 *  Calls the function with parameters previously stored using \a set.
	 *  Does NOT catch exceptions thrown \a function when called with the stored parameters.
	 *  Any such exceptions will have to be dealt with by the caller.
	 *  \a FUNCTION is required to be invocable using the parameters stored in the map.
	 *
	 *  \note Note: The parameter map is NOT modified by this function.
	 *    An arbitrary number of \a submit calls may take place once all parameters have been set.
	 *
	 *  \note A note on performance: As all parameters have already been set,
	 *    no more expensive operations such string comparisons or extra copies of parameter values are required.
	 *    As such, the overhead of \a submit should be negligible in almost all settings.
	 */
	template <typename FUNCTION>
	auto submit(FUNCTION &&function) const requires(std::is_invocable_v<FUNCTION, PARAMETERS...>);

private:
	static constexpr size_t n_parameters = sizeof...(PARAMETERS);
	std::array<std::size_t, n_parameters> m_parameter_name_hashes;
	using parameter_tuple_t = std::tuple<std::optional<std::remove_cv_t<std::remove_reference_t<PARAMETERS>>>...>;
	parameter_tuple_t m_stored_values;

	void throw_if_index_out_of_range(size_t index) const;

	template <size_t INDEX>
	void throw_if_no_value_stored_for_index() const;

	struct TruePredicate;

	template <size_t INDEX>
	struct BaseTypeAt;
	template <size_t INDEX>
	using BaseTypeAt_t = typename BaseTypeAt<INDEX>::type;

	template <typename TYPE>
	struct IsSettableFrom;

	template <typename TYPE>
	struct IsGettableAs;

	template <class CT_PREDICATE, typename RT_PREDICATE, typename FUNCTION>
	void pass_first_index_matching_predicate_to(const RT_PREDICATE &runtime_predicate, const FUNCTION &&func) const;

	auto ensure_name_matches(const std::string_view &name) const;
};


/////////////////////////////////////////////////////////////
//////////////////     ParameterMap     /////////////////////
//////////////////    Implementation    /////////////////////
/////////////////////////////////////////////////////////////

template <typename... PARAMETERS>
template <typename... PARAM_NAMES>
ParameterMap<PARAMETERS...>::ParameterMap(PARAM_NAMES &&... names) requires(sizeof...(PARAMETERS) ==
																																						sizeof...(PARAM_NAMES))
		: m_parameter_name_hashes{std::hash<std::string>{}(std::forward<PARAM_NAMES>(names))...} {}

template <typename... PARAMETERS>
template <typename T>
void ParameterMap<PARAMETERS...>::set(const std::string_view &name, T &&value) {
	pass_first_index_matching_predicate_to<IsSettableFrom<T>>(ensure_name_matches(name),
																														[&](auto i) { set<i.value>(value); });
}


template <typename... PARAMETERS>
template <typename T>
void ParameterMap<PARAMETERS...>::set(size_t index, T &&value) {
	throw_if_index_out_of_range(index);
	pass_first_index_matching_predicate_to<IsSettableFrom<T>>([&](auto i) { return i == index; },
																														[&](auto i) { set<i.value>(value); });
}

template <typename... PARAMETERS>
template <size_t INDEX, typename T>
void ParameterMap<PARAMETERS...>::set(T &&value) {
	std::get<INDEX>(m_stored_values) = std::forward<T>(value);
}

template <typename... PARAMETERS>
template <typename T>
[[nodiscard]] const std::remove_cv_t<std::remove_reference_t<T>> &ParameterMap<PARAMETERS...>::get(
		const std::string_view &name) const {
	const std::optional<std::remove_cv_t<std::remove_reference_t<T>>> *ret = nullptr;
	pass_first_index_matching_predicate_to<IsGettableAs<T>>(ensure_name_matches(name), [&](auto i) {
		throw_if_no_value_stored_for_index<i.value>();
		ret = &(std::get<i.value>(m_stored_values));
	});
	return ret->value();
}

template <typename... PARAMETERS>
template <typename T>
[[nodiscard]] const std::remove_cv_t<std::remove_reference_t<T>> &ParameterMap<PARAMETERS...>::get(size_t index) const {
	throw_if_index_out_of_range(index);
	const std::optional<std::remove_cv_t<std::remove_reference_t<T>>> *ret = nullptr;
	pass_first_index_matching_predicate_to<IsGettableAs<T>>([&](auto i) { return i == index; },
																													[&](auto i) {
																														throw_if_no_value_stored_for_index<i.value>();
																														ret = &(std::get<i.value>(m_stored_values));
																													});
	return ret->value();
}

template <typename... PARAMETERS>
template <size_t INDEX>
[[nodiscard]] auto ParameterMap<PARAMETERS...>::get() const requires(INDEX < sizeof...(PARAMETERS)) {
	throw_if_no_value_stored_for_index<INDEX>();
	return std::get<INDEX>(m_stored_values).value();
}

template <typename... PARAMETERS>
template <size_t INDEX>
[[nodiscard]] bool ParameterMap<PARAMETERS...>::is_set() const noexcept requires(INDEX < sizeof...(PARAMETERS)) {
	return std::get<INDEX>(m_stored_values).has_value();
}

template <typename... Parameters>
[[nodiscard]] bool ParameterMap<Parameters...>::is_set(const std::string_view &name) {
	bool ret = false;
	pass_first_index_matching_predicate_to<TruePredicate>(ensure_name_matches(name),
																												[&](auto i) { ret = is_set<i.value>(); });
	return ret;
}

template <typename... Parameters>
[[nodiscard]] bool ParameterMap<Parameters...>::is_set(size_t index) {
	bool ret = false;
	pass_first_index_matching_predicate_to<TruePredicate>([&](auto i) { return i == index; },
																												[&](auto i) { ret = is_set<i.value>(); });
	return ret;
}

template <typename... PARAMETERS>
void ParameterMap<PARAMETERS...>::clear() noexcept {
	detail::static_for<0, n_parameters>([&](auto i) { std::get<i.value>(m_stored_values).reset(); });
}

template <typename... PARAMETERS>
template <typename FUNCTION>
auto ParameterMap<PARAMETERS...>::submit(FUNCTION &&function) const
		requires(std::is_invocable_v<FUNCTION, PARAMETERS...>) {
	detail::static_for<0, n_parameters>([&](auto i) {
		if (!is_set<i.value>()) {
			throw std::runtime_error("Unable to call function: No stored value for parameter");
		}
	});
	return detail::apply_optionals(function, m_stored_values);
}

////////////////////// Private Members //////////////////////

template <typename... PARAMETERS>
void ParameterMap<PARAMETERS...>::throw_if_index_out_of_range(size_t index) const {
	if (index >= n_parameters) {
		throw std::out_of_range(std::string{"Index should be range [0 .. "} + std::to_string(n_parameters - 1) + "]");
	}
}

template <typename... PARAMETERS>
template <size_t INDEX>
void ParameterMap<PARAMETERS...>::throw_if_no_value_stored_for_index() const {
	if (!is_set<INDEX>()) {
		throw std::runtime_error("Parameter does not have a stored value");
	}
}

template <typename... PARAMETERS>
struct ParameterMap<PARAMETERS...>::TruePredicate {
	static constexpr auto value_for = [](auto) { return true; };
};

template <typename... PARAMETERS>
template <size_t INDEX>
struct ParameterMap<PARAMETERS...>::BaseTypeAt {
	using type =
			typename std::remove_cv_t<std::remove_reference_t<decltype(std::get<INDEX>(m_stored_values))>>::value_type;
};

template <typename... PARAMETERS>
template <typename TYPE>
struct ParameterMap<PARAMETERS...>::IsSettableFrom {
	static constexpr auto value_for = [](auto index) {
		return std::is_convertible_v<std::remove_cv_t<std::remove_reference_t<TYPE>>, BaseTypeAt_t<index.value>>;
	};
};

template <typename... PARAMETERS>
template <typename TYPE>
struct ParameterMap<PARAMETERS...>::IsGettableAs {
	static constexpr auto value_for = [](auto index) {
		return std::is_same_v<std::remove_cv_t<std::remove_reference_t<TYPE>>, BaseTypeAt_t<index.value>>;
	};
};


template <typename... PARAMETERS>
template <class CT_PREDICATE, typename RT_PREDICATE, typename FUNCTION>
void ParameterMap<PARAMETERS...>::pass_first_index_matching_predicate_to(const RT_PREDICATE &runtime_predicate,
																																				 const FUNCTION &&func) const {
	bool found = false;

	detail::static_for<0, n_parameters>([&](auto i) {
		if constexpr (CT_PREDICATE::value_for(i)) {
			if (!found && runtime_predicate(static_cast<size_t>(i.value))) {
				found = true;
				func(i);
			}
		}
	});
	if (!found) {
		throw std::invalid_argument("No parameters match the given input");
	}
}


template <typename... PARAMETERS>
auto ParameterMap<PARAMETERS...>::ensure_name_matches(const std::string_view &name) const {
	auto name_hash = std::hash<std::string_view>{}(name);
	return [&, name_hash](auto i) { return m_parameter_name_hashes.at(size_t(i)) == name_hash; };
}


/////////////////////////////////////////////////////////////
//////////////////      Utilities       /////////////////////
/////////////////////////////////////////////////////////////

namespace detail {
template <class F, class Tuple, std::size_t... I>
constexpr decltype(auto) apply_optionals_impl(F &&f, Tuple &&t, std::index_sequence<I...>) {
	return std::invoke(std::forward<F>(f), *std::get<I>(std::forward<Tuple>(t))...);
}

template <class F, class Tuple>
constexpr decltype(auto) apply_optionals(F &&f, Tuple &&t) {
	return apply_optionals_impl(std::forward<F>(f),
															std::forward<Tuple>(t),
															std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

template <int First, int Last, typename Lambda>
inline void static_for(Lambda const &f) {
	if constexpr (First < Last) {
		f(std::integral_constant<int, First>{});
		static_for<First + 1, Last>(f);
	}
}
}  // namespace detail


}  // namespace qbouts

#endif
