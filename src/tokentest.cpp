#include "token.h"
#include "test.h"

static void Numbers()
{
	std::string s = "3 +3 +3.0 .3 -.3";
	Tokenizer izer(s);

	Token t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == "3");
	TEST(t.dValue == 3.0);

	t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == "+3");
	TEST(t.dValue == 3.0);

	t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == "+3.0");
	TEST(t.dValue == 3.0);

	t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == ".3");
	TEST_FP(t.dValue, 0.3);

	t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == "-.3");
	TEST_FP(t.dValue, -0.3);
}

static void Identifiers()
{
	std::string s = "foo bar42 _hello";
	Tokenizer izer(s);

	Token t = izer.getNext();
	TEST(t.type == TokenType::identifier);
	TEST(t.value == "foo");

	t = izer.getNext();
	TEST(t.type == TokenType::identifier);
	TEST(t.value == "bar42");

	t = izer.getNext();
	TEST(t.type == TokenType::identifier);
	TEST(t.value == "_hello");
}

static void Symbols()
{
	std::string s = "( ( ++*)";
	Tokenizer izer(s);

	Token t = izer.getNext();
	TEST(t.type == TokenType::leftParen);
	TEST(t.value == "(");

	t = izer.getNext();
	TEST(t.type == TokenType::leftParen);
	TEST(t.value == "(");

	t = izer.getNext();
	TEST(t.type == TokenType::plus);
	TEST(t.value == "+");

	t = izer.getNext();
	TEST(t.type == TokenType::plus);
	TEST(t.value == "+");

	t = izer.getNext();
	TEST(t.type == TokenType::multiply);
	TEST(t.value == "*");

	t = izer.getNext();
	TEST(t.type == TokenType::rightParen);
	TEST(t.value == ")");
}

static void SimLang()
{
	static const int N = 3;
	std::string s[N] = {
		"dec x = (7 + y0) / 3.1",
		"dec x=(7+y0)/3.1",
		"dec x = (7+y0)\n/3.1"
	};
	for (int i = 0; i < N; i++) {
		Tokenizer izer(s[i]);

		Token t = izer.getNext();
		TEST(t.type == TokenType::identifier);
		TEST(t.value == "dec");

		t = izer.getNext();
		TEST(t.type == TokenType::identifier);
		TEST(t.value == "x");

		t = izer.getNext();
		TEST(t.type == TokenType::assign);

		t = izer.getNext();
		TEST(t.type == TokenType::leftParen);

		t = izer.getNext();
		TEST(t.type == TokenType::number);
		TEST(t.dValue == 7.0);

		t = izer.getNext();
		TEST(t.type == TokenType::plus);

		t = izer.getNext();
		TEST(t.type == TokenType::identifier);
		TEST(t.value == "y0");

		t = izer.getNext();
		TEST(t.type == TokenType::rightParen);

		t = izer.getNext();
		TEST(t.type == TokenType::divide);

		t = izer.getNext();
		TEST(t.type == TokenType::number);
		TEST_FP(t.dValue, 3.1);
	}
}

static void TwoNumbers()
{
	std::string s = "1+2";
	Tokenizer izer(s);
	Token t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == "1");
	t = izer.getNext();
	TEST(t.type == TokenType::plus);
	t = izer.getNext();
	TEST(t.type == TokenType::number);
	TEST(t.value == "2");
}

void Tokenizer::test()
{
	RUN_TEST(Numbers());
	RUN_TEST(Identifiers());
	RUN_TEST(Symbols());
	RUN_TEST(SimLang());
//	RUN_TEST(TwoNumbers()); FIXME bug
}