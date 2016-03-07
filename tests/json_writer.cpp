#include <gold_fish/dom_reader.h>
#include <gold_fish/dom_writer.h>
#include <gold_fish/json_reader.h>
#include <gold_fish/json_writer.h>
#include <gold_fish/stream.h>
#include "unit_test.h"

namespace gold_fish { namespace dom
{
	TEST_CASE(string)
	{
		auto w = [&](const document& d)
		{
			stream::string_writer s;
			write(json::write(s), d);
			s.flush();
			return s.data;
		};

		test(w(true) == "true");
		test(w(false) == "false");
		test(w(nullptr) == "null");
		test(w(undefined{}) == "null");
		test(w(0ull) == "0");
		test(w(1ull) == "1");
		test(w(std::numeric_limits<uint64_t>::max()) == "18446744073709551615");
		test(w(0ll) == "0");
		test(w(1ll) == "1");
		test(w(-1ll) == "-1");
		test(w(std::numeric_limits<int64_t>::max()) == "9223372036854775807");
		test(w(std::numeric_limits<int64_t>::min()) == "-9223372036854775808");
		test(w(text_string("")) == "\"\"");
		test(w(text_string(u8"a\u0001\b\n\r\t\"\\/")) == "\"a\\u0001\\b\\n\\r\\t\\\"\\\\/\"");
		test(w(array{}) == "[]");
		test(w(array{ 1ull }) == "[1]");
		test(w(array{ 1ull, text_string("abc"), array{} }) == "[1,\"abc\",[]]");
		test(w(map{}) == "{}");
		test(w(map{ { text_string("a"), 1ull } }) == "{\"a\":1}");
		test(w(map{ { text_string("a"), 1ull }, { text_string("b"), 2ull } }) == "{\"a\":1,\"b\":2}");

		test(w(text_string("/B")) == "\"/B\"");
		test(w(byte_string({})) == "\"\\/B\"");
		test(w(byte_string({ 1 })) == "\"\\/BAQ==\"");
		test(w(byte_string({ 1, 2, 3 })) == "\"\\/BAQID\"");
		test(w(map{ { 1ull, 1ull } }) == "{1:1}");
	}

	TEST_CASE(test_roundtrip)
	{
		auto run = [](const char* data)
		{
			auto w = stream::string_writer{};
			auto r = stream::read_string_literal(data);
			dom::write(json::write(w), dom::load_in_memory(json::read(r)));
			w.flush();
			test(data == w.data);
		};

		run("[null]");
		run("[true]");
		run("[false]");
		run("[0]");
		run("[\"foo\"]");
		run("[]");
		run("{}");
		run("[0,1]");
		run("{\"foo\":\"bar\"}");
		run("{\"a\":null,\"foo\":\"bar\"}");
		run("[-1]");
		run("[-2147483648]");
		run("[-1234567890123456789]");
		run("[-9223372036854775808]");
		run("[1]");
		run("[2147483647]");
		run("[4294967295]");
		run("[1234567890123456789]");
		run("[9223372036854775807]");
		//run("[0.0]");
		//run("[-0.0]");
		run("[1.2345]");
		run("[-1.2345]");
		//run("[5e-324]");
		//run("[2.225073858507201e-308]");
		//run("[2.2250738585072014e-308]");
		//run("[1.7976931348623157e308]");
	}
}}