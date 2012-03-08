#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>

int main( int argc, char **argv)
{
	CppUnit::TestResult controller;
	CppUnit::BriefTestProgressListener l;
	CppUnit::TextUi::TestRunner runner; 
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	CppUnit::TestResultCollector result;
	
	/* Add listener to show tests names */
	controller.addListener(&l);	
	controller.addListener(&result);
	/* Add the tests */
	runner.addTest( registry.makeTest() );

	/* Run the tests */
	runner.run(controller);
	
	return result.wasSuccessful() ? 0 : 1;
}
