#include "token.h"
#include "test.h"

static void Numbers()
{
	std::string s = "3 3.0 .3";
	Tokenizer izer(s);

	Token t = izer.get();
	TEST(t.type == TokenType::NUMBER);
	TEST(t.lexeme == "3");
	TEST(t.dValue == 3.0);

	t = izer.get();
	TEST(t.type == TokenType::NUMBER);
	TEST(t.lexeme == "3.0");
	TEST(t.dValue == 3.0);

	t = izer.get();
	TEST(t.type == TokenType::NUMBER);
	TEST(t.lexeme == ".3");
	TEST_FP(t.dValue, 0.3);
}

static void Identifiers()
{
	std::string s = "foo bar42 _hello";
	Tokenizer izer(s);

	Token t = izer.get();
	TEST(t.type == TokenType::IDENT);
	TEST(t.lexeme == "foo");

	t = izer.get();
	TEST(t.type == TokenType::IDENT);
	TEST(t.lexeme == "bar42");

	t = izer.get();
	TEST(t.type == TokenType::IDENT);
	TEST(t.lexeme == "_hello");
}

static void Symbols()
{
	std::string s = "( ( ++*)";
	Tokenizer izer(s);

	Token t = izer.get();
	TEST(t.type == TokenType::LEFT_PAREN);
	TEST(t.lexeme == "(");

	t = izer.get();
	TEST(t.type == TokenType::LEFT_PAREN);
	TEST(t.lexeme == "(");

	t = izer.get();
	TEST(t.type == TokenType::PLUS);
	TEST(t.lexeme == "+");

	t = izer.get();
	TEST(t.type == TokenType::PLUS);
	TEST(t.lexeme == "+");

	t = izer.get();
	TEST(t.type == TokenType::MULT);
	TEST(t.lexeme == "*");

	t = izer.get();
	TEST(t.type == TokenType::RIGHT_PAREN);
	TEST(t.lexeme == ")");
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

		Token t = izer.get();
		TEST(t.type == TokenType::IDENT);
		TEST(t.lexeme == "dec");

		t = izer.get();
		TEST(t.type == TokenType::IDENT);
		TEST(t.lexeme == "x");

		t = izer.get();
		TEST(t.type == TokenType::EQUAL);

		t = izer.get();
		TEST(t.type == TokenType::LEFT_PAREN);

		t = izer.get();
		TEST(t.type == TokenType::NUMBER);
		TEST(t.dValue == 7.0);

		t = izer.get();
		TEST(t.type == TokenType::PLUS);

		t = izer.get();
		TEST(t.type == TokenType::IDENT);
		TEST(t.lexeme == "y0");

		t = izer.get();
		TEST(t.type == TokenType::RIGHT_PAREN);

		t = izer.get();
		TEST(t.type == TokenType::DIVIDE);

		t = izer.get();
		TEST(t.type == TokenType::NUMBER);
		TEST_FP(t.dValue, 3.1);
	}
}

static void TwoNumbers()
{
	std::string s = "1+2";
	Tokenizer izer(s);
	Token t = izer.get();
	TEST(t.type == TokenType::NUMBER);
	TEST(t.lexeme == "1");
	t = izer.get();
	TEST(t.type == TokenType::PLUS);
	t = izer.get();
	TEST(t.type == TokenType::NUMBER);
	TEST(t.lexeme == "2");
}

void Tokenizer::test()
{
	RUN_TEST(Numbers());
	RUN_TEST(Identifiers());
	RUN_TEST(Symbols());
	RUN_TEST(SimLang());
	RUN_TEST(TwoNumbers());
}