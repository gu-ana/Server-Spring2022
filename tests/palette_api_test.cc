#include <string>
#include <vector>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "palette_api.h"

class PaletteAPITest : public ::testing::Test
{
	void SetUp()
	{
		palette = {};
		names = {};
	}

	protected:
		std::string color;
		std::vector<std::string> palette;
		std::vector<std::string> names;
};

// Test that word to color API works
TEST_F(PaletteAPITest, GetColorFromWord)
{
	color = get_color("Test");
	EXPECT_TRUE(color != "");
}

// Test that phrases work for word to color API
TEST_F(PaletteAPITest, GetColorFromPhrase)
{
	color = get_color("Test Phrase");
	EXPECT_TRUE(color != "");
}

// Test that color to palette API works
TEST_F(PaletteAPITest, GetPaletteFromColor)
{
	color = "93AB89";
	palette = get_palette(color);
	EXPECT_TRUE(palette.size() != 0);
}

// Test that palette to names API works
TEST_F(PaletteAPITest, GetNamesFromPalette)
{
	palette.push_back("#93AB89");
	palette.push_back("#93AB89");
	names = PaletteAPI::get_names(palette);
	EXPECT_TRUE(names.size() != 0);
}

// Test that PaletteAPI::get_names() returns an empty vector if input is empty vector
TEST_F(PaletteAPITest, EmptyNames)
{
	names = PaletteAPI::get_names(palette);
	EXPECT_TRUE(names.size() == 0);
}

// Test that PaletteAPI::get_colors() returns a non-empty vector
TEST_F(PaletteAPITest, GetColors)
{
	palette = PaletteAPI::get_colors("Test");
	EXPECT_TRUE(palette.size() != 0);
}