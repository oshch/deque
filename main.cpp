#include "gtest/gtest.h"
#include "deque.hpp"

#include <iostream>
#include <algorithm>
#include <deque>
#include <vector>
#include <random>
#include <climits>

using std::cerr;

std::default_random_engine generator(501);

class DequeTest: public ::testing::TestWithParam<size_t>{};

int getNumber(){
	std::uniform_int_distribution<int> Number(INT_MIN, INT_MAX);
	return Number(generator);
}

size_t getIndex(size_t left, size_t right){
	std::uniform_int_distribution<size_t> Index(left, right - 1);
	return Index(generator);
}


void getPop(Deque<int> &deque, std::deque<int> &std_deque, int &my_value, int &std_value) {
	if (std_deque.empty())
		return;

	if (getIndex(0, 2)) {
		my_value = deque.front();
		std_value = std_deque.front();
		deque.popFront();
		std_deque.pop_front();
	} else {
		my_value = deque.back();
		std_value = std_deque.back();
		deque.popBack();
		std_deque.pop_back();
	}
}

void getPush(Deque<int> &deque, std::deque<int> &std_deque, int value){
	if (getIndex(0, 2)){
		deque.pushBack(value);
		std_deque.push_back(value);
	} else {
		deque.pushFront(value);
		std_deque.push_front(value);
	}
}

TEST_P(DequeTest, TestOfPushAndPop){
	Deque<int> deque;
	std::deque<int> std_deque;

	size_t test_size  = GetParam();

	for (size_t i = 0; i < test_size; ++i){
		getPush(deque, std_deque, getNumber());

		ASSERT_EQ(deque.size(), std_deque.size());
	}

	ASSERT_TRUE(std::equal(deque.begin(), deque.end(), std_deque.begin()));

	for (size_t i = 0; i < test_size; ++i){
		int my_value(1), std_value(0);

		getPop(deque, std_deque, my_value, std_value);

		ASSERT_EQ(deque.size(), std_deque.size());
		ASSERT_EQ(my_value, std_value);
	}

	ASSERT_EQ(deque.empty(), std_deque.empty());
}

enum TypeOfEvent {
	CHECK_VALUE,
	CHANGE_VALUE,
	PUSH,
	POP,
	BACK,
	FRONT,
	OPERATIONS_COUNT
};

struct Event {
	TypeOfEvent type;
	size_t left, right, index;
	int value;
};

Event getEvent(std::default_random_engine &event_generator, size_t length){
	std::uniform_int_distribution<size_t> Events(0, OPERATIONS_COUNT - 1);
	Event event;

	event.type = static_cast<TypeOfEvent>(Events(event_generator));
	event.index = getIndex(0, length);
	event.left = getIndex(0, length -1);
	event.right = getIndex(event.left, length);
	event.value = getNumber();

	return event;
}

void generateStartValues(size_t length, Deque<int> &deque, std::deque<int> &std_deque){
	for (size_t i = 0; i < length; ++i)
		getPush(deque, std_deque, getNumber());
}

std::vector<Event> generateEvents(std::default_random_engine &event_generator, size_t number_of_event, size_t length){
	std::vector<Event> events;

	for (size_t i = 0; i < number_of_event; ++i){
		events.push_back(getEvent(event_generator, length));
		if (events[i].type == PUSH)
			length++;
		if (events[i].type == POP && length != 0)
			length--;
	}

	return events;
}

TEST_P(DequeTest, TestManyDifferentEvents){
	Deque<int> deque;
	std::deque<int> std_deque;

	const size_t start_length = 1000;
	generateStartValues(start_length, deque, std_deque);

	size_t number_of_events = GetParam();

	std::default_random_engine event_generator(number_of_events);

	std::vector<Event> events = generateEvents(event_generator, number_of_events, start_length);

    for (auto event: events){
		int my_value(0), std_value(1);
		if (event.type == CHECK_VALUE)
            ASSERT_EQ(deque[event.index], std_deque[event.index]);
        if (event.type == CHANGE_VALUE)
            deque[event.index] = std_deque[event.index] = event.value;
        if (event.type == PUSH)
            getPush(deque, std_deque, event.value);
        if (event.type == POP)
            getPop(deque, std_deque, my_value, std_value);
        if ((event.type == BACK) && (std_deque.size() != 0))
        	ASSERT_EQ(deque.back(), std_deque.back());
        if ((event.type == FRONT) && (std_deque.size() != 0))
            ASSERT_EQ(deque.front(), std_deque.front());

		ASSERT_EQ(deque.size(), std_deque.size());
		ASSERT_EQ(deque.empty(), std_deque.empty());
	}

	ASSERT_TRUE(std::equal(deque.begin(), deque.end(), std_deque.begin()));
	ASSERT_TRUE(std::equal(deque.cbegin(), deque.cend(), std_deque.cbegin()));

	ASSERT_TRUE(std::equal(deque.rbegin(), deque.rend(), std_deque.rbegin()));
	ASSERT_TRUE(std::equal(deque.crbegin(), deque.crend(), std_deque.crbegin()));
}

TEST_P(DequeTest, ConstDequeTest){
	Deque<int> deque;
	std::deque<int> std_deque;
	generateStartValues(GetParam(), deque, std_deque);

	const Deque<int> const_deque(deque);
	const std::deque<int> const_std_deque(std_deque);

	ASSERT_TRUE(std::equal(const_deque.begin(), const_deque.end(), const_std_deque.begin()));
	ASSERT_TRUE(std::equal(const_deque.rbegin(), const_deque.rend(), const_std_deque.rbegin()));

	ASSERT_EQ(const_deque.back(), const_std_deque.back());
	ASSERT_EQ(const_deque.front(), const_std_deque.front());

	for (size_t i = 0; i < const_std_deque.size(); ++i)
		ASSERT_EQ(const_deque[i], const_std_deque[i]);
}

INSTANTIATE_TEST_CASE_P(TestName, DequeTest, ::testing::Values(1, 50, 500, 5000, 150000, 3000000));

int main(int argc, char *argv[]){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
