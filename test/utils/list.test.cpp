#include "../test.h"

#include "utils/list.h"

//------------------------------------------------------------------------------
// tests
//------------------------------------------------------------------------------
//
using namespace remo::utils;

class MyItem: public ListItem
{
};

typedef List<MyItem> MyList;

//------------------------------------------------------------------------------
//
TEST(List, empty)
{
	MyList list;
	
	EXPECT_EQ(list.get_count(), (size_t)0);
	EXPECT_EQ(list.get_first(), nullptr);
	EXPECT_EQ(list.get_last(), nullptr);
}



//------------------------------------------------------------------------------
//
TEST(List, push_front)
{
	MyList list;
	
	// add first
	MyItem item1;
	list.push_front(&item1);
	EXPECT_EQ(list.get_count(), (size_t)1);
	EXPECT_EQ(list.get_first(), &item1);
	EXPECT_EQ(list.get_last(), &item1);

	// add second
	MyItem item2;
	list.push_front(&item2);
	EXPECT_EQ(list.get_count(), (size_t)2);
	EXPECT_EQ(list.get_first(), &item2);
	EXPECT_EQ(list.get_last(), &item1);

	// check linkage
	EXPECT_EQ(item1.get_list(), &list);
	EXPECT_EQ(item1.get_prev(), &item2);
	EXPECT_EQ(item1.get_next(), nullptr);

	EXPECT_EQ(item2.get_list(), &list);
	EXPECT_EQ(item2.get_prev(), nullptr);
	EXPECT_EQ(item2.get_next(), &item1);
}

//------------------------------------------------------------------------------
//
TEST(List, push_back)
{
	MyList list;
	
	// add first
	MyItem item1;
	list.push_back(&item1);
	EXPECT_EQ(list.get_count(), (size_t)1);
	EXPECT_EQ(list.get_first(), &item1);
	EXPECT_EQ(list.get_last(), &item1);

	// add second
	MyItem item2;
	list.push_back(&item2);
	EXPECT_EQ(list.get_count(), (size_t)2);
	EXPECT_EQ(list.get_first(), &item1);
	EXPECT_EQ(list.get_last(), &item2);

	// check linkage
	EXPECT_EQ(item1.get_list(), &list);
	EXPECT_EQ(item1.get_prev(), nullptr);
	EXPECT_EQ(item1.get_next(), &item2);

	EXPECT_EQ(item2.get_list(), &list);
	EXPECT_EQ(item2.get_prev(), &item1);
	EXPECT_EQ(item2.get_next(), nullptr);
}

//------------------------------------------------------------------------------
//
TEST(List, remove_first)
{
	MyList list;
	
	MyItem item1;
	list.push_back(&item1);
	MyItem item2;
	list.push_back(&item2);
	MyItem item3;
	list.push_back(&item3);

	EXPECT_EQ(list.get_count(), (size_t)3);

	// remove first
	list.remove(&item1);
	EXPECT_EQ(list.get_count(), (size_t)2);
	EXPECT_EQ(list.get_first(), &item2);
	EXPECT_EQ(list.get_last(), &item3);

	// check linkage
	EXPECT_EQ(item1.get_list(), nullptr);
	EXPECT_EQ(item1.get_prev(), nullptr);
	EXPECT_EQ(item1.get_next(), nullptr);

	EXPECT_EQ(item2.get_list(), &list);
	EXPECT_EQ(item2.get_prev(), nullptr);
	EXPECT_EQ(item2.get_next(), &item3);

	EXPECT_EQ(item3.get_list(), &list);
	EXPECT_EQ(item3.get_prev(), &item2);
	EXPECT_EQ(item3.get_next(), nullptr);
}

//------------------------------------------------------------------------------
//
TEST(List, remove_middle)
{
	MyList list;
	
	MyItem item1;
	list.push_back(&item1);
	MyItem item2;
	list.push_back(&item2);
	MyItem item3;
	list.push_back(&item3);

	EXPECT_EQ(list.get_count(), (size_t)3);

	// remove middle
	list.remove(&item2);
	EXPECT_EQ(list.get_count(), (size_t)2);
	EXPECT_EQ(list.get_first(), &item1);
	EXPECT_EQ(list.get_last(), &item3);

	// check linkage
	EXPECT_EQ(item1.get_list(), &list);
	EXPECT_EQ(item1.get_prev(), nullptr);
	EXPECT_EQ(item1.get_next(), &item3);

	EXPECT_EQ(item2.get_list(), nullptr);
	EXPECT_EQ(item2.get_prev(), nullptr);
	EXPECT_EQ(item2.get_next(), nullptr);

	EXPECT_EQ(item3.get_list(), &list);
	EXPECT_EQ(item3.get_prev(), &item1);
	EXPECT_EQ(item3.get_next(), nullptr);
}

//------------------------------------------------------------------------------
//
TEST(List, remove_last)
{
	MyList list;
	
	MyItem item1;
	list.push_back(&item1);
	MyItem item2;
	list.push_back(&item2);
	MyItem item3;
	list.push_back(&item3);

	EXPECT_EQ(list.get_count(), (size_t)3);

	// remove last
	list.remove(&item3);
	EXPECT_EQ(list.get_count(), (size_t)2);
	EXPECT_EQ(list.get_first(), &item1);
	EXPECT_EQ(list.get_last(), &item2);

	// check linkage
	EXPECT_EQ(item1.get_list(), &list);
	EXPECT_EQ(item1.get_prev(), nullptr);
	EXPECT_EQ(item1.get_next(), &item2);

	EXPECT_EQ(item2.get_list(), &list);
	EXPECT_EQ(item2.get_prev(), &item1);
	EXPECT_EQ(item2.get_next(), nullptr);

	EXPECT_EQ(item3.get_list(), nullptr);
	EXPECT_EQ(item3.get_prev(), nullptr);
	EXPECT_EQ(item3.get_next(), nullptr);
}

//------------------------------------------------------------------------------
//
TEST(List, destroy_item)
{
	MyList list;

	{	
		MyItem item1;
		list.push_back(&item1);
		EXPECT_EQ(list.get_count(), (size_t)1);
	} // item1 will be destroyed when leaving this scope
	
	EXPECT_EQ(list.get_count(), (size_t)0);
}
