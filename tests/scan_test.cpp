#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, ParseInteger)
{
    std::string_view input = "42";
    std::string_view format = "{%d}";

    auto result = stdx::scan<int>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<0>(), 42);
}

TEST(ScanTest, ParseFloat)
{
    std::string_view input = "3.14";
    std::string_view format = "{%f}";

    auto result = stdx::scan<float>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(result.value().values<0>(), 3.14f);
}

TEST(ScanTest, ParseMultipleTypes)
{
    std::string_view input = "42 3.14 Hello";
    std::string_view format = "{%d} {%f} {%s}";

    auto result = stdx::scan<int, float, std::string>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<0>(), 42);
    EXPECT_FLOAT_EQ(result.value().values<1>(), 3.14f);
    EXPECT_EQ(result.value().values<2>(), "Hello");
}

TEST(ScanTest, InvalidFormat)
{
    std::string_view input = "abc";
    std::string_view format = "{%f}";

    auto result = stdx::scan<float>(input, format);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Invalid input: invalid argument");
}

TEST(ScanTest, EmptyInput)
{
    std::string_view input = "";
    std::string_view format = "{%d}";

    auto result = stdx::scan<int>(input, format);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Nothing parsed");
}

TEST(ScanTest, MismatchedFormat)
{
    std::string_view input = "3.14 42";
    std::string_view format = "{%d}";

    auto result = stdx::scan<int>(input, format);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message, "Invalid input: not a whole input transform");
}

TEST(ScanTest, ParseString)
{
    std::string_view input = "HelloWorld";
    std::string_view format = "{%s}";

    auto result = stdx::scan<std::string>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<std::string>(), "HelloWorld");
}

TEST(ScanTest, ParseStringWithSpaces)
{
    std::string_view input = "Hello World";
    std::string_view format = "{%s}";

    auto result = stdx::scan<std::string>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<0>(), "Hello World");
}

TEST(ScanTest, ParseStringWithSpacesTwo)
{
    std::string_view input = "Hello World";
    std::string_view format = "{%s} {}";

    auto result = stdx::scan<std::string, std::string>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<0>(), "Hello");
    EXPECT_EQ(result.value().values<1>(), "World");
}

TEST(ScanTest, ParseStringView)
{
    std::string_view input = "HelloView";
    std::string_view format = "{%s}";

    auto result = stdx::scan<std::string_view>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<0>(), "HelloView");
}

TEST(ScanTest, ParseStringViewTwo)
{
    std::string_view input = "HelloView";
    std::string_view format = "{}";

    auto result = stdx::scan<std::string_view>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<0>(), "HelloView");
}

TEST(ScanTest, ParseAndFindTypes)
{
    std::string_view input = "I want to sum 42 and 3.14 numbers.";
    std::string_view format = "I want to sum {} and {} numbers.";

    auto result = stdx::scan<int8_t, float>(input, format);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(typeid(decltype(result.value().values<0>())).name() == typeid(int8_t).name());
    ASSERT_TRUE(typeid(decltype(result.value().values<1>())).name() == typeid(float).name());
}

TEST(ScanTest, ParseFormatIntWithoutSpecPlaceholder)
{
    std::string_view input = "123 number";
    std::string_view format = "{} number";

    auto result = stdx::scan<int>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<int>(), 123);
}

TEST(ScanTest, ParseFormatIncorrect)
{
    std::string_view input = "number";
    std::string_view format = "{} number";

    auto result = stdx::scan<std::string>(input, format);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error().message, std::string("Unformatted text in input and format string are different"));

    format = "number {}";
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error().message, std::string("Unformatted text in input and format string are different"));
}

TEST(ScanTest, ParseFormatIncorrectTypesSec)
{
    std::string_view input = "number";
    std::string_view format = "{}";

    auto result = stdx::scan<std::string, float>(input, format);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error().message, std::string("Number of placeholders and input types are not equal"));
}

TEST(ScanTest, ParseFormatOverflowInt)
{
    std::string_view input = "12312321321321321312312321312213";
    std::string_view format = "{%d}";

    auto result = stdx::scan<int>(input, format);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error().message, std::string("Value out of range"));
}

TEST(ScanTest, ParseFormatWrongPlaceholder)
{
    std::string_view input = "12312321";
    std::string_view format = "{%e}";

    auto result = stdx::scan<int>(input, format);

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error().message, std::string("Unexpected format"));
}

TEST(ScanTest, ParseFormatNegativeNumbers)
{
    std::string_view input = "-1 -100500 -3.14";
    std::string_view format = "{%d} {%d} {%f}";

    auto result = stdx::scan<int8_t, int32_t, double>(input, format);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().values<int8_t>(), -1);
    EXPECT_EQ(result.value().values<int32_t>(), -100500);
    EXPECT_FLOAT_EQ(result.value().values<double>(), -3.14);
}

TEST(ScanTest, ParseFormatWrongSign)
{
    std::string_view input = "-1 -100500 -3.14";
    std::string_view format = "{%u} {%u} -3.14";

    auto result = stdx::scan<uint8_t, uint32_t>(input, format);
    
    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error().message, std::string("Invalid input: invalid argument"));
}
