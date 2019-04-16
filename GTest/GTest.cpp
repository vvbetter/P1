// GTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "gtest/googletest/include/gtest.h"


TEST(AAA, a1)
{
	EXPECT_EQ(2, 1 + 1);
}
TEST(AAA, a2)
{
	EXPECT_EQ(2, 1 + 3);
}

int main(int argc,char** argv)
{
	testing::InitGoogleTest(&argc, argv);
    std::cout << "Hello World!\n"; 
	return RUN_ALL_TESTS();
}
