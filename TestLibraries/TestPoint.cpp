#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestLibraries
{		
	TEST_CLASS(TestPoint)
	{
	public:
		
		TEST_METHOD(testPointDefaultConstructor_shouldCreateObject)
		{
			Point* point=new Point();
			Assert::IsTrue(point);
			delete point;
			Point point1;
		}

		TEST_METHOD(testPointConstructorWithValue_shouldGetRightValues_1)
		{
			Point point(3, 2);
			Assert::IsTrue(point.x()==3);
			Assert::IsTrue(point.y()==2);
		}

		TEST_METHOD(testPointConstructorWithValue_shouldGetRightValues_2)
		{
			Point point(5, 3);
			Assert::IsTrue(point.x()==5);
			Assert::IsTrue(point.y()==3);
		}

		TEST_METHOD(testPointOperatorEqual_shouldGetRightValues_1)
		{
			Point point(5, 3);
			Point point2;
			point2=point;
			Assert::IsTrue(point2.x()==point.x() && point2.x()==5);
			Assert::IsTrue(point2.y()==point.y() && point2.y()==3);
		}

		TEST_METHOD(testPointOperatorEqual_shouldGetRightValues_2)
		{
			Point point(3, 2);
			Point point2;
			point2=point;
			Assert::IsTrue(point2.x()==point.x() && point2.x()==3);
			Assert::IsTrue(point2.y()==point.y() && point2.y()==2);
		}

		TEST_METHOD(testPointConstructorWithCopy_shouldGetRightValues_1)
		{
			Point point(5, 3);
			Point point2(point);
			Assert::IsTrue(point2.x()==point.x() && point2.x()==5);
			Assert::IsTrue(point2.y()==point.y() && point2.y()==3);
		}

		TEST_METHOD(testPointConstructorWithCopy_shouldGetRightValues_2)
		{
			Point point(6, 0);
			Point point2(point);
			Assert::IsTrue(point2.x()==point.x() && point2.x()==6);
			Assert::IsTrue(point2.y()==point.y() && point2.y()==0);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnTrueWhenYes)
		{
			Point point(6, 0);
			Point point2(point);
			Assert::IsTrue(point==point2);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnFalseWhenNot_1)
		{
			Point point(6, 0);
			Point point2(6, 1);
			Assert::IsFalse(point==point2);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnFalseWhenNot_2)
		{
			Point point(6, 0);
			Point point2(5, 3);
			Assert::IsFalse(point==point2);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnFalseWhenNot_3)
		{
			Point point(6, 0);
			Point point2(4, 0);
			Assert::IsFalse(point==point2);
		}

		TEST_METHOD(testPointAreDifferent_shouldReturnFalseWhenNot)
		{
			Point point(6, 0);
			Point point2(point);
			Assert::IsFalse(point!=point2);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnTrueWhenYes_1)
		{
			Point point(6, 0);
			Point point2(6, 1);
			Assert::IsTrue(point!=point2);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnTrueWhenYes_2)
		{
			Point point(6, 0);
			Point point2(5, 3);
			Assert::IsTrue(point!=point2);
		}

		TEST_METHOD(testPointAreEqual_shouldReturnTrueWhenYes_3)
		{
			Point point(6, 0);
			Point point2(4, 0);
			Assert::IsTrue(point!=point2);
		}

	};
}