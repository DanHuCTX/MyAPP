#include "stdafx.h"
#include "..\Sensor\Utility.h"

class UtilityTest : public testing::Test
{
public:
	static void SetUpTestCase()
	{
	}
	
	static void TearDownTestCase()
	{

	}

	virtual void SetUp()
	{

	}

	virtual void TearDown()
	{
	}
};

TEST_F(UtilityTest, TestKeyExisted)
{

	EXPECT_NE(1,0);
}