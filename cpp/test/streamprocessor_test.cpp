#include <gtest/gtest.h>
#include <sstream>
#include <algorithm>

#include "streamprocessor.hpp"

using StreamProcessorNewLineTest = ::testing::Test;
using StreamProcessorExtractWordTest = ::testing::Test;

TEST_F(StreamProcessorNewLineTest, NewLine) {
    std::string str = "line\nanother line";
    std::stringstream ss(str);

    ss.ignore(4);
    EXPECT_TRUE(StreamProcessor::NewLine(ss));
}

TEST_F(StreamProcessorNewLineTest, NewLineWithCarriageReturn) {
    std::string str = "line\r\nanother line";
    std::stringstream ss(str);

    ss.ignore(4);
    EXPECT_TRUE(StreamProcessor::NewLine(ss));
}

TEST_F(StreamProcessorNewLineTest, CarriageReturnWithoutNewLine) {
    std::string str = "line\ranother line";
    std::stringstream ss(str);

    ss.ignore(4);
    EXPECT_FALSE(StreamProcessor::NewLine(ss));
}

TEST_F(StreamProcessorNewLineTest, NoNewLine) {
    std::string str = "line more words";
    std::stringstream ss(str);

    ss.ignore(4);
    EXPECT_FALSE(StreamProcessor::NewLine(ss));
}

TEST_F(StreamProcessorNewLineTest, eof) {
    std::string str = "line";
    std::stringstream ss(str);

    ss.ignore(4);
    EXPECT_FALSE(StreamProcessor::NewLine(ss));
}

TEST_F(StreamProcessorExtractWordTest,
       3WordsSameLineWithSpacesSeparators) {
    std::string str = "first second third\n";
    std::stringstream ss(str);
    StreamProcessor processor(ss);

    std::stringstream ss_ref(str);
    std::string word;
    std::string word_ref;

    auto success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_FALSE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);
}

TEST_F(StreamProcessorExtractWordTest,
       3WordsSameLineWithMultipleSpacesSeparators) {
    std::string str = "first   second          third\n";
    std::stringstream ss(str);
    StreamProcessor processor(ss);

    std::stringstream ss_ref(str);
    std::string word;
    std::string word_ref;

    auto success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word);
    ss_ref >> word_ref;
    ASSERT_FALSE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);
}

TEST_F(StreamProcessorExtractWordTest,
       3WordsMultiLineWithMultipleSpacesSeparators) {
    std::string str = "first\n   second   \n      third\n";
    std::stringstream ss(str);
    StreamProcessor processor(ss);

    std::stringstream ss_ref(str);
    std::string word;
    std::string word_ref;

    auto success = processor.ExtractWord(word, true);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, true);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, true);
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, true);
    ss_ref >> word_ref;
    ASSERT_FALSE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);
}

TEST_F(StreamProcessorExtractWordTest,
       3WordsMultiLineWithColonSeparators) {
    std::string str = "first:second:third\n";
    std::stringstream ss(str);
    StreamProcessor processor(ss);

    std::string str_ref = str;
    std::replace(str_ref.begin(), str_ref.end(), ':', ' ');
    std::stringstream ss_ref(str_ref);
    std::string word;
    std::string word_ref;

    auto success = processor.ExtractWord(word, false, false, {':'});
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, false, false, {':'});
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, false, false, {':'});
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, false, false, {':'});
    ss_ref >> word_ref;
    ASSERT_FALSE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);
}

TEST_F(StreamProcessorExtractWordTest,
       3WordsSameLineWithInvalidChar) {
    std::string str = "first second third\n";
    std::stringstream ss(str);
    StreamProcessor processor(ss);

    std::string word;

    auto success = processor.ExtractWord(word, false,  false, {' '}, {' '}, {'r'});
    ASSERT_FALSE(success == StreamProcessor::Result::Success);
}

TEST_F(StreamProcessorExtractWordTest,
       3WordsSameLineWithoutInvalidChar) {
    std::string str = "first second third\n";
    std::stringstream ss(str);
    StreamProcessor processor(ss);

    std::stringstream ss_ref(str);
    std::string word;
    std::string word_ref;

    auto success = processor.ExtractWord(word, false,  false, {' '}, {' '}, {'a'});
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, false,  false, {' '}, {' '}, {'a'});
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, false,  false, {' '}, {' '}, {'a'});
    ss_ref >> word_ref;
    ASSERT_TRUE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);

    success = processor.ExtractWord(word, false,  false, {' '}, {' '}, {'a'});
    ss_ref >> word_ref;
    ASSERT_FALSE(success == StreamProcessor::Result::Success);
    EXPECT_EQ(word, word_ref);
}


