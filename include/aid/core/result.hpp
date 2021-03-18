#ifndef AID_INCLUDE_AID_CORE_RESULT_HPP
#define AID_INCLUDE_AID_CORE_RESULT_HPP

#include <iostream>
#include <optional>

namespace aid::core
{

template<typename Ok, typename Err>
class Result;

template<typename Ok, typename Err>
auto ok(Ok &&value) -> Result<Ok, Err>;

template<typename Ok, typename Err>
auto err(Err &&value) -> Result<Ok, Err>;

/**
 * Result that represent either a success (Ok) or a failure (Err).
 *
 *
 * @tparam Ok success value type
 * @tparam Err error value type
 */
template<typename Ok, typename Err>
class Result
{
public:
    using OkType [[maybe_unused]] = Ok;
    using ErrType [[maybe_unused]] = Err;

    /**
     * Construct a result from an optional value or error.
     *
     * The result constructor will exit the application if both ok and err are empty or if both have a value.
     * To avoid this happening, helper functions (@see ::aid::ok() and @see ::aid::err()) have been defined.
     *
     * @param ok optional Ok value
     * @param err optional error value
     */
    explicit Result(std::optional<Ok> ok, std::optional<Err> err)
    {
        auto has_ok = ok.has_value();
        auto has_err = err.has_value();
        if (has_ok && has_err) {
            std::clog << "Trying to construct a Result with both an Ok and Err value." << std::endl;
            std::exit(1);
        }
        if (!has_ok && !has_err) {
            std::clog << "Trying to construct a Result without an Ok or an Err value." << std::endl;
            std::exit(1);
        }
        Oke = ok;
        Erre = err;
    }

    Result(const Result &) = delete;

    /**
     * Move the data from another Result into a new instance.
     * @param other result being moved
     */
    Result(Result &&other) noexcept
    {
        *this = std::move(other);
    }

    Result &operator=(const Result &) = delete;

    /**
     * Move the data from another Result into this instance.
     * @param other result being moved
     * @return Result
     */
    Result &operator=(Result &&other) noexcept
    {
        if (this != &other) {
            Oke = std::move(other.Oke);
            Erre = std::move(other.Erre);
        }
        return *this;
    }

    bool operator==(const Result &rhs) const
    {
        return Oke == rhs.Oke && Erre == rhs.Erre;
    }

    bool operator!=(const Result &rhs) const
    {
        return rhs != *this;
    }

    /**
     * Check if a value i stored inside the Result.
     * @return True if a value is stored inside the Result.
     */
    [[nodiscard]] bool isOk() const
    {
        return Oke.has_value();
    }
    /**
     * Check if an error is stored inside the Result.
     * @return True if an error is stored inside the Result.
     */
    [[nodiscard]] bool isErr() const
    {
        return Erre.has_value();
    }

    /**
     * Maps a Result<Ok, Err> to Result<Func(Ok&&), Err> by applying a function f to the contained Ok value.
     *
     * Because data is moved internally, the behaviour of the Result after applying this function is undefined.
     *
     * If an Err is stored, it is moved and returned into the new Result.
     *
     * If no value or error is stored in the Result, the function exits the application.
     * This indicates something is seriously wrong with the state of this object.
     * @tparam Func Function type being applied to the stored Ok value.
     * @param f function being applied to the stored Ok value
     * @return A new Result with the result of the applied function f or an error if that was initially present.
     */
    template<typename Func>
    [[nodiscard]] auto map(Func &&F) -> Result<std::result_of_t<Func && (Ok &&)>, Err>
    {

        if (isOk()) {
            return Result<std::result_of_t<Func && (Ok &&)>, Err>(std::move(F(std::move(*Oke))), {});
        }
        else if (isErr()) {
            return Result<std::result_of_t<Func && (Ok &&)>, Err>({}, std::move(*Erre));
        }
        else {
            std::clog << "Trying to apply a map function to a Result that has no value and no error." << std::endl;
            std::exit(1);
        }
    }

    /**
     * Maps a Result<Ok, Err> to Result<Ok, Func(Err&&)> by applying a function F to the contained Err value.
     *
     * Because data is moved internally, the behaviour of the Result after applying this function is undefined.
     *
     * If an Ok value is stored, it is moved and returned into the new Result.
     *
     * If no value or error is stored in the Result, the function exits the application.
     * This indicates something is seriously wrong with the state of this object.
     * @tparam Func Function type being applied to the stored Err value.
     * @param F function being applied to the stored Err value
     * @return A new Result with either the value or the result of the applied function F on the initial error.
     */
    template<typename Func>
    [[nodiscard]] auto mapErr(Func &&F) -> Result<Ok, std::result_of_t<Func && (Err &&)>>
    {
        if (isOk()) {
            return Result<Ok, std::result_of_t<Func && (Err &&)>>{std::move(*Oke), {}};
        }
        else if (isErr()) {
            return Result<Ok, std::result_of_t<Func && (Err &&)>>{{}, std::move(F(std::move(*Erre)))};
        }
        else {
            std::clog << "Trying to apply a mapErr function to a Result that has no value and no error." << std::endl;
            std::exit(1);
        }
    }

    /**
     * Map a Result<Ok, Err> to U by applying a function F to the contained Ok value. If Result stores an
     * Err, then the default value is returned.
     *
     * Because data is moved internally, the behaviour of the Result after applying this function is undefined.
     *
     * @tparam U Type of the default value in case in an error is stored or the return type of F in case an Ok value is stored.
     * @tparam Func Function type being applied to the stored Ok value.
     * @param Default Value returned if an Err is stored in the Result
     * @param F function being applied to the stored Ok value
     * @return The default value if an Err is stored in the Result or the return value of F if an Ok value is stored in Result.
     */
    template<typename U, typename Func>
    [[nodiscard]] auto mapOr(U &&Default, Func &&F) -> U
    {
        if (isOk()) {
            return std::move(F(std::move(*Oke)));
        }
        return std::forward<U>(Default);
    }

    /**
     * Map a Result<Ok, Err> to U. U is the return type of Func and of DefaultFunc.
     *
     * If this Result stores an Ok value, function F transforms the input into type U.
     * In case this Result stores an Err value, function Df transforms the input into type U.
     *
     * Because data is moved internally, the behaviour of the Result after applying this function is undefined.
     *
     * @tparam DefaultFunc Function type being applied to the stored Err value if the result is not Ok
     * @tparam Func Function type being applied to the stored Ok value
     * @param Df Default function being applied to an Err if this result is not Ok
     * @param F Function being applied to the stored Ok value
     * @return Transformed result of either F or Df respective on the Ok or Err value.
     */
    template<typename DefaultFunc, typename Func,
             typename std::enable_if<
                 std::is_same<
                     std::result_of_t<DefaultFunc && (Err &&)>,
                     std::result_of_t<Func && (Ok &&)>>::value,
                 bool>::type = true>
    [[nodiscard]] auto mapOrElse(DefaultFunc &&Df, Func &&F) -> std::result_of_t<Func && (Ok &&)>
    {
        if (isOk()) {
            return std::move(F(std::move(*Oke)));
        }
        return std::move(Df(std::move(*Erre)));
    }

    /**
     * Return Res if the Result is Ok, otherwise return the Err value of this instance.
     *
     * Because data is moved internally, the behaviour of the Result after applying this function is undefined.
     *
     * @tparam U Ok type of Res and the return type
     * @param Res Result that is returned if this instance is Ok
     * @return Result is Res if this instance is Ok or the Err value of this instance if not.
     */
    template<typename U>
    [[nodiscard]] auto and_(Result<U, Err> &&Res) -> Result<U, Err>
    {
        if (isOk()) {
            return std::move(Res);
        }
        return ::aid::core::err<U, Err>(std::move(*Erre));
    }

    /**
     * Call F if this Result is Ok, otherwise return the Err value of this Result.
     *
     * Because data is moved internally, the behaviour of the Result after applying this function is undefined.
     *
     * @tparam Func Function type being applied to the stored Ok value
     * @tparam R Return type of F with a possible different Ok type, with an idential Err Type as this instance.
     * @param F Function applied to a stored Ok value
     * @return If this Result stores an Ok value, F is applied to that value, otherwise Err of this instance is returned.
     */
    template<typename Func, typename R = std::result_of_t<Func && (Ok &&)>>
    [[nodiscard]] auto andThen(Func &&F) -> R
    {
        if (isOk()) {
            return F(std::move(*Oke));
        }
        return ::aid::core::err<Ok, typename R::ErrType>(std::move(*Erre));
    }

    /**
     * Returns true if the Result contains an Ok value that is equal to x.
     * @tparam U Type of argument
     * @param x check if this value is stored inside the Result
     * @return True if the Result contains an Ok value that is equal to x.
     */
    template<typename U>
    [[nodiscard]] auto contains(const U &x) -> bool
    {
        if (isOk()) {
            auto value = Oke.value();
            if (value == x) return true;
        }
        return false;
    }

    /**
     * Return true if the Result contains an Err value that is equal to x.
     * @tparam U Type of the argument
     * @param x check if this value is stored inside the Result
     * @return True if the Result contains an Err value that is equal to x.
     */
    template<typename U>
    [[nodiscard]] auto containsErr(const U &x) -> bool
    {
        if (isErr()) {
            auto value = Erre.value();
            if (value == x) return true;
        }
        return false;
    }

    /**
     * Return Res if the result is Err, otherwise return the Ok value in this Result.
     *
     * Function is called ore because or is a C++ keyword.
     *
     * @tparam U Err type of res and the return value.
     * @param Res other Result returned if this Result has an Err value
     * @return A result with either the Ok value of this Result or Res in case of an Err.
     */
    template<typename U>
    [[nodiscard]] auto ore(Result<Ok, U> &&Res) -> Result<Ok, U>
    {
        if (isOk()) {
            return ok<Ok, U>(std::move(*Oke));
        }
        return std::move(Res);
    }

    template<typename Func, typename R = std::result_of_t<Func && (Err &&)>>
    [[nodiscard]] auto orElse(Func &&F) -> R
    {
        if (isOk()) {
            //            using R = std::result_of_t<Func&&(Err&&)>;
            return R(std::move(*Oke), {});
        }
        return std::move(F(std::move(*Erre)));
    }

    /**
     * Get the contained Ok value, consuming itself.
     *
     * The internal Ok value is set to an empty optional.
     * The function exits the application if no Ok value is stored in the Result.
     * @return Ok value
     */
    Ok value()
    {
        if (!Oke.has_value()) {
            std::clog << "Trying to get the value of a result which doesn't have a value" << std::endl;
            std::exit(1);
        }
        // move the internal value into a temporary.
        auto tmp = std::move(*Oke);
        // reinitialise the internal ok value to an empty optional.
        Oke = std::optional<Ok>{};
        return std::move(tmp);
    }

    /**
     * Get the contained Ok value, if it exists, otherwise return the default value.
     *
     * The internal Ok value is set to an empty optional.
     * @param Default default provided value if no Ok value is present.
     * @return Returns the Ok value if it exists, otherwise return the default value.
     */
    Ok valueOr(Ok Default)
    {
        if (!Oke.has_value()) {
            return Default;
        }
        // move the internal value into a temporary.
        auto tmp = std::move(*Oke);
        // reinitialise the internal ok value to an empty optional.
        Oke = std::optional<Ok>{};
        return std::move(tmp);
    }

    /**
     * Returns an Ok value stored inside the Result, otherwise log a message and exit.
     * @param msg passed message in case no Ok value is stored in the Result
     * @return Returns an Ok value stored inside the Result.
     */
    auto expect(const char *msg) -> Ok
    {
        if (isOk()) {
            return std::move(*Oke);
        }
        std::clog << msg << std::endl;
        std::exit(1);
    }

    /**
     * Returns an Err value stored inside the Result, otherwise log a message and exit.
     * @param msg passed message in case no Err value is stored in the Result
     * @return Returns an Err value stored inside the Result.
     */
    auto expectErr(const char *msg) -> Ok
    {
        if (isErr()) {
            return std::move(*Erre);
        }
        std::clog << msg << std::endl;
        std::exit(1);
    }

    /**
     * Get the contained Err value, consuming itself.
     *
     * The internal Err value is set to an empty optional.
     * The function exits the application if no Err value is stored in the Result.
     * @return Ok value
     */
    Err err()
    {
        if (!Erre.has_value()) {
            std::clog << "Trying to get the error of a result which doesn't have an error" << std::endl;
            std::exit(1);
        }
        // move the internal value into a temporary.
        auto tmp = std::move(*Erre);
        // reinitialise the internal err value to an empty optional.
        Erre = std::optional<Err>{};
        return std::move(tmp);
    }

    /**
     * Get the contained Err value, if it exists, otherwise return the default value.
     *
     * The internal Err value is set to an empty optional.
     * @param default_value default provided value if no Err value is present.
     * @return Returns the Err value if it exists, otherwise return the default value.
     */
    Err errOr(Err default_value)
    {
        if (!Erre.has_value()) {
            return default_value;
        }
        // move the internal value into a temporary.
        auto tmp = std::move(*Erre);
        // reinitialise the internal err value to an empty optional.
        Erre = std::optional<Err>{};
        return std::move(tmp);
    }

    template<typename T, typename E>
    friend Result<T, E> ok(T &&value);

    template<typename T, typename E>
    friend Result<T, E> err(T &&value);

    template<typename O, typename E>
    friend std::ostream &operator<<(std::ostream &os, const Result<O, E> &r);


private:
    std::optional<Ok> Oke;
    std::optional<Err> Erre;
};

/**
 * Create an Oke result with a value.
 * @tparam Ok Value type of the Result
 * @tparam Err Erre type of the Result
 * @param value data being moved inside the Result.
 * @return Result instance with a value.
 */
template<typename Ok, typename Err>
auto ok(Ok &&value) -> Result<Ok, Err>
{
    //    return Result<Oke, Erre>{std::optional{std::forward<Oke>(value)}, std::optional<Erre>()};
    return Result<Ok, Err>{std::optional{value}, std::optional<Err>{}};
}

/**
 * Create an Err result with an error.
 * @tparam Ok Value type of the Result
 * @tparam Err Err type of the Result
 * @param value error being moved inside the Result.
 * @return Result instance with an error.
 */
template<typename Ok, typename Err>
auto err(Err &&value) -> Result<Ok, Err>
{
    return Result<Ok, Err>{std::optional<Ok>{}, std::optional<Err>{value}};
}

template<typename O, typename E>
std::ostream &operator<<(std::ostream &os, const Result<O, E> &r)
{
    if (r.isOk()) {
        os << "Ok{";
        os << r.Oke.value();
        os << "}";
    }
    if (r.isErr()) {
        os << "Err{";
        os << r.Erre.value();
        os << "}";
    }
    return os;
}

}// namespace aid

#endif//AID_INCLUDE_AID_CORE_RESULT_HPP
