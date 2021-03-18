#include <aid/core/result.hpp>
#include <catch2/catch.hpp>
#include <iostream>

using namespace aid::core;

TEST_CASE("Construct a Ok Result", "[Result]")
{
    auto res1 = ok<int, std::string>(5);
    auto v1 = std::vector{5, 3, 2};
    auto res2 = ok<std::vector<int>, std::string>(std::move(v1));
    REQUIRE(res1.isOk() == true);
    REQUIRE(res1.isErr() == false);
    REQUIRE(res2.isOk() == true);
    REQUIRE(res2.isErr() == false);
}

double convert(int &&i)
{
    int j = i;
    return 2.5 * (double) j;
}

TEST_CASE("Map a Result", "[Result]")
{
    auto res1 = ok<int, std::string>(5);
    Result<double, std::string> res2 = res1.map([](auto &&val) {int j=val; return j * 2.0; });
    REQUIRE(res2.isOk() == true);
    REQUIRE(res2.value() == 10.0);
    auto res3 = err<int, std::string>("error");
    Result<int, bool> res4 = res3.mapErr([](std::string &&val) {
        std::string tmp{std::move(val)};
        if (tmp == "error") {
            return true;
        }
        else {
            return false;
        }
    });
    REQUIRE(res4.isErr());
    REQUIRE(res4.err());
    double res5 = ok<int, std::string>(5).mapOr(10, [](auto &&val) { return std::forward<decltype(val)>(val) * 3; });
    REQUIRE(res5 == 15.0);
    double res6 = err<int, std::string>("error").mapOr(10, [](auto &&val) { return std::forward<decltype(val)>(val) * 3; });
    REQUIRE(res6 == 10.0);
    double res7 = ok<int, std::string>(5).mapOrElse(
        [](auto &&val) {
            auto s = std::forward<decltype(val)>(val);
            if (s == "error") return 1.0;
            else
                return 2.0;
        },
        [](auto &&val) {
            return std::forward<decltype(val)>(val) * 3.0;
        });
    REQUIRE(res7 == 15.0);
    double res8 = err<int, std::string>("error").mapOrElse(
        [](auto &&val) {
            auto s = std::forward<decltype(val)>(val);
            if (s == "error") return 1.0;
            else
                return 2.0;
        },
        [](auto &&val) {
            return std::forward<decltype(val)>(val) * 3.0;
        });
    REQUIRE(res8 == 1.0);
    double res9 = err<int, std::string>("errors").mapOrElse(
        [](auto &&val) {
            auto s = std::forward<decltype(val)>(val);
            if (s == "error") return 1.0;
            else
                return 2.0;
        },
        [](auto &&val) {
            return std::forward<decltype(val)>(val) * 3.0;
        });
    REQUIRE(res9 == 2.0);
}

TEST_CASE("Value of a Result", "[Result]")
{
    auto res1 = ok<int, std::string>(5);
    REQUIRE(res1.value() == 5.0);
    auto res2 = err<int, std::string>("error");
    REQUIRE(res2.valueOr(10) == 10);
    auto res3 = err<int, std::string>("error");
    REQUIRE(res3.err() == std::string{"error"});
    auto res4 = ok<int, std::string>(10);
    REQUIRE(res4.errOr("not equal") == std::string{"not equal"});
}

Result<int, int> sq(int &&x) { return ok<int, int>(x * x); }
Result<int, int> er(int &&x)
{
    int y = x;
    return err<int, int>(std::move(y));// NOLINT(performance-move-const-arg)
}


TEST_CASE("Or else of a Result", "[Result]")
{
    using R = Result<int, int>;
    using O = std::optional<int>;
    REQUIRE(ok<int, int>(2).orElse(sq).orElse(sq) == R{{2}, {}});
    REQUIRE(ok<int, int>(2).orElse(er).orElse(sq) == R{{2}, {}});
    REQUIRE(err<int, int>(3).orElse(sq).orElse(er) == R{{9}, {}});
    REQUIRE(err<int, int>(3).orElse(er).orElse(er) == R{{}, {3}});
}

TEST_CASE("And of a Result", "[Result")
{
    {
        auto x = ok<int, std::string>(2);
        auto y = err<std::string, std::string>({"late error"});
        auto exp = err<std::string, std::string>({"late error"});
        REQUIRE(x.and_(std::move(y)) == exp);
    }
    {
        auto x = err<int, std::string>("early error");
        auto y = ok<std::string, std::string>("foo");
        auto exp = err<std::string, std::string>("early error");
        REQUIRE(x.and_(std::move(y)) == exp);
    }
    {
        auto x = err<int, std::string>("not a 2");
        auto y = err<std::string, std::string>("late error");
        auto exp = err<std::string, std::string>("not a 2");
        REQUIRE(x.and_(std::move(y)) == exp);
    }
    {
        auto x = ok<int, std::string>(2);
        auto y = err<std::string, std::string>("different result type");
        auto exp = err<std::string, std::string>("different result type");
        REQUIRE(x.and_(std::move(y)) == exp);
    }
}

TEST_CASE("AndThen of a Result", "[Result]")
{
    REQUIRE(ok<int, int>(2).andThen(sq).andThen(sq) == ok<int, int>(16));
    REQUIRE(ok<int, int>(2).andThen(sq).andThen(er) == err<int, int>(4));
    REQUIRE(ok<int, int>(2).andThen(er).andThen(sq) == err<int, int>(2));
    REQUIRE(err<int, int>(3).andThen(sq).andThen(sq) == err<int, int>(3));
}

TEST_CASE("Contains in a Result", "[Result]") {
    REQUIRE(ok<int, std::string>(5).contains(5));
    REQUIRE(err<int, std::string>("error").containsErr(std::string{"error"}));
}

TEST_CASE("Expect a value or error in a Result", "[Result]") {
    REQUIRE(ok<int, int>(3).expect("not printed") == 3);
    REQUIRE(err<int, int>(3).expectErr("not printed") == 3);
}