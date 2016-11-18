#include "../src/atlasbuilder.h"
#define BOOST_TEST_MODULE AtlasBuilderTest
#include <boost/test/unit_test.hpp>
#

BOOST_AUTO_TEST_CASE( my_test )
{
	const char* args = { "test/fixtures/test.png" };
	int retCode = atlasbuilder( 1, &args );
    BOOST_CHECK( retCode == 0 );
}
