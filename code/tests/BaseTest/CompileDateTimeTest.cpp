#include <Base/CompileDateTime.h>

#define BOOST_TEST_MODULE CompileDateTimeTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE (testOutput)
{
	boost::posix_time::ptime p = BFG::compileDateTime();

	std::cout << "Output of BFG::compileDateTime(): " << BFG::compileDateTime() << std::endl;
}

BOOST_AUTO_TEST_CASE (testAnomalities)
{
	boost::posix_time::ptime p = BFG::compileDateTime();

	bool isNormal = !p.is_special();

	BOOST_REQUIRE (isNormal);
}

BOOST_AUTO_TEST_CASE (testContent)
{
	boost::posix_time::ptime p = BFG::compileDateTime();

	std::string iso = to_iso_string(p);

	std::cout << "As iso string: " << iso << std::endl;

	bool sizeOk = iso.size() == 15;
	bool yearOk = atoi(iso.substr(0, 4).c_str()) > 2010;

	BOOST_REQUIRE (sizeOk && yearOk);
}
