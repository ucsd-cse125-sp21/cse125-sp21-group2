#include "msd/blocking_iterator.hpp"

#include "gtest/gtest.h"
#include "msd/channel.hpp"

TEST(ChannelIteratorTest, Dereference)
{
    msd::channel<int> channel;
    msd::blocking_iterator<msd::channel<int>> it(channel);

    int in = 1;
    in >> channel;
    in = 2;
    in >> channel;

    EXPECT_EQ(1, *it);
    EXPECT_EQ(2, *it);
}

TEST(ChannelIteratorTest, NotEqualStop)
{
    msd::channel<int> channel;
    msd::blocking_iterator<msd::channel<int>> it(channel);

    channel.close();

    EXPECT_FALSE(it != it);
}

TEST(ChannelIteratorTest, NotEqualContinue)
{
    msd::channel<int> channel;
    msd::blocking_iterator<msd::channel<int>> it(channel);

    1 >> channel;

    EXPECT_TRUE(it != it);
}
