// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "stdtest.h"
#include "../uqueue.h"
#include "../ustack.h"

void TestStackAndQueue (void)
{
    stack<int> s;
    cout << "Testing stack: ";
    for (size_t i = 0; i < 5; ++ i)
	s.push (1 + i);
    s.emplace (42);
    cout << "popping:";
    while (!s.empty()) {
	cout << ' ' << s.top();
	s.pop();
    }
    cout << endl;

    queue<int> q;
    cout << "Testing queue: ";
    for (size_t k = 0; k < 5; ++ k)
	q.push (1 + k);
    q.emplace (42);
    cout << "popping:";
    while (!q.empty()) {
	cout << ' ' << q.front();
	q.pop();
    }
    cout << endl;
}

StdBvtMain (TestStackAndQueue)
