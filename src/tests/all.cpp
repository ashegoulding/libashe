#include <gtest/gtest.h>

#include "../libashe/StringUtils.hpp"
#include "../libashe/helper.hpp"


TEST(StringUtils, joinstr)
{
    std::vector<std::string> v;

    v = {""};
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ""), "");
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ", "), "");

    v = {"", "", ""};
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ""), "");
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ", "), ", , ");

    v = {"A"};
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ""), "A");
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ", "), "A");

    v = {"1", "2"};
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ""), "12");
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ", "), "1, 2");

    v = {"1", "2", "3"};
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ""), "123");
    EXPECT_EQ(ashe::join_str(v.begin(), v.end(), ", "), "1, 2, 3");
}

TEST(StringUtils, trim)
{
    // Without locale
    EXPECT_EQ("", ashe::trim(""));
    EXPECT_EQ("", ashe::trim("  \t \r \n"));
    EXPECT_EQ("abc", ashe::trim("abc"));
    EXPECT_EQ("abc", ashe::trim("\t\n\r abc\t\n\r "));
    EXPECT_EQ("abc", ashe::trim("\t\n\r abc"));
    EXPECT_EQ("abc", ashe::trim("abc\t\n\r "));

    EXPECT_EQ(L"", ashe::trim(L""));
    EXPECT_EQ(L"", ashe::trim(L"  \t \r \n"));
    EXPECT_EQ(L"abc", ashe::trim(L"abc"));
    EXPECT_EQ(L"abc", ashe::trim(L"\t\n\r abc\t\n\r "));
    EXPECT_EQ(L"abc", ashe::trim(L"\t\n\r abc"));
    EXPECT_EQ(L"abc", ashe::trim(L"abc\t\n\r "));

    {
        // Unicode spaces
        static const wchar_t *SPACES[] = {
            L"\u0020", L"\u1680", L"\u2000", L"\u2001", L"\u2002",
            L"\u2003", L"\u2004", L"\u2005", L"\u2006", L"\u2008",
            L"\u2009", L"\u200A", L"\u205F", L"\u3000"};
        std::locale lc("nb_NO");

        for (const auto &v : SPACES) {
            EXPECT_EQ(L"", ashe::trim(v, lc));
        }
    }
}

TEST(StringUtils, cases)
{
    {
        // Without locale
        EXPECT_EQ("abcdefghijklmnopqrstuvwxyz",
                  ashe::lower("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
        EXPECT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                  ashe::upper("abcdefghijklmnopqrstuvwxyz"));
        EXPECT_EQ(L"abcdefghijklmnopqrstuvwxyz",
                  ashe::lower(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
        EXPECT_EQ(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                  ashe::upper(L"abcdefghijklmnopqrstuvwxyz"));
    }
    {
        // With locale
        std::locale lc("nb_NO");

        EXPECT_EQ(L"abcdefghijklmnopqrstuvwxyzåäøöæ",
                  ashe::lower(L"ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄØÖÆ", lc));
        EXPECT_EQ(L"ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄØÖÆ",
                  ashe::upper(L"abcdefghijklmnopqrstuvwxyzåäøöæ", lc));
    }
}

int main(int argc, char **argv)
{
    static const std::vector<int32_t> FLAGS = {
        (int32_t)ashe::LAsheInitFlag::OSCODE, ASHE_HOST_OSCODE,
        (int32_t)ashe::LAsheInitFlag::ALL_ABILITIES, 1};
    int ret;

    ::testing::InitGoogleTest(&argc, argv);
    ashe::init_LAshe(FLAGS.data(), FLAGS.size(), nullptr, 0);

    ret = RUN_ALL_TESTS();

    ashe::deinit_LAshe();

    return ret;
}
